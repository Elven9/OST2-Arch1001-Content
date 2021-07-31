// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

int main(){
	int a = 0x1301;
	int b = 0x0100;

	// test -> compute and, set the flag then discard the result
	// cmp -> compute sub, set the flag then discard the result
	if(a & b){
		return 0x70dd1e;
	}
	else{
		// 沒有開優化的 compiler 居然在同個位置產生了兩個相同的 jmp，
		// 真的滿沒腦的XD

		// 同樣的，為什麼這邊會同時看到 and 跟 test，也是因為我們沒開 optimization
		// compiler 產出一堆笨笨的 code 拉
		return 0x707;
	}
}