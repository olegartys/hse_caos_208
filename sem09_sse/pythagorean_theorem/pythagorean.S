	.global pythagorean

pythagorean:
    push %ebp
    mov  %esp, %ebp

    movss 8(%ebp), %xmm1      // x
    movss 12(%ebp), %xmm2     // y

    mulss %xmm1, %xmm1
    mulss %xmm2, %xmm2

    addss %xmm1, %xmm2

    sqrtss %xmm2, %xmm2

    sub     $4, %esp        // резервируем память
    movss   %xmm2, (%esp)   // копируем значение float из %xmm2 в стек
    flds    (%esp)          // загружаем из стека на %st(0) // FPU: %st(0) .. %st(7)
    add     $4, %esp        // очищаем стек

    pop  %ebp
    ret
