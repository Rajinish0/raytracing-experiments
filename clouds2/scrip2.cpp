#include <vector>
#include <random>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <cmath>
#include <array>

class PerlinWorleyGenerator {
private:
    struct Point3D {
        float x, y, z;
        Point3D(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
    };

    struct Vec3 {
        float x, y, z;
        Vec3(float _x, float _y, float _z) : x(_x), y(_y), z(_z) {}
        
        float dot(const Vec3& v) const {
            return x * v.x + y * v.y + z * v.z;
        }
    };

    // Improved hash for gradients
    std::vector<Vec3> generateGradients() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(-1.0f, 1.0f);
        
        std::vector<Vec3> gradients;
        for (int i = 0; i < 256; i++) {
            float x = dis(gen);
            float y = dis(gen);
            float z = dis(gen);
            float len = std::sqrt(x*x + y*y + z*z);
            gradients.emplace_back(x/len, y/len, z/len);
        }
        return gradients;
    }

    float smoothstep(float t) {
        return t * t * (3 - 2 * t);
    }

    float remap(float value, float oldMin, float oldMax, float newMin, float newMax) {
        float normalized = (value - oldMin) / (oldMax - oldMin);
        return newMin + normalized * (newMax - newMin);
    }

    // // Tileable distance calculation
    // float tileableDistance(const Point3D& p1, const Point3D& p2, float size = 1.0f) {
    //     float dx = std::min(std::abs(p1.x - p2.x), size - std::abs(p1.x - p2.x));
    //     float dy = std::min(std::abs(p1.y - p2.y), size - std::abs(p1.y - p2.y));
    //     float dz = std::min(std::abs(p1.z - p2.z), size - std::abs(p1.z - p2.z));
    //     return std::sqrt(dx * dx + dy * dy + dz * dz);
    // }

    std::vector<Point3D> generateFeaturePoints(int numPoints, std::mt19937& gen) {
        std::uniform_real_distribution<float> dis(0.0f, 1.0f);
        std::vector<Point3D> points;
        for (int i = 0; i < numPoints; ++i) {
            points.emplace_back(dis(gen), dis(gen), dis(gen));
        }
        return points;
    }

    // Tileable Perlin noise
    float perlinNoise(float x, float y, float z, const std::vector<Vec3>& gradients) {
        // Ensure coordinates are wrapped
        x = fmod(x, 1.0f);
        y = fmod(y, 1.0f);
        z = fmod(z, 1.0f);
        if (x < 0) x += 1.0f;
        if (y < 0) y += 1.0f;
        if (z < 0) z += 1.0f;

        int x0 = static_cast<int>(x * 255) & 255;
        int y0 = static_cast<int>(y * 255) & 255;
        int z0 = static_cast<int>(z * 255) & 255;
        
        float xf = x * 255.0f - x0;
        float yf = y * 255.0f - y0;
        float zf = z * 255.0f - z0;

        float u = smoothstep(xf);
        float v = smoothstep(yf);
        float w = smoothstep(zf);

        Vec3 pos(xf, yf, zf);
        
        // Calculate dot products with gradients
        float dots[8];
        for(int i = 0; i < 8; ++i) {
            int ix = (x0 + (i & 1)) & 255;
            int iy = (y0 + ((i >> 1) & 1)) & 255;
            int iz = (z0 + ((i >> 2) & 1)) & 255;
            
            const Vec3& grad = gradients[ix ^ iy ^ iz];
            Vec3 dp(xf - (i & 1), yf - ((i >> 1) & 1), zf - ((i >> 2) & 1));
            dots[i] = grad.dot(dp);
        }

        // Trilinear interpolation
        float x1 = remap(u, 0.0f, 1.0f, dots[0], dots[1]);
        float x2 = remap(u, 0.0f, 1.0f, dots[2], dots[3]);
        float x3 = remap(u, 0.0f, 1.0f, dots[4], dots[5]);
        float x4 = remap(u, 0.0f, 1.0f, dots[6], dots[7]);
        
        float y1 = remap(v, 0.0f, 1.0f, x1, x2);
        float y2 = remap(v, 0.0f, 1.0f, x3, x4);
        
        return remap(w, 0.0f, 1.0f, y1, y2);
    }

    float perlinFBM(float x, float y, float z, int octaves, float lacunarity, float persistence, 
                    const std::vector<Vec3>& gradients) {
        float value = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float totalAmplitude = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            value += perlinNoise(x * frequency, y * frequency, z * frequency, gradients) * amplitude;
            totalAmplitude += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        return value / totalAmplitude;
    }

private:
    float tileableDistance(const Point3D& p1, const Point3D& p2, float size = 1.0f) {
        float dx = std::min(std::abs(p1.x - p2.x), size - std::abs(p1.x - p2.x));
        float dy = std::min(std::abs(p1.y - p2.y), size - std::abs(p1.y - p2.y));
        float dz = std::min(std::abs(p1.z - p2.z), size - std::abs(p1.z - p2.z));
        return std::sqrt(dx * dx + dy * dy + dz * dz) * 2.0f; // Doubled distance scale
    }

    float invertedWorleyNoise(const Point3D& p, const std::vector<Point3D>& featurePoints) {
        float minDist = 2.0f; // Increased from 1.0f to allow for darker values
        for (const auto& point : featurePoints) {
            float dist = tileableDistance(p, point);
            minDist = std::min(minDist, dist);
        }
        return std::pow(1.0f - minDist * 0.5f, 1.5f); // Added power to darken
    }

    float worleyFBM(const Point3D& p, int octaves, float lacunarity, float persistence,
                    const std::vector<std::vector<Point3D>>& octavePoints) {
        float value = 0.0f;
        float amplitude = 1.0f;
        float frequency = 1.0f;
        float totalAmplitude = 0.0f;

        for (int i = 0; i < octaves; ++i) {
            Point3D scaledP(
                fmod(p.x * frequency, 1.0f),
                fmod(p.y * frequency, 1.0f),
                fmod(p.z * frequency, 1.0f)
            );
            float noiseValue = invertedWorleyNoise(scaledP, octavePoints[i]);
            value += noiseValue * amplitude;
            totalAmplitude += amplitude;
            amplitude *= persistence;
            frequency *= lacunarity;
        }

        // Apply additional contrast
        float normalizedValue = value / totalAmplitude;
        return std::pow(normalizedValue, 1.2f); // Increased contrast
    }

public:

    std::vector<unsigned char> generate3DNoise(int width, int height, int depth) {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::vector<Vec3> gradients = generateGradients();
        
        std::vector<unsigned char> output(width * height * depth * 4);
        
        // Adjusted parameters for darker output
        struct ChannelParams {
            int octaves;
            float lacunarity;
            float persistence;
            int basePoints;
        };

        std::array<ChannelParams, 4> channelParams = {{
            {4, 2.0f, 0.6f, 16},   // Red (more points, higher persistence)
            {4, 2.0f, 0.6f, 16},   // Green
            {5, 2.0f, 0.55f, 20},  // Blue
            {6, 2.0f, 0.5f, 24}    // Alpha
        }};

        
        // Generate feature points for Worley noise
        std::array<std::vector<std::vector<Point3D>>, 4> allFeaturePoints;
        for (int channel = 0; channel < 4; ++channel) {
            const auto& params = channelParams[channel];
            std::vector<std::vector<Point3D>> channelPoints;
            for (int octave = 0; octave < params.octaves; ++octave) {
                int numPoints = params.basePoints * (1 << octave);
                channelPoints.push_back(generateFeaturePoints(numPoints, gen));
            }
            allFeaturePoints[channel] = std::move(channelPoints);
        }

        #pragma omp parallel for collapse(3)
        for (int z = 0; z < depth; ++z) {
            for (int y = 0; y < height; ++y) {
                for (int x = 0; x < width; ++x) {
                    Point3D current(
                        static_cast<float>(x) / width,
                        static_cast<float>(y) / height,
                        static_cast<float>(z) / depth
                    );

                    float perlinValue = perlinFBM(
                        current.x, current.y, current.z,
                        4, 2.0f, 0.5f, gradients
                    );

                    for (int channel = 0; channel < 4; ++channel) {
                        const auto& params = channelParams[channel];
                        float worleyValue = worleyFBM(
                            current,
                            params.octaves,
                            params.lacunarity,
                            params.persistence,
                            allFeaturePoints[channel]
                        );

                        int index = ((z * height * width + y * width + x) * 4) + channel;
                        
                        if (channel == 0) {
                            // Modified Perlin-Worley combination for darker output
                            float greenWorley = worleyFBM(current, 
                                channelParams[1].octaves,
                                channelParams[1].lacunarity,
                                channelParams[1].persistence,
                                allFeaturePoints[1]);
                            
                            worleyValue = worleyValue * 0.7f + // Reduced base value
                                         remap(perlinValue, 0.0f, 1.0f, greenWorley * 0.8f, 0.9f); // Reduced range
                        }
                        
                        // Final contrast adjustment
                        worleyValue = std::pow(worleyValue, 1.2f);
                        output[index] = static_cast<unsigned char>(worleyValue * 255);
                    }
                }
            }
        }

        return output;
    }

};

int main() {
    const int SIZE = 128;
    PerlinWorleyGenerator generator;
    std::vector<unsigned char> noiseTexture = generator.generate3DNoise(SIZE, SIZE, SIZE);
	int dimensions[] = {SIZE, SIZE, SIZE, 4};
	std::ofstream f("myfile.bin", std::ios::binary);
	f.write(reinterpret_cast<char*>(dimensions), sizeof(dimensions));
	f.write(reinterpret_cast<char*>(noiseTexture.data()), noiseTexture.size());
	f.close();
   
   	std::cout << "DONE" << std::endl;
    return 0;
}
