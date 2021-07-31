// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

short main() {
    short a;
    int b[6];
    long long c;
    a = 0xbabe;
    c = 0xba1b0ab1edb100d;
    b[1] = a;

    // 當在執行這行時，b[1] + c 因為 c 是 long long，也就是 quadword，所以
    // add 的時候是用 rax 在計算，最後放回去 b 時只使用 eax，達到 truncate 的效果，
    // 最後也只放到 dword 的位置上
    b[4] = b[1] + c;

    // Currently the optimization from compiler had been disabled. 
    // The code produced by compiler is kind of stupid lol.
    return b[4];
}
