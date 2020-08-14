#!/usr/bin/env python3

from PIL import Image, ImageFont, ImageDraw
import sys

tileSize = 48

tiles = []

def cropTile(img):
    x = 0
    y = 16
    for i in range(0, 4):
        for j in range(0, 4):
            box = (x, y, x + tileSize, y + tileSize)
            region = img.crop(box)
            tiles.append(region)
            x = x + tileSize
        x = 0
        y = y + 16 + tileSize

if len(sys.argv) < 1:
    exit(1)

rawFiles = []
f = open(sys.argv[1], 'r')
for line in f.readlines():
    rawFiles.append(line.strip('\n'))

for fileName in rawFiles:
    print(fileName)
    img = Image.open(fileName)
    print(img.format, img.size, img.mode)
    cropTile(img)

print(len(tiles))

tileCount = len(tiles)
cx = 0
cy = 0
if tileCount > 32:
    cx = 32
    cy = int(tileCount / cx)
else:
    cx = tileCount
    cy = 1

dest = Image.new('RGBA', (cx * tileSize, cy * tileSize))
for j in range(0, cy):
    for i in range(0, cx):
        dest.paste(tiles[j * cx + i], (i * tileSize, j * tileSize))

font = ImageFont.truetype("../assets/PingFang.ttc", 12)
draw = ImageDraw.Draw(dest)
for j in range(0, cy):
    for i in range(0, cx):
        draw.text((i * tileSize, j * tileSize), "%d,%d" % (i, j), font=font, fill=256)
dest.save('dest.png')
