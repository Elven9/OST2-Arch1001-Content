// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

// 把 pragma 打開後，assembly 並沒有太大的變化，只有每個數值的位置都不同了
// 全部都 pack 在一起了
#pragma pack(1)
typedef struct mystruct {
    // 這裡存進去的只有用 ax, 16bit 的資料，
    // data type size 舊制這樣來的 XD

    // stack 上的順序也會根據 declare 的順序來擺放
    // 跟之前得到的觀察一樣

    // short 也會有自己的 padding
    short a;
    int b[6];
    long long c;
} mystruct_t;
//#pragma

short main() {
    mystruct_t foo;
    foo.a = 0xbabe;
    foo.c = 0xba1b0ab1edb100d;
    foo.b[1] = foo.a;
    foo.b[4] = foo.b[1] + foo.c;

    // return short 的方法，用 movzx 一個 word 把不必要的資料都 truncate 掉
    return foo.b[4];
}
