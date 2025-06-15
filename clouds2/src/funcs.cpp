#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "funcs.h" 
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <random>
#include <string>
#include <cmath>
#include <fstream>
#include <ostream>
#include <openssl/sha.h>



namespace funcs{
	/*
	ALWAYS MAKE SURE THIS FUNCTION IS CALLED AFTER THE OPENGL CONTEXT IS LOADED
	THIS glGenTextures goes straight to hell if the context is not initialized
	and is very hard to debug
	*/

	unsigned int TextureFromFile(const std::string & path, std::string directory) {
		int width, height, nChannels;
		// std::cout << directory << " " << path << std::endl;
		unsigned char* data = stbi_load((directory + "/" + path).c_str(), &width, &height, &nChannels, 0);

		// std::cout << (directory + "/" + path).c_str() << std::endl;
		unsigned int id = 1;
		glGenTextures(1, &id);


		// std::cout << "HERE NOW " << std::endl;

		if (data) {
			GLenum format;
			if (nChannels == 1)
				format = GL_RED;
			else if (nChannels == 3)
				format = GL_RGB;
			else if (nChannels == 4)
				format = GL_RGBA;

			glBindTexture(GL_TEXTURE_2D, id);
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);

			stbi_image_free(data);
		}
		else
			std::cout << "Couldn't load texture";

		return id;
	}

	unsigned int loadCubeMap(std::vector<std::string> faces) {
		/*
		* faces should be in this order
		* RIGHT, LEFT, TOP, BOTTOM, FRONT, BACK
		*/


		int width, height, nChannels;
		unsigned int id = 1;
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_CUBE_MAP, id);
		for (unsigned int i = 0; i < faces.size(); ++i) {
			unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nChannels, 0);
			if (data) {
				glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
					0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
			}
			else {
				std::cerr << "Failed to load texture: " << faces[i] << std::endl;
			}

			stbi_image_free(data);
		}

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
		return id;
	}

	Mesh genSphere() {
		std::vector<Vertex> vertices; 
		std::vector<Texture> textures;
		std::vector<unsigned int> indices;
		std::cout << "started" << std::endl;
		constexpr int NUM_POINTS = 100;
		constexpr float PI = glm::pi<float>();
		constexpr float PI_HALF = glm::half_pi<float>();
		constexpr float dtheta = 2 * PI / (float)(NUM_POINTS-1);
		constexpr float dphi = PI / (float)(NUM_POINTS - 1);

		std::cout << dtheta << std::endl;
		std::cout << dphi << std::endl;

		Vertex vertex;
		vertex.position = glm::vec3(
			0,
			glm::sin(-PI_HALF),
			0
		);

		vertex.normal = glm::vec3(
			0, 
			glm::sin(-PI_HALF),
			0
		);

		vertex.texCoord = glm::vec2(0.5, 0);

		vertices.push_back(vertex);

		std::cout << "Pi/2: " << PI_HALF << std::endl;
		int c1 = 0, c2 = 0;
		//for (float phi = -PI_HALF; phi < PI_HALF; phi += dphi){
		float phi;
		float theta;
		for (int i =0; i < NUM_POINTS; ++i){
			phi = -PI_HALF + dphi * i;
			//std::cout << phi << std::endl;
			//std::cout << phi + dphi <<" " << ( abs( (phi + dphi) - PI_HALF ) < 1e-6)<< std::endl;
			c1++;
			c2 = 0;
			//for (float theta = 0; theta < 2 * PI; theta += dtheta) {
			for (int j = 0; j < NUM_POINTS; ++j) {
				theta = dtheta * j;
				//std::cout << "THETA : " << theta << std::endl;
				c2++;
				Vertex v;
				glm::vec3 p(
					glm::cos(theta) * glm::cos(phi),
					glm::sin(phi),
					glm::sin(theta) * glm::cos(phi)
				);
				v.position = p;
				v.normal = p;


				v.texCoord = glm::vec2(
					theta / (2 * PI),
					(phi + PI_HALF) / (PI));
				vertices.push_back(v);
			}
		}

		Vertex v2;
		v2.position = glm::vec3(
			0,
			glm::sin(PI_HALF),
			0
		);

		v2.normal = glm::vec3(
			0,
			glm::sin(PI_HALF),
			0
		);

		v2.texCoord = glm::vec2(0.5, 1);
		vertices.push_back(v2);

		for (unsigned int j = 0; j < NUM_POINTS; ++j) {
			indices.push_back(0);
			indices.push_back(j + 1);
			indices.push_back((j + 1 + 1) % NUM_POINTS);
		}

		for (unsigned int i = 0; i < NUM_POINTS - 1; ++i) {
			for (unsigned int j = 0; j < NUM_POINTS; ++j) {
				indices.push_back( (j % NUM_POINTS) + (i % NUM_POINTS) * NUM_POINTS  + 1) ;
				indices.push_back( ( (j + 1) % NUM_POINTS) + (i % NUM_POINTS) * NUM_POINTS + 1);
				indices.push_back((j % NUM_POINTS) + ((i + 1)% NUM_POINTS) * NUM_POINTS + 1);

				indices.push_back((j % NUM_POINTS) + ((i + 1) % NUM_POINTS) * NUM_POINTS + 1);
				indices.push_back(((j + 1) % NUM_POINTS) + (i % NUM_POINTS) * NUM_POINTS + 1);
				indices.push_back(((j + 1) % NUM_POINTS) + ((i+1) % NUM_POINTS) * NUM_POINTS + 1);
			}
		}


		for (unsigned int j = 0; j < NUM_POINTS; ++j) {
			indices.push_back(vertices.size() - 1);
			indices.push_back(j + (NUM_POINTS - 1) * (NUM_POINTS) + 1);
			indices.push_back((j + 1) % NUM_POINTS + (NUM_POINTS - 1) * (NUM_POINTS)+1);
		}

		std::cout << "generating the texture" << std::endl;
		// Texture tx{ "D:/earth2.png", "texture_diffuse" };
		Texture tx{
			"earth2.png",
			"D:",
			"texture_diffuse"
		};

		//tx.id = TextureFromFile("earth2.png", "D:");
		//tx.path = "earth2.png";
		//tx.type = "texture_diffuse";

		textures.push_back(tx);

		std::cout << "x axis " << c1;

		return Mesh(vertices, textures, indices);
	}

	Mesh genTorus() {
		std::vector<Vertex> vertices;
		std::vector<Texture> textures;
		std::vector<unsigned int> indices;
		std::cout << "started" << std::endl;
		constexpr int NUM_POINTS = 100;
		constexpr float PI = glm::pi<float>();
		constexpr float dtheta = 2 * PI / (float)(NUM_POINTS - 1);
		constexpr float dphi = 2 * PI / (float)(NUM_POINTS - 1);

		std::cout << dtheta << std::endl;
		std::cout << dphi << std::endl;
		float theta, phi;
		//for (float theta = 0; theta < 2 * PI; theta += dtheta) {
		for (int i =0; i < NUM_POINTS; ++i){
			theta = dtheta * i;
			for (int j =0; j < NUM_POINTS; ++j){
				phi = dphi * j;
			//for (float phi = 0; phi < 2 * PI; phi += dphi) {
				/*
				* so think of a circle translated by r' along the x axis 
				* (if r is the radius of the circle and r' = r + ri) then
				* ri can be thought of as the inner radius of the torus
				* 
				* the equation of this circle is
				* f(phi, theta) =  <cos(phi) + r', sin(phi), 0>
				* then we just rotate this about the y axis to get the equation of the torus
				*
				* I think this vector for normal is good, but I haven't proven it yet.
				* it's just a rotation of the unit circle about the y axis
				* so geometrically, it makes sense. 
				* 
				* and n = df/dtheta x df/dphi  -- where x is cross prod
				* so I could compute it that way too
				* 
				*/
				Vertex v;
				glm::vec3 p(
					glm::cos(theta) * ( glm::cos(phi) + 2),
					glm::sin(phi),
				   -glm::sin(theta) * ( glm::cos(phi) + 2)
				);
				v.position = p;
				v.normal = glm::vec3(
					glm::cos(theta) * glm::cos(phi),
					glm::sin(phi),
				   -glm::sin(theta) * glm::cos(phi)
				);

				v.texCoord = glm::vec2(
					theta / (2 * PI),
					phi / ( 2* PI) );
				vertices.push_back(v);
			}
		}

		for (unsigned int i = 0; i < NUM_POINTS + 1; ++i) {
			for (unsigned int j = 0; j < NUM_POINTS + 1; ++j) {
				indices.push_back((j % NUM_POINTS) + (i % NUM_POINTS) * NUM_POINTS);
				indices.push_back(((j + 1) % NUM_POINTS) + (i % NUM_POINTS) * NUM_POINTS);
				indices.push_back((j % NUM_POINTS) + ((i + 1) % NUM_POINTS) * NUM_POINTS);

				indices.push_back((j % NUM_POINTS) + ((i + 1) % NUM_POINTS) * NUM_POINTS);
				indices.push_back(((j + 1) % NUM_POINTS) + (i % NUM_POINTS) * NUM_POINTS);
				indices.push_back(((j + 1) % NUM_POINTS) + ((i + 1) % NUM_POINTS) * NUM_POINTS);
			}
		}

		Texture tx;
		tx.id = TextureFromFile("earth2.png", "D:");
		tx.path = "earth2.png";
		tx.type = "texture_diffuse";

		textures.push_back(tx);

		//std::cout << "x axis " << c1;

		return Mesh(vertices, textures, indices);
	}

	// Function to calculate SHA-256 hash
	std::string sha256(const std::string& input) {
		unsigned char hash[SHA256_DIGEST_LENGTH];
		SHA256(reinterpret_cast<const unsigned char*>(input.c_str()), input.size(), hash);
		
		// Convert hash to a hexadecimal string
		std::string hash_str;
		for (unsigned char c : hash) {
			char buf[3];
			snprintf(buf, sizeof(buf), "%02x", c);
			hash_str.append(buf);
		}
		return hash_str;
	}

	// Deterministic random function
	std::tuple<double, double, double> deterministic_random(const std::string& input) {
		// Hash the input string
		std::string hash_str = sha256(input);
		
		// Convert part of the hash to an integer
		size_t seed = std::stoull(hash_str.substr(0, 16), nullptr, 16);
		
		// Seed a PRNG and generate a random value
		std::mt19937_64 rng(seed); // 64-bit Mersenne Twister
		std::uniform_real_distribution<double> dist(0.0, 1.0);
		
		return {dist(rng), dist(rng), dist(rng)};
	}

	float dist(std::tuple<float, float, float> p1, std::tuple<float, float, float> p2){
		auto [x1, y1, z1] =p1;
		auto [x2, y2, z2] =p2;

		float dx = x2 - x1,
			  dy = y2 - y1,
			  dz = z2 - z1;
		
		return pow(dx*dx + dy*dy + dz*dz, 0.5f);
	}

	GLuint genWorleyNoise(unsigned int width, unsigned int height, unsigned int breadth, unsigned int num_tiles){
		// float boxWidth  	= (float)width / (float)num_tiles, 
		// 	  boxHeight 	= (float)height / (float)num_tiles,
		// 	  boxBreadth 	= (float)breadth / (float)num_tiles;

		// GLubyte vals[height * breadth * width]; 

		// unsigned int z, y, x, tileX, tileY, tileZ;
		// int i, j, k;

		// const float max_dist = pow(boxWidth * boxWidth + boxHeight * boxHeight + boxBreadth * boxBreadth, 0.5f);

		// for (z = 0; z < breadth; ++z){
		// 	std::ofstream file {"out"+ std::to_string(z) +".ppm"}; 
		// 	file << "P3\n";
		// 	file << width << ' ' << height << "\n255\n";
		// 	for (y = 0; y < height; ++y){
		// 		for (x = 0; x < width; ++x){
		// 			tileX = x / boxWidth;
		// 			tileY = y / boxHeight;
		// 			tileZ = z / boxBreadth;
		// 			float min_d = 1000.0f;
		// 			float color = 0.0f;
		// 			for (i = -1; i <= 1; ++i){
		// 				for (j = -1; j <= 1; ++j){
		// 					for (k =-1; k <= 1; ++k){
		// 					auto [ anchor_x,  anchor_y, anchor_z] = deterministic_random(
		// 											  std::to_string((i + tileY + num_tiles)%num_tiles) + 
		// 											  std::to_string((j + tileX + num_tiles)%num_tiles) + 
		// 											  std::to_string((k + tileZ + num_tiles)%num_tiles)
		// 											  );
		// 						anchor_x = (tileX + j + anchor_x) * boxWidth;
		// 						anchor_y = (tileY + i + anchor_y) * boxHeight;
		// 						anchor_z = (tileZ + k + anchor_z) * boxBreadth;
		// 						min_d = std::min(min_d, dist( {x, y, z},{anchor_x, anchor_y, anchor_z} ));
		// 					}
		// 				}
		// 			}
		// 			color += min_d/max_dist;
		// 			vals[z * width * height +  y * width + x] = static_cast<GLubyte>((1 - color) * 255.0f);
		// 			unsigned int v = vals[z * width * height +  y * width + x];
		// 			file << v << ' ' << v << ' ' << v << '\n';
		// 			// std::cout << '(' << x << ','
		// 					//   << y << ',' << z << ')' << ":" << static_cast<int>(vals[z * width * height +  y * width + x]) << std::endl;
		// 		}
		// 	}
		// 	// file.close();
		// }

		std::ifstream file("new_raw_data.raw", std::ios::binary);
		if (!file){
			std::cerr << "Failed to open file.";
		}

		// int DEPTH, HEIGHT, WIDTH;
		struct HeaderInfo {
			uint32_t DEPTH;
			uint32_t HEIGHT;
			uint32_t WIDTH;
			uint32_t CHANNELS;
		} info ;
		// file.read(reinterpret_cast<char*>(&DEPTH), sizeof(int));
		// file.read(reinterpret_cast<char*>(&HEIGHT), sizeof(int));
		// file.read(reinterpret_cast<char*>(&WIDTH), sizeof(int));
		// std::cout << sizeof(HeaderInfo) << ' ' << sizeof(int) * 3 << std::endl;

		// i dont want to use sizeof(HeaderInfo) bcz I don't trust padding
		file.read(reinterpret_cast<char*>(&info), sizeof(int) * 4 );

		std::cout << info.DEPTH << ' ' <<  info.HEIGHT<< ' ' << info.WIDTH
				  << ' ' << info.CHANNELS << std::endl;

		size_t sz = info.DEPTH * info.HEIGHT * info.WIDTH * info.CHANNELS * sizeof(uint8_t);
		std::vector<uint8_t> data(sz / sizeof(uint8_t));
		std::cout << "DATA SIZE" << data.size() << ' ' << sizeof(uint8_t) << std::endl;
		file.read(reinterpret_cast<char*>(data.data()), sz);
		file.close();
		std::cout << "READ THE DATA" << std::endl;
		std::cout << "SAMPLE: " << (int)data[0] << std::endl;


		unsigned int tId;
		glGenTextures(1, &tId);

		glBindTexture(GL_TEXTURE_3D, tId);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, info.WIDTH, info.HEIGHT, info.DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glBindTexture(GL_TEXTURE_3D, 0);

		return tId;
	}


	GLuint loadWeatherData(std::string fname){
		std::ifstream file (fname, std::ios::binary);
		if (!file){
			std::cerr << "Couldn't open file " << fname << std::endl;
			return 0;
		}

		struct HeaderInfo {
			uint32_t HEIGHT;
			uint32_t WIDTH;
			uint32_t CHANNELS;
		} info;

		file.read(reinterpret_cast<char*>(&info), sizeof(int) * 3);

		std::cout << info.HEIGHT<< ' ' << info.WIDTH
				  << ' ' << info.CHANNELS << std::endl;

		size_t sz = info.HEIGHT * info.WIDTH * info.CHANNELS * sizeof(uint8_t);
		std::vector<uint8_t> data(sz / sizeof(uint8_t));
		std::cout << "DATA SIZE" << data.size() << ' ' << sizeof(uint8_t) << std::endl;
		file.read(reinterpret_cast<char*>(data.data()), sz);
		file.close();
		std::cout << "READ THE DATA" << std::endl;
		std::cout << "SAMPLE: " << (int)data[0] << std::endl;
		std::cout << "SAMPLE: " << (int)data[1] << std::endl;


		unsigned int tId;
		glGenTextures(1, &tId);

		glBindTexture(GL_TEXTURE_2D, tId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, info.WIDTH, info.HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		return tId;

	}

	GLuint loadDetailTexture(std::string fname){
		std::cout << "READING DETAIL TEXTURE" << std::endl;
		std::ifstream file(fname, std::ios::binary);
		if (!file){
			std::cerr << "Failed to open file.";
			return 0;
		}

		// int DEPTH, HEIGHT, WIDTH;
		struct HeaderInfo {
			uint32_t DEPTH;
			uint32_t HEIGHT;
			uint32_t WIDTH;
			uint32_t CHANNELS;
		} info ;

		file.read(reinterpret_cast<char*>(&info), sizeof(int) * 4 );

		std::cout << info.DEPTH << ' ' <<  info.HEIGHT<< ' ' << info.WIDTH
				  << ' ' << info.CHANNELS << std::endl;

		size_t sz = info.DEPTH * info.HEIGHT * info.WIDTH * info.CHANNELS * sizeof(uint8_t);
		std::vector<uint8_t> data(sz / sizeof(uint8_t));
		std::cout << "DATA SIZE" << data.size() << ' ' << sizeof(uint8_t) << std::endl;
		file.read(reinterpret_cast<char*>(data.data()), sz);
		file.close();
		std::cout << "READ THE DATA" << std::endl;
		std::cout << "SAMPLE: " << (int)data[0] << std::endl;


		unsigned int tId;
		glGenTextures(1, &tId);

		glBindTexture(GL_TEXTURE_3D, tId);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGB, info.WIDTH, info.HEIGHT, info.DEPTH, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glBindTexture(GL_TEXTURE_3D, 0);

		return tId;
	}

	GLuint loadGeneric3dTexture(std::string fname){
		std::cout << "LOADING " << fname << std::endl;
		std::ifstream file(fname, std::ios::binary);
		if (!file){
			std::cerr << "Failed to open file.";
			return 0;
		}

		struct HeaderInfo {
			uint32_t DEPTH;
			uint32_t HEIGHT;
			uint32_t WIDTH;
			uint32_t CHANNELS;
		} info ;

		file.read(reinterpret_cast<char*>(&info), sizeof(int) * 4 );

		std::cout << fname << " METADATA: " << std::endl;
		std::cout << info.DEPTH << ' ' <<  info.HEIGHT<< ' ' << info.WIDTH
				  << ' ' << info.CHANNELS << std::endl;

		size_t sz = info.DEPTH * info.HEIGHT * info.WIDTH * info.CHANNELS * sizeof(uint8_t);
		std::vector<uint8_t> data(sz / sizeof(uint8_t));
		std::cout << "DATA SIZE" << data.size() << ' ' << sizeof(uint8_t) << std::endl;
		file.read(reinterpret_cast<char*>(data.data()), sz);
		file.close();
		std::cout << "READ THE DATA" << std::endl;
		std::cout << "SAMPLE: " << (int)data[0] << std::endl;


		unsigned int tId;
		glGenTextures(1, &tId);

		glBindTexture(GL_TEXTURE_3D, tId);
		glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA, info.WIDTH, info.HEIGHT, info.DEPTH, 0, GL_RGBA, GL_UNSIGNED_BYTE, data.data());

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_REPEAT);

		glBindTexture(GL_TEXTURE_3D, 0);

		return tId;
	}



	GLuint loadGeneric2dTexture(std::string fname){
		std::cout << "LOADING " << fname << std::endl;
		std::ifstream file (fname, std::ios::binary);
		if (!file){
			std::cerr << "Couldn't open file " << fname << std::endl;
			return 0;
		}

		struct HeaderInfo {
			uint32_t HEIGHT;
			uint32_t WIDTH;
			uint32_t CHANNELS;
		} info;

		file.read(reinterpret_cast<char*>(&info), sizeof(int) * 3);

		std::cout << info.HEIGHT<< ' ' << info.WIDTH
				  << ' ' << info.CHANNELS << std::endl;

		size_t sz = info.HEIGHT * info.WIDTH * info.CHANNELS * sizeof(uint8_t);
		std::vector<uint8_t> data(sz / sizeof(uint8_t));
		std::cout << "DATA SIZE" << data.size() << ' ' << sizeof(uint8_t) << std::endl;
		file.read(reinterpret_cast<char*>(data.data()), sz);
		file.close();
		std::cout << "READ THE DATA" << std::endl;
		std::cout << "SAMPLE: " << (int)data[0] << std::endl;
		std::cout << "SAMPLE: " << (int)data[1] << std::endl;

		std::cout << "HERE NOW" << std::endl;

		unsigned int tId;
		glGenTextures(1, &tId);
		std::cout << "AND FIN HERE NOW"  << tId << std::endl;

		glBindTexture(GL_TEXTURE_2D, tId);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, info.WIDTH, info.HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data());
		std::cout << "AND STUCK" << std::endl;

		// Set texture parameters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		std::cout << "AND STUCK" << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		std::cout << "AND STUCK" << std::endl;
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		// glGenerateMipmap(GL_TEXTURE_2D);

		glBindTexture(GL_TEXTURE_2D, 0);

		std::cout << "AND STUCK" << std::endl;

		return tId;

	}
}