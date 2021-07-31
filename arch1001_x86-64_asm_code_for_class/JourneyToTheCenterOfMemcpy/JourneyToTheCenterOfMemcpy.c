// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdio.h>
#include <string.h>

#pragma pack(1)
typedef struct mystruct{
	int var1;

	// First try: Change from 4 to 12 (to reach transferring 16 bytes)
	// Second try: Change from 12 to 29 (to reach transferring above 32 bytes)
	// Third try: Change from 29 to 253 (to reach transferring above 256 bytes)
	// Forth try: Change from 253 to 8189 (to reach transferring above 0x2000 bytes)
	char var2[10000];
} mystruct_t;
#pragma

#define uint64 unsigned long long
uint64 main(){
	mystruct_t a, b;
	a.var1 = 0xFF;

	// Standard Library Calling 是一堆 jmp !!
	// 每個 Function 的位置則存在 Memory 中，jmp 會先拿一個 quadword 出來
	// 直接 jmp 過去
	memcpy(&b, &a, sizeof(mystruct_t)); 

	// memcpy 會根據 r8，也就是 size 的大小，決定要 jmp 到哪邊去執行，目前有 0xF bit, 0x20 bit 的分別
	return 0xAce0fBa5e;
} 
