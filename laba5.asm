.model small
.stack 100h

.data
    command_args_error db "Invalid arguments!", 10, 13, "Enter source file name", 10, 13, "$"
    filename_error     db "You entered invalid file name!", 10, 13, "$"
    
    command_args db 127 dup(0)
    
    hfile  dw ?
    symbol db ?
    flag   db 0
    
    readfp   dw 0, 0
    writefp  dw 0, 0 
    curstrfp dw 0, 0
    
.code
set_file_pointer macro fp1, fp2
    mov ax, word ptr fp2
    mov word ptr fp1, ax
    mov ax, word ptr fp2[2]
    mov word ptr fp1[2], ax
endm


read_symbol macro fp
    push bx
    push cx
    push dx
           
    mov ax, 4200h
    mov bx, hfile
    mov cx, word ptr fp[2]
    mov dx, word ptr fp
    int 21h
    
    mov ah, 3fh
    mov cx, 1
    lea dx, symbol
    int 21h
    
    add word ptr fp, 1
    adc word ptr fp[2], 0
    
    pop dx
    pop cx
    pop bx
endm


write_symbol macro fp
    push bx
    push cx
    push dx
    
    mov ax, 4200h
    mov bx, hfile
    mov cx, word ptr fp[2]
    mov dx, word ptr fp
    int 21h
    
    mov ah, 40h
    mov bx, hfile
    mov cx, 1
    lea dx, symbol
    int 21h
    
    add word ptr fp, 1
    adc word ptr fp[2], 0
    
    pop dx
    pop cx
    pop bx
endm


main: 
    call get_command_args 
    cmp ax, 1 
    lea dx, command_args_error
    jne error     
    
    mov dx, offset command_args
    
    call open_file
    cmp ax, 1
    lea dx, filename_error  
    jne error
    
    call delete_strings
    
    call close_file
        
    jmp exit   
          
error:
    call output_string 
    
    
exit:
    mov ax, 4c00h
    int 21h
    
    
get_command_args proc
    mov cl, ds:[0080h]
    cmp cl, 1
    jge command_line_args
    mov ax, @data
    mov ds, ax
    xor ax, ax
    jmp command_args_return
    
command_line_args:
    cld
    xor ch, ch
    mov di, 81h
    mov al, ' '
    repz scasb ;cmp al, [di++]
    
    dec di
    mov si, di
    mov ax, @data
    mov es, ax
    lea di, command_args
    rep movsb
    movsb   
    
    mov ax, @data
    mov ds, ax
    mov ax, 1
    
command_args_return:
    ret
get_command_args endp


open_file proc
    lea dx, command_args
    mov ax, 3d02h ;write/read
    int 21h
    jc open_error
    mov hfile, ax
    mov ax, 1
    jmp open_file_return
    
open_error:
    xor ax, ax
    
open_file_return:
    ret
open_file endp 


delete_strings proc
    xor ax, ax

check_symbol:
    read_symbol readfp
    cmp ax, 1 ;read bytes
    jl eof 
    
    ;cmp symbol, ' '
    ;je check_symbol
    ;cmp symbol, 09h
    ;je check_symbol 
    cmp symbol, 0Ah
    je check_line
    cmp symbol, 0Dh
    je check_symbol
    jne set_flag

set_flag:
    mov flag, 1
    jmp check_symbol
    
check_line:
    cmp flag, 0
    jne no_empty
    call read_write_string
    set_file_pointer curstrfp, readfp 
    jmp check_symbol
    
no_empty:
    set_file_pointer curstrfp, readfp
    mov flag, 0
    jmp check_symbol
    
eof:
    call set_eof
    ret 
delete_strings endp


close_file proc
    mov ah, 3eh
    mov bx, hfile
    int 21h
    ret
close_file endp


read_write_string proc
    xor ax, ax
    
read_write_cycle:
    read_symbol curstrfp
    cmp ax, 1
    jl return
                 
    write_symbol writefp
    
    cmp symbol, 0Ah
    je return 
    
    jmp read_write_cycle
        
return:
    ret
read_write_string endp


set_eof proc
    mov ax, 4200h
    mov bx, hfile
    mov cx, writefp[2]
    mov dx, writefp
    int 21h
    
    mov ah, 40h
    mov bx, hfile
    xor cx, cx
    lea dx, symbol
    int 21h
    
    ret
set_eof endp

              
output_string proc
    mov ah, 09h
    int 21h
    ret
output_string endp  