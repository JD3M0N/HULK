.data
str0: .asciiz "hola"

.text
.globl main

factorial_iterative:
    # Prolog
    addi $sp, $sp, -48
    sw $ra, 44($sp)
    sw $fp, 40($sp)
    move $fp, $sp
    sw $a0, 36($sp)  # n
    # Function: factorial_iterative
    # Allocating variable: result at offset -16
    li $t0, 1
    sw $t0, -16($fp)
    # Allocating variable: i at offset -20
    li $t0, 1
    sw $t0, -20($fp)
L0:
    # Allocating variable: t0 at offset -24
    lw $t0, -20($fp)
    lw $t1, -12($fp)
    sle $t2, $t0, $t1
    sw $t2, -24($fp)
    lw $t0, -24($fp)
    li $t1, 0
    beq $t0, $t1, L1
    # Allocating variable: t1 at offset -28
    lw $t0, -16($fp)
    lw $t1, -20($fp)
    mul $t2, $t0, $t1
    sw $t2, -28($fp)
    lw $t0, -28($fp)
    sw $t0, -16($fp)
    # Allocating variable: t2 at offset -32
    lw $t0, -32($fp)
    sw $t0, -20($fp)
    j L0
L1:
    lw $v0, -16($fp)
    # Epilog
    lw $ra, 44($sp)
    lw $fp, 40($sp)
    addi $sp, $sp, 48
    jr $ra

sum_range:
    # Prolog
    addi $sp, $sp, -48
    sw $ra, 44($sp)
    sw $fp, 40($sp)
    move $fp, $sp
    sw $a0, 36($sp)  # start
    sw $a1, 32($sp)  # end
    # Function: sum_range
    # Allocating variable: sum at offset -20
    li $t0, 0
    sw $t0, -20($fp)
    # Allocating variable: current at offset -24
    lw $t0, -12($fp)
    sw $t0, -24($fp)
L2:
    # Allocating variable: t3 at offset -28
    lw $t0, -24($fp)
    lw $t1, -16($fp)
    sle $t2, $t0, $t1
    sw $t2, -28($fp)
    lw $t0, -28($fp)
    li $t1, 0
    beq $t0, $t1, L3
    # Allocating variable: t4 at offset -32
    lw $t0, -20($fp)
    lw $t1, -24($fp)
    add $t2, $t0, $t1
    sw $t2, -32($fp)
    lw $t0, -32($fp)
    sw $t0, -20($fp)
    # Allocating variable: t5 at offset -36
    lw $t0, -36($fp)
    sw $t0, -24($fp)
    j L2
L3:
    lw $v0, -20($fp)
    # Epilog
    lw $ra, 44($sp)
    lw $fp, 40($sp)
    addi $sp, $sp, 48
    jr $ra

main:
    # Prolog
    addi $sp, $sp, -48
    sw $ra, 44($sp)
    sw $fp, 40($sp)
    move $fp, $sp
    # Function: entry
    # Allocating variable: t6 at offset -12
    li $a0, 5
    jal factorial_iterative
    sw $v0, -12($fp)
    # Allocating variable: t7 at offset -16
    lw $a0, -12($fp)
    lw $a0, -12($fp)
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    # Allocating variable: t8 at offset -20
    li $a0, 1
    li $a1, 10
    jal sum_range
    sw $v0, -20($fp)
    # Allocating variable: t9 at offset -24
    lw $a0, -20($fp)
    lw $a0, -20($fp)
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    # Allocating variable: t10 at offset -28
    li $a0, str0
    la $a0, str0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 0
    # Epilog
    lw $ra, 44($sp)
    lw $fp, 40($sp)
    addi $sp, $sp, 48
    li $v0, 10
    syscall

