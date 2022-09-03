import numpy
from PIL import Image

def get_image(image_path):
    """Get a numpy array of an image so that one can access values[x][y]."""
    image = Image.open(image_path, "r")
    width, height = image.size
    pixel_values = list(image.getdata())
    if image.mode == "RGB":
        channels = 3
    elif image.mode == "L":
        channels = 1
    else:
        print("Unknown mode: %s" % image.mode)
        return None
    pixel_values = numpy.array(pixel_values).reshape((height, width, channels))
    return pixel_values


image = get_image("spectrum.png")

for y in range(image.shape[0]):
    print('{', end='')
    for x in range(image.shape[1]):
        if (x == image.shape[1] - 1):
            print('{0:#08x}'.format((image[y][x][0] << 16) + (image[y][x][1] << 8) + image[y][x][2]), end = '')
        else:
            print('{0:#08x},'.format((image[y][x][0] << 16) + (image[y][x][1] << 8) + image[y][x][2]), end = ' ')
    print('},')
