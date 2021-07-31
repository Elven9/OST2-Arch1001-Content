// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdlib.h>

int main(int argc, char** argv)
{
	int a, b, c;

	// 這裡預設給的值是 4294967295，也就是 FFFFFFFF，但因為這邊的 a 是 signed int
	// 最終 atoi 傳回去給 rax 的值只剩 7FFFFFFF，直接給正數最大值，負數方向也是。
	// 所以 atoi 只要給個超過 32bit 範圍的數值，就會直接回傳最大值了
	a = atoi(argv[1]);
	b = a * 8;

	// 這邊有個奇怪的地方，compiler 自行加了一行 add eax, edx，就直接 overflow 了，
	// 不確定為什麼會這樣
	c = b / 16;
	return c;
}