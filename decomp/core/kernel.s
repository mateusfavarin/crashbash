.set noreorder
.align 2
.global printf
.type printf, @function

printf:
    li    $t2, 0xA0
    jr    $t2
    li    $t1, 0x3F

.set noreorder
.align 2
.global getStackPtr
.type getStackPtr, @function

getStackPtr:
    jr    $ra
    move  $v0, $sp

.set noreorder
.align 2
.global setStackPtr
.type setStackPtr, @function

setStackPtr:
    move  $v0, $sp
    jr    $ra
    move  $sp, $a0
