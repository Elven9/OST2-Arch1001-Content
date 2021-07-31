# BoomLab

Content: BombLab!!, Lab
Created: July 29, 2021 4:57 PM
Last Modified: July 31, 2021 11:09 AM

---

官方文件連結﹔[Getting Started with WinDbg (User-Mode)](https://docs.microsoft.com/en-us/windows-hardware/drivers/debugger/getting-started-with-windbg)

Bomblab Executable Provider: [OpenSecurityTraining / Arch1001_x86-64_asm_code_for_class · GitLab](https://gitlab.com/opensecuritytraining/arch1001_x86-64_asm_code_for_class)

可以把 Windbg 的字體改成等寬字眼睛會舒服很多

# Main function

因為這次有給 pdb 檔，所以可以很輕鬆地就定位到 main 的位置﹔

```
bp main
```

`uf main` :

```
// Some initialization going on, including one char buffer for input

// 看起來這邊會讀入某個檔案當作 input
49 00007ff6`f2001a1a 4889054fee0000  mov     qword ptr [bomb!infile (00007ff6`f2010870)],rax

// initialize bomb ?
// Maybe there is a need to closely examine this function
00007ff6`f2001acd e8aaf6ffff      call    bomb!ILT+375(initialize_bomb) (00007ff6`f200117c)

// Found phase 1 function !!
00007ff6`f2001af7 e85bf8ffff      call    bomb!ILT+850(phase_1) (00007ff6`f2001357)
```

# Phase 1

`uf phase_1` :

```
bomb!phase_1 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 23]:
   23 00007ff6`f2002010 48894c2408      mov     qword ptr [rsp+8],rcx
   23 00007ff6`f2002015 55              push    rbp
   23 00007ff6`f2002016 57              push    rdi
   23 00007ff6`f2002017 4881ece8000000  sub     rsp,0E8h
   23 00007ff6`f200201e 488d6c2420      lea     rbp,[rsp+20h]
   23 00007ff6`f2002023 488bfc          mov     rdi,rsp
   23 00007ff6`f2002026 b93a000000      mov     ecx,3Ah
   23 00007ff6`f200202b b8cccccccc      mov     eax,0CCCCCCCCh
   23 00007ff6`f2002030 f3ab            rep stos dword ptr [rdi]
   23 00007ff6`f2002032 488b8c2408010000 mov     rcx,qword ptr [rsp+108h]
15732480 00007ff6`f200203a 488d0dc93f0100  lea     rcx,[bomb!_NULL_IMPORT_DESCRIPTOR <PERF> (bomb+0x2600a) (00007ff6`f201600a)]
15732480 00007ff6`f2002041 e8a7f3ffff      call    bomb!ILT+1000(__CheckForDebuggerJustMyCode) (00007ff6`f20013ed)
   25 00007ff6`f2002046 488d15aba10000  lea     rdx,[bomb!`string' (00007ff6`f200c1f8)]
   25 00007ff6`f200204d 488b8de0000000  mov     rcx,qword ptr [rbp+0E0h]
   25 00007ff6`f2002054 e8d6f2ffff      call    bomb!ILT+810(strings_not_equal) (00007ff6`f200132f)
   25 00007ff6`f2002059 85c0            test    eax,eax
   25 00007ff6`f200205b 7405            je      bomb!phase_1+0x52 (00007ff6`f2002062)  Branch

bomb!phase_1+0x4d [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 26]:
   26 00007ff6`f200205d e854f3ffff      call    bomb!ILT+945(explode_bomb) (00007ff6`f20013b6)

bomb!phase_1+0x52 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 32]:
   32 00007ff6`f2002062 488da5c8000000  lea     rsp,[rbp+0C8h]
   32 00007ff6`f2002069 5f              pop     rdi
   32 00007ff6`f200206a 5d              pop     rbp
   32 00007ff6`f200206b c3              ret
```

紅字那行最重要，strings_not_equal 拿了兩個變數比較，rdx rcx 看起來是兩個 string 的位置。如果 return 的值是 0 的話就會略過 explode_bomb 這個 function!

下一步就是去看我們的 input 到底跟什麼比了，可以直接 `pc` 到 calling strings_not_equal 前，觀察 rdx, rcx 裡面記憶體位置上的值

```
0:000> da rcx
00007ff6`f2010200  "input"
0:000> da rdx
00007ff6`f200c1f8  "I am just a renegade hockey mom."
00007ff6`f200c218  ""
```

We just found our Answer!

# Phase 2

`uf phase_2` :

```
// read six number ??
44 00007ff6`f20020e2 e8ebefffff      call    bomb!ILT+205(read_six_numbers) (00007ff6`f20010d2)
```

實際 step over 後，read_six_numbers 裡感覺也有 bomb_explode，所以要先處理 read_six_numbers 這個 function

## read_six_numbers

rcx → address of user input string

rdx → ???

read_six_numbers 主要功能，會從 rbp+108h 讀出一個 quadword 的值，對其加加減減後存入每個 register

```
00007ff6`f2002ffb 488b8508010000  mov     rax,qword ptr [rbp+108h]
00007ff6`f2003002 4883c014        add     rax,14h
00007ff6`f2003006 488b8d08010000  mov     rcx,qword ptr [rbp+108h] ss:000000ba`1cd0f818=000000ba1cd0f858
00007ff6`f200300d 4883c110        add     rcx,10h
00007ff6`f2003011 488b9508010000  mov     rdx,qword ptr [rbp+108h]
00007ff6`f2003018 4883c20c        add     rdx,0Ch
00007ff6`f200301c 4c8b8508010000  mov     r8,qword ptr [rbp+108h]
00007ff6`f2003023 4983c008        add     r8,8
00007ff6`f2003027 4c8b8d08010000  mov     r9,qword ptr [rbp+108h]
00007ff6`f200302e 4983c104        add     r9,4

// 這裡感覺就是人手寫 assembly 產出來的 code，compiler 不會產出這麼好懂得 code 吧XD
```

數字為﹔

```
0:000> dq rbp+108h l1
000000ba`1cd0f818  000000ba`1cd0f858
```

接這把這些計算過後的值存入記憶體中，呼叫 sscanf。所以綜合以上觀察，前面那一串相加是在準備 sscanf 的 argument 啦，那個數字是記憶體中的位置，我們的 input 值是 read-in 的值，要給六個 decimal 數字

```
// r8 的值
00007ff6`f2003046 4c8b8508010000  mov     r8,qword ptr [rbp+108h]

// Command
0:000> da rcx
00007ff6`f2010250  "input"
0:000> da rdx
00007ff6`f200c460  "%d %d %d %d %d %d"
```

確保輸入 6 個數字後，可以避掉第一個 explode bomb !

## Back to Phase2

接下來 phase 2 會 check 輸入的數字第一個是否為 1，是的話就可以避掉 phase_2 第一個 explode_bomb

```
00007ff6`f20020f0 837c052801      cmp     dword ptr [rbp+rax+28h],1
```

接下來是計算第二個數字﹔

```
bomb!phase_2+0x75 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 49]:
   49 00007ff6`f2002105 8b4504          mov     eax,dword ptr [rbp+4]
   49 00007ff6`f2002108 ffc0            inc     eax
   49 00007ff6`f200210a 894504          mov     dword ptr [rbp+4],eax

bomb!phase_2+0x7d [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 49]:
   49 00007ff6`f200210d 837d0406        cmp     dword ptr [rbp+4],6
   49 00007ff6`f2002111 7d1f            jge     bomb!phase_2+0xa2 (00007ff6`f2002132)

50 00007ff6`f2002113 48634504        movsxd  rax,dword ptr [rbp+4]
50 00007ff6`f2002117 8b4d04          mov     ecx,dword ptr [rbp+4]
50 00007ff6`f200211a ffc9            dec     ecx
50 00007ff6`f200211c 4863c9          movsxd  rcx,ecx
50 00007ff6`f200211f 8b4c8d28        mov     ecx,dword ptr [rbp+rcx*4+28h]
50 00007ff6`f2002123 d1e1            shl     ecx,1
50 00007ff6`f2002125 394c8528        cmp     dword ptr [rbp+rax*4+28h],ecx
50 00007ff6`f2002129 7405            je      bomb!phase_2+0xa0 (00007ff6`f2002130)
```

實際觀看一下發現，這段程式碼的邏輯是，每次都會取上一個數字的值，取出來後乘二比較。所以 phase 2 需要輸入的值為﹔

```
1 2 4 8 16 32
```

phase 2 GOGO!

# Phase 3

一樣一開始是個 scanf，但這次只要輸入兩個數字，r8 (rbp+4) , r9 (rbp+24h) 分別存入兩個數字﹔

```
0:000> da rcx
00007ff6`f20102a0  "input"
0:000> da rdx
00007ff6`f200c220  "%d %d"
```

下一段可以看到第一個數字需要 ≤ 7 才不會爆炸，後續會拿這個 index 從 memory 拿出特定的 instruction offset，0 ~ 7 共八種可能

```
// 計算 jmp 位置
bomb!phase_3+0x90 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 77]:
   77 00007ff6`f2002220 48638534010000  movsxd  rax,dword ptr [rbp+134h]
   77 00007ff6`f2002227 488d0dd2ddfeff  lea     rcx,[bomb!_enc$textbss$begin <PERF> (bomb+0x0) (00007ff6`f1ff0000)]
   77 00007ff6`f200222e 8b8481cc220100  mov     eax,dword ptr [rcx+rax*4+122CCh]
   77 00007ff6`f2002235 4803c1          add     rax,rcx
   77 00007ff6`f2002238 ffe0            jmp     rax
```

```
// rcx+122CCh 上的數值
0:000> dd rcx+122CCh
00007ff6`f20022cc  0001223a 00012245 00012250 0001225b
00007ff6`f20022dc  00012264 0001226d 00012276 0001227f
00007ff6`f20022ec  cccccccc cccccccc cccccccc cccccccc
00007ff6`f20022fc  cccccccc cccccccc cccccccc cccccccc
```

0 → add eax 274h

1 → sub eax 24Ch

2 → add eax 2B0h

3 ~ 7 重複的 sub eax 7Eh, add eax 7Eh

執行完這一段後，又有一行 instruction 只要超過 5，又會爆炸了，第二個輸入值要跟剛剛循環過後的值一樣

```
00007ff6`f200228f 837d0405        cmp     dword ptr [rbp+4],5
00007ff6`f2002293 7f08            jg      bomb!phase_3+0x10d (00007ff6`f200229d) [br=0]

00007ff6`f2002295 8b4524          mov     eax,dword ptr [rbp+24h]
00007ff6`f2002298 394544          cmp     dword ptr [rbp+44h],eax
00007ff6`f200229b 7405            je      bomb!phase_3+0x112 (00007ff6`f20022a2)
```

所以其中一種 phase 3 組合如下

```
5 -126
```

Phase 3 搞定!

# Phase 4

一樣，起始 input 兩個數字

```
0:000> da rcx
00007ff6`f20102f0  "input"
0:000> da rdx
00007ff6`f200c220  "%d %d"
```

Not Exploding 1: 第一個數字需要大於 0，小於等於 14

```
00007ff6`f20023c1 837d0400        cmp     dword ptr [rbp+4],0 ss:00000087`ed5cf934=0000000a
00007ff6`f20023c5 7c06            jl      bomb!phase_4+0x7d (00007ff6`f20023cd)
00007ff6`f20023c7 837d040e        cmp     dword ptr [rbp+4],0Eh ss:00000087`ed5cf934=0000000a
00007ff6`f20023cb 7e05            jle     bomb!phase_4+0x82 (00007ff6`f20023d2)
00007ff6`f20023cd e8e4efffff      call    bomb!ILT+945(explode_bomb) (00007ff6`f20013b6)
```

## func4

這裡跳出一個意義不明的 function，他的 argument 長這樣

rcx → 第一個 input 的數字 (rbp+4)

rdx → 0, (xor edx edx)

r8 → 14 (0Eh)

---

rbp + 100h → 第一個 parameter, user control

rbp+108h → rdx, 第二個 parameter

rbp + 110h → r8 第三個 Parameter

---

...這邊居然有個 Recursion 出現

```
bomb!func4+0x95 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 124]:
  124 00007ff6`f2001fa5 8b4504          mov     eax,dword ptr [rbp+4]
  124 00007ff6`f2001fa8 ffc0            inc     eax
  124 00007ff6`f2001faa 448b8510010000  mov     r8d,dword ptr [rbp+110h]
  124 00007ff6`f2001fb1 8bd0            mov     edx,eax
  124 00007ff6`f2001fb3 8b8d00010000    mov     ecx,dword ptr [rbp+100h]
  124 00007ff6`f2001fb9 e81cf3ffff      call    bomb!ILT+725(func4) (00007ff6`f20012da)
  124 00007ff6`f2001fbe 034504          add     eax,dword ptr [rbp+4]
  124 00007ff6`f2001fc1 eb05            jmp     bomb!func4+0xb8 (00007ff6`f2001fc8)
```

PsudoCode

```
func (int a, int b, int c) {
	tmp = ((c - b) - (c - b < 0 ? -1 : 0)) / 2 + b
	
	if tmp <= a {
		if tmp >= a {
			rax = tmp
			return
		} else {
			rax = tmp + func4(a, tmp+1, c)
			return
		}
	} else {
		rax = tmp + func4(a, b, tmp-1)
		return
	}
}
```

## 回到 phase_4

func4 最終 return 值需要是 10 才不會 explode，且剛剛第二個 input 數字需要是 10 才不會爆炸。從 func4 的邏輯可以看到，第一個數字給 3 才會剛好 return 10。

Phase 4 Completed!!!

# Phase 5

一樣，輸入兩個數字

```
0:000> da rdx
00007ff6`f200c220  "%d %d"
0:000> da rcx
00007ff6`f2010340  "input"
```

---

rbp+64h → First Input

rbp+84h → Second Input

---

```
0:000> dd rcx
00007ff6`f200f1d0  0000000a 00000002 0000000e 00000007
00007ff6`f200f1e0  00000008 0000000c 0000000f 0000000b
00007ff6`f200f1f0  00000000 00000004 00000001 0000000d
00007ff6`f200f200  00000003 00000009 00000006 00000005
00007ff6`f200f210  00000000 00000000 00000000 00000000
00007ff6`f200f220  00000000 00000000 00000000 00000000
00007ff6`f200f230  00000000 00000000 00000000 00000000
00007ff6`f200f240  00000000 00000000 00000000 00000000
```

第一個 input 只會管最後一個 byte 的值 ( and 0xF )，從上面這一串記憶體中的值也可以觀察到這個點，0 ~ 15 的 offset

這裡跟之前解一個 ppt 裡的 micro 題有點像，每次會取出不同的值，那個值是下一個位置的 index，最終目的是讓 index 等於 16，也就是全部路徑都有走過

```
bomb!phase_5+0x96 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 202]:
  202 00007ff6`f20024f6 837d640f        cmp     dword ptr [rbp+64h],0Fh
  202 00007ff6`f20024fa 7428            je      bomb!phase_5+0xc4 (00007ff6`f2002524)  Branch

bomb!phase_5+0xc4 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 208]:
  208 00007ff6`f2002524 837d040f        cmp     dword ptr [rbp+4],0Fh
  208 00007ff6`f2002528 750b            jne     bomb!phase_5+0xd5 (00007ff6`f2002535)
```

所以 first input 要是 5，到第 5 個位置開始這個 sequence，第二個 Input 則是所有到過的值的加總 - 5。phase 5 的輸入如下﹔

```
5 115
```

Phase 5 Complete !!!!

# Phase 6

phase 6 又從新用到 read_six_number 了!! 所以直接先給 6 個數字吧。

```
1 2 3 4 5 6
```

從 assembly 的大致長相來看，rbp+04ch 又是一個 counter，而且這次 phase 6 有好幾個 counter 為 6 的 loop，每次都有不同的動作

```
bomb!phase_6+0x6e [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 295]:
  295 00007ff6`f200261e 8b85c4000000    mov     eax,dword ptr [rbp+0C4h]
  295 00007ff6`f2002624 ffc0            inc     eax
  295 00007ff6`f2002626 8985c4000000    mov     dword ptr [rbp+0C4h],eax

bomb!phase_6+0x7c [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 295]:
  295 00007ff6`f200262c 83bdc400000006  cmp     dword ptr [rbp+0C4h],6
  295 00007ff6`f2002633 7d69            jge     bomb!phase_6+0xee (00007ff6`f200269e)
```

## First Stage

條件 1：≥ 1 and ≤ 6

```
bomb!phase_6+0x85 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 296]:
  296 00007ff6`f2002635 486385c4000000  movsxd  rax,dword ptr [rbp+0C4h]
  296 00007ff6`f200263c 837c854801      cmp     dword ptr [rbp+rax*4+48h],1
  296 00007ff6`f2002641 7c0e            jl      bomb!phase_6+0xa1 (00007ff6`f2002651)
```

條件 2：後面 5 個數字不能跟第一個一樣

```
bomb!phase_6+0xb6 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 299]:
  299 00007ff6`f2002666 8b85e4000000    mov     eax,dword ptr [rbp+0E4h]
  299 00007ff6`f200266c ffc0            inc     eax
  299 00007ff6`f200266e 8985e4000000    mov     dword ptr [rbp+0E4h],eax

bomb!phase_6+0xc4 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 299]:
  299 00007ff6`f2002674 83bde400000006  cmp     dword ptr [rbp+0E4h],6
  299 00007ff6`f200267b 7d1f            jge     bomb!phase_6+0xec (00007ff6`f200269c)

bomb!phase_6+0xcd [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 300]:
  300 00007ff6`f200267d 486385c4000000  movsxd  rax,dword ptr [rbp+0C4h]
  300 00007ff6`f2002684 48638de4000000  movsxd  rcx,dword ptr [rbp+0E4h]
  300 00007ff6`f200268b 8b4c8d48        mov     ecx,dword ptr [rbp+rcx*4+48h]
  300 00007ff6`f200268f 394c8548        cmp     dword ptr [rbp+rax*4+48h],ecx
  300 00007ff6`f2002693 7505            jne     bomb!phase_6+0xea (00007ff6`f200269a)
```

接下來發現，這是一個大 loop (條件 1 + 條件 2)，每個數字皆不能小於 1 和 大於 6，且所有數字不能相同

## Second Stage

```
// Run to Cursor 00007ff7`e59f269e
bomb!phase_6+0xee [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 306]:
  306 00007ff7`e59f269e c785c400000000000000 mov dword ptr [rbp+0C4h],0
  306 00007ff7`e59f26a8 eb0e            jmp     bomb!phase_6+0x108 (00007ff7`e59f26b8)
```

rbp+28h → 7ff7e59ff050，這個位置上有數字值以及下一個對應的位置，目前看來都是 -10h 的位置

rbp+48h → input 的位置

rbp+78h → 存放上面那個位置

---

- rax +8 可以得到下一個 position

```
0:000> dq 7ff7e59ff000
00007ff7`e59ff000  00000006`00000200 00000000`00000000
00007ff7`e59ff010  00000005`000003a7 00007ff7`e59ff000
00007ff7`e59ff020  00000004`00000393 00007ff7`e59ff010
00007ff7`e59ff030  00000003`00000215 00007ff7`e59ff020
00007ff7`e59ff040  00000002`000001c2 00007ff7`e59ff030
00007ff7`e59ff050  00000001`00000212 00007ff7`e59ff040
```

Second Stage 的功用就是按照 6 個使用者輸入的數字，把它當作 index count，在 rbp+78h 位置上一次存入下一個 index 上的位置，**感覺就是 Linked List 拉!** 難怪前面要先確認每個數字都不相同，且只能限制在 1 到 6

## Third Stage

```
// Run to Cursor 00007ff7`e59f2716
bomb!phase_6+0x108 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 306]:
  306 00007ff7`e59f26b8 83bdc400000006  cmp     dword ptr [rbp+0C4h],6
  306 00007ff7`e59f26bf 7d55            jge     bomb!phase_6+0x166 (00007ff7`e59f2716)
```

Third Stage 的唯一功用只是從 rbp+78h，也就是上一階段整理好的 link，直接寫到 7ff7e59ff050 方便下一階段找 link 而已。

## Fourth Stage

```
// Run to Cursor 00007ff7`e59f2775
bomb!phase_6+0x19a [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 316]:
  316 00007ff7`e59f274a 83bdc400000006  cmp     dword ptr [rbp+0C4h],6
  316 00007ff7`e59f2751 7d22            jge     bomb!phase_6+0x1c5 (00007ff7`e59f2775)
```

條件：當前 index 所代表的位置的值要比下一個 index 代表的值大才不會 explode !

反推回去需要研究一下 rbp+28h 上值所代表的意義。

所以 Phase_6 需要輸入的值為：

```
5 4 3 1 6 2
```

## Final Stage

```
// Run to Cursor 00007ff7`e59f27d1
bomb!phase_6+0x1f3 [C:\Users\user\Desktop\bomb-source-with-selected-phases-used-in-arch1001\src\phases.c @ 324]:
  324 00007ff7`e59f27a3 83bdc400000005  cmp     dword ptr [rbp+0C4h],5
  324 00007ff7`e59f27aa 7d25            jge     bomb!phase_6+0x221 (00007ff7`e59f27d1)
```

如果能到這裡就 defused 了 !!!

**整體來看，這感覺是個在 check linked list 是不是 decending order 的 function**

# 搞定拉!!!!!

最終解答：

```
I am just a renegade hockey mom.
1 2 4 8 16 32
5 -126
3 10
5 115
5 4 3 1 6 2
```

在有 pdb 檔的狀況下，第一次解 bomb lab 花了 1 天