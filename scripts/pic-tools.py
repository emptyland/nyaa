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

font = ImageFont.truetype("/Users/niko/Documents/nyaa/assets/PingFang.ttc", 12)
draw = ImageDraw.Draw(dest)
for j in range(0, cy):
    for i in range(0, cx):
        draw.text((i * tileSize, j * tileSize), "%d,%d" % (i, j), font=font, fill=256)
dest.save('dest.png')

# picks = [
#     [2,3,6,7,8,9,10,11,12,13,18,19,22,23,24,25,27,28,29,],
#     [2,3,6,7,8,9,10,11,12,13,28,29],
#     [6,7,8,9,12,13,18,19,22,23,24,29,30],
#     [3,7,18,19,22,23,25,28,29,30,31],
#     [6,7,8,28,29,30],
#     [2,3,6,7,10,11,12,13,17,18,19,20,22,23,24,25,27,28,29],
#     [6,18,19,22,23,26,27,28,29],
#     [8,9,10,11,13,14,15,26,27,28],
#     [8,9,12,18,19,22,23,24,25,26,28,29,30],
#     [2,3,6,7,8,9,10,12,13,18,19,20,24,25,26,27,28,29],
#     [2,3,4,8,9,10,11,12,13,18,19,20],
#     [2,3,4,15,18,19,20,22,23,24,25,28,29,30,31],
#     [2,3,8,9,10,12,13,14,15,17,18,19,20,24,25,26,27,28,29],
#     [2,6,9,12,13,18,19,22,23,24,25,27,29,30,31],
#     [0,1,2,3,4,5,8,9,10,11,23,24,27,29,30,31],
#     [25,27],
#     [12,13,22,23,24,25,26,27,28,29,30,31],
#     [2,9,10,11,12,13,15,18,19,20,24,25,26,27,28,29],
#     [2,3,6,7,8,9,10,11,12,13,14,15,18,19,22,23,24,25,26,27,28,29,30,31],
#     [8,9,10,11,12,13,14,15,18,19,22,23,24,25,26,27,28,29,30,31],
#     [14,15,18,19,20,22,23,24,25,28,29,30,31],
#     [2,3,12,13,24,25,26,27,28,30,31],
#     [8,9,10,12,14,19,23,24,25,28,29,30],
#     [6,7,829,30,31],
#     [10,11,12,14,15,18,26,28],
#     [2,3,7,11,13,14,19,30,31],
#     [18,19,22,23,26,27,28,29],
#     [23],
#     [23,26,27,29,30,31],
#     [2,6,7,8,14,15,18,28,29],
#     [25,26,28,29],
#     [2,3,6,7,8,9,10,11,26,27,28,29],
#     [2,3,6,7,8,9,10,11,26,27,28,29],
#     [2,12,13,25,26,27,28,29],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [],
#     [3,4,6,7,8,9,10,11,12,13,14,19,22,23,24,25,26,27,28,29,30],
#     [],
#     [],
#     [6,7],
#     [6,7,22,23],
#     [2,3,7,8],
#     [2,3],
#     [],
#     [],
#     [20,28,29],
#     [31],
#     [],
# ]