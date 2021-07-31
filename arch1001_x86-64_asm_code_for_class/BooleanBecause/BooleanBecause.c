// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdio.h>
#define uint64 unsigned long long

unsigned long long main() {
    unsigned int i = 0x50da;
    unsigned int j = 0xc0ffee;
    uint64 k = 0x7ea707a11ed;

    // 這邊有個好玩的議題，像這邊根據 manual 來看，i & j 的結果會 auto convert 到 long long
    // 型別。因為是 unsigned 的關係，convert 上去時並不是 sign extend 喔，根據規格書 6.3.1.8
    // unsigned int 在 greater or "equal" rank 的狀況下，會被自動 convert 上去

    // 所以如果今天 i 和 j 都是 signed int，到時候 conversion 到 long long 會先發生，
    // k 的結果會是一個很大的數字
    k ^= ~(i & j) | 0x7ab00;
    return k;
}