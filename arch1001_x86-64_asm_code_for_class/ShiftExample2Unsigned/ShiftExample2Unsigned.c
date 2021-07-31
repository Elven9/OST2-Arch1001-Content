#// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdlib.h>

int main(int argc, char** argv)
{
    // 因為是 unsigned int，所以 compiler 可以很輕鬆的直接把這些
    // 2 的倍數的乘除直接轉換成 shift，加速 program 的運算
    unsigned int a, b, c;
    a = atoi(argv[1]);
    b = a * 8;
    c = b / 16;
    return c;
}