#include "perlin.h"


float Perlin1d::g(float a) {
	return (1 - std::cos(M_PI * a))/2;
}


float Perlin1d::f(float x) {
	std::cout << x << std::endl;
	float x1 = arr[(int)std::floor(x)];
	float x2 = arr[((int)std::floor(x) + 1) % dim];
	float a = x - std::floor(x);
	return x1 * (1 - g(a)) + x2 * g(a);
}

float Perlin1d::noise(float x) {
	return f(x - dim * ((int)(x / dim)));
}

Perlin1d::Perlin1d(int d, int pers = 0.5, int oct = 4)
	:dim(d), arr(new float[d]), persistence(pers), octaves(oct) {
	for (int i = 0; i < d; ++i)
		arr[i] = std::rand() % 5;
}

float Perlin1d::perlin(float x) {
	float out = 0;
	for (int i = 0; i < octaves; ++i)
		out += (1.0f / (1 << i)) * noise(x * (1 << i));
	return out;
}

Perlin1d::~Perlin1d() {
	delete[] arr;
}







myVector::myVector(float x, float y)
	:x(x), y(y) {}

myVector::myVector()
	:x(0), y(0) {}

myVector myVector::operator+(myVector v) {
	return myVector(x + v.x, y + v.y);
}

myVector myVector::operator-(myVector v) {
	return myVector(x - v.x, y - v.y);
}

myVector myVector::operator*(float a) {
	return myVector(a * x, a * y);
}

myVector myVector::operator/(float a) {
	return myVector(x / a, y / a);
}

float myVector::dot(myVector v) {
	return x * v.x + y * v.y;
}

float myVector::mag() {
	return std::pow(x * x + y * y, 0.5);
}

myVector myVector::normalized() {
	return (*this) / this->mag();
}

myVector myVector::rand() {
	float ang = static_cast <float> (std::rand()) / ((static_cast <float> (RAND_MAX)) / (2 * M_PI));
	return myVector(std::cos(ang), std::sin(ang));
	//return myVector(std::rand(), std::rand()).normalized(); // bad
}


std::ostream& operator<<(std::ostream& os, myVector& v) {
	os << '(' << v.x << ',' << v.y << ')' << std::endl;
	return os;
}


float Perlin2d::g(float t) {
	return t * t * t * (t * (t * 6 - 15) + 10);
}


template <typename T>
T Perlin2d::lerp(T x1, T x2, float a) {
	return x1 + (x2 - x1) * a;
}

//glm::vec2 Perlin2d::lerp(glm::vec2 x1, glm::vec2 x2, float a) {
//	return x1 + (x2 - x1) * a;
//}

float Perlin2d::grad(int hash, double x, double y) {
	switch (hash & 3) { // get the last two bits
	case 0: return x + y;
	case 1: return -x + y;
	case 2: return x - y;
	case 3: return -x - y;
	default: return 0;
	}
}

glm::vec2 Perlin2d::getGrad(int hash) {
	switch (hash & 3) {
	case 0: return glm::vec2(1.0f, 1.0f);
	case 1: return glm::vec2(-1.0, 1.0f);
	case 2: return glm::vec2(1.0f, -1.0f);
	case 3: return glm::vec2(-1.0f, -1.0f);
	}
}

glm::vec3 Perlin2d::normal(float x, float y) {
	int xi = (int)std::floor(x) & 255;
	int yi = (int)std::floor(y) & 255;

	float xf = x - std::floor(x);
	float yf = y - std::floor(y);

	int h1 = perm[perm[xi] + yi];
	int h2 = perm[perm[xi + 1] + yi];
	int h3 = perm[perm[xi] + yi + 1];
	int h4 = perm[perm[xi + 1] + yi + 1];

	glm::vec2 g1 = getGrad(h1);
	glm::vec2 g2 = getGrad(h2);
	glm::vec2 g3 = getGrad(h3);
	glm::vec2 g4 = getGrad(h4);

	float u = g(xf);
	float v = g(yf);
	// z = f(x, y) -> tangent plane = z = fx(x-x0) + fy(y-y0) + f(x0, y0)
	// -> Normal = <fx, fy -1>
	// perlin2d uses random gradients to generate the terrain 
	// i m interpolating the gradients to the get the gradient at x,y 
	// in the same way gradients are interpolated to the get the height map.


	/*
	* TODO::
	* use cross prods to calculate normals.
	*/
	return glm::vec3(lerp( lerp(g1, g2, u), lerp(g3, g4, u), v ), -1.0f);
}

float Perlin2d::noise(float x, float y) {
	//std::cout << x << "," << y << std::endl;


	//TODO replace %sz with &(sz-1) after tests.
	int xi = (int)std::floor(x) & 255;
	int yi = (int)std::floor(y) & 255;

	//std::cout << xi << "," << yi << std::endl;

	 float xf = x - std::floor(x);
	 float yf = y - std::floor(y);

	 int h1 = perm[perm[xi  ] + yi    ];
	 int h2 = perm[perm[xi+1] + yi    ];
	 int h3 = perm[perm[xi  ] + yi + 1];
	 int h4 = perm[perm[xi+1] + yi + 1];


	 float g1 = grad(h1, xf, yf);
	 float g2 = grad(h2, xf-1, yf);
	 float g3 = grad(h3, xf, yf-1);
	 float g4 = grad(h4, xf-1, yf-1);

	 float u = g(xf);
	 float v = g(yf);
	 return lerp( lerp(g1, g2, u), lerp(g3, g4, u), v );


	//myVector vec = myVector(x - std::floor(x), y - std::floor(y));


	//myVector g0 = grads[p[(yi + p[xi % sz]) % sz]];
	//myVector g1 = grads[p[(yi + p[(xi + 1) % sz]) % sz]];
	//myVector g2 = grads[p[(((yi + 1) % sz) + p[xi % sz]) % sz]];
	//myVector g3 = grads[p[(((yi + 1) % sz) + p[(xi + 1) % sz]) % sz]];

	//float v0 = (vec - myVector(0, 0)).dot(g0);
	//float v1 = (vec - myVector(1, 0)).dot(g1);
	//float v2 = (vec - myVector(0, 1)).dot(g2);
	//float v3 = (vec - myVector(1, 1)).dot(g3);
	//float u = g(vec.x);
	//float v = g(vec.y);

	//return lerp(lerp(v0, v1, u), lerp(v2, v3, u), v);
}


Perlin2d::Perlin2d(int bufferSize, int octaves)
	:grads(new myVector[bufferSize]), octaves(octaves), sz(bufferSize) {
	for (int i = 0; i < bufferSize; ++i) {
		grads[i] = myVector::rand();
		//std::cout << grads[i];
		//std::cout << grads[i].mag() << std::endl;

	}

	for (int i = 0; i < 512; ++i) {
		perm[i] = p[i % 255];
	}
	//p = std::vector<int>(1, 2);
}


float Perlin2d::perlin(float x, float y) {

	float out = 0;
	float a = 1;
	float p = 1;
	for (int i = 0; i < octaves; ++i) {
		out += p * noise(x * a, y * a);
		p *= 0.5;
		a *= 2;
	}
	//out += (1.0f/(1 << i))*noise(x * (1 << i), y * (1 << i) );
	return out;
}

Perlin2d::~Perlin2d() {
	delete[] grads;
}