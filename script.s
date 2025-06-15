.data
str0: .asciiz "hola"

.text
.globl main

factorial_iterative:
    # Function: factorial_iterative
    addi $sp, $sp, -32
    sw $ra, 28($sp)
    sw $fp, 24($sp)
    move $fp, $sp
    li $t0, 1.000000
    li $t1, 1.000000
L0:
    beq $t2, $t3, L1
    mul $t4, $t0, $t1
    move $t0, $t4
    move $t1, $t5
    j L0
L1:
    move $v0, $t0
    lw $ra, 28($sp)
    lw $fp, 24($sp)
    addi $sp, $sp, 32
    jr $ra

sum_range:
    # Function: sum_range
    addi $sp, $sp, -32
    sw $ra, 28($sp)
    sw $fp, 24($sp)
    move $fp, $sp
    li $t6, 0.000000
    move $t7, stack_0
L2:
    beq stack_4, $t3, L3
    add stack_8, $t6, $t7
    move $t6, stack_8
    move $t7, stack_12
    j L2
L3:
    move $v0, $t6
    lw $ra, 28($sp)
    lw $fp, 24($sp)
    addi $sp, $sp, 32
    jr $ra

main:
    # Function: entry
    addi $sp, $sp, -32
    sw $ra, 28($sp)
    sw $fp, 24($sp)
    move $fp, $sp
    move $a0, stack_16
    jal factorial_iterative
    move stack_20, $v0
    move $a0, stack_20
    move $a0, stack_20
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    move $a0, stack_24
    move $a1, stack_28
    jal sum_range
    move stack_32, $v0
    move $a0, stack_32
    move $a0, stack_32
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    move $a0, stack_36
    move $a0, stack_36
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    move $v0, $t3
    lw $ra, 28($sp)
    lw $fp, 24($sp)
    addi $sp, $sp, 32
    li $v0, 10
    syscall

