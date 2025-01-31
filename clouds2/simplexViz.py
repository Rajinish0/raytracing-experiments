import noise
from PIL import Image
import numpy as np

HEIGHT = 512
WIDTH  = 512
scale  = 3


img = np.empty((HEIGHT, WIDTH), np.uint8)
for i in range(HEIGHT):
	for j in range(WIDTH):
		y = i/(HEIGHT-1)
		x = j/(WIDTH-1)

		img[i, j] = (noise.snoise2(x*scale, y*scale, octaves = 1)/2 + .5) * 255

Image.fromarray(img).save('test_simplex.png')
