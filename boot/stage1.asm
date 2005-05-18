; PROG.ASM

[BITS 16]
[ORG 0]

STRUC segment_descriptor
  seg_length0_15:	resw 1		; low word of the segment length
  base_addr0_15:	resw 1		; low word of base address
  base_addr16_23:	resb 1		; low byte of high word of base addr.
  flags:		resb 1		; segment type and misc. flags
  access:		resb 1		; highest nibble of segment length
  					; and access flags
  base_addr24_31:	resb 1		; highest byte of base address
ENDSTRUC

STRUC interrupt_descriptor
  .offset0_15:		resw	1		; low word of handler offset
  .selector0_15:	resw	1		; segment selector
  .zero_byte:		resb	1		; unused in this descriptor format
  .flags:		resb	1		; flag-byte
  .offset16_31:		resw	1		; high word of handler offset
ENDSTRUC

stackseg16  EQU  9000h
stackbase16 EQU 0C000h - 2
stackbase32 EQU 9C000h - 2
stage2base  EQU 10000h

start:
        mov ax, cs
        mov ds, ax
	mov ss, ax
    	mov sp, 8000h

read:
        mov ax, 1000h       ; ES:BX = 1000:0000
        mov es, ax          ;
        xor bx, bx          ;

        mov ah, 2           ; Load disk data to ES:BX
        mov al, 20          ; Load 128 sectors
        mov ch, 0           ; Cylinder=0
        mov cl, 10          ; Sector=10
        mov dh, 0           ; Head=0
        mov dl, 0           ; Drive=0
        int 13h             ; Read!

        jc read             ; ERROR => Try again
	
read_done:
        xor bx, bx
        mov ah, 3
        int 10h             ; get cursor position to dh, dl

	mov ax, 9000h       ;
	mov es, ax          ;
        mov ah, 13h         ; Print "Loading..."
        mov al, 1           ;
        mov bx, 7h          ;
        mov cx, end-string  ;
        mov bp, string      ;
        int 10h             ;

A20Address:
	; Set A20 Address line here
    	CLI
        CALL enableA20
    	STI

        JMP continue
enableA20:
        call enableA20o1

        jnz  enableA20done
        mov  al,   0d1h
        out  64h,  al

        call enableA20o1
        jnz  enableA20done

        mov  al,   0dfh
        out  60h,  al

enableA20o1:
        mov ecx, 20000h

enableA20o1l:
	jmp     $+2

        in      al, 64h
        test    al, 2
        loopnz  enableA20o1l
enableA20done:
        ret

continue:
        lgdt	[ds:gdt_reg]

	cli

 	mov     eax, cr0
    	or      eax, 1
        mov     cr0, eax

	jmp     $+2          ; clear cache
    	jmp     intoflat

intoflat:
	db      066h, 0eah
        dw      prot, 09h
        dw      08h

[BITS 32]
prot:   ; Set up segments
        mov ebx, 10h
        mov ds,  bx
        mov es,  bx
        mov fs,  bx
        mov gs,  bx

        ; Set up stack segment
        mov ebx, 18h
        mov ss,  bx
        mov eax, stackbase32
        mov esp, eax

        push eax
        mov ebx, stage2base
        call ebx

hang:                       ; Hang!
        hlt

;;; GDT DATA

string: db 13, 10, "Loading ZSOS...", 13, 10
end:

        dd 0

gdt_reg: dw gdt_size-1, 0400h, 09h

times 1024-($-$$) db 0

dummy_dscr:	istruc segment_descriptor
	at seg_length0_15, dw 0
   	at base_addr0_15,  dw 0 
  	at base_addr16_23, db 0     
	at flags,          db 0 
  	at access,         db 0  
  	at base_addr24_31, db 0  
iend

code32_dscr: istruc segment_descriptor
	at seg_length0_15, dw 0ffffh
   	at base_addr0_15,  dw 0
  	at base_addr16_23, db 0     
	at flags,          db 09ah 
  	at access,         db 0cfh  
  	at base_addr24_31, db 0  
iend

data32_dscr: istruc segment_descriptor
	at seg_length0_15, dw 0ffffh
   	at base_addr0_15,  dw 0
  	at base_addr16_23, db 0     
	at flags,          db 092h 
  	at access,         db 0cfh  
  	at base_addr24_31, db 0  
iend

core32_dscr: istruc segment_descriptor
	at seg_length0_15, dw 0ffffh
   	at base_addr0_15,  dw 0
  	at base_addr16_23, db 0     
	at flags,          db 092h 
  	at access,         db 0cfh  
  	at base_addr24_31, db 0  
iend

code16_dscr: istruc segment_descriptor
	at seg_length0_15, dw 0ffffh
   	at base_addr0_15,  dw 0
  	at base_addr16_23, db 0     
	at flags,          db 09ah 
  	at access,         db 0  
  	at base_addr24_31, db 0  
iend

data16_dscr: istruc segment_descriptor
	at seg_length0_15, dw 0ffffh
   	at base_addr0_15,  dw 0
  	at base_addr16_23, db 0     
	at flags,          db 092h 
  	at access,         db 0  
  	at base_addr24_31, db 0  
iend

gdt_size EQU $-dummy_dscr

times 4096-($-$$) db 0




