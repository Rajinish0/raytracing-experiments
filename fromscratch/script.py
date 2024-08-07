import cv2, sys


if len(sys.argv) < 2:
    print(f"Usage {sys.argv[0]} filename")
    sys.exit(1)

fname = sys.argv[1]
img = cv2.imread(fname)
cv2.imwrite('out.png', img)
