#include "rtweekend.h"

#include "hittable.h"
#include "hittable_list.h"
#include "material.h"
#include "sphere.h"
#include "camera.h"

#include <fstream>



int main(){
	

	hittable_list world;
	
	auto mat_ground = make_shared<lambertian>(color(0.8, 0.8, 0.0));
	auto mat_center = make_shared<lambertian>(color(0.1, 0.2, 0.5));
	auto mat_left 	= make_shared<dielectric>(1.50);
	auto mat_bubble = make_shared<dielectric>(1.00/1.50);
	auto mat_right 	= make_shared<metal>(color(0.8, 0.6, 0.2), 1.0);

	//world.add(make_shared<sphere>(point3(0, 0, -1), 0.5));
	world.add(make_shared<sphere>(point3(0, -100.5, -1), 100, mat_ground));
	world.add(make_shared<sphere>(point3(0.0, 0.0, -1.2), 0.5, mat_center));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.5, mat_left));
	world.add(make_shared<sphere>(point3(-1.0, 0.0, -1.0), 0.4, mat_bubble));
	world.add(make_shared<sphere>(point3( 1.0, 0.0, -1.0), 0.5, mat_right));

	Camera cam;
	cam.samples_per_pixel = 100;
	cam.max_depth 		  = 50;
	cam.lookfrom 		  = point3(-2, 2, 1);
	cam.lookat 			  = point3(0, 0, -1);
	cam.vfov			  = deg_to_rad(20.0);
	cam.defocus_angle	  = deg_to_rad(10.0);
	cam.focus_dist 		  = 3.4;

	cam.render(world);


}

