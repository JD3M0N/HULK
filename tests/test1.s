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

    # DEBUG: Discovered method: sound
    # DEBUG: Discovered method: sound
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

getPet:
    # Function: getPet
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # x
    # Binary operation: t0 = x > 5.000000
    lw $t0, 4($fp)
    li $t1, 5
    sgt $t2, $t0, $t1
    # Temporal t0 result stored in $t2
    # IF t0 == 0 GOTO L0
    beq $t2, $zero, L0
    # ALLOCATE Animal -> t2
    li $t2, 1
    # Object of type Animal allocated with ID 1
    # SETATTR t2.vtable = vtable_Animal
    # VTable set for object t2
    # Temporal assignment: t1 = t2 (register only)
    j L1
L0:
    # ALLOCATE Dog -> t3
    li $t2, 2
    # Object of type Dog allocated with ID 2
    # SETATTR t3.vtable = vtable_Dog
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
    li $a0, 5
    jal getPet
    # VCALL: t5 = t4.sound()
    # Object in temporal t4, using previous result
    move $t0, $v0
    # Polymorphic call to sound method
    # Check object type and call appropriate method
    move $a0, $t0
    jal sound_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 6
    jal getPet
    # VCALL: t8 = t7.sound()
    # Object in temporal t7, using previous result
    move $t0, $v0
    # Polymorphic call to sound method
    # Check object type and call appropriate method
    move $a0, $t0
    jal sound_polymorphic
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
sound_polymorphic:
    # Polymorphic dispatcher for sound method
    # $a0 contains object with type info
    
    # Prolog for dispatcher
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    move $fp, $sp
    
    # Check object type
    li $t1, 1
    beq $a0, $t1, sound_call_f0
    li $t1, 2
    beq $a0, $t1, sound_call_f1
    
    # Default case - should not happen
    li $v0, 0
    j sound_dispatcher_end
    
sound_call_f0:
    jal f0
    j sound_dispatcher_end
    
sound_call_f1:
    jal f1
    j sound_dispatcher_end
    
sound_dispatcher_end:
    # Epilog for dispatcher
    lw $ra, 12($sp)
    lw $fp, 8($sp)
    addi $sp, $sp, 16
    jr $ra

