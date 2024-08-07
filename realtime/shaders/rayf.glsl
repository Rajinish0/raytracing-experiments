#version 330 core

#define MAX_SPHERES 100
#define OPAQUE_T      0x01
#define RELECT_T      0x02
#define REFRACT_T     0x04

#define FROM_TEXTURE 0x01
#define FROM_COLOR   0x02

#define EPS_F 0.0000001

struct Sphere {
    vec3 pos; // world position
    vec3 col;
    float radius;
    int type;
    int textType;
    float ior;
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
uniform int numSpheres; 
uniform Sphere mySphere[MAX_SPHERES];


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



vec4 Trace(Ray r, int depth){
    float minT, ior;
    bool intersects;
    bool intersectOne = false;
    int k;
    vec3 collPoint, normalVec;
    bool frontFace;
    while (depth-- > 0){
        minT = 100000000;
        k = -1;
        for (int i =0; i < numSpheres; i++){
            intersects = false;
            float t = findCollision(r, mySphere[i], intersects);
            if (intersects && t > 0 && t < minT){
                intersectOne = true;
                minT = t;
                collPoint = r.pos + r.direc*t;
                normalVec = (collPoint - mySphere[i].pos )/mySphere[i].radius;
                /* 
                normalVec will always be in the direction opposite to the ray
                */
                if (dot(r.direc, normalVec) > 0){
                    normalVec *= -1; 
                    frontFace = false;
                }
                else {
                    frontFace = true;
                }
                ior = mySphere[i].ior;
                k = i;
            }
        }
        if (k != -1){
            if (mySphere[k].type == OPAQUE_T){
                if (mySphere[k].textType == FROM_TEXTURE){
                    vec2 tCoord = sphereCoordToTex( normalVec );
                    return texture(texture_diffuse1, tCoord);
                }
                else
                    return vec4(mySphere[k].col, 1.0);
            }
            /*
            Adding ref*0.01:
                when I am just on the edge of the sphere or i am inside the sphere 
                I want to it to return the second equation from the quadratic formula 
                "the plus version", so in the above collision function it just checks
                wether t is less than zero or not in which case either the sphere is behind me 
                or i'm inside the sphere (the minus version would give the intersection behind).                
                The problem is the edges, there's weird quirks for points exactly on the surface of the sphere
                so for REFLECT_T i just push the collision point by a smudge outside the sphere 
                this does not (should not) have any umwanted visual effects but fixes the edge problem.
                For RELFECT_T I push it a little inside the sphere (or outside) so I can get the 
                positive version of the quadratic formula.
            */
            else if (mySphere[k].type == RELECT_T){
                vec3 ref = reflect(r.direc, normalVec);
                r = Ray(collPoint + ref*0.001, 
                        ref );
            }
            else if (mySphere[k].type == REFRACT_T){
                // vec3 ref = refract(r.direc, normalVec, 0.95f);
                // r = Ray(collPoint + ref*0.001,
                //         ref);

                vec3 normd = normalize(r.direc);
                float cos_theta = min(dot(-normd, normalVec), 1.0);
                float sin_theta = sqrt(1.0f - cos_theta * cos_theta);

                float ri = (frontFace) ? 1.0f/ior : ior;
                
                vec3 direction;
                if (ri * sin_theta > 1.0)
                    direction = reflect(normd, normalVec);
                else 
                    direction = refract(normd, normalVec, ri);

                //vec3 ref = refract(r.direc, normalVec, ratio);
                r = Ray(collPoint + direction*0.001, 
                        direction);
            }
        }
        else
            break;
    }
    // return vec4(1.0, 0.0, 0.0, 1.0);
    return texture(skybox, r.direc);
}

void main(){

    vec3 raydir = gRayDir();
    Ray r = {
        camPos,
        raydir
    };

    fcol = Trace(r, 10);

    // float zMax = -10000.0;
    // // fcol = texture(skybox, vec3(fragpos, -1.0));
    // fcol = texture(skybox, r.direc);
    // for (int i =0; i < numSpheres; ++i){
    //     bool intersects = false;
    //     float colT = findCollision(r, mySphere[i], intersects);
    //     vec3 collPoint = r.pos + r.direc * colT;

    //     if (intersects && colT > 0) {
    //         vec3 normalVec = (collPoint - mySphere[i].pos )/mySphere[i].radius;
    //         vec3 ref = reflect(r.direc, normalVec);
    //         if (collPoint.z > zMax){
    //             zMax = collPoint.z;
    //             vec2 tCoord = sphereCoordToTex( (collPoint - mySphere[i].pos)/mySphere[i].radius  );
    //             fcol = texture(texture_diffuse1, tCoord);
    //             // fcol = texture(skybox, ref);
    //         }
    //     }
    // }

}


