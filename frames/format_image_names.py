import os
import re

def rename_png_files(root_dir):
    pattern = re.compile(r'(\d{2})\.png$', re.IGNORECASE)

    for dirpath, _, filenames in os.walk(root_dir):
        for filename in filenames:
            if filename.lower().endswith('.png'):
                match = pattern.search(filename)
                if match:
                    new_name = f"{match.group(1)}.png"
                    old_path = os.path.join(dirpath, filename)
                    new_path = os.path.join(dirpath, new_name)

                    if old_path != new_path and not os.path.exists(new_path):
                        os.rename(old_path, new_path)
                        print(f"Renamed: {old_path} -> {new_path}")
                    elif os.path.exists(new_path):
                        print(f"Skipped (conflict): {new_path}")
                else:
                    print(f"Skipped (no 2-digit number): {filename}")

# Example usage
rename_png_files('/Users/tomraynes/Desktop/MSc CS/Programming in C/Projects/Orbit/frames')
