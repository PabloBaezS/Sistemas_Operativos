import math
import multiprocessing
import copyreg
import types

def read_image(filename):
    with open(filename, "rb") as f:
        header = f.readline().decode().strip()
        assert header == "P5", "Only support PGM files (P5)"

        # Skip comments
        while True:
            line = f.readline().decode().strip()
            if not line.startswith("#"):
                break

        # Read width and height
        width, height = map(int, line.split())

        max_val = int(f.readline().strip())
        # Skip max value

        # Read image data
        image_data = []
        for _ in range(height):
            row = []
            for _ in range(width):
                pixel = ord(f.read(1))
                row.append(pixel)
            image_data.append(row)
        return image_data, width, height


def apply_threshold(image_matrix, threshold):
    # Apply thresholding to convert image to binary
    thresholded_image = [[0 if pixel < threshold else 255 for pixel in row] for row in image_matrix]
    return thresholded_image


def rotate_image(image_matrix, angle):
    # Determine dimensions of the image
    rows = len(image_matrix)
    cols = len(image_matrix[0])

    # Initialize a new matrix for rotated image
    rotated_image = [[0] * cols for _ in range(rows)]

    # Convert angle to radians
    theta = angle * (3.141592653589793 / 180.0)

    # Calculate center of the image
    center_x = rows / 2
    center_y = cols / 2

    # Iterate through each pixel in the original image
    for i in range(rows):
        for j in range(cols):
            # Translate the coordinates to center
            x = i - center_x
            y = j - center_y

            # Perform rotation transformation
            new_x = int(x * math.cos(theta) - y * math.sin(theta) + center_x)
            new_y = int(x * math.sin(theta) + y * math.cos(theta) + center_y)

            # Check if new coordinates are within bounds
            if 0 <= new_x < rows and 0 <= new_y < cols:
                rotated_image[new_x][new_y] = image_matrix[i][j]

    return rotated_image


def scale_image(image_matrix, scale_factor):
    def scale_image(image_matrix, scale_factor):
        # Determine dimensions of the image
        rows = len(image_matrix)
        cols = len(image_matrix[0])

        # Calculate new dimensions after scaling
        new_rows = int(rows * scale_factor)
        new_cols = int(cols * scale_factor)

        # Initialize a new matrix for scaled image
        scaled_image = [[0] * new_cols for _ in range(new_rows)]

        # Iterate through each pixel in the scaled image
        for i in range(new_rows):
            for j in range(new_cols):
                # Calculate corresponding pixel position in original image
                original_x = int(i / scale_factor)
                original_y = int(j / scale_factor)

                # Check if original coordinates are within bounds
                if 0 <= original_x < rows and 0 <= original_y < cols:
                    scaled_image[i][j] = image_matrix[original_x][original_y]

        return scaled_image


def parallel_rotate_scale(image_matrix, angle, scale_factor):
    pool = multiprocessing.Pool()

    # Define custom pickling functions for our functions
    def reduce_function(func):
        return getattr, (func.__module__, func.__name__)

    copyreg.pickle(types.FunctionType, reduce_function)

    # Rotate the image
    rotated_image = pool.apply(rotate_image, (image_matrix, angle))

    # Scale the rotated image
    scaled_image = pool.apply(scale_image, (rotated_image, scale_factor))

    pool.close()
    pool.join()

    return scaled_image


def main():
    # Read the image file
    image_matrix, width, height = read_image("input_image.pgm")

    # Apply thresholding
    thresholded_image = apply_threshold(image_matrix, 128)  # Example threshold value

    # Rotate by 45 degrees and scale by a factor of 2
    rotated_scaled_image = parallel_rotate_scale(thresholded_image, 45, 2)

    # Output or further processing
    for row in rotated_scaled_image:
        print(row)


if __name__ == "__main__":
    main()