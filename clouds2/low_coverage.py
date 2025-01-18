import numpy as np
import random
import cv2

def generate_circle_texture(size=512, num_circles=1500):
    image = np.zeros((size, size), dtype=np.uint8)
    
    for _ in range(num_circles):
        radius = int(random.random() * 10)
        x = int(random.uniform(radius, size-radius))
        y = int(random.uniform(radius, size-radius))
        
        cv2.circle(image, (x, y), radius, 255, -1, cv2.LINE_AA)

    return image

# Generate the image
image = generate_circle_texture()

# Save the image
cv2.imwrite('circle_texture.png', image)
