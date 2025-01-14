from PIL import Image
import numpy as np
import hashlib, random
import math, sys

if len(sys.argv) < 3:
	print(f"USAGE {sys.argv[0]} output-file-name texture-file-name [NUM_TILES=10]")
	sys.exit()

D = 128
W = 128
H = 128
NUM_TILES = 16
BOX_WIDTH  =  W / NUM_TILES
BOX_HEIGHT =  H / NUM_TILES 
BOX_DEPTH  =  D / NUM_TILES
OUTPUT_NAME = f"{sys.argv[1]}.png"
TEXTURE_FILE = f"{sys.argv[2]}.raw"
TILES = [8, 16, 32, 64] # low, medium, high, higest

def random_f(pos : tuple):
	p_str = str(pos).encode()
	hash_val = hashlib.sha256(p_str).hexdigest()
	random.seed(hash_val)
	return (random.random() for i in range(len(pos)))

def distSqr(p1 : tuple, p2: tuple):	
	return sum( ( (t[0] - t[1])*(t[0]-t[1]) for t in zip(p1, p2) ) )

def dist(p1 : tuple, p2 : tuple):
	return math.sqrt(distSqr(p1, p2))


def generateWorley():
	arr = np.empty((H, W), np.uint8)
	MAX_DIST = math.sqrt(BOX_WIDTH * BOX_WIDTH + BOX_HEIGHT*BOX_HEIGHT)
	for y in range(H):
		for x in range(W):
			tileX, tileY = x // BOX_WIDTH, y // BOX_HEIGHT
			color = 0.0
			min_d = float('inf')
			for i in range(-1, 2):
				for j in range(-1, 2):
					anchor_x, anchor_y = random_f(( (tileY + i) % NUM_TILES, (tileX + j) % NUM_TILES ) )
					anchor_x = (tileX + j + anchor_x) * BOX_WIDTH
					anchor_y = (tileY + i + anchor_y) * BOX_HEIGHT
					min_d = min(min_d, dist( (x, y), (anchor_x, anchor_y) ) )
			color += (min_d/MAX_DIST)
			arr[y, x] = int((1 - color) * 255)
			print("{:2f}".format((y * W + x)/(H*W)), end='\r')
	Image.fromarray(arr).save(OUTPUT_NAME)
	print("SAVED THE PICTURE")

def generateWorley3d():
	arr = np.empty((D, H, W), np.uint8)
	MAX_DIST = math.sqrt(BOX_WIDTH * BOX_WIDTH + BOX_HEIGHT*BOX_HEIGHT + BOX_HEIGHT*BOX_HEIGHT)
	for z in range(D):
		for y in range(H):
			for x in range(W):
				tileX, tileY, tileZ = x // BOX_WIDTH, y // BOX_HEIGHT, z // BOX_DEPTH
				color = 0.0
				min_d = float('inf')
				for i in range(-1, 2):
					for j in range(-1, 2):
						for k in range(-1, 2):
							anchor_x, anchor_y, anchor_z = random_f(( (tileY + i) % NUM_TILES, (tileX + j) % NUM_TILES, (tileZ + k) % NUM_TILES ) )
							anchor_x = (tileX + j + anchor_x) * BOX_WIDTH
							anchor_y = (tileY + i + anchor_y) * BOX_HEIGHT
							anchor_z = (tileZ + k + anchor_z) * BOX_DEPTH
							min_d = min(min_d, distSqr( (x, y, z), (anchor_x, anchor_y, anchor_z) ) )
				min_d = math.sqrt(min_d)
				color += (min_d/MAX_DIST)
				arr[z, y, x] = int((1 - color) * 255)
				print("{:2f}".format((z*W*H + y * W + x)/(H*W*D)), end='\r')
	with open(TEXTURE_FILE, 'wb') as f:
		f.write(np.array([D, H, W], dtype=np.int32).tobytes())
		f.write(arr.tobytes())
	for i in range(10):
		Image.fromarray(arr[i]).save(OUTPUT_NAME.split('.')[0] + str(i) + '.png' )
		print("Saved the picture")


def generateWorley3d_rgba():
	arr = np.empty((D, H, W, 4), np.uint8)
	for a in range(4):
		NUM_TILES = TILES[a]
		BOX_WIDTH  =  W / NUM_TILES
		BOX_HEIGHT =  H / NUM_TILES 
		BOX_DEPTH  =  D / NUM_TILES
		MAX_DIST = math.sqrt(BOX_WIDTH * BOX_WIDTH + BOX_HEIGHT*BOX_HEIGHT + BOX_DEPTH*BOX_DEPTH)
		for z in range(D):
			for y in range(H):
				for x in range(W):
					tileX, tileY, tileZ = x // BOX_WIDTH, y // BOX_HEIGHT, z // BOX_DEPTH
					color = 0.0
					min_d = float('inf')
					for i in range(-1, 2):
						for j in range(-1, 2):
							for k in range(-1, 2):
								anchor_x, anchor_y, anchor_z = random_f(( (tileY + i) % NUM_TILES, (tileX + j) % NUM_TILES, (tileZ + k) % NUM_TILES ) )
								anchor_x = (tileX + j + anchor_x) * BOX_WIDTH
								anchor_y = (tileY + i + anchor_y) * BOX_HEIGHT
								anchor_z = (tileZ + k + anchor_z) * BOX_DEPTH
								min_d = min(min_d, distSqr( (x, y, z), (anchor_x, anchor_y, anchor_z) ) )
					min_d = math.sqrt(min_d)
					color += (min_d/MAX_DIST)
					arr[z, y, x, a] = int((1 - color) * 255)
					print("{:.2f}".format((a*W*D*H + z*W*H + y * W + x)/(H*W*D*4)), end='\r')
	with open(TEXTURE_FILE, 'wb') as f:
		f.write(np.array([D, H, W, 4], dtype=np.int32).tobytes())
		f.write(arr.tobytes())
	for j in range(4):
		for i in range(5):
			Image.fromarray(arr[i, :, :, j]).save(OUTPUT_NAME.split('.')[0] + str(j) + "_" + str(i) + '.png' )
		print("Saved the picture")

if __name__ == '__main__':
	generateWorley3d_rgba()
