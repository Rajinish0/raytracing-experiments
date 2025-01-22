import numpy as np
from PIL import Image
import random, noise
import sys

if len(sys.argv) < 2:
	print(f"USAGE: {sys.argv[0]} (low|high)")
	sys.exit()

if sys.argv[1] == 'high':
	DEPTH = 128
	WIDTH = 128
	HEIGHT = 128
	CHANNELS = 4
	MODE = 'HIGH'
elif sys.argv[1] == 'low':
	DEPTH = 32
	WIDTH = 32
	HEIGHT = 32
	CHANNELS = 3
	MODE = 'LOW'
else:
	print("WRONG ARG")
	sys.exit()
	

def deterministic_random(val):
    random.seed(hash(val))
    return [random.random() for _ in range(len(val))]

def distSqr(t1, t2):
    return sum((p[0]-p[1])*(p[0]-p[1]) for p in zip(t1, t2))

def worleyNoise(x, y, z, numCells):
    x = x % 1
    y = y % 1
    xCell = int(x * numCells)
    yCell = int(y * numCells)
    zCell = int(z * numCells)
    
    cellX = x * numCells
    cellY = y * numCells
    cellZ = z * numCells
    
    minDist = float('inf')
    for i in range(-1, 2):
        for j in range(-1, 2):
            for k in range(-1, 2):
                neighborX = (xCell + j) % numCells
                neighborY = (yCell + i) % numCells
                neighborZ = (zCell + k) % numCells
                
                offsetX, offsetY, offsetZ = deterministic_random((neighborX, neighborY, neighborZ))

                dist = distSqr(
                    (cellX, cellY, cellZ), 
                    (xCell + j + offsetX, yCell + i + offsetY, zCell + k + offsetZ)
                    )
                minDist = min(minDist, dist)
        
    # Invert and normalize
    return 1-(minDist/3)**.5

def remap(v, olo, ohi, nlo, nhi):
    return nlo + (v-olo)*(nhi - nlo)/(ohi - olo)

def worleyNoiseFbm(x, y, z):
    freqMul = [2, 8, 14, 20, 26, 32]
    baseCount = 4
    worley0 = worleyNoise(x, y, z, freqMul[0] * baseCount)
    worley1 = worleyNoise(x, y, z, freqMul[1] * baseCount)
    worley2 = worleyNoise(x, y, z, freqMul[2] * baseCount)
    worley3 = worleyNoise(x, y, z, freqMul[3] * baseCount)
    worley4 = worleyNoise(x, y, z, freqMul[4] * baseCount)
    worley5 = worleyNoise(x, y, z, freqMul[5] * baseCount)
    pnoise = noise.pnoise3(x, y, z, octaves = 3, lacunarity = 2., repeatx = 1, repeaty = 1, repeatz = 1)
    wfbm = worley0 * .625 + worley1 * .25 + worley2 * .125

    perlinworley = remap(pnoise, .0, 1., wfbm, 1.)

    WFBM0 = worley1*.625 + worley2 * .25 + worley3 * .125
    WFBM1 = worley2*.625 + worley3 * .25 + worley4 * .125
    WFBM2 = worley3*.625 + worley4 * .25 + worley5 * .125

    return (perlinworley * perlinworley, WFBM0, WFBM1, WFBM2)

    # value = 0
    # amplitude = 1.
    # freq = 1
    # baseNumCells = 8
    # totalAmpl = 0.
    # for i in range(octaves):
    #     value += worleyNoise(x*freq, y*freq, baseNumCells) * amplitude
    #     totalAmpl += amplitude
    #     amplitude += persistence
    #     freq *= lacunarity
    #     baseNumCells = int(freq * baseNumCells)

    # return value/totalAmpl    

def worleyNoiseFbmlow(x, y, z):
    freqMul = [1, 2, 4, 8]
    baseCount = 2
    worley0 = worleyNoise(x, y, z, freqMul[0] * baseCount)
    worley1 = worleyNoise(x, y, z, freqMul[1] * baseCount)
    worley2 = worleyNoise(x, y, z, freqMul[2] * baseCount)
    worley3 = worleyNoise(x, y, z, freqMul[3] * baseCount)
    WFBM0 = worley0*.625 + worley1 * .25 + worley2 * .125
    WFBM1 = worley1*.625 + worley2 * .25 + worley3 * .125
    WFBM2 = worley2*.75 + worley3 * .25
    return (WFBM0, WFBM1, WFBM2)

def generate():
    img = np.zeros((DEPTH, HEIGHT, WIDTH, CHANNELS), np.uint8)
    for k in range(DEPTH):
        z = k / DEPTH
        for i in range(HEIGHT):
            y = i/HEIGHT
            for j in range(WIDTH):
                x = j/WIDTH
                # Scale the output to 0-255 range and clamp
                # value = min(255, max(0, worleyNoise(x, y, 8) * 255))
                if MODE == 'HIGH':
                    value = worleyNoiseFbm(x, y, z)
                else:
                     value = worleyNoiseFbmlow(x, y, z)
                img[k, i, j] = np.array(value)*255
                print("{:.2f}".format((k*WIDTH*HEIGHT+i*WIDTH+j)/(DEPTH * HEIGHT * WIDTH)), end='\r')
    print()
    
    print("Done, saving.")
    for i in range(CHANNELS):
        Image.fromarray(img[0, :, :, i]).save(f'test_img_{MODE}_{i}.png')
        print("Saved.")
    print("SAVING DATA: ")
    with open(f'f_data_{MODE}.raw', 'wb') as f:
        f.write(np.array([DEPTH, HEIGHT, WIDTH, CHANNELS], np.int32).tobytes())
        f.write(img.tobytes())
    print("DONE")

generate()
