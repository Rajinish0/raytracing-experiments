#include <fstream>
#include "hittable.h"
#include "material.h"
#include <thread> 
#include <mutex>
#include <string>
class Camera{
public:
	double aspect_ratio = 16.0/9.0;
	int	image_width = 400;
	int samples_per_pixel = 10;
	int max_depth = 10;
	color background = color(.70, .80, 1.);

	double vfov = pi / 2.0;
	point3 lookfrom = point3(0.0);
	point3 lookat 	= point3(0, 0, -1.0);
	point3 vup 		= vec3(0, 1, 0);

	double defocus_angle = 0; //this is the angle of the cone centered at center of the viewport (or its apex is at that cente)
	double focus_dist = 10.0;

	std::mutex console_mutex;

	void job(int thread_id, int startHeight, int endHeight, std::vector<color>& v, const hittable& world){
		auto print_fn = [thread_id](std::mutex& mtx, std::string str){
			std::lock_guard<std::mutex> lock(mtx);
			std::cout << "\033[" << thread_id + 1 << ";0H";
			std::cout << str << std::endl;
		};

		for (int i = startHeight; i < endHeight; i++){
			for (int j =0; j < image_width; j++){
				color pixel_color(0);
				for (int sample =0; sample < samples_per_pixel; sample++){
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, world, max_depth);
				}
				v[j + (i - startHeight)*image_width] = pixel_samples_scale * pixel_color;
				// v.push_back(pixel_samples_scale*pixel_color);
			}
			print_fn(console_mutex, "Thread " + std::to_string(thread_id) + " processing " + std::to_string(i));
		}

		print_fn(console_mutex, "Finished!!!!!!!!!!!!!!!!!!!!!!!!!!!");
	}

	void renderFast(const hittable& world){
		init();
		std::ofstream file {"out.ppm"};

		file << "P3\n";
		file << image_width << ' ' << image_height << "\n255\n";
		
		int numThreads = 5;
		int heightPerThread = image_height / numThreads;
		std::vector<std::vector<color>> v(numThreads);
		std::vector<std::thread> threads;

		std::cout << "\033[2J\033[1;1H" << std::flush; //clear the console move to 1, 1

		for (int i =0; i < numThreads; ++i){
			int start = i* heightPerThread;
			int end = (i != numThreads - 1) ? (i + 1)*heightPerThread : image_height;
			v[i].resize( (end-start)*image_width );
			threads.emplace_back([this, &world, &v, i, start, end](){
				job(i, start, end, v[i], world);	
			});
		}

		// int i = numThreads - 1;
		// threads.emplace_back([this, &world, &v, i, heightPerThread](){
		// 		job(i, i*heightPerThread, image_height, v[i], world);	
		// 	});

		for (auto& thread : threads)
			thread.join();

		std::cout << "\033[" << numThreads+1 << ";0H" << std::flush;

		std::cout << "All threads have completed. Writing colors to file" << std::endl;			
		for (auto& vec : v){
			for (auto& col : vec){
				write_color(file, col);
			}
		}
		
		std::cout << "Done.\n";

		file.close();
	}						
	

	void render(const hittable& world){
		init();
		std::ofstream file {"out.ppm"};


		file << "P3\n";
		file << image_width << " " << image_height << "\n";
		file << 255 << '\n';

		// auto q_du = pixel_du / 4.0; // q as in quarter..
		// auto q_dv = pixel_dv / 4.0; 

		for (int i =0 ; i < image_height; i++){
			std::clog << "\rRemaining: " << image_height-i << ' ' << std::flush;
			for (int j = 0; j < image_width; j++){
				//auto pixel_center = pixel00_loc+(pixel_du*j)+(pixel_dv*i);
				//FOR ANTI ALIASING
				/*
				auto ptl = pixel_center - q_du - q_dv;
				auto ptr = pixel_center + q_du - q_dv;
				auto pbl = pixel_center - q_du + q_dv;
			   	auto pbr = pixel_center + q_du + q_dv;

				//auto ray_direction = pixel_center - camera_center;
				//ray r(camera_center, ray_direction);
				//color pixel_color = ray_color(r, world);
				//
				ray r1(camera_center, ptl);
				ray r2(camera_center, ptr);
				ray r3(camera_center, pbl);
				ray r4(camera_center, pbr);

				color pixel_color = ( ray_color(r1, world) + ray_color(r2, world) + ray_color(r3, world) + ray_color(r4, world) ) / 4.0;
				auto pc = color( ((double) i) / (W - 1),
								 ((double) j) / (H - 1),
								 0.0 );
				*/

				color pixel_color(0);
				for (int sample =0; sample < samples_per_pixel; sample++){
					ray r = get_ray(i, j);
					pixel_color += ray_color(r, world, max_depth);
				}

				write_color(file, pixel_samples_scale*pixel_color);
				
			}
		}
		
		std::cout << "\rDone.                 \n";

		file.close();
		
	}
private:
	int image_height;
	point3 pixel00_loc, camera_center;
	vec3 pixel_du, pixel_dv;
	double pixel_samples_scale;
	vec3 u, w, v;
	vec3 defocus_disk_u, defocus_disk_v;

	void init(){

	image_height = (int)(image_width/aspect_ratio);
	image_height = (image_height < 1) ? 1 : image_height;

	pixel_samples_scale = 1.0/samples_per_pixel;


	/*
	 * focal length is the distance from the camera
	 * to that virtual plane (i can imagine it as basically the near plane)
	 *
	 * h = tan(theta/2) = (vheight/2)/flength
	 * vheight = 2 * flength * h;
	 * */
	camera_center = lookfrom;

//	auto focal_length = (lookfrom - lookat).length();
	auto h = std::tan(vfov / 2.0);
//	auto viewport_height = 2.0 * h * focal_length;
	auto viewport_height = 2.0 * h * focus_dist;
	auto viewport_width = viewport_height * (double(image_width)/image_height);
	//camera_center = point3(0, 0, 0);

	/*viewport can be imagined as a virtual grid in 3d
	 * through which the rays pass through, it needs to have
	 * the same aspect ratio but the height and the width can be varied
	 * */
	
	//using right hand coords, so w is going the other way
	//i dont like this, might change this later
	w = unit_vector(lookfrom - lookat);
	u = unit_vector(cross(vup, w));
	v = cross(w, u);

	//auto viewport_u = vec3(viewport_width, 0, 0);
	//auto viewport_v = vec3(0, -viewport_height, 0);
	
	/*it's easier to image a plane of size viewport_width
	 * and viewport_height centered at camera_center
	 * then we push it by focal length in the -w direction
	 * or in the actual lookat direction...
	 */
	auto viewport_u = viewport_width * u;
	auto viewport_v = viewport_height * -v;

	pixel_du = viewport_u / image_width;
	pixel_dv = viewport_v / image_height;
	
	//-viewport_v/2 actually moves it up by half the height
//	auto viewport_upper_left = camera_center - vec3(0, 0, focal_length) - viewport_u/2 - viewport_v/2;
	auto viewport_upper_left = camera_center - (focus_dist * w) - viewport_u/2 - viewport_v/2;
	pixel00_loc = viewport_upper_left + 0.5*(pixel_du + pixel_dv);

	//this is the radius of the circle around the camera center
	//so if i imagine the cone popping out from viewport
	//with angle defocus_angle then it has this relation with the radius
	//tan(theta/2) = defocus_rad/focus_dist
	auto defocus_rad = focus_dist * std::tan(defocus_angle);
	defocus_disk_u = u * defocus_rad;
	defocus_disk_v = v * defocus_rad;

	}
	
	color ray_color(const ray& r, const hittable& world, int depth){
		if (depth < 0)
			return color(0.0);
		hit_record rec;

		if (!world.hit(r, interval(0.001, infinity), rec))
			return background;

		// if (world.hit(r, interval(0.001, infinity), rec)){
		ray scattered;
		color atten;
		color color_from_emisson = rec.mat->emitted(rec.u, rec.v, rec.p);

		if (!rec.mat->scatter(r, rec, atten, scattered))
			return color_from_emisson;
			
		color color_from_scatter = atten * ray_color(scattered, world, depth-1);
		return color_from_emisson + color_from_scatter;
			// return color(0.0);

		// vec3 unit = unit_vector(r.direction());
		// auto a = 0.5*(unit.y() + 1.0); // 0.5*([0.0, 2.0]) = [0.0, 1.0]
	
		//lerp
		// return (1.0-a)*color(1.0) + a*color(0.5, 0.7, 1.0);

	}

	ray get_ray(int i, int j){
		auto offset = sample_square();
		auto pixel_sample = pixel00_loc 
					+ (j+offset.x())*(pixel_du) 
					+ (i+offset.y())*(pixel_dv);
		auto ray_orig = (defocus_angle <= 0) ? camera_center : defocus_disk_sample();
		auto ray_dir  = pixel_sample - ray_orig;
		double tm = random_double();

		return ray(ray_orig, ray_dir, tm);
	}

	point3 defocus_disk_sample() const{
		auto p = random_in_unit_disk();
		return camera_center + (defocus_disk_u * p[0]) + (defocus_disk_v * p[1]);
	}

	vec3 sample_square() const{
		//[-.5, .5] square
		return vec3(random_double() - 0.5, random_double()-0.5, 0);
	}
};
