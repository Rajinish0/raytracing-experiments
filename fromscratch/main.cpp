#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"
#include "bvh.h"
#include "quad.h"
#include "circle.h"


#include <fstream>



void bouncing_spheres(){
	

	hittable_list world;
	// auto ground_material = make_shared<lambertian>(color(0.5));
	auto groundTexture = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9));
	auto ground_material = make_shared<lambertian>(groundTexture);
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, ground_material));

	for (int a = -11; a < 11; ++a){
		for (int b = -11; b < 11; ++b){
			auto choose_mat = random_double();
			point3 center(a + 0.9 * random_double(), 0.2, b + 0.9*random_double());
			if ((center - point3(4, 0.2, 0)).length() > 0.9){
				shared_ptr<material> sphere_mat;

				if (choose_mat < 0.8){
					auto whiteness = color::random() * color::random();
					sphere_mat = make_shared<lambertian>(whiteness);
					world.add(make_shared<sphere>(center, 0.2, sphere_mat));
				}

				else if (choose_mat < 0.95){
					auto whiteness = color::random(0.5, 1);
					auto fuzz = random_double(0, 0.5);
					sphere_mat = make_shared<metal>(whiteness, fuzz);
					world.add(make_shared<sphere>(center, 0.2, sphere_mat));
				}

				else {
					sphere_mat = make_shared<dielectric>(1.5);
					world.add(make_shared<sphere>(center, 0.2, sphere_mat));
				}
			}
		}
	}

	auto mat1 = make_shared<dielectric>(1.5);
	world.add(make_shared<sphere>(point3(0, 1, 0), 1.0, mat1));

	auto mat2 = make_shared<lambertian>(color(0.4, 0.2, 0.1));
	world.add(make_shared<sphere>(point3(-4, 1, 0), 1.0, mat2));

	auto mat3 = make_shared<metal>(color(0.7, 0.6, 0.5), 0.0);
	world.add(make_shared<sphere>(point3(4, 1, 0), 1.0, mat3));

	world = hittable_list(make_shared<bvh_node>(world));


	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;

	cam.lookfrom 		  = point3(13, 2, 3);
	cam.lookat 			  = point3(0, 0, 0);
	cam.vfov			  = deg_to_rad(20.0);

	cam.defocus_angle	  = deg_to_rad(0.6);
	cam.focus_dist 		  = 10.0;

	cam.renderFast(world);


}

void textured_spheres(){
	hittable_list world;

	// auto checker = make_shared<image_texture>("earthmap.jpg");
	auto checker = make_shared<checker_texture>(0.32, color(.2, .3, .1), color(.9));

	world.add(make_shared<sphere>(point3(0, -10, 0), 10, make_shared<lambertian>(checker)));
	world.add(make_shared<sphere>(point3(0,  10, 0), 10, make_shared<lambertian>(checker)));

	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;

	cam.lookfrom 		  = point3(13, 2, 3);
	cam.lookat 			  = point3(0, 0, 0);
	cam.vfov			  = deg_to_rad(20.0);

	cam.defocus_angle	  = deg_to_rad(0.6);
	cam.focus_dist 		  = 10.0;

	cam.renderFast(world);
}


void earth(){
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);
	auto globe = make_shared<sphere>(point3(0.), 2, earth_surface);

	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;

	cam.lookfrom 		  = point3(0, 0, 12);
	cam.lookat 			  = point3(0, 0, 0);
	cam.vfov			  = deg_to_rad(20.0);

	cam.defocus_angle	  = deg_to_rad(0.6);
	cam.focus_dist 		  = 10.0;

	cam.renderFast(hittable_list(globe));
}

void perling_spheres(){
	hittable_list world;

	auto perlin_texture = make_shared<noise_texture>(4.);
	world.add(make_shared<sphere>(point3(0., -1000., 0.), 1000., make_shared<lambertian>(perlin_texture)));
	world.add(make_shared<sphere>(point3(0., 2., 0.), 2., make_shared<lambertian>(perlin_texture)));

	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;

	cam.lookfrom 		  = point3(13, 2, 3);
	cam.lookat 			  = point3(0, 0, 0);
	cam.vfov			  = deg_to_rad(20.0);

	cam.defocus_angle	  = deg_to_rad(0.6);
	cam.focus_dist 		  = 10.0;

	cam.renderFast(world);
}

void quads(){
	hittable_list world;

	auto left_red 	  = make_shared<lambertian>(color(1.0, 0.2, 0.2));
	auto back_green   = make_shared<lambertian>(color(0.2, 1.0, 0.2));
	auto right_blue   = make_shared<lambertian>(color(0.2, 0.2, 1.0));
	auto upper_orange = make_shared<lambertian>(color(1.0, 0.5, 0.0));
	auto lower_teal   = make_shared<lambertian>(color(0.2, 0.8, 0.8));

	// world.add(
	// 	make_shared<quad>(point3(-3, -2, 5), vec3(0, 0., -4.), vec3(0., 4., 0.), left_red)
	// );
	// world.add(
	// 	make_shared<quad>(point3(-2, -2, 5), vec3(4., 0., 0), vec3(0., 4., 0.), back_green)
	// );
	// world.add(
	// 	make_shared<quad>(point3(-3, -2, 1), vec3(0, 0., 4.), vec3(0., 4., 0.), right_blue)
	// );
	// world.add(
	// 	make_shared<quad>(point3(-2, 3, 1), vec3(4., 0., 0), vec3(0., 0., 4.), upper_orange)
	// );
	// world.add(
	// 	make_shared<quad>(point3(-2, -3, 5), vec3(4., 0., 0), vec3(0., 0., -4), lower_teal)
	// );

    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), left_red));
    world.add(make_shared<quad>(point3(-2,-2, 0), vec3(4, 0, 0), vec3(0, 4, 0), back_green));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), right_blue));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), upper_orange));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), lower_teal));

	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;

	cam.lookfrom 		  = point3(0, 0, 9);
	cam.lookat 			  = point3(0, 0, 0);
	// cam.vfov			  = deg_to_rad(20.0);
	cam.vfov 			  = deg_to_rad(80.0);

	cam.defocus_angle	  = deg_to_rad(0.);
	cam.focus_dist 		  = 10.0;



	cam.renderFast(world);
}


void quads_with_texture(){
	hittable_list world;
	auto earth_texture = make_shared<image_texture>("earthmap.jpg");
	auto earth_surface = make_shared<lambertian>(earth_texture);

	// auto left_red 	  = make_shared<lambertian>(earth_surface);
	// auto back_green   = make_shared<lambertian>(earth_surface);
	// auto right_blue   = make_shared<lambertian>(earth_surface);
	// auto upper_orange = make_shared<lambertian>(earth_surface);
	// auto lower_teal   = make_shared<lambertian>(earth_surface);


    world.add(make_shared<quad>(point3(-3,-2, 5), vec3(0, 0,-4), vec3(0, 4, 0), earth_surface));
    world.add(make_shared<circle>(point3(0, 0, 0),vec3(4, 0, 0), vec3(0, 1, 0), earth_surface));
    world.add(make_shared<quad>(point3( 3,-2, 1), vec3(0, 0, 4), vec3(0, 4, 0), earth_surface));
    world.add(make_shared<quad>(point3(-2, 3, 1), vec3(4, 0, 0), vec3(0, 0, 4), earth_surface));
    world.add(make_shared<quad>(point3(-2,-3, 5), vec3(4, 0, 0), vec3(0, 0,-4), earth_surface));

	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;

	cam.lookfrom 		  = point3(0, 0, 9);
	cam.lookat 			  = point3(0, 0, 0);
	// cam.vfov			  = deg_to_rad(20.0);
	cam.vfov 			  = deg_to_rad(80.0);

	cam.defocus_angle	  = deg_to_rad(0.);
	cam.focus_dist 		  = 10.0;



	cam.renderFast(world);
}

void simple_light(){
	hittable_list world;

	auto pertext = make_shared<noise_texture>(4);
	world.add(make_shared<sphere>(point3(0, -1000, 0), 1000, make_shared<lambertian>(pertext)));
	world.add(make_shared<sphere>(point3(0, 2, 0), 2, make_shared<lambertian>(pertext)));

	auto difflight = make_shared<diffuse_light>(color(4.));
	world.add(make_shared<sphere>(point3(0,7,0), 2, difflight));
	world.add(make_shared<quad>(point3(3, 1, -2), vec3(2,0., 0.), vec3(0., 2., 0.), difflight));

	Camera cam;
	cam.aspect_ratio 	  = 16.0 / 9.0;
	cam.image_width 	  = 400;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;
	cam.background 		  = color(0.);

	cam.lookfrom 		  = point3(26, 3, 6);
	cam.lookat 			  = point3(0, 0, 0);
	cam.vfov			  = deg_to_rad(20.0);

	cam.defocus_angle	  = 0.;
	// cam.focus_dist 		  = 10.0;



	cam.renderFast(world);

}


void cornell_box() {
    hittable_list world;

    auto red   = make_shared<lambertian>(color(.65, .05, .05));
    auto white = make_shared<lambertian>(color(.73, .73, .73));
    auto green = make_shared<lambertian>(color(.12, .45, .15));
    auto light = make_shared<diffuse_light>(color(15, 15, 15));

    world.add(make_shared<quad>(point3(555,0,0), vec3(0,555,0), vec3(0,0,555), green));
    world.add(make_shared<quad>(point3(0,0,0), vec3(0,555,0), vec3(0,0,555), red));
    world.add(make_shared<quad>(point3(343, 554, 332), vec3(-130,0,0), vec3(0,0,-105), light));
    world.add(make_shared<quad>(point3(0,0,0), vec3(555,0,0), vec3(0,0,555), white));
    world.add(make_shared<quad>(point3(555,555,555), vec3(-555,0,0), vec3(0,0,-555), white));
    world.add(make_shared<quad>(point3(0,0,555), vec3(555,0,0), vec3(0,555,0), white));

    Camera cam;

    cam.aspect_ratio      = 1.0;
    cam.image_width       = 600;
    cam.samples_per_pixel = 200;
    cam.max_depth         = 50;
    cam.background        = color(0);

    cam.vfov     = deg_to_rad(40.);
    cam.lookfrom = point3(278, 278, -800);
    cam.lookat   = point3(278, 278, 0);
    cam.vup      = vec3(0,1,0);

    cam.defocus_angle = 0;

    cam.renderFast(world);
}



int main(){
	switch (8){
		case 1: bouncing_spheres(); break;
		case 2: textured_spheres(); break;
		case 3: earth(); break;
		case 4: perling_spheres(); break;
		case 5: quads(); break;
		case 6: quads_with_texture(); break;
		case 7: simple_light(); break;
		case 8: cornell_box(); break;
	}
}