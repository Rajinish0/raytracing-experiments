#include <iostream>
#include <fstream>
#include <vector>
#include <cmath>
#include <random>
#include <thread>
#include <mutex>
#include <atomic>
#include <algorithm>
#include <cstdint>
#include <array>
#include <png.h>

// Constants
const int D = 128, H = 128, W = 128;
const std::array<int, 4> TILES = {4, 8, 16, 32};
const std::string TEXTURE_FILE = "texture.bin";
const std::string OUTPUT_NAME = "output";

// Function to calculate squared distance
inline double distSqr(const std::array<double, 3>& p1, const std::array<double, 3>& p2) {
    return std::pow(p1[0] - p2[0], 2) + std::pow(p1[1] - p2[1], 2) + std::pow(p1[2] - p2[2], 2);
}

// Random generator function
std::array<double, 3> random_f(int y, int x, int z) {
    std::mt19937 generator(y * 73856093 ^ x * 19349663 ^ z * 83492791); // Unique seed per coordinate
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    return {dist(generator), dist(generator), dist(generator)};
}

// Save PNG image slices
void saveSliceAsPNG(const std::vector<uint8_t>& data, int width, int height, const std::string& filename) {
    FILE* fp = fopen(filename.c_str(), "wb");
    if (!fp) {
        std::cerr << "Failed to open file: " << filename << "\n";
        return;
    }

    png_structp png = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
    if (!png) {
        fclose(fp);
        return;
    }

    png_infop info = png_create_info_struct(png);
    if (!info) {
        png_destroy_write_struct(&png, nullptr);
        fclose(fp);
        return;
    }

    if (setjmp(png_jmpbuf(png))) {
        png_destroy_write_struct(&png, &info);
        fclose(fp);
        return;
    }

    png_init_io(png, fp);

    png_set_IHDR(
        png, info, width, height,
        8, PNG_COLOR_TYPE_GRAY, PNG_INTERLACE_NONE,
        PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT
    );

    png_write_info(png, info);

    for (int y = 0; y < height; ++y) {
        png_write_row(png, data.data() + y * width);
    }

    png_write_end(png, nullptr);
    png_destroy_write_struct(&png, &info);
    fclose(fp);
}

// Generate Worley noise
void generateWorley3D(std::vector<uint8_t>& arr, int channel) {
	std::cout << channel << " started" << std::endl; //to do: add mtx
    int NUM_TILES = TILES[channel];
    double BOX_WIDTH = W / static_cast<double>(NUM_TILES);
    double BOX_HEIGHT = H / static_cast<double>(NUM_TILES);
    double BOX_DEPTH = D / static_cast<double>(NUM_TILES);
    double MAX_DIST = std::sqrt(BOX_WIDTH * BOX_WIDTH + BOX_HEIGHT * BOX_HEIGHT + BOX_DEPTH * BOX_DEPTH);

    for (int z = 0; z < D; ++z) {
        for (int y = 0; y < H; ++y) {
            for (int x = 0; x < W; ++x) {
                int tileX = x / BOX_WIDTH;
                int tileY = y / BOX_HEIGHT;
                int tileZ = z / BOX_DEPTH;

                double min_d = std::numeric_limits<double>::max();

                for (int i = -1; i <= 1; ++i) {
                    for (int j = -1; j <= 1; ++j) {
                        for (int k = -1; k <= 1; ++k) {
                            int neighborX = (tileX + j + NUM_TILES) % NUM_TILES;
                            int neighborY = (tileY + i + NUM_TILES) % NUM_TILES;
                            int neighborZ = (tileZ + k + NUM_TILES) % NUM_TILES;

                            auto anchor = random_f(neighborY, neighborX, neighborZ);
                            double anchorX = (tileX + j + anchor[0]) * BOX_WIDTH;
                            double anchorY = (tileY + i + anchor[1]) * BOX_HEIGHT;
                            double anchorZ = (tileZ + k + anchor[2]) * BOX_DEPTH;

                            min_d = std::min(min_d, distSqr({x, y, z}, {anchorX, anchorY, anchorZ}));
                        }
                    }
                }

                double normalized = std::sqrt(min_d) / MAX_DIST;
                arr[(z * H * W + y * W + x) * 4 + channel] = static_cast<uint8_t>((1.0 - normalized) * 255);
            }
        }
    }
	std::cout << channel << " finished" << std::endl;
}

int main() {
    std::vector<uint8_t> arr(D * H * W * 4, 0);

    // Generate noise for each channel using threads
    std::array<std::thread, 4> threads;
    for (int channel = 0; channel < 4; ++channel) {
        threads[channel] = std::thread(generateWorley3D, std::ref(arr), channel);
    }
    for (auto& t : threads) {
        t.join();
    }

    // Write the binary texture file
    std::ofstream textureFile(TEXTURE_FILE, std::ios::binary);
    if (textureFile.is_open()) {
        int dimensions[] = {D, H, W, 4};
        textureFile.write(reinterpret_cast<char*>(dimensions), sizeof(dimensions));
        textureFile.write(reinterpret_cast<char*>(arr.data()), arr.size());
        textureFile.close();
    }

    // Save PNG slices
    for (int channel = 0; channel < 4; ++channel) {
        for (int slice = 0; slice < 5; ++slice) {
            std::vector<uint8_t> sliceData(H * W);
            for (int y = 0; y < H; ++y) {
                for (int x = 0; x < W; ++x) {
                    sliceData[y * W + x] = arr[(slice * H * W + y * W + x) * 4 + channel];
                }
            }
            saveSliceAsPNG(sliceData, W, H, OUTPUT_NAME + "_" + std::to_string(channel) + "_" + std::to_string(slice) + ".png");
        }
    }

    std::cout << "Saved the texture and PNG slices.\n";
    return 0;
}

