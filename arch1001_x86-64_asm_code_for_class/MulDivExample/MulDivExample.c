// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

// 這邊如果改成 long long 的話，cqo 會出現 (雖然說我不知道為啥拉 感覺很不需要)
// div -> idiv
#define uint64 unsigned long long
uint64 main(){
	uint64 a = 0xdefec7ed;
	a *= 0xde7ec7ab1e;

	// 正常來說 division 是用到 rdx:rax 來除除數，但如果今天用不到那麼大的範圍的話
	// compiler 會把 rdx 直接 zero out (xor rdx rdx)

	// rax -> quotient, rdx -> remainder
	a /= 0x2bad505ad;
	return a;
}