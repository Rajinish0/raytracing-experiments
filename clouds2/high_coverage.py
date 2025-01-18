import noise
import numpy as np

from PIL import Image

WIDTH = 512
HEIGHT = 512

def gen(scale = 5.0, octaves=6, persistence = 0.5, lacunarity = 2.0, base = 0):
	img = np.zeros((HEIGHT, WIDTH))
	for i in range(HEIGHT):
		for j in range(WIDTH):
			nx = j/WIDTH
			ny = i/HEIGHT
			n_v = noise.pnoise2(nx * scale, ny * scale, octaves = octaves, persistence=persistence, lacunarity = lacunarity, base = base)
			img[i, j] = n_v
	img = (img-np.min(img))/(np.max(img) - np.min(img))
	img = (img * 255).astype(np.uint8)
	Image.fromarray(img).save('high_coverage.png')
	print("DONE")

gen()


