#version 430

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;
layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform sampler2D texture_diffuse1;
uniform sampler2D depthTexture;
uniform sampler2D weather_data;
uniform sampler3D texture_clouds;
uniform sampler3D detailTexture;

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
float densityMultiplier     = 5.0; 
// float scale                 = 0.1;

uniform float densityThreshold;
uniform float scale;

uniform vec3 offSet;

vec3 sunDirection = normalize(vec3(0.0f, 1.0f, -2.0f));
// vec3 sunDirection = normalize(
//     vec3(
//     cos(offSet.z),
//     sin(offSet.z),
//     -1.0
//     )
// );

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
uniform float weatherScale = 1.0;
uniform float higherScale = 1.0f;

float getCloudDensity2(vec3 p, vec3 boundsMin, vec3 boundsMax){
    vec3 sampleT = ((p - bounding_rect.pos + offSet * 10.0f)/ (bounding_rect.dims) + 0.5f) * scale;
    float heightPercent = (p.y - boundsMin.y)/(boundsMax.y - boundsMin.y);
    // sampleT += offSet/100.0f;
    float globalCoverage = 0.75f;
    vec4 T = texture(texture_clouds, sampleT);
    float pw = T.r;
    float wfbm = T.g * 0.625 + T.b * .25 + T.a * .125;

    vec4 weather = texture(weather_data, (p.xz + offSet.xz * 10.0f) * weatherScale);
    float WMc = max(weather.r, saturate(globalCoverage-.5) * weather.g * 2.);

    // float heightPercent = (p.y - (bounding_rect.pos.y-bounding_rect.dims.y/2))/bounding_rect.dims.y;
    float SRb = saturate(remap(heightPercent, 0., 0.07, 0., 1.));

    float SRt = saturate(remap(heightPercent, .2, 1., 1., 0.));
    float SA = SRb * SRt; // heightGradient

    float DRb = heightPercent * saturate(remap(heightPercent, 0., 0.15, 0., 1.));
    float DRt = saturate(remap(heightPercent, 0.9, 1.0, 1., 0));
    float DA = 0.5 * DRb * DRt * 0.5 * 2; // density

    float SNsample = remap(pw, wfbm - 1, 1., 0., 1.);
    // float WMc = max(1., saturate(.9 - .5) * 1. * 2.);
    float SN = saturate(
        remap(
            SNsample * SA,
            1 - globalCoverage * WMc, 1.,
            0., 1.
        ) * DA
    ); // base density
    // float cloud = remap(pw, wfbm - 1, 1., 0., 1.);
    // float heightGradient = saturate(
    //     remap(heightPercent, 0., .2, 0., 1.)
    // ) * saturate(
    //     remap(heightPercent, 1, .8, 0, 1)
    // );
    // cloud = cloud * heightGradient;
    // cloud = cloud + 5 * .1;
    float baseDensity = SN;
    
    sampleT = ((p - bounding_rect.pos)/ (bounding_rect.dims) + 0.5f)*higherScale;
    vec4 D = textureLod(detailTexture, sampleT, 0);
    float Dfbm = D.r * .625 + D.g * .25 + D.b * .125;
    float Dmod = .35 * exp(-globalCoverage * .75) * lerp(Dfbm, 1- Dfbm, saturate(heightPercent * 5));
    float Snd = saturate(remap(
        SNsample * SA, 1-globalCoverage * WMc, 1., 
        0., 1.
    ));

    // Snd= max(0., Snd- densityThreshold);

    float d = saturate(
        remap(
            Snd, Dmod, 1., 0., 1
        ) * DA
    );

    // return max(0., d-densityThreshold) * densityMultiplier;
    return clamp(d, 0.0, 1.0)*2.0;
    // return clamp(d, 0.0, 1.0);
    // return max(0., SN- densityThreshold);
    // cloud = remap(cloud, .85, 1., 0., 1.);
    // return max(0., cloud-densityThreshold) * densityMultiplier;
}

float getCloudDensity3(vec3 p, vec3 boundsMin, vec3 boundsMax){
    vec3 sampleT = ((p - bounding_rect.pos + offSet/5.0)/ (bounding_rect.dims) + 0.5f) * scale;
    vec4 T = texture(texture_clouds, sampleT);
    float pw = T.r;
    float wfbm = T.g * 0.625 + T.b * .25 + T.a * .125;

    // float heightPercent = (p.y - bounding_rect.pos.y)/bounding_rect.dims.y;
    float heightPercent = (p.y-boundsMin.y)/(boundsMax.y-boundsMin.y);
    float cloud = remap(pw, wfbm - 1, 1., 0., 1.);
    float heightGradient = saturate(
        remap(heightPercent, 0., .2, 0., 1.)
    ) * saturate(
        remap(heightPercent, 1, .7, 0, 1)
    );
    cloud = cloud * heightGradient;
    float bD = cloud + .01;

    if (bD > 0){
        vec3 sampleT = ((p - bounding_rect.pos + offSet/5.0)/ (bounding_rect.dims) + 0.5f) * higherScale;
        vec4 D = texture(detailTexture, sampleT);
        float Dfbm = D.r * .625 + D.g * .25 + D.b * .125;
        Dfbm = (sin(offSet.z) + 1.)/2.0;
        // Dfbm = 0.;
        float detailErodeWeight = (1 - cloud)*(1 - cloud)*(1-cloud);
        float cD = bD - (1-Dfbm)*detailErodeWeight * .5;
        cD = cD * densityMultiplier * .1;
        return max(0., cD - densityThreshold);
    }

    // cloud = max(0., cloud-densityThreshold);
    return 0;
}

vec3 getWeatherData(vec3 p){
    vec2 uv = (((p - bounding_rect.pos)/(bounding_rect.dims) + 0.5f) * weatherScale).xz;
    return texture(weather_data, uv).rgb;
}

#define STRATUS_GRADIENT vec4(0.0, 0.1, 0.2, 0.3)
#define STRATOCUMULUS_GRADIENT vec4(0.02, 0.2, 0.48, 0.625)
#define CUMULUS_GRADIENT vec4(0.00, 0.1625, 0.88, 0.98)


float getDensityForCloud(float heightFraction, float cloudType){
	float stratusFactor = 1.0 - clamp(cloudType * 2.0, 0.0, 1.0);
	float stratoCumulusFactor = 1.0 - abs(cloudType - 0.5) * 2.0;
	float cumulusFactor = clamp(cloudType - 0.5, 0.0, 1.0) * 2.0;

	vec4 baseGradient = stratusFactor * STRATUS_GRADIENT + stratoCumulusFactor * STRATOCUMULUS_GRADIENT + cumulusFactor * CUMULUS_GRADIENT;

	// gradicent computation (see Siggraph 2017 Nubis-Decima talk)
	float result = remap(heightFraction, baseGradient.x, baseGradient.y, 0.0, 1.0) * remap(heightFraction, baseGradient.z, baseGradient.w, 1.0, 0.0);
	return result;
}

uniform float coverageMultiplier = 1.f;

float getCloudDensity4(vec3 p, vec3 boundsMin, vec3 boundsMax, bool highQuality){
    p += offSet*10.0;
    vec3 sampleT = ((p - bounding_rect.pos)/ (bounding_rect.dims) + 0.5f) * scale;
    // sampleT += offSet/50.0f;
    float heighFraction = (p.y - boundsMin.y)/(boundsMax.y - boundsMin.y);
    vec4 T = texture(texture_clouds, sampleT);
    float pw = T.r;
    float wfbm = T.g * .625 + T.b * .25 + T.a * .125;
    float baseCloud = remap(pw, wfbm - 1, 1., 0., 1.);
    // float heightGradient = saturate(
    //     remap(heighFraction, 0., .2, 0., 1.)
    // ) * saturate(
    //     remap(heighFraction, 1, .7, 0, 1)
    // );
    // baseCloud *= heightGradient;
    baseCloud = max(baseCloud-densityThreshold, 0.);
    vec3 weatherData = getWeatherData(p);
    float densityGrad = getDensityForCloud(heighFraction, weatherData.g );
    baseCloud *= densityGrad;

    float coverage = clamp(weatherData.r, 0., 1.) * coverageMultiplier;
    float coverageCloud = remap(baseCloud, coverage, 1., 0., 1.);
    coverageCloud *= coverage;
    // coverageCloud *= mix(1., .0, clamp(heighFraction/coverage, 0., 1.));
    
    float finalCloud = coverageCloud;
    if (baseCloud > 0 && highQuality){
        sampleT = ((p - bounding_rect.pos)/ (bounding_rect.dims) + 0.5f) * higherScale;
        vec3 highFT = texture(detailTexture, sampleT).rgb;
        float highFBM = highFT.r * .625 + highFT.g * .25 + highFT.b * .125;
        // highFBM = (sin(offSet.z) + 1.)/2.0;
        float highModifier = mix(highFBM, 1.0-highFBM, saturate(heighFraction * 10.0));
        finalCloud = remap(finalCloud, highModifier*0.2, 1.0, 0.0, 1.0);
    }
    return clamp(finalCloud, 0., 1.);
}



// const float SIGMA = 0.9f;

uniform float SIGMA = 0.9f;

float beers(float dist, float abs){
    return exp(-dist * abs);
}

float lightMarch(vec3 p, vec3 boundsMin, vec3 boundsMax){
    Ray r;
    r.pos = p;
    r.dir = sunDirection;
    _box_intersect_info info = rayIntersectsBox(r, bounding_rect);

    // if (info.intersect){
    float distInsideBox = info.distInsideBox;

    float stepSize = distInsideBox / 6.;
    stepSize = 0.03;
    float totalDensity = 0;
    for (int step = 0; step < 6; ++step){
        totalDensity += max(0, getCloudDensity4(p, boundsMin, boundsMax, true)*stepSize);
        p += r.dir * stepSize * (step + 1);
    }
    // float trans = exp(-totalDensity * 1.21);
    float trans = beers(totalDensity, SIGMA);
    return trans;
    // return .15 + trans * (1- .15);
    // }
    return 0.0;
}

float PI = 3.14159265359;

float HenyeyGreenstein(float g, float mu) {
  float gg = g * g;
	return (1.0 / (4.0 * PI))  * ((1.0 - gg) / pow(1.0 + gg - 2.0 * g * mu, 1.5));
}


vec3 sunColor = vec3(1.0, 0.95686275, 0.839215);

uniform float HG = .3;

vec3 rayMarch(Ray r, vec3 backgroundColor, vec3 skyColBase, float depth){
    vec3 boundsMax = bounding_rect.pos + bounding_rect.dims/2;
    vec3 boundsMin = bounding_rect.pos - bounding_rect.dims/2;
    const int MAX_STEPS = 128;
    float totalDensity = 0.;
    float trans = 1.;
    float lightEnergy = 0.0;
    float phase = HenyeyGreenstein(HG, dot(r.dir, sunDirection));

    _box_intersect_info info = rayIntersectsBox(r, bounding_rect);
    depth = linearizeDepth(depth);
    float stepSize = info.distInsideBox / MAX_STEPS;
    vec3 p = r.pos + r.dir * info.distToBox;
    vec3 col = vec3(0.0);

    if (info.intersect && info.distToBox < depth){
        for (int i =0; i < MAX_STEPS; ++i){
            float density = getCloudDensity4(p, boundsMin, boundsMax, true);
            if (density > 0){
                // float diffuse = clamp(
                //     (density - getCloudDensity4(p + .3 * sunDirection, boundsMin, boundsMax))/.3,
                //      0.0, 1.0
                //      );
                // vec3 lin = vec3(0.60,0.60,0.75) * 1.1 + 0.8 * vec3(1.0,0.6,0.3) * diffuse;
                // vec4 color = vec4(mix(vec3(1.0), vec3(0.0), density), density );
                // color.rgb *= lin;
                // color.rgb *= color.a;
                // col += color * (1.0-col.a);
                float lightT = lightMarch(p, boundsMin, boundsMax);
                // float lum = density * phase;
                // trans *= lightT;
                // lightEnergy += trans * lum;
                lightEnergy += density * stepSize * trans * lightT * phase;
                trans *= exp(-density * stepSize * SIGMA);
                if (trans < .01)
                    break;
            }
            totalDensity += density * stepSize;
            p += r.dir * stepSize;
        }

        // return lightEnergy;
        // return backgroundColor * exp(-totalDensity);
        // return backgroundColor.xyz * trans + lightEnergy*sunColor;
        // return backgroundColor.rgb * (1-col.a) + col.rgb;
    }

    // float dstFog = 1-exp(-max(0, depth) * 8 * .0001);
    // vec3 sky = dstFog * skyColBase;
    // backgroundColor = backgroundColor;// * (1-dstFog) + sky;

    // float focusedEyeCos = pow(saturate(dot(r.dir, sunDirection)), 1);
    // float sun =saturate(
    //     HenyeyGreenstein(.9995, focusedEyeCos) * trans
    // );

    // vec3 cloudCol = lightEnergy * sunColor;
    // col = backgroundColor * trans + cloudCol;
    // col = clamp(col, 0.0, 1.0) * (1-sun) + sunColor * sun;

    col = backgroundColor + sunColor * lightEnergy;

    return col;
}


void main(){
    ivec2 fragCoord = ivec2(gl_GlobalInvocationID.xy);
    vec2 tCoord = fragCoord/vec2(800., 600.);
    vec2 nds = 2.0*(fragCoord/vec2(800., 600.))-1.0;
    vec4 nds_4d = vec4(nds, 0.0, 1.0);
    vec4 dir = invProjMat * nds_4d;
    vec3 rayDir = (invViewMat * vec4(dir.x, dir.y, -1.0, 0.0)).xyz;
    rayDir = normalize(rayDir.xyz);

    Ray r;
    r.pos = camPos;
    r.dir = rayDir;


    vec3 color;
    vec3 sunColor = vec3(1.0,0.5,0.3);
    float sun = clamp(dot(sunDirection, r.dir), 0.0, 1.0 );
    // Base sky color
    color = vec3(0.7,0.7,0.90);
    // Add vertical gradient
    // color -= 0.8 * vec3(0.90,0.75,0.90) * r.dir.y;
    // Add sun color to sky
    color += 0.5 * sunColor * pow(sun, 100.0);

    
    // _box_intersect_info info = rayIntersectsBox(r, bounding_rect);
    vec3 col = texture(texture_diffuse1, tCoord).rgb;
    float depth = texture(depthTexture, tCoord).r;

    if (depth == 1.0){
        // col = vec4(color, 1.0);
        // col = color;
    }

    // vec3 res = rayMarch(r, col.xyz, color, depth);
    vec3 res = rayMarch(r, color, color, depth);
    // fcol = vec4(res, 1.0);
    imageStore(imgOutput, fragCoord, vec4(res, 1.0));
}