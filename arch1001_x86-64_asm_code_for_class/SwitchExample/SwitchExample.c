// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdlib.h>
int main(int argc, char* argv[]) {
    int a = atoi(argv[1]);

    // switch 在 assembly 裡其實會占用到一個自己的空間，也許是因為
    // 沒有開優化的關係，這裡並不是直觀的像我之前所想的那樣，是直接
    // 拿 a 來比較

    // 還有個地方是，每個 case 其實是一個 cmp + je 的組合，case 裡面的東西會統一寫在
    // 所有 case 結束的下面

    // ----> 所以這代表，switch 如果沒有 break 的話，就會接續執行接下來 case 中的程式碼!!!!
    // 沒有 break 或 return 的話，就不會有 jmp 出現在 case 邏輯之間了!!!
    switch (a) {
    case 0:
        return 1;
    case 1:
        return 2;
    default:
        return 3;
    }
    return 0xfee1fed;
}