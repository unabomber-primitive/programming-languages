; print_string.asm 
section .data
message: db  'hello, world!', 10

section .text
global _start

exit:
    mov    rax, 60
    xor     rdi, rdi          
    syscall

string_length:
  mov rax, 0
    .loop:
      xor rax, rax
    .count:
      cmp byte [rdi+rax], 0
      je .end
      inc rax
      jmp .count
    .end:
      ret
print_string:
  mov     rsi, rdi
  mov     rdx, rsi
  mov     rax, 1
  mov     rdi, 1
  syscall
  ret

_start:
    mov rdi, message
    mov rsi,14  
    call print_string
    call exit                    ; это вызов функции exit
