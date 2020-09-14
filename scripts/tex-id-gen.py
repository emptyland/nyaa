#!/usr/bin/env python3

import sys

if len(sys.argv) < 2:
    exit(1)


# ..                  | 200040 | 256,0,64,64         | 4     |

def genTileTexId(id, cx, cy, size):
    print('------------------------------------------------------------------------------------------------------------------------')
    index = 0
    for j in range(0, cy):
        for i in range(0, cx):
            bound = (i * size, j * size, size, size)
            boundText = ('%d,%d,%d,%d' % bound).ljust(19)
            indexText = ('%d' % (index)).ljust(5)
            print('..                  | %s | %s | %s | %d,%d' % (id, boundText, indexText, i, j))
            id = id + 10
            index = index + 1

avatarDirs = [
    'avatar up',
    'avatar right',
    'avatar down',
    'avatar left',
]
# 24 x 32
def genAvatarTexId(id, cx, cy):
    print('------------------------------------------------------------------------------------------------------------------------')
    for x in range(0, cx * 12, cx * 3):
        for y in range(0, cy * 8, cy * 4):
            for j in range(0, 4):
                index = 0
                for i in range(0, 3):
                    bound = (x + i * cx, y + j * cy, cx, cy)
                    boundText = ('%d,%d,%d,%d' % bound).ljust(19)
                    indexText = ('%d' % (index)).ljust(5)
                    print('..                  | %s | %s | %s | %s' % (id, boundText, indexText, avatarDirs[j]))
                    id = id + 10
                    index = index + 1
    return id

# 100000 | 24,32   | 101020,101010,101030 | 101050,101040,101060 | 101080,101070,101090 | 101110,101100,101120 |  0.2    | 1,1,1,1    | boy-1
def genAvatarSpriteId(id, aid, cx, cy):
    u = aid + 0
    r = aid + 30
    d = aid + 60
    l = aid + 90
    upText = '%d,%d,%d' % (u, u + 10, u + 20)
    rightText = '%d,%d,%d' % (r, r + 10, r + 20)
    downText = '%d,%d,%d' % (d, d + 10, d + 20)
    leftText = '%d,%d,%d' % (l, l + 10, l + 20)
    sizeText = ('%d,%d' % (cx, cy)).ljust(7)
    print('%d | %s | %s | %s | %s | %s | 0.2     | 1,1,1,1    | ' % (id, sizeText, upText, rightText, downText, leftText))
    return id + 10

if sys.argv[1] == 'tile':
    genTileTexId(int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
elif sys.argv[1] == 'item':
    # 16 x 40 x 24
    genTileTexId(int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))
elif sys.argv[1] == 'avatar':
    id = int(sys.argv[2])
    for i in range(0, int(sys.argv[5])):
        id = genAvatarTexId(id, int(sys.argv[3]), int(sys.argv[4]))
elif sys.argv[1] == 'avatar.sprite':
    id = int(sys.argv[2])
    cx = int(sys.argv[3])
    cy = int(sys.argv[4])
    aid = int(sys.argv[5])
    for i in range(0, int(sys.argv[6])):
        id = genAvatarSpriteId(id, aid, cx, cy)
        aid = aid + 120

