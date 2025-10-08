.intel_syntax noprefix
.data
.text
.global main
main:
  push rbp
  mov rbp, rsp
  mov rax, 1
  push rax
  mov rax, 1
  pop rbx
  add rax, rbx
  mov rsp, rbp
  pop rbp
  ret

