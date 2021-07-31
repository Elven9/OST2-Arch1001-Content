PUBLIC asm_scratchpad

.code
asm_scratchpad PROC
   ;code goes here

   ;mov eax, 0xaabbccdd
   db 0b8h, 0DDh, 0CCh, 0BBh, 0AAh

   ;sahf
   db 9Eh

   ;jz mylabel
   ;+5
   db 74h, 5h

   ;and eax, 0x31137
   db 25h, 37h, 11h, 03h, 00h

   ;mylabel
   ;ret
   db 0c3h
asm_scratchpad ENDP
end