.set noreorder
.align 2
.global printf
.type printf, @function

printf:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x3F
