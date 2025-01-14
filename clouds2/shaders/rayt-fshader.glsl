#version 330 core

#define MAX_SPHERES 20

struct Sphere {
    vec3 pos; // world position
    vec3 col;
    float radius;
};

struct Ray{
    vec3 pos;
    vec3 direc;
};

struct Collision{
    vec3 collisionPoint;
    vec3 normalVec;
};

uniform int numSpheres; 
uniform samplerCube skybox;
uniform Sphere mySphere;

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

vec3 findCollision(Ray r, Sphere sp, out bool isCollided){
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

    float a = dot(r.direc, r.direc);
    float b = dot(r.pos, r.direc) - dot(sp.pos, r.direc);
    float c = dot(r.pos, r.pos) - 2*dot(r.pos, sp.pos) + dot(sp.pos, sp.pos) - sp.radius*sp.radius;

    float disc = b*b - 4*a*c;
    if (disc < 0){
        isCollided = false;
        return vec3(0.0f);
    }
    isCollided = true;
    float t = ( -b + sqrt(disc) ) / (2 * a);
    vec3 collisionPoint = r.pos + r.direc*t;
    // vec3 normalVec = (collisionPoint-sp.pos)/sp.radius;

    return collisionPoint;
    // return Collision(collisionPoint, normalVec);
}


in vec2 fragpos;
in vec3 col;


uniform vec3 camPos;
uniform mat4 invProjMat;
uniform mat4 invViewMat;

out vec4 fcol;


vec3 getRayDir(vec2 fpos){
    /*
    * The perspective transformation transforms the culled pyramid
    * to a -1, 1 cube such that the near plane gets mapped to n and the far 
    * plane gets mapped to -f (because we divide by -z and n must get mapped to -1) 
    * so something like (r, 0, -n, 1) will get mapped to (n, smth, -n, n)
    * and then to ndc it will be (1, smth/n, -1)
    * now to go back from (x, y) ndc to those coords 
    * I'll just apply the inv matrix to (x, y, 0, 1) 
    * there's 1 in w to allow translation, and the z coord of this does not affect
    * what the inv will return for x, y so i'll just keep it at 0. 
    */

    vec4 ndc = vec4(fpos.x, fpos.y, 0.0, 1.0); 
    vec4 view = invProjMat*ndc;
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


void main(){

    vec3 raydir = getRayDir(fragpos);
    Ray r = {
        camPos,
        raydir
    };

    bool intersects = false;

    vec3 coll = findCollision(r, mySphere, intersects);

    if (!intersects)
        fcol = texture(skybox, vec3(fragpos, -1.0));
        // fcol = vec4(col, 1.0);
    else {
        vec3 normalVec = (coll - mySphere.pos )/mySphere.radius;
        vec3 ref = reflect(r.direc, normalVec);
        // fcol = texture(skybox, vec3(ref.x, ref.y, 1.0) );
        fcol = texture(skybox, ref);
        // fcol = texture(skybox, normalVec);
    }
        // fcol = vec4(mySphere.col, 1.0);

}


