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
mov [bp+si], cx
mov [bp+di], dx
mov [di], di

; 8 bit disp
mov ax, [bx+si+100]
mov [bx+di-10], bx

; 16 bit disp
mov bx, [bx+di+16000]
mov [bx-18934], ax

; direct address
mov [13495], bp
mov bp, [39]