// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

short main(){
	int a;
	
	// 重點是這行，多了一個 initialization 的動作
	// 這邊終於引出了第一個指令會需要用到 rcx 當作 counter，看一次要寫幾個 byte 再決定 counter 是多少
	// 然後 rdi 當作寫入位置的地方了 (lea rax [b]; mov rdi rax;)
	// rax 當作要填入的數值 (xor rax rax)

	// 喔還有一個點，rdi 是 callee saved register，所以一進去 main 就先 push rdi 到 stack 上了
	short b[64] = {0};
	a = 0x5eaf00d;
	b[1] = (short)a;
	return b[1];
}