#ifndef TEXTURE_H
#define TEXTURE_H

#include "rtweekend.h"
#include "color.h"
#include "rtw_stb_image.h"
#include "perlin.h"

class Texture {
public:
    virtual ~Texture() = default;
    virtual color value(double u, double v, const point3& p) const = 0;
};


class solid_color : public Texture {
public:
    solid_color(const color& albedo) 
        :albedo(albedo) {}

    solid_color(double r, double g, double b)
        :albedo(r, g, b) {}

    color value(double u, double v, const point3& p) const override {
        return albedo;
    }

private:
    color albedo;
};

class checker_texture : public Texture {
public:
    checker_texture(double scale, shared_ptr<Texture> even, shared_ptr<Texture> odd)
        :inv_scale(1./scale), even_texture(even), odd_texture(odd){}
    
    checker_texture(double scale, const color& c1, const color& c2)
        :inv_scale(1./scale), even_texture(make_shared<solid_color>(c1)), 
         odd_texture(make_shared<solid_color>(c2)) {}

    color value(double u, double v, const point3& p) const override 
    {
        auto xInt = int(std::floor(p.x() * inv_scale));
        auto yInt = int(std::floor(p.y() * inv_scale));
        auto zInt = int(std::floor(p.z() * inv_scale));

        bool isEven = (xInt + yInt + zInt) % 2 == 0;

        return (isEven) ? even_texture->value(u, v, p) : odd_texture->value(u, v, p);
    }
private:
    double inv_scale;
    shared_ptr<Texture> even_texture;
    shared_ptr<Texture> odd_texture; 

};


class image_texture : public Texture {
public:
    image_texture(const char *fname)
        :img(fname){}

    color value(double u, double v, const point3& p) const override 
    {
        if (img.height() <= 0) return color(0, 1, 1);

        u = interval(0, 1).clamp(u);
        v = 1. - interval(0, 1).clamp(v);

        int x = u * (img.width());
        int y = v * (img.height());

        const unsigned char * c = img.pixel_data(x, y);
        auto scale = 1./255.;
        return color(scale*c[0], scale*c[1], scale*c[2]);
    }

private:
    rtw_image img;
};

class noise_texture : public Texture {
public:
    noise_texture(double scale)
        :scale(scale){}

    color value(double, double, const point3& p) const override 
    {
        // return color(1.) * .5 * (1. + noise.noise(p * scale));
        // return color(1.) * noise.fbm(p, 7);
        return color(.5) * (1 + std::sin(scale * p.z() + 10 * noise.fbm(p, 7)));
    }

private:
    perlin noise;
    double scale;
};


#endif 