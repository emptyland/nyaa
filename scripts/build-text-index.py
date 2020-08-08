#!/usr/bin/env python3

print('''#pragma once
#ifndef NYAA_RESOURCE_TEXT_ID_H_
#define NYAA_RESOURCE_TEXT_ID_H_

namespace nyaa {
namespace res {

enum TextID {''')

f = open('assets/text/en_US.txt', 'r')
for line in f.readlines():
    if line[0] == '#' or line[0] == '-':
        continue
    line = line.strip('\n')
    items = line.split('|')
    print('    %s, // %s' % (items[0].strip(' '), items[1].strip(' ')))
    #print(items[0].strip(' '), items[1].strip(' '))

print('''}; // enum TextID

} // namespace res
} // namespace nyaa

#endif // NYAA_RESOURCE_TEXT_ID_H_''')