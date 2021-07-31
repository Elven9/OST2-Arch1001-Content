# Reverse Engineering Memcpy

Content: Memcpy, rep movs
Created: July 24, 2021 9:02 AM
Last Modified: July 24, 2021 4:18 PM

# Source Code

[課程提供的程式碼](https://gitlab.com/opensecuritytraining/arch1001_x86-64_asm_code_for_class) Journey to the Center of the Memcpy

```c
// This Source Code Form is subject to the terms of 
// the Mozilla Public License, v. 2.0. If a copy of 
// the MPL was not distributed with this file, You can 
// obtain one at https://mozilla.org/MPL/2.0/.

#include <stdio.h>
#include <string.h>

#pragma pack(1)
typedef struct mystruct{
	int var1;
	char var2[4];
} mystruct_t;
#pragma

#define uint64 unsigned long long
uint64 main(){
	mystruct_t a, b;
	a.var1 = 0xFF;

	memcpy(&b, &a, sizeof(mystruct_t)); 

	return 0xAce0fBa5e;
}
```

主要目標是在 memcpy 的 library code 中找到 `rep movs` 。教授提示說可以透過修改 struct 的內容來調整 memcpy 的執行邏輯，已找到我們的目標 instruction。

# Memcpy Argument

其實根據 x86-64 的 calling convension 就可以知道了，但這邊簡單速記一下﹔

- rcx → Destination
- rdx → Source
- r8   → Size of content in bytes

# Standard Library 的呼叫流程

Standard Library Calling 是一堆 jmp !!

```
// Visiual Studio 中把符號選項打開後的結果
0000000140002E62  jmp         qword ptr [__imp_memcpy (01400040A8h)]  
0000000140002E68  jmp         qword ptr [__imp___C_specific_handler (0140004090h)]  
0000000140002E6E  jmp         qword ptr [__imp___std_type_info_destroy_list (0140004088h)]  
0000000140002E74  jmp         qword ptr [__imp___current_exception (0140004098h)]  
0000000140002E7A  jmp         qword ptr [__imp___current_exception_context (01400040A0h)]  
0000000140002E80  jmp         qword ptr [__imp__seh_filter_exe (0140004178h)]  
0000000140002E86  jmp         qword ptr [__imp__set_app_type (0140004170h)]  
0000000140002E8C  jmp         qword ptr [__imp___setusermatherr (0140004168h)]  
0000000140002E92  jmp         qword ptr [__imp__configure_narrow_argv (0140004160h)]  
0000000140002E98  jmp         qword ptr [__imp__initialize_narrow_environment (0140004158h)]  
0000000140002E9E  jmp         qword ptr [__imp__get_initial_narrow_environment (0140004150h)]

// ....
```

在 caller 程式碼中準備好 argument 後，會透過 `call 0000000140002E62` 直接跳到上面 jmp 指令的位置，而從 memory 中提取出來的數值則是 memcpy 的 instruction 位置。

# 根據 Transfer Size 來決定執行邏輯

memcpy 會根據所要搬運的大小，分為幾個 level 來決定相對應的程式邏輯﹔

```
00007FFAF27412FA  cmp         r8,0Fh  
00007FFAF27412FE  ja          00007FFAF2741410
// 小於 16 bytes 邏輯在這
```

## r8 < 0xF 的狀況

會根據 R8 大小不同而從記憶體中存取不同位置的 instruction 已執行相對應的邏輯﹔

```
00007FFAF27412F3  lea         r10,[7FFAF2740000h]

// ....

00007FFAF2741310  mov         r9d,dword ptr [r10+r8*4+28000h]  
00007FFAF2741318  add         r9,r10  
00007FFAF274131B  jmp         r9
```

記憶體內容﹔

```
// Memory Position  Content
0x00007FFAF2768000  000014040000131e  
0x00007FFAF2768008  0000138f00001358  
0x00007FFAF2768010  000013ef0000140a  
0x00007FFAF2768018  00001360000013e0  
0x00007FFAF2768020  000013c5000013fd  
0x00007FFAF2768028  00001340000013b6  
0x00007FFAF2768030  000013a0000013d3  
0x00007FFAF2768038  0000132000001378
```

舉幾個例子﹔

- **r8 = 0 (Move Nothing)** → r9 = 0x7ffaf274131e, The only instruction there is `ret` , do nothing.
- **r8 = 1 (Move 1 byte)** → r9 = 0x7ffaf2741404, The instruction extract only one byte from memory point by `rdx` , and store it back to memory position point by `rcx` .

    ```
    00007FFAF2741404  movzx       ecx,byte ptr [rdx]  
    00007FFAF2741407  mov         byte ptr [rax],cl  
    00007FFAF2741409  ret
    ```

- **r8 = 2, 4, 8 (Move 2, 4, 8 bytes)** → Just like r8 = 1, the transfer can be done in three instructions.
- **Rest of possible values (Move below 16 bytes)** → It's just multiple combination of moving qword, dword, word, byte to transfer data from source (`rdx`) to destination (`rcx`)

    ```
    // Transferring 6 bytes for instance 
    00007FFAF27413E0  mov         ecx,dword ptr [rdx]  
    00007FFAF27413E2  movzx       r8d,word ptr [rdx+4]  
    00007FFAF27413E7  mov         dword ptr [rax],ecx  
    00007FFAF27413E9  mov         word ptr [rax+4],r8w  
    00007FFAF27413EE  ret
    ```

## r8 ≤ 0x20 的狀況

程式會直接使用 128 bit 的 `xmm` 系列 registers (SSE 支援) 直接處理掉﹔

```
00007FFAF2741416  movdqu      xmm1,xmmword ptr [rdx]  
00007FFAF274141A  movdqu      xmm2,xmmword ptr [rdx+r8-10h]  
00007FFAF2741421  movdqu      xmmword ptr [rcx],xmm1  
00007FFAF2741425  movdqu      xmmword ptr [rcx+r8-10h],xmm2  
00007FFAF274142C  ret
```

TODO: 這裡有個問題，我不太確定為什麼他會需要多移一個 `rdx+r8-0x10`，這邊需要再確認一下

⇒ 馬上就想懂了XD，因為這個區段寫死最多可以搬兩個 128 bit 的東西啊

[XMMWORD | Microsoft Docs](https://docs.microsoft.com/en-us/cpp/assembler/masm/xmmword?view=msvc-160)

## 其他狀況

- **rdx ≥ rcx (source 的 memory 位置高於 Destination)**
    - **rdx + r8 ≥ rcx (???，接下來還有一大堆的 jmp，同時似乎有些動作可以省略)**

    這邊是在處理重疊的問題，因為 dst 跟 src 重疊的話可能會有大問題

- **[0x7FFAEC615018] < 3 (??? 是個 Flag ???)**
- **r8 ≤ 0x2000 (大小不超過 0x2000)** → Transfer with ymm (256-bit SIMD Support) to transfer first 256 bit and last 256 bit data
    - **Next Check r8 ≤ 0x100 →** Same as r8 < 0xF implementation

        ```
        // plus 0x1F to r8 and mask out last five bit present in r8 and then 
        // shift right by 5 bit
        00007FFAEC6E1548  lea         r9,[r8+1Fh]  
        00007FFAEC6E154C  and         r9,0FFFFFFFFFFFFFFE0h  
        00007FFAEC6E1550  mov         r11,r9  
        00007FFAEC6E1553  shr         r11,5  

        // Same method as r8 < 0xF
        00007FFAEC6E1557  mov         r11d,dword ptr [r10+r11*4+28040h]  
        00007FFAEC6E155F  add         r11,r10  
        00007FFAEC6E1562  jmp         r11
        ```

        這邊也是一樣用指令組合的方式來搬，只是這邊使用的是 `ymm` 系列的 register，所以單位才會需要改成 256 bit 來看!!

    - **r8 > 0x100 →** TODO: 這邊我想不出來一個合理的解釋，為什麼要做一些 predefined 的加減，而且還動到 `rcx`, `rdx`, `r8` 了

        ```
        00007FFAF274146C  vmovdqu     ymm0,ymmword ptr [rdx]  
        00007FFAF2741470  vmovdqu     ymm5,ymmword ptr [rdx+r8-20h]

        // ...

        00007FFAF2741484  mov         r9,rcx  
        00007FFAF2741487  and         r9,1Fh  
        00007FFAF274148B  sub         r9,20h  
        00007FFAF274148F  sub         rcx,r9  
        00007FFAF2741492  sub         rdx,r9  
        00007FFAF2741495  add         r8,r9
        ```

        Assumption 1: 感覺是為了 loop 的問題，開頭的 32 bytes 可能在這兩行已經處理過了，後面 ymm0 也會搬過去，所以這邊要先減掉 (因為 stack 是往低位長，但 reference 範圍是往上 reference 的，所以這邊故意多了一個 `sub r9, 20h` 讓數值變負數)

        ```
        // 找到了，處理尾數的部分
        // 這有點像是，因為前面的寫法開頭尾巴可能會有小東西寫不了
        // 所以多寫一個搬運，直接把完整的 32 bytes 頭尾在覆蓋過去一次
        00007FFAF274146C  vmovdqu     ymm0,ymmword ptr [rdx]  
        00007FFAF2741470  vmovdqu     ymm5,ymmword ptr [rdx+r8-20h]
        ```

- **扣掉 0x1F 的資料後，只要大於 0x100 bytes** → 透過寫死的編排，透過大量的 loop 來搬運數值
    - 他會拿 2 先跟某個位置 (0x7FFAF27653BC) 上的值做 `test` ，如果對的話就 jmp 到上面的 instruction。但這個位置上什麼都沒有啊??? 但我們的目標 rep movs 在上面的 instruction 那邊 QQ

        ```
        00007FFAF29912D0  push        rdi  
        00007FFAF29912D1  push        rsi  
        00007FFAF29912D2  mov         rdi,rcx  
        00007FFAF29912D5  mov         rsi,rdx  
        00007FFAF29912D8  mov         rcx,r8  
        00007FFAF29912DB  rep movs    byte ptr [rdi],byte ptr [rsi]  
        00007FFAF29912DD  pop         rsi  
        00007FFAF29912DE  pop         rdi  
        00007FFAF29912DF  ret
        ```

        TODO: 教授上課的影片中是有值的，但我沒辦法 reproduce 出來，所以估計是版本問題吧??

        Future Works: 這裡有人討論了完整一整篇 `rep movs` 好強

        [https://stackoverflow.com/questions/43343231/enhanced-rep-movsb-for-memcpy](https://stackoverflow.com/questions/43343231/enhanced-rep-movsb-for-memcpy)

    - test 沒有過的話，會進到接下來的 Loop，透過對 `ymm1` ~ `ymm4` 的使用，每個 loop 重複兩次，一次 loop 下來可以搬運 256 bytes 的資料