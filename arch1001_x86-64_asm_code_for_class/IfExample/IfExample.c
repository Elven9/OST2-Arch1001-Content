// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

// 每個 conditional jump 都會跟 code 上寫的相反
// 原因很簡單，因為 asm 是不成功往下執行，但 code 是成功往下執行
int main(){
	int a = -1, b = 2;

	// 沒有開優化的 code 這邊每個 if 都會 load 資料進 eax，
	// 然後都會下一次 cmp 去更新 RFLAGS，而且都是一個 register 跟 memory 裡的
	// 東西去比較，有夠浪費效能的
	if(a == b){
		return 1;
	}
	if(a > b){
		return 2;
	}
	if(a < b){
		return 3;
	}
	return 0xdefea7;
}