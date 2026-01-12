#!/usr/bin/env python3
import argparse
import os
import struct
from PIL import Image

MAGIC = b"GOPB"
VERSION = 1

def to_gop_bgra_bytes(path: str, store_alpha_in_reserved: bool) -> tuple[int, int, bytes]:
  img = Image.open(path).convert("RGBA")
  w, h = img.size
  rgba = img.tobytes()  # RGBA order

  out = bytearray(w * h * 4)
  for i in range(0, len(rgba), 4):
    r = rgba[i + 0]
    g = rgba[i + 1]
    b = rgba[i + 2]
    a = rgba[i + 3]
    out[i + 0] = b
    out[i + 1] = g
    out[i + 2] = r
    out[i + 3] = a if store_alpha_in_reserved else 0

  return w, h, bytes(out)


def write_gopb_bin(out_path: str, w: int, h: int, pixels: bytes, alpha: bool):
  flags = 1 if alpha else 0

  # < = little-endian
  # 4s  = magic
  # H   = version (uint16)
  # H   = flags   (uint16)
  # I   = width   (uint32)
  # I   = height  (uint32)
  header = struct.pack("<4sHHII", MAGIC, VERSION, flags, w, h)

  with open(out_path, "wb") as f:
    f.write(header)
    f.write(pixels)


def main():
  ap = argparse.ArgumentParser(description="Convert PNG/WebP to GOPB .bin (header + BGRA payload)")
  ap.add_argument("input", help="Input image (png/webp/...)")
  ap.add_argument("-o", "--output", help="Output .bin (default: <input>.gopb.bin)")
  ap.add_argument("--alpha", action="store_true", help="Store alpha in Reserved byte (otherwise Reserved=0)")
  args = ap.parse_args()

  in_path = args.input
  out_path = args.output
  if not out_path:
    base = os.path.splitext(os.path.basename(in_path))[0]
    out_path = base + ".gopb.bin"

  w, h, pixels = to_gop_bgra_bytes(in_path, args.alpha)
  write_gopb_bin(out_path, w, h, pixels, args.alpha)

  print(f"Wrote {out_path}: {w}x{h}, {len(pixels)} bytes payload, total {16+len(pixels)} bytes")


if __name__ == "__main__":
  main()
