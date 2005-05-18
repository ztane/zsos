    ; 1.ASM
    ; Print "====" on the screen and hang

    ; Tell the compiler that this is offset 0.
    ; It isn't offset 0, but it will be after the jump.
    [BITS 16]
    [ORG 0]

            jmp 07C0h:start     ; Goto segment 07C0

    start:
            ; Update the segment registers
            mov ax, cs
            mov ds, ax
            mov es, ax

            xor bx, bx
            mov ah, 3
            int 10h             ; get cursor position to dh, dl

            mov ah, 0eh         ; Print "===="
            mov al, 'B'         ;
            mov bx, 4           ;
            int 10h             ;
    reset:                      ; Reset the floppy drive
            xor ax, ax          ;
            xor dl, dl          ; Drive=0 (=A)
            int 13h             ;
            jnc reset_ok 	;

            mov ah, 0eh		; Print "===="
            mov al, 'F'         ;
            mov bx, 4           ;
	    int 10h		;
	    jmp reset

    reset_ok:

            mov al, 'L'         ;
            mov ah, 0eh         ;
            mov bx, 4           ;
            int 10h             ;

    read:
            mov ax, 9000h       ; ES:BX = 9000:0000
            mov es, ax          ;
            xor bx, bx          ;

            mov ah, 2           ; Load disk data to ES:BX
            mov al, 8           ; Load 8 sectors
            mov ch, 0           ; Cylinder=0
            mov cl, 2           ; Sector=2
            mov dh, 0           ; Head=0
            mov dl, 0           ; Drive=0
            int 13h             ; Read!

            jc read             ; ERROR => Try again
            jmp 9000h:0000      ; Jump to the program

    hang1:                       ; Hang!
            hlt

    times 510-($-$$) db 0
    dw 0AA55h
