.set noreorder
.align 2
.global printf
.type printf, @function

printf:
    lui   $t1, 0x8070
    sw    $sp, 0x0($t1)
    sw    $ra, 0x4($t1)
    lui   $t1, 0x8060
    move  $sp, $t1
    li    $t2, 0xA0
    jalr  $t2
    li    $t1, 0x3F
    lui   $t1, 0x8070
    lw    $ra, 0x4($t1)
    jr    $ra
    lw    $sp, 0x0($t1)
