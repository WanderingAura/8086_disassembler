;; reg to reg ;;
; full width
mov ax, bx
mov bx, bp
mov si, di

; half width
mov bl, ah
mov ah, al
mov cl, dl

;; rm to reg ;;
; no disp
mov ax, [bx+si]
mov bx, [bx+di]
mov cx, [bp+si]
mov cx, [bp+di]
mov di, [di]

; 8 bit disp
mov ax, [bx+si+100]

; 16 bit disp
mov bx, [bx+di+16000]

; direct address
mov ax, [13495]