#!/usr/bin/env python3

print('''#pragma once
#ifndef NYAA_RESOURCE_CUBE_KINDS_INL_H_
#define NYAA_RESOURCE_CUBE_KINDS_INL_H_

#define DEFINE_CUBE_KINDS(V) \\''')

f = open('assets/textures/cube.txt', 'r')
for line in f.readlines():
    if line[0] == '#' or line[0] == '-':
        continue
    line = line.strip('\n')
    items = line.split('|')
    print('    V(%s) \\' % (items[1].strip(' ')))

print('''
#endif // NYAA_RESOURCE_CUBE_KINDS_INL_H_''')