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

    # DEBUG: Discovered method: makeSound
    # DEBUG: Discovered method: makeSound
f0:
    # Function: f0
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    li $v0, 1
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
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    li $v0, 2
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

f2:
    # Function: f2
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    li $v0, 3
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

createPet:
    # Function: createPet
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # pet_type
    # Binary operation: t0 = pet_type == 1.000000
    lw $t0, 4($fp)
    li $t1, 1
    seq $t2, $t0, $t1
    # Temporal t0 result stored in $t2
    # IF t0 == 0 GOTO L0
    beq $t2, $zero, L0
    # ALLOCATE Dog -> t2
    li $t2, 1
    # Object of type Dog allocated with ID 1
    # SETATTR t2.vtable = vtable_Dog
    # VTable set for object t2
    # Temporal assignment: t1 = t2 (register only)
    j L1
L0:
    # ALLOCATE Cat -> t3
    li $t2, 2
    # Object of type Cat allocated with ID 2
    # SETATTR t3.vtable = vtable_Cat
    # VTable set for object t3
    # Temporal assignment: t1 = t3 (register only)
L1:
    # Returning temporal result from $t2
    move $v0, $t2
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
    li $a0, 1
    jal createPet
    # VCALL: t5 = t4.makeSound()
    # Object in temporal t4, using previous result
    move $t0, $v0
    # Polymorphic call to makeSound method
    # Check object type and call appropriate method
    move $a0, $t0
    jal makeSound_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 2
    jal createPet
    # VCALL: t8 = t7.makeSound()
    # Object in temporal t7, using previous result
    move $t0, $v0
    # Polymorphic call to makeSound method
    # Check object type and call appropriate method
    move $a0, $t0
    jal makeSound_polymorphic
    move $a0, $v0
    li $v0, 1
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


# Polymorphic dispatch functions
makeSound_polymorphic:
    # Polymorphic dispatcher for makeSound method
    # $a0 contains object with type info
    
    # Prolog for dispatcher
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    move $fp, $sp
    
    # Check object type
    li $t1, 1
    beq $a0, $t1, makeSound_call_f0
    li $t1, 2
    beq $a0, $t1, makeSound_call_f1
    li $t1, 3
    beq $a0, $t1, makeSound_call_f2
    
    # Default case - should not happen
    li $v0, 0
    j makeSound_dispatcher_end
    
makeSound_call_f0:
    jal f0
    j makeSound_dispatcher_end
    
makeSound_call_f1:
    jal f1
    j makeSound_dispatcher_end
    
makeSound_call_f2:
    jal f2
    j makeSound_dispatcher_end
    
makeSound_dispatcher_end:
    # Epilog for dispatcher
    lw $ra, 12($sp)
    lw $fp, 8($sp)
    addi $sp, $sp, 16
    jr $ra

