import os
from PIL import Image

# Use current working directory
input_dir = os.getcwd()

# Loop through all PNG files in the directory
for filename in os.listdir(input_dir):
    if filename.lower().endswith(".png"):
        filepath = os.path.join(input_dir, filename)
        with Image.open(filepath) as img:
            width, height = img.size

            # Calculate the coordinates for central 150x150 crop
            left = (width - 560) // 2
            upper = (height - 560) // 2
            right = left + 560
            lower = upper + 560

            # Crop and rotate
            cropped = img.crop((left, upper, right, lower))
            rotated = cropped.rotate(90, expand=True)  # 90 degrees counter-clockwise

            # Overwrite original file
            rotated.save(filepath)

print("All PNG files processed and overwritten.")
