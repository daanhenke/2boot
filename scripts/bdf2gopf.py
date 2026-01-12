#!/usr/bin/env python3
import argparse
import math
import os
import struct
from dataclasses import dataclass
from typing import Dict, List, Optional, Tuple

from PIL import Image

# Hardcoded, predictable ASCII atlas
FIRST = 0x20
LAST = 0x7E
COLS = 16
ROWS = 6  # ceil((0x7E-0x20+1)/16) = 6

MAGIC = b"GOPF"
VERSION = 1
FLAG_ALPHA_IN_RESERVED = 1  # always on for font mask atlases


@dataclass
class Glyph:
  codepoint: int
  bbx_w: int
  bbx_h: int
  bbx_xoff: int
  bbx_yoff: int
  dwidth_x: int
  bitmap_rows: List[int]  # each row bits, MSB-first within padded bytes


def parse_bdf(path: str) -> Tuple[Dict[int, Glyph], Tuple[int, int, int, int], int, int]:
  """
  Returns:
    glyphs: codepoint -> Glyph
    font_bbx: (w,h,xoff,yoff) from FONTBOUNDINGBOX
    ascent, descent
  """
  glyphs: Dict[int, Glyph] = {}
  font_bbx = (0, 0, 0, 0)
  ascent = 0
  descent = 0

  with open(path, "r", encoding="utf-8", errors="replace") as f:
    lines = [ln.rstrip("\n") for ln in f]

  i = 0
  while i < len(lines):
    ln = lines[i].strip()

    if ln.startswith("FONTBOUNDINGBOX"):
      _, w, h, xoff, yoff = ln.split()
      font_bbx = (int(w), int(h), int(xoff), int(yoff))
    elif ln.startswith("FONT_ASCENT"):
      ascent = int(ln.split()[1])
    elif ln.startswith("FONT_DESCENT"):
      descent = int(ln.split()[1])

    if ln.startswith("STARTCHAR"):
      codepoint = -1
      bbx_w = bbx_h = bbx_xoff = bbx_yoff = 0
      dwidth_x = 0
      bitmap_hex: List[str] = []
      in_bitmap = False

      i += 1
      while i < len(lines):
        ln2 = lines[i].strip()
        if ln2 == "ENDCHAR":
          break
        if ln2.startswith("ENCODING"):
          codepoint = int(ln2.split()[1])
        elif ln2.startswith("BBX"):
          _, w, h, xoff, yoff = ln2.split()
          bbx_w, bbx_h, bbx_xoff, bbx_yoff = map(
            int, (w, h, xoff, yoff))
        elif ln2.startswith("DWIDTH"):
          parts = ln2.split()
          dwidth_x = int(parts[1]) if len(parts) > 1 else 0
        elif ln2 == "BITMAP":
          in_bitmap = True
        elif in_bitmap and ln2:
          bitmap_hex.append(ln2)
        i += 1

      if codepoint >= 0:
        bytes_per_row = (bbx_w + 7) // 8
        rows: List[int] = []
        for row_hex in bitmap_hex[:bbx_h]:
          row_hex = row_hex.strip().zfill(bytes_per_row * 2)
          rows.append(int(row_hex, 16))
        while len(rows) < bbx_h:
          rows.append(0)

        glyphs[codepoint] = Glyph(
          codepoint=codepoint,
          bbx_w=bbx_w,
          bbx_h=bbx_h,
          bbx_xoff=bbx_xoff,
          bbx_yoff=bbx_yoff,
          dwidth_x=dwidth_x,
          bitmap_rows=rows,
        )

    i += 1

  return glyphs, font_bbx, ascent, descent


def blit_glyph_to_mask(mask: Image.Image,
                      cell_x: int, cell_y: int,
                      cell_w: int, cell_h: int,
                      g: Optional[Glyph],
                      font_bbx: Tuple[int, int, int, int],
                      ascent: int) -> None:
  """
  Draw the glyph as an 8-bit mask into `mask` (mode 'L').

  Baseline is placed at y = cell_y + ascent.
  BBX yoff is offset from baseline to the bottom of the BBX.
  """
  if g is None or g.bbx_w <= 0 or g.bbx_h <= 0:
    return

  fb_w, fb_h, fb_xoff, fb_yoff = font_bbx

  baseline_y = cell_y + ascent
  dst_x = cell_x + (g.bbx_xoff - fb_xoff)
  dst_y = baseline_y - (g.bbx_yoff + g.bbx_h)

  cell_right = cell_x + cell_w
  cell_bottom = cell_y + cell_h

  px = mask.load()
  bytes_per_row = (g.bbx_w + 7) // 8
  row_bits_width = bytes_per_row * 8  # padded to byte boundary

  for row in range(g.bbx_h):
    row_bits = g.bitmap_rows[row]
    for col in range(g.bbx_w):
      bit_index = (row_bits_width - 1) - col  # MSB-first
      if (row_bits >> bit_index) & 1:
        x = dst_x + col
        y = dst_y + row
        if cell_x <= x < cell_right and cell_y <= y < cell_bottom:
          px[x, y] = 255


def write_gopf(path: str, w: int, h: int, cell_w: int, cell_h: int, mask: bytes) -> None:
  """
  mask: alpha bytes length w*h, 0..255
  Writes BGRA payload where Reserved = mask, RGB = 255.
  """
  if len(mask) != w * h:
    raise ValueError("mask size mismatch")

  # Build BGRA pixels
  out = bytearray(w * h * 4)
  for i in range(w * h):
    j = i * 4
    a = mask[i]
    out[j + 0] = 255  # B
    out[j + 1] = 255  # G
    out[j + 2] = 255  # R
    out[j + 3] = a    # Reserved (mask)

  flags = FLAG_ALPHA_IN_RESERVED
  header = struct.pack("<4sHHIIHH", MAGIC, VERSION,
                      flags, w, h, cell_w, cell_h)

  with open(path, "wb") as f:
    f.write(header)
    f.write(out)


def main():
  ap = argparse.ArgumentParser(description="BDF -> GOPF (GOPB-like + cell size), fixed ASCII 0x20..0x7E in 16x6 grid.")
  ap.add_argument("input_bdf", help="Input .bdf")
  ap.add_argument("-o", "--output", help="Output .bin (default: <bdfname>.gopf)")
  ap.add_argument("--webp", nargs="?", const=True, default=False, help="Also emit a lossless WebP atlas (optional path)")
  args = ap.parse_args()

  in_path = args.input_bdf
  if args.output:
    out_bin = args.output
  else:
    base = os.path.splitext(os.path.basename(in_path))[0]
    out_bin = base + ".gopf"

  glyphs, font_bbx, ascent, descent = parse_bdf(in_path)
  fb_w, fb_h, fb_xoff, fb_yoff = font_bbx
  if fb_w <= 0 or fb_h <= 0:
    raise SystemExit("BDF is missing a valid FONTBOUNDINGBOX; cannot derive cell size.")

  cell_w, cell_h = fb_w, fb_h
  atlas_w = COLS * cell_w
  atlas_h = ROWS * cell_h

  # Create alpha mask image
  mask_img = Image.new("L", (atlas_w, atlas_h), 0)

  for c in range(FIRST, LAST + 1):
    idx = c - FIRST
    cx = (idx % COLS) * cell_w
    cy = (idx // COLS) * cell_h
    blit_glyph_to_mask(mask_img, cx, cy, cell_w, cell_h,
                        glyphs.get(c), font_bbx, ascent)

  # Write GOPF bin
  write_gopf(out_bin, atlas_w, atlas_h, cell_w, cell_h, mask_img.tobytes())
  print(f"Wrote {out_bin}")
  print(f"Atlas {atlas_w}x{atlas_h}, cell {cell_w}x{cell_h}, layout ASCII 0x20..0x7E in 16x6 grid.")
  print("Runtime: idx=c-0x20; gx=(idx%16)*cell_w; gy=(idx/16)*cell_h; mask in Reserved byte.")

  # Optional WebP for inspection
  if args.webp:
    if args.webp is True:
      out_webp = os.path.splitext(out_bin)[0] + ".webp"
    else:
      out_webp = args.webp

    rgba = Image.new("RGBA", (atlas_w, atlas_h), (255, 255, 255, 0))
    rgba.putalpha(mask_img)
    rgba.save(out_webp, "WEBP", lossless=True, method=6)
    print(f"Wrote {out_webp} (lossless WebP)")


if __name__ == "__main__":
  main()
