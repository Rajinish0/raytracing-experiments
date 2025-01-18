#version 330 core

in vec2 tCoord;

uniform sampler2D texture_diffuse1;
uniform sampler2D depthTexture;
uniform sampler2D weather_data;
uniform sampler3D texture_clouds;
uniform sampler3D detailTexture;

out vec4 fcol;


uniform mat4 invProjMat;
uniform mat4 invViewMat;
uniform float near;
uniform float far;


struct Ray{
    vec3 pos;
    vec3 dir;
};


struct Rectange {
    vec3 pos; // center (world pos)
    vec3 dims; //width, height, breadth
};

struct interval{
    float min;
    float max;
};

struct _itvl_intersect_info{
    bool intersect;
    interval itvl;
};

float remap(float val, float orig_lo, float orig_hi, float targ_lo, float targ_hi){
    //lerp(a, b, p) = a + (b-a)*a;
    //this is basically that on steroids
    return targ_lo + (val - orig_lo) * (targ_hi - targ_lo) / (orig_hi - orig_lo);
}

_itvl_intersect_info intervals_intersect(interval i1, interval i2){
    _itvl_intersect_info ret_info;
    // float i1_len = i1.max - i1.min;
    // float i2_len = i2.max - i2.min;
    // float maxm = max(i1.max, i2.max);
    // float minm = min(i1.min, i2.min);
    // ret_info.intersect = maxm - minm < i1_len + i2_len;
    ret_info.intersect = !(i1.min > i2.max || i2.min > i1.max);
    if (ret_info.intersect){
        ret_info.itvl.min = max(i1.min, i2.min);
        ret_info.itvl.max = min(i1.max, i2.max);
    }
    return ret_info;
}

float linearizeDepth(float depth){
    //depth is b/w 0 and 1 (0 for near, 1 for far)
    float ndc = depth * 2.0 - 1.0; // [0, 1] -> [-1, 1]
    return (2.0 * near * far) / (far + near - ndc * (far - near));
}


vec3 gRayDir(){
    vec2 coords = gl_FragCoord.xy;
    //float aspRatio = 800.0f/600.0f;
    float px = (2 * ((coords.x + 0.5)/800.0f) - 1);// * (sqrt(2) / 2.0f) * aspRatio;
    float py = (2*((coords.y + 0.5)/600.0f) - 1);// * sqrt(2)/2.0f;
    vec3 dir = (invProjMat*vec4(px, py, 0, 1)).xyz;
    vec4 rayDir = invViewMat*vec4(dir.x, dir.y, -1.0, 0.0);
    return normalize(rayDir.xyz);
}

interval getInterval(float v1, float v2){
    return (v1 < v2) ? interval(v1, v2) : interval(v2, v1);
}

bool isValidInterval(interval itvl){
    return itvl.max >= itvl.min && (itvl.min >= 0 || itvl.max >= 0);
}

struct _box_intersect_info{
    bool intersect;
    float distToBox;
    float distInsideBox;
};

_box_intersect_info rayIntersectsBox(Ray r, Rectange box){
    _box_intersect_info ret_info;
    vec3 box_min = box.pos - box.dims/2.0f;
    vec3 box_max = box.pos + box.dims/2.0f;
    vec3 max_intervals = (box_max - r.pos)/r.dir;
    vec3 min_intervals = (box_min - r.pos)/r.dir;
    interval i1 = getInterval(min_intervals.x, max_intervals.x);
    interval i2 = getInterval(min_intervals.y, max_intervals.y);
    interval i3 = getInterval(min_intervals.z, max_intervals.z);
    _itvl_intersect_info info = intervals_intersect(i1, i2);
    if (!info.intersect) {
        ret_info.intersect = false;
    }
    else{
        _itvl_intersect_info info2 = intervals_intersect(info.itvl, i3);
        ret_info.intersect = (info2.intersect && isValidInterval(info2.itvl));
        // ret_info.distToBox = (info2.itvl.min) > 0 ? info2.itvl.min : 0;
        ret_info.distToBox = max(info2.itvl.min, 0);
        // ret_info.distInsideBox = (info2.itvl.min > 0)  ? (info2.itvl.max - info2.itvl.min) : info2.itvl.max;
        ret_info.distInsideBox = min(info2.itvl.max, info2.itvl.max - info2.itvl.min);
        // ret_info.distToBox = info2.itvl.min;
        // ret_info.distInsideBox = info2.itvl.max - info2.itvl.min;
    }
    return ret_info;
}


struct Sphere {
    vec3 pos; // world position
    vec3 col;
    float radius;
};

// bool rayIntersectsSphere(Ray r, Sphere sp){
//     /*
//     a line intersects a hypersphere (in general)
//     if the distance b/w the center of the hypersphere 
//     and the line is less than or equal to the radius 
//     of the hypersphere
//     */
//     vec3 distVec  = dot((sp.pos-r.pos),
//                          r.dir)*r.dir - sp.pos;
//     return (length(distVec) <= sp.radius);
// }

bool rayIntersectsSphere(Ray r, Sphere sp) {
    vec3 L = sp.pos - r.pos;

    float t_closest = dot(L, r.dir);

    float d2 = dot(L, L) - t_closest * t_closest;

    return d2 <= sp.radius * sp.radius;
}

uniform Rectange bounding_rect;
uniform vec3 camPos;

// float densityThreshold      = 0.7;
float densityMultiplier     = 4.0; 
// float scale                 = 0.1;

uniform float densityThreshold;
uniform float scale;

uniform vec3 offSet;

// vec3 sunDirection = normalize(vec3(0.0f, 1.0f, -2.0f) + sin(offSet));
vec3 sunDirection = normalize(
    vec3(
    cos(offSet.z),
    sin(offSet.z),
    -1.0
    )
);

float density(vec3 p, Rectange bounding_rect, float scale, vec3 offSet){
    vec3 sampleT = ((p - bounding_rect.pos + offSet)/ (bounding_rect.dims/2.0) + 0.5f) * scale;
    return texture(texture_clouds, sampleT).r;
    // return T.r + 0.625 * T.g + 0.25 * T.b + 0.125 * T.a;
}

// float calcDirectionalDeriv(vec3 pos, vec3 d, float eps){
    // return (density(pos + d * eps) - density(pos))/eps;
// }

float nDensity(vec3 p){
    vec3 sampleT = ((p - bounding_rect.pos + offSet)/ (bounding_rect.dims/2.0) + 0.5f) * scale;
    vec4 T = texture(texture_clouds, sampleT);
    T = max(vec4(0.0), T-vec4(densityThreshold));
    return T.r + 0.625 * T.g + 0.25 * T.b + 0.125 * T.a;
}

float getDensity(vec3 pos){
    vec4 T = texture(texture_clouds, pos);
    // return T.r;
    // return remap(T.r, (T.g * 0.625 + T.b * 0.25 + T.a * 0.125)-1, 1, 0, 1);
    // T.r + 0.625 * T.g + 
    T = max(vec4(0.0), T-vec4(0.5));
    return T.r + 0.625 * T.g + 0.25 * T.b + 0.125 * T.a;
}

float getCloudDensity(vec3 p){
    vec3 sampleT = ((p - bounding_rect.pos + offSet/5.0)/ (bounding_rect.dims) + 0.5f) * scale;
    vec4 T = texture(texture_clouds, sampleT);
    float pw = T.r;
    float wfbm = T.g * 0.625 + T.b * .25 + T.a * .125;
    float cloud = remap(pw, wfbm - 1, 1., 0., 1.);

    float heightPercent = (p.y - bounding_rect.pos.y)/bounding_rect.dims.y;

    cloud = remap(cloud, .85, 1., 0., 1.);
    return cloud;
}


float saturate(float v){
    return clamp(v, 0.0, 1.0);
}

float lerp(float a, float b, float p){
    return a + (b-a)*p;
}

float weatherTexturMultiplier = 10.;

float getCloudDensity2(vec3 p){
    vec3 sampleT = ((p - bounding_rect.pos + offSet*2.)/ (bounding_rect.dims) + 0.5f) * scale;
    vec4 T = texture(texture_clouds, sampleT);
    float pw = T.r;
    float wfbm = T.g * 0.625 + T.b * .25 + T.a * .125;

    vec4 weather = texture(weather_data, p.xz * weatherTexturMultiplier);
    // float WMc = max(weather.r, saturate(.7-.5) * weather.g * 2.);

    float heightPercent = (p.y - (bounding_rect.pos.y-bounding_rect.dims.y/2))/bounding_rect.dims.y;
    float SRb = saturate(remap(heightPercent, 0., 0.7, 0., 1.));

    float SRt = saturate(remap(heightPercent, .2, 1., 1., 0.));
    float SA = SRb * SRt; // heightGradient

    float DRb = heightPercent * saturate(remap(heightPercent, 0., 0.15, 0., 1.));
    float DRt = saturate(remap(heightPercent, 0.9, 1.0, 1., 0));
    float DA = 0.5 * DRb * DRt * 0.5 * 2; // density

    float SNsample = remap(pw, wfbm - 1, 1., 0., 1.);
    float WMc = max(1., saturate(.9 - .5) * 1. * 2.);
    float SN = saturate(
        remap(
            SNsample * SA,
            1 - .7 * WMc, 1.,
            0., 1.
        ) * DA
    ); // base density
    // float cloud = remap(pw, wfbm - 1, 1., 0., 1.);
    // float heightGradient = saturate(
    //     remap(heightPercent, 0., .2, 0., 1.)
    // ) * saturate(
    //     remap(heightPercent, 1, .7, 0, 1)
    // );
    // cloud = cloud * heightGradient;
    // cloud = cloud + 5 * .1;
    float baseDensity = SN;
    
    vec4 D = texture(detailTexture, sampleT);
    float Dfbm = D.r * .625 + D.g * .25 + D.b * .125;
    float Dmod = .35 * exp(-.7 * .75) * lerp(Dfbm, 1- Dfbm, saturate(heightPercent * 5));
    float Snd = saturate(remap(
        SNsample * SA, 1-.7 * WMc, 1., 
        0., 1.
    ));

    float d = saturate(
        remap(
            Snd, Dmod, 1., 0., 1
        ) * DA
    );

    return max(0., d-densityThreshold) * densityMultiplier;
    // return max(0., SN- densityThreshold);
    // cloud = remap(cloud, .85, 1., 0., 1.);
    // return max(0., cloud-densityThreshold);
}

float getCloudDensity3(vec3 p){
    vec3 sampleT = ((p - bounding_rect.pos + offSet/5.0)/ (bounding_rect.dims) + 0.5f) * scale;
    vec4 T = texture(texture_clouds, sampleT);
    float pw = T.r;
    float wfbm = T.g * 0.625 + T.b * .25 + T.a * .125;

    float heightPercent = (p.y - bounding_rect.pos.y)/bounding_rect.dims.y;
    float cloud = remap(pw, wfbm - 1, 1., 0., 1.);
    float heightGradient = saturate(
        remap(heightPercent, 0., .2, 0., 1.)
    ) * saturate(
        remap(heightPercent, 1, .7, 0, 1)
    );
    cloud = cloud * heightGradient;
    float bD = cloud + .01;

    if (bD > 0){
        vec4 D = texture(detailTexture, sampleT);
        float Dfbm = D.r * .625 + D.g * .25 + D.b * .125;
        float detailErodeWeight = (1 - cloud)*(1 - cloud)*(1-cloud);
        float cD = bD - (1-Dfbm)*detailErodeWeight * .5;
        cD = cD * densityMultiplier * .1;
        return max(0., cD - densityThreshold);
    }

    // cloud = max(0., cloud-densityThreshold);
    return 0;
}



float lightMarch(vec3 p){
    Ray r;
    r.pos = p;
    r.dir = sunDirection;
    _box_intersect_info info = rayIntersectsBox(r, bounding_rect);

    if (info.intersect){
        float distInsideBox = info.distInsideBox;

        float stepSize = distInsideBox / 6.;
        float totalDensity = 0;
        for (int step = 0; step < 6; ++step){
            p += r.dir * stepSize;
            totalDensity += getCloudDensity2(p)*stepSize;
        }
        float trans = exp(-totalDensity * .5);
        // return trans;
        return .7 + trans * (1- .7);
    }
    return 0.0;
}

float PI = 3.14159265359;

float HenyeyGreenstein(float g, float mu) {
  float gg = g * g;
	return (1.0 / (4.0 * PI))  * ((1.0 - gg) / pow(1.0 + gg - 2.0 * g * mu, 1.5));
}

void main(){
    Ray r;
    r.pos = camPos;
    r.dir = gRayDir();
    // Sphere sp;
    // sp.pos = vec3(0.0f);
    // sp.col = vec3(0.0f);
    // Rectange box;
    // box.pos = vec3(0.0f);
    // box.dims = vec3(4.0f, 1.0f, 1.5f);
    // // sp.radius = 0.5f;

    vec3 color;
    vec3 sunColor = vec3(1.0,0.7,0.1);
    float sun = clamp(dot(sunDirection, r.dir), 0.0, 1.0 );
    // Base sky color
    color = vec3(0.7,0.7,0.90);
    // Add vertical gradient
    // color -= 0.8 * vec3(0.90,0.75,0.90) * r.dir.y;
    // Add sun color to sky
    color += 0.5 * sunColor * pow(sun, 100.0);

    
    _box_intersect_info info = rayIntersectsBox(r, bounding_rect);
    vec4 col = texture(texture_diffuse1, tCoord);
    float depth = texture(depthTexture, tCoord).r;

    if (depth == 1.0){
        col = vec4(color, 1.0);
    }

    if (info.intersect && info.distToBox < linearizeDepth(depth)){
        // vec3 p = r.pos + r.dir * info.distToBox;
        // p = (p - bounding_rect.pos) / (bounding_rect.dims/2.0) + 0.5;
        // p *= 2.0;
        // fcol = vec4(vec3(texture(texture_clouds, p).r), 1.0f);
        fcol = vec4(vec3(0.0), 1.0);


        vec3 p = r.pos + r.dir * info.distToBox; //intersection point
        float t = 0.0;
        float totalDensity = 0.0;
        vec4 res = vec4(0.0);
        vec3 sampleT = ((p - bounding_rect.pos)/ (bounding_rect.dims) + 0.5f);

        // float c = texture(weather_data, sampleT.xy).r;
        // fcol = vec4(vec3(c), 1.0);

        // float cloud = getCloudDensity(p);

        // float v = texture(texture_clouds, sampleT).r;
        // fcol = vec4(vec3(cloud), 1.0);
        float trans = 1.;
        // vec3 lightEnergy = vec3(0.);
        float lightEnergy = 0.;
        float phase = HenyeyGreenstein(.3, dot(r.dir, sunDirection));
        int numSteps = 300;
        float stepSize = info.distInsideBox / float(numSteps);
        vec4 fincol = vec4(0.);
        vec3 fincolor = vec3(0.);
        for (int i =0; i < numSteps; i++){
            // p = r.pos + r.dir * (info.distToBox + t);
            // float d = nDensity(p);
            // if (d > 0) {
            //     float diffuse = clamp(
            //         (d - nDensity(p + 0.3 * sunDirection))/0.3, 0.0, 1.0
            //     );
            //     vec3 lin = vec3(0.60,0.60,0.75) * 1.1 + 0.8 * vec3(1.0,0.6,0.3) * diffuse;
            //     vec4 color = vec4(mix(vec3(1.0), vec3(0.0), d), d );
            //     color.rgb *= lin;
            //     color.rgb *= color.a;
            //     res += color * (1.0-res.a);
            //     // res = vec4(vec3(1.0), 0.5);
            // }

            p = r.pos + r.dir * (info.distToBox + stepSize*i);
            vec3 sampleT = ((p - bounding_rect.pos + offSet)/ (bounding_rect.dims) + 0.5f) * scale;
            // sampleT = clamp(sampleT, 0.0, 1.0);
            // float density = texture(texture_clouds, sampleT).r;
            // float density = getDensity(sampleT);
            // float d = density(p, bounding_rect, scale, offSet);
            float d = getCloudDensity2(p);
            if (d > 0){
                //do the light calc
                float lightT = lightMarch(p);
                lightEnergy += d * stepSize * trans * lightT * phase;
                trans *= exp(-d * stepSize * .5);
                if (trans < .01)
                    break;
                // fincolor += .5 * vec3(.8) * d * lightT * trans * stepSize;
            }
            // if (d > 0){
            //     float diffuse = clamp(d - getCloudDensity2(p + .3 * sunDirection)/.3, .0, 1.);
            //     // float diffuse = 1.;
            //     vec3 lin = vec3(.6, .6, .75) * 1.1 + .8 * vec3(1., .6, .3) * diffuse;
            //     vec4 mcolor = vec4(mix(vec3(1.0), vec3(.0), d), d);
            //     mcolor.rgb *= lin;
            //     mcolor.rgb *= mcolor.a;
            //     fincol += mcolor * (1.-fincol.a);
            // }
            // d = max(0.0, d - densityThreshold) * densityMultiplier;
            totalDensity += d * 0.01;
            // t += 0.01;
        }
        // color = color.rgb * (1.0 - res.a) + res.rgb;
        // fcol = vec4(color, 1.0);
        // fcol = col * exp(-totalDensity);
        vec3 cloudCol = lightEnergy * vec3(1.0);
        vec3 nC = col.xyz * trans + cloudCol;
        // fcol = vec4(nC, 1.);
        fcol = vec4(col.xyz + lightEnergy * sunColor + vec3(.1), 1.);
        // fcol = trans * col + vec4(fincolor, 1.);
        // color = col.rgb * (1.-fincol.a) + fincol.rgb*1.2;
        // fcol = vec4(color, 1.);
    }
    else{
        fcol = col;
    }
    // fcol = vec4(0.0f, 0.0f, 0.0f, 1.0f);
}