import numpy as np
import noise
from PIL import Image

def generate_tileable_perlin(size=128, scale=50.0, octaves=6, persistence=0.5, lacunarity=2.0, base=0):
	noise_map = np.zeros((size, size, size))

	for z in range(size):
		for y in range(size):
			for x in range(size):
				# Convert to normalized coordinate
				nx = x / size
				ny = y / size
				nz = z / size
				# Sample noise
				noise_value = noise.pnoise3(nx * scale,
											ny * scale,
											nz * scale,
											octaves=octaves,
											persistence=persistence,
											lacunarity=lacunarity,
											repeatx=int(scale),
											repeaty=int(scale),
											repeatz=int(scale),
											base=base
											)

				noise_map[z, y, x] = noise_value

	# Normalize to 0-1 range
	noise_map = (noise_map - np.min(noise_map)) / (np.max(noise_map) - np.min(noise_map))
	return noise_map


if __name__ == '__main__':
	print("Generating perlin data")

	arr = generate_tileable_perlin(scale=10.0, octaves=4)
	with open('tileable_perlin_3d.raw', 'wb') as f:
		f.write((arr*255).astype(np.uint8).tobytes())
	Image.fromarray(
		(arr[0, ...]*255).astype(np.uint8)
		).save('test_image.png')

	print("Done, Saved a 2d slice in test_image.png")



