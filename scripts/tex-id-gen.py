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
            print('..                  | %s | %s | %s | ' % (id, boundText, indexText))
            id = id + 10
            index = index + 1
            

if sys.argv[1] == 'tile':
    genTileTexId(int(sys.argv[2]), int(sys.argv[3]), int(sys.argv[4]), int(sys.argv[5]))