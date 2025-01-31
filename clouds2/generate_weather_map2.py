import numpy as np
from PIL import Image
import sys

if (len(sys.argv) < 3):
	print(f"USAGE: {sys.argv[0]} coverage_file cloud_type")
	sys.exit()

LOW_COVERAGE_IMG = sys.argv[1]
HIGH_COVERAGE_IMG = sys.argv[2]

SIZE = 512
img = np.empty((SIZE, SIZE, 4), np.uint8)
img[..., 0] = np.array(Image.open(LOW_COVERAGE_IMG))
img[..., 1] = np.array(Image.open(HIGH_COVERAGE_IMG))
img[..., 2] = 255 # height one for now
img[..., 3] = (.5 * 255) # grayish density

with open('weather_data_2.raw', 'wb') as f:
	f.write(np.array([SIZE, SIZE, 4], np.int32).tobytes())
	f.write(img.tobytes())
print('DONE')
