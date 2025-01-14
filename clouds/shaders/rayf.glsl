#version 330 core

#define MAX_SPHERES 100
#define OPAQUE_T      0x01
#define RELECT_T      0x02
#define REFRACT_T     0x04

#define FROM_TEXTURE 0x01
#define FROM_COLOR   0x02

#define EPS_F 0.001
#define MAX_STEPS   256
#define MARCH_STEP  0.08
#define MAX_DIST = 1000.0

struct Sphere {
    vec3 pos; // world position
    vec3 col;
    float radius;
    int type;
    int textType;
    float ior;
};

struct HitRecord {
    vec3 color;
    vec3 normal;
};

struct Ray{
    vec3 pos;
    vec3 direc;
};

struct Collision{
    vec3 collisionPoint;
    vec3 normalVec;
};

uniform samplerCube skybox;
uniform sampler2D texture_diffuse1;
uniform sampler2D uNoise;
uniform float uTime;

uniform int numSpheres; 
uniform Sphere mySphere[MAX_SPHERES];

const vec3 lightPos = vec3(-1.0f, 7.0f, 2.0f);
const float inf = 100000.0f;


bool rayIntersectsSphere(Ray r, Sphere sp){
    /*
    a line intersects a hypersphere (in general)
    if the distance b/w the center of the hypersphere 
    and the line is less than or equal to the radius 
    of the hypersphere
    */
    vec3 distVec  = dot((sp.pos-r.pos),
                         r.direc)*r.direc - sp.pos;
    return (length(distVec) <= sp.radius);
}

float findCollision(Ray r, Sphere sp, out bool isCollided){
    isCollided = false;
    /*
    i can represent ray as a function of time like this
    r(t) = pos + t*direc
    and the sphere is basically 
    f(x) = r^2 (where x is a vector)
    which is (x-c).(x-c) = r^2 (where c is the center)

    so the intersection point is 
    (r(t) - c).(r(t) - c) = r^2
    which is quadratic in t.
    */

    /*
    note to self:
        dont trust the equations u derive past midnight
    */

    vec3 q = (r.pos - sp.pos);
    float a = dot(r.direc, r.direc);
    // float b = dot(r.pos, r.direc) - dot(sp.pos, r.direc);
    float b = 2 * dot(q, r.direc);
    // float c = dot(r.pos, r.pos) - 2*dot(r.pos, sp.pos) + dot(sp.pos, sp.pos) - sp.radius*sp.radius;
    float c = dot(q, q) - sp.radius * sp.radius;

    float disc = b*b - 4*a*c;
    if (disc < 0){
        isCollided = false;
        return 0.0f;
        // tOut = 0;
        // return vec3(0.0f);
    }
    /*
    im doing "-" bcz i want the lower t value (the first t that hits the sphere) 
    but in general maybe i might have to fix this
    */
    float t = ( -b - sqrt(disc) ) / (2 * a);

    // if (length(sp.pos-r.pos) < sp.radius)
    if (t < 0)
        t = (-b + sqrt(disc)) / (2 * a);

    isCollided = true;
    return t;
    // vec3 collisionPoint = r.pos + r.direc*t;
    // vec3 normalVec = (collisionPoint-sp.pos)/sp.radius;

    // return collisionPoint;
    // return Collision(collisionPoint, normalVec);
}



in vec2 fragpos;
in vec3 col;


uniform vec3 camPos;
uniform mat4 invProjMat;
uniform mat4 invViewMat;
uniform mat4 sphereModel;

out vec4 fcol;


vec3 gRayDir(){
    /*
    This is better then the below function because it shoots the ray through the center of the pixel
    i did not want to hard code the global consts of projecion matrix here, 
    so im just hitting the [-1, 1] screen coords with the inverse Projection matrix

    but the logic is basically the same
    */

    vec2 coords = gl_FragCoord.xy;
    //float aspRatio = 800.0f/600.0f;
    float px = (2 * ((coords.x + 0.5)/800.0f) - 1);// * (sqrt(2) / 2.0f) * aspRatio;
    float py = (2*((coords.y + 0.5)/600.0f) - 1);// * sqrt(2)/2.0f;
    vec3 dir = (invProjMat*vec4(px, py, 0, 1)).xyz;
    vec4 rayDir = invViewMat*vec4(dir.x, dir.y, -1.0, 0.0);
    return normalize(rayDir.xyz);
}

vec3 getRayDir(vec2 fpos){
    /*
    * The perspective transformation transforms the truncated pyramid
    * to a -1, 1 cube such that the near plane gets mapped to n and the far 
    * plane gets mapped to -f (because we divide by -z and n must get mapped to -1) 
    * so something like (r, 0, -n, 1) will get mapped to (n, smth, -n, n)
    * and then to ndc it will be (1, smth/n, -1)
    * now to go back from (x, y) ndc to those coords 
    * I'll just apply the inv matrix to (x, y, 0, 1) 
    * there's 1 in w to allow translation, and the z coord of this does not affect
    * what the inv will return for x, y so i'll just keep it at 0. 
    */

    vec4 ndc = vec4(fpos.x, fpos.y, 0, 1);
    vec4 view = invProjMat*ndc;
    // view /= view.w;
    /*
    at this point, the situation is back to camera being at the origin
    and pointing in the negative z direction with the view vector 
    pointing through some point of the near plane (which is the projecion plane)

    to get where the camera is actually pointing, i just apply view in reverse, 
    however since I only need the direction, i'll make the w component 0
    */
    view.z = -1.0;
    view.w = 0.0; // disable translation caused by camera movement, i only need the rotation part to get the direction

    vec3 rayDir = (invViewMat*view).xyz;
    return normalize(rayDir);
}

vec2 sphereCoordToTex(vec3 sphereCoord){
    /*
    THIS ASSUMES THE sphere is centered at 0 and has radius 1
    then the equation of the sphere is 

    f(theta, phi) = [
    cos(theta) * cos(phi),
    sin(phi),
    sin(theta) * cos(phi)
    ]

    from documentation:
        asin maps to [-pi/2, pi/2]
        acos maps to [0, pi]
    the equations below are catered to map these ranges

    there is still a goddamn line through the texture 
    TO DO: fix that
    */

    /*
    this is really the only place i could think of for applying the local transformation
    which is just a rotation (hence the 0 w component) it is also essential
    */

    sphereCoord = vec3(sphereModel * vec4(sphereCoord, 0.0));

    const float PI = 3.141592;

    float phi = asin(sphereCoord.y);
    float theta = acos(sphereCoord.x * (1 / cos(phi)));
    float x, y;
    if (sphereCoord.z > 0)
        x = (PI - theta) / (2.0 * PI);
    else
        x = (theta + PI) / (2.0 * PI);
    
    y = ( (PI/2.0 - phi) / PI );

    return vec2(x, y);
}


// float noise(vec3 x){
//     vec3 p = floor(x);
//     vec3 f = fract(x);

//     f = f * f * (3.0 - 2.0 * f);

//     // vec2 uv = (p.xy + vec2(800.0 * 57.0/420.0, 600.0 * 113.0/236.0) * p.z) + f.xy;
//     // vec2 tex = texture(uNoise, (uv + 0.5)/vec2(800.0, 600.0), 0.0).yx;
//     vec2 uv = p.xy + f.xy;
//     vec2 tex = texture(uNoise, (uv + 0.5), 0.0).yx;

//     return mix( tex.x, tex.y, f.z ) * 2.0 - 1.0;
// }

// float noise(vec3 x ) {
//   vec3 p = floor(x);
//   vec3 f = fract(x);
//   f = f*f*(3.0-2.0*f);

//   vec2 uv = (p.xy+vec2(37.0,239.0)*p.z) + f.xy;
//   vec2 tex = textureLod(uNoise,(uv+0.5)/256.0,0.0).yx;

//   return mix( tex.x, tex.y, f.z ) * 2.0 - 1.0;
// }

float hash( float n )
{
    return fract(sin(n)*43758.5453);
}

float noise( in vec3 x )
{
    vec3 p = floor(x);
    vec3 f = fract(x);

    f = f*f*(3.0-2.0*f);

    float n = p.x + p.y*57.0 + 113.0*p.z;

    float res = mix(mix(mix( hash(n+  0.0), hash(n+  1.0),f.x),
                        mix( hash(n+ 57.0), hash(n+ 58.0),f.x),f.y),
                    mix(mix( hash(n+113.0), hash(n+114.0),f.x),
                        mix( hash(n+170.0), hash(n+171.0),f.x),f.y),f.z);
    return res;
}

// float fbm(vec3 p){
//     vec3  q = p + uTime * 0.5 * vec3(1.0, -0.2, -1.0);
//     float g = noise(q);

//     float f         = 0.0;
//     float scale     = 0.5;
//     float factor    = 2.02;

//     for (int i =0; i < 6; i++){
//         f += scale * noise(q);
//         q *= factor;
//         factor += 0.21;
//         scale *= 0.5;
//     }

//     return f;

// }


mat3 m = mat3( 0.00,  0.80,  0.60,
              -0.80,  0.36, -0.48,
              -0.60, -0.48,  0.64 );

float fbm( vec3 p )
{
    float f;
    p = p + uTime * 0.5;
    f  = 0.5000*noise( p ); p = m*p*2.02;
    f += 0.2500*noise( p ); p = m*p*2.03;
    f += 0.12500*noise( p ); p = m*p*2.01;
    f += 0.06250*noise( p );
    return f;
}

float smin( float a, float b, float k )
{
    k *= 1.0;
    float r = exp2(-a/k) + exp2(-b/k);
    return -k*log2(r);
}

float smin2( float a, float b, float k )
{
    k *= 4.0;
    float h = max( k-abs(a-b), 0.0 )/k;
    return min(a,b) - h*h*k*(1.0/4.0);
}

float sdf_sp(Sphere sp, vec3 pos){
    return length(sp.pos-pos) - sp.radius;
}

// float sdTorus( vec3 p, vec2 t )
// {
//   vec2 q = vec2(length(p.xz)-t.x,p.y);
//   return length(q)-t.y;
// }


const float PI = 3.141596;

// float sdf(vec3 pos){
//     Sphere sp = mySphere[0];
//     Sphere sp2 = mySphere[1];
//     float displacement = sin(2*pos.x) * sin(2*pos.y) * sin(2*pos.z) * 0.25;
//     vec3 p = pos - sp.pos;
//     float t = sdTorus(p, vec2(5.0, 5.0));
//     return smin2(
//             t + displacement,
//             sdf_sp(sp2, pos),
//             5.0f
//             );
// }


float sdf(vec3 pos){
    float mm = inf;
    for (int i =0; i < 1; ++i){
        mm = smin2(mm, sdf_sp(mySphere[i], pos), 5.0f);
    }
    return mm;
}

// float density(vec3 pos){
//     // return -sdf(pos);
//     float f = fbm(pos);
//     return f - (length(mySphere[0].pos - pos) - mySphere[0].radius);
//     // return f - sdf(pos);
// }


mat2 rotate2D(float a) {
  float s = sin(a);
  float c = cos(a);
  return mat2(c, -s, s, c);
}

float nextStep(float t, float len, float smo) {
  float tt = mod(t += smo, len);
  float stp = floor(t / len) - 1.0;
  return smoothstep(0.0, smo, tt) + stp;
}

float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
  vec3 ab = b - a;
  vec3 ap = p - a;

  float t = dot(ab, ap) / dot(ab, ab);
  t = clamp(t, 0.0, 1.0);

  vec3 c = a + t * ab;

  float d = length(p - c) - r;

  return d;
}

float sdSphere(vec3 p, float radius) {
  return length(p) - radius;
}

float sdTorus(vec3 p, vec2 r) {
  float x = length(p.xz) - r.x;
  return length(vec2(x, p.y)) - r.y;
}

float sdCross(vec3 p, float s) {
  float da = max(abs(p.x), abs(p.y));
  float db = max(abs(p.y), abs(p.z));
  float dc = max(abs(p.z), abs(p.x));

  return min(da, min(db, dc)) - s;
}

float sdCone( vec3 p, vec2 c, float h )
{
  // c is the sin/cos of the angle, h is height
  // Alternatively pass q instead of (c,h),
  // which is the point at the base in 2D
  vec2 q = h*vec2(c.x/c.y,-1.0);
    
  vec2 w = vec2( length(p.xz), p.y );
  vec2 a = w - q*clamp( dot(w,q)/dot(q,q), 0.0, 1.0 );
  vec2 b = w - q*vec2( clamp( w.x/q.x, 0.0, 1.0 ), 1.0 );
  float k = sign( q.y );
  float d = min(dot( a, a ),dot(b, b));
  float s = max( k*(w.x*q.y-w.y*q.x),k*(w.y-q.y)  );
  return sqrt(d)*sign(s);
}


float density(vec3 p) {
  p.xz *= rotate2D(uTime);
  p.yz *= rotate2D(uTime);
  vec3 p1 = p;
//   p1.xz *= rotate2D(-PI * 0.3 + uTime);
//   p1.yz *= rotate2D(PI * 0.75  + uTime);
//   p1.xy *= rotate2D(PI);

  float s1 = sdTorus(p1, vec2(2.3, 0.9));
//   float s1 = sdCone(p1, vec2(1.0), 3.0);
  float s2 = sdCross(p1 * 2.0, 0.6);
  float s3 = sdSphere(p, 1.5);
  float s4 = sdCapsule(p, vec3(-2.0, -1.5, 0.0), vec3(2.0, 1.5, 0.0), 1.0);

  float t = mod(nextStep(uTime, 3.0, 1.2), 4.0);

  float distance = mix(s1, s2, clamp(t, 0.0, 1.0));
  distance = mix(distance, s3, clamp(t - 1.0, 0.0, 1.0));
  distance = mix(distance, s4, clamp(t - 2.0, 0.0, 1.0));
  distance = mix(distance, s1, clamp(t - 3.0, 0.0, 1.0));

  float f = fbm(p);

  return -distance + f;
}


vec3 calcNormal(vec3 pos){
    vec3 d = vec3(0.001, 0.0, 0.0);
    float dX = sdf(pos + d.xyy) - sdf(pos - d.xyy);
    float dY = sdf(pos + d.yxy) - sdf(pos - d.yxy);
    float dZ = sdf(pos + d.yyx) - sdf(pos - d.yyx);
    return normalize(vec3(dX, dY, dZ));
}

float calcDirectionalDeriv(vec3 pos, vec3 d, float eps = 0.3){
    //assumes d is normalized
    //from basic calc: (f(pos + d*h) - f(pos))/h
    return (density(pos + d * eps) - density(pos))/eps;
}



const vec3 SUN_POSITION = vec3(5.0, 0.0, 0.0);
vec3 Trace(Ray r){

    vec3 r0 = r.pos;
    vec3 rd = r.direc;
    vec3 col = vec3(0.0);
    vec3 normal = vec3(0.0);
    float total = 0.0;

    for (int i =0; i < MAX_STEPS; ++i){
        
        float d = sdf(r0);

        if (d < EPS_F){
            col = vec3(1.0, 0.0, 0.0);
            normal = calcNormal(r0);
            vec3 lightVec = normalize(lightPos - r0);
            vec3 camVec = normalize(camPos - r0);
            vec3 ref = reflect(-lightVec, normal);
            float spec = pow(max(0.0, dot(ref, camVec)), 32);
            float diff = max(0.0, dot(lightVec, normal));
            return col * diff + vec3(0.5f) * spec + col * vec3(0.1);
            // return HitRecord(col * diff + vec3(1.0) * spec, normal);
        }

        r0 += r.direc * d;
        total += d;

        if (total > 1000.0)
            break;
    }
    return vec3(0.0);
    // return HitRecord(texture(skybox, r.direc).xyz, normal);
    // return texture(skybox, r.direc);
}


const vec3 sunDirection = normalize(SUN_POSITION);

vec4 TraceDensity(Ray r){
    vec3 r0 = r.pos;
    vec3 rd = r.direc;
    vec4 col = vec4(0.0);
    vec3 normal = vec3(0.0);
    float total = 0.0;


    for (int i =0; i < MAX_STEPS; ++i){
        float d = density(r0);
        if (d > 0) {
            // float diffuse = clamp( (density(r0) - density(r0 + 0.3 *sunDirection) ) )
            float diffuse = clamp( -calcDirectionalDeriv(r0, sunDirection) , 0.0, 1.0 );
            vec3 lin = vec3(0.60,0.60,0.75) * 1.1 + 0.8 * vec3(1.0,0.6,0.3) * diffuse;
            vec4 color = vec4(mix(vec3(1.0), vec3(0.0), d), d );
            color.rgb *= lin;
            color.rgb *= color.a;
            col += color * (1.0-col.a);
            // col = vec4(1.0, 1.0f, 1.0f, 0.5);
        }

        r0 += (r.direc * MARCH_STEP);
        total += 0.08;
    }
    col.rgb *= 1.2;
    return col;
}

void main(){

    vec3 raydir = gRayDir();
    vec2 uv = gl_FragCoord.xy/vec2(800.0, 600.0);
    uv -= 0.5;
    uv.x *= (800.0/600.0);
    // vec3 nuv = normalize((invViewMat*vec4(vec3(uv, -1.0), 0.0)).xyz);

    // Ray r = {
    //     camPos,
    //     nuv
    // };
    Ray r;
    r.pos = camPos;
    r.direc = raydir;

  vec3 color;
  float sun = clamp(dot(sunDirection, raydir), 0.0, 1.0 );
  // Base sky color
  color = vec3(0.7,0.7,0.90);
  // Add vertical gradient
  color -= 0.8 * vec3(0.90,0.75,0.90) * raydir.y;
  // Add sun color to sky
  color += 0.5 * vec3(1.0,0.5,0.3) * pow(sun, 10.0);

  // Cloud
  vec4 res = TraceDensity(r);
  color = color * (1.0 - res.a) + res.rgb;
  fcol = vec4(color, 1.0);

    // fcol = TraceDensity(r);
    //fcol = vec4(col, 1.0f);
}


