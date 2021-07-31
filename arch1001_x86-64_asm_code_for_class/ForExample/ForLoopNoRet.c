// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdio.h>

int main(){
   int i;
   // i 的值在沒有優化的時候，會一直存回去 memory 中，每次 inc 時會提到 reg 裡
   for(i = 0; i < 10; i++){
      printf("i = %d\n", i);
   }

   // 加這行是故意要 show 出 dec 這個 assembly instruction
   i--;

   // 這裡我想到一個問題，沒有 return 值會有 undefined 行為，進而導致使用 status 的
   // Program 壞掉的原因，就是因為 rax 根本就沒有設阿，搞不好還是剛剛用過的值

   // 但 compiler 現在都會幫你自動加上一行 xor eax eax 了，直接 return 0
}