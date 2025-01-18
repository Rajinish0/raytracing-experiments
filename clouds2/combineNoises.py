import numpy as np
from PIL import Image


def remap(v, olo, ohi, tlo, thi):
	return tlo + (v-olo)*(thi-tlo)/(ohi - olo)

with open('tileable_perlin_3d.raw', 'rb') as f:
	arr = np.frombuffer(f.read(), np.uint8)
	arr = arr.reshape(128, 128, 128)

with open('raw_data_orig.raw', 'rb') as f:
	f.read(4 * 4) # 4 ints, meta data
	arr2 = np.frombuffer(f.read(), np.uint8)
	arr2 = arr2.reshape(128, 128, 128, 4)

W = arr2[..., 0].astype(np.float64)/255 # worley
P = arr.astype(np.float64)/255

WP = remap(P, 0.0, 1.0, W, 1.0)
print(W.shape, P.shape)
Image.fromarray(
	(WP[0]*255).astype(np.uint8)
).save('test_image.png')

A = np.empty((128, 128, 128, 4), np.uint8)
A[..., 0] = (WP*255).astype(np.int8)
A[..., 1:] = arr2[..., 1:]
with open('new_raw_data.raw', 'wb') as f:
	f.write(np.array([128, 128, 128, 4], np.uint32))
	f.write(A.tobytes())

