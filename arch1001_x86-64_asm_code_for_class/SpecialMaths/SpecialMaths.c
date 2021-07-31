// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdlib.h>

int main(int argc, char** argv) {
    int a;
    //reminder: atoi() converts an
    //ASCII string to an integer

    // ecx -> argc 也就是 parameter 的數量
    // rdx -> 裡面是 argv 的位置，對應到記憶體中的內容，就是那個字串的位置
    a = atoi(argv[1]);
    return 2 * argc + a;
}