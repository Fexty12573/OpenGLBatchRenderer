from cubemap_splitter import split_cubemap
from pathlib import Path
from PIL import Image
import sys


def get_closest_power_of_2(num: int) -> int:
    return 2 ** (num - 1).bit_length()

def main():
    if len(sys.argv) < 2:
        print("Usage: cubemap_tool.py <cubemap_file>")
        return

    path = Path(sys.argv[1])
    output_path = path.parent / path.stem
    if not output_path.exists():
        output_path.mkdir()
    
    format = 1 if len(sys.argv) < 3 else int(sys.argv[2])
    split_cubemap(sys.argv[1], format, str(output_path))

    # Resize all outputs to be square and a power of 2
    for file in output_path.glob("*.png"):
        img = Image.open(file)
        wh = get_closest_power_of_2(max(img.size))
        img = img.resize((wh, wh), Image.Resampling.BICUBIC)
        img.save(file)

if __name__ == "__main__":
    main()
