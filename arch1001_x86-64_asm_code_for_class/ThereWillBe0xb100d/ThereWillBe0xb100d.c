// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

int main(){
	// 這份 code 居然 generate 出 rep stos 了，他把整個 array 都寫入 0xC
	char buf[40];
	buf[44] = 42;
	// 在執行完 buf[39]=42 還多出了一個 checkStackVal 的 call 動作?

	// 綜合上面兩個發現，以及故意把 idx 改成 41 強迫他 Overflow，可以發現
	// 那些多出來的 0xC，就是 check overflow 的機制拉

	// 但顯然這個機制滿基本的，試驗結果來看只要 index 超過 43 就偵測不到了
	// 而且他的 0xC 的範圍也只有寫 0x10 * dword 的長度而已，遠一點的肯定找不到了

	// Basic Runtime Check
	// 如果去 visual code 「基礎執行階段檢查」把選項改成預設的話，就不會有這些多產出來的 code 了
	// 也就是說，這不是一個 security check 的 feature，production 時通常會關掉
	return 0xb100d;
}