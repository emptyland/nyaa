#!/usr/bin/env python3

print('''#pragma once
#ifndef NYAA_RESOURCE_TEXT_IDS_INL_H_
#define NYAA_RESOURCE_TEXT_IDS_INL_H_

namespace nyaa {
namespace res {

#define DEFINE_TEXT_KINDS(V) \\''')

f = open('assets/text/en_US.txt', 'r')
for line in f.readlines():
    if line[0] == '#' or line[0] == '-':
        continue
    line = line.strip('\n')
    items = line.split('|')
    print('    V(%s) \\' % (items[0].strip(' ')))

print('''

enum TextID {
#define DEFINE_ENUM(name) name,
    DEFINE_TEXT_KINDS(DEFINE_ENUM)
#undef DEFINE_ENUM
    MAX_TEXT_ID,
};  // enum TextID

} // namespace res
} // namespace nyaa

#endif // NYAA_RESOURCE_TEXT_IDS_INL_H_''')