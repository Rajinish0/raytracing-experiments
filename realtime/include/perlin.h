#pragma once
#define _USE_MATH_DEFINES
#include <iostream>
#include <math.h>
#include <fstream>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



class Perlin1d {
private:
	int dim;
	float* arr;
	float persistence = 0.5;
	int octaves = 4;


	float g(float a);
	float f(float x);
	float noise(float x);


public:
	Perlin1d(int d, int pers, int oct);
	float perlin(float x);
	~Perlin1d();
};


struct myVector {
	float x, y;


	myVector(float x, float y);
	myVector();
	myVector operator+(myVector v);
	myVector operator-(myVector v);
	myVector operator*(float a);
	myVector operator/(float a);
	float dot(myVector v);
	float mag();
	myVector normalized();


	static myVector rand();
};


class Perlin2d {
private:
	myVector* grads;
	int octaves, sz, perm[512];

	static constexpr int p[256] = { 151,160,137,91,90,15,                 // Hash lookup table as defined by Ken Perlin.  This is a randomly
131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,    // arranged array of all numbers from 0-255 inclusive.
190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
	};


	float g(float t);
	template <typename T>
	T lerp(T x1, T x2, float a);
	//glm::vec2 lerp(glm::vec2 x1, glm::vec2 x2, float a);
	float static grad(int hash, double x, double y);
	glm::vec2 static getGrad(int hash);

public:
	Perlin2d(int bufferSize = 256, int octaves = 4);
	float noise(float x, float y);
	float perlin(float x, float y);
	glm::vec3 normal(float x, float y);
	~Perlin2d();

};