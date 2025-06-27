.data

.text
.globl main
.globl _start

_start:
    # Punto de entrada ELF
    jal main
    # Terminar programa
    li $v0, 10
    syscall

f0:
    # Function: f0
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    li $v0, 5
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

f1:
    # Function: f1
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    li $v0, 10
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

bar:
    # Function: bar
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # x
main:
    # Function: entry
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    beq $t2, $zero, L0
    # Temporal t1 managed in registers only
    # Temporal t2 managed in registers only
    # Error: Variable t2 not found or is temporal
    j L1
L0:
    # Temporal t1 managed in registers only
    # Temporal t3 managed in registers only
    # Error: Variable t3 not found or is temporal
L1:
    li $v0, 0
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
    jal bar
    li $a0, 6
    jal bar
    li $v0, 0
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

