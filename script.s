.data
str0: .asciiz "hola"

.text
.globl main
.globl _start

_start:
    # Punto de entrada ELF
    jal main
    # Terminar programa
    li $v0, 10
    syscall

factorial_iterative:
    # Function: factorial_iterative
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # n
    # Variable result at offset 12
    li $t0, 1
    sw $t0, 12($fp)
    # Variable i at offset 16
    li $t0, 1
    sw $t0, 16($fp)
L0:
    # Temporal t0 managed in registers only
    lw $t0, 16($fp)
    lw $t1, 4($fp)
    sle $t2, $t0, $t1
    # Result of i <= n in $t2
    beq $t2, $zero, L1
    # Temporal t1 managed in registers only
    lw $t0, 12($fp)
    lw $t1, 16($fp)
    mul $t2, $t0, $t1
    # Result of result * i in $t2
    # Updating result with operation result
    sw $t2, 12($fp)
    # Incrementing i
    lw $t0, 16($fp)
    addi $t0, $t0, 1
    sw $t0, 16($fp)
    j L0
L1:
    lw $v0, 12($fp)
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

sum_range:
    # Function: sum_range
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # start
    sw $a1, 8($fp)  # end
    # Variable sum at offset 12
    li $t0, 0
    sw $t0, 12($fp)
    # Variable current at offset 16
    lw $t0, 4($fp)
    sw $t0, 16($fp)
L2:
    # Temporal t3 managed in registers only
    lw $t0, 16($fp)
    lw $t1, 8($fp)
    sle $t2, $t0, $t1
    # Result of current <= end in $t2
    beq $t2, $zero, L3
    # Temporal t4 managed in registers only
    lw $t0, 12($fp)
    lw $t1, 16($fp)
    add $t2, $t0, $t1
    # Result of sum + current in $t2
    # Updating sum with operation result
    sw $t2, 12($fp)
    # Incrementing current
    lw $t0, 16($fp)
    addi $t0, $t0, 1
    sw $t0, 16($fp)
    j L2
L3:
    lw $v0, 12($fp)
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

main:
    # Function: entry
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    li $a0, 5
    jal factorial_iterative
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 1
    li $a1, 10
    jal sum_range
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    la $a0, str0
    li $v0, 4
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $v0, 0
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

