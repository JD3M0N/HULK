.data
str0: .asciiz "Moving"
str1: .asciiz "Driving on road"
str2: .asciiz "Racing fast!"

.text
.globl main
.globl _start

_start:
    # Punto de entrada ELF
    jal main
    # Terminar programa
    li $v0, 10
    syscall

    # DEBUG: Discovered method: move
    # DEBUG: Discovered method: move
    # DEBUG: Discovered method: move
f0:
    # Function: f0
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    # Error: Return value str0 not found
    li $v0, 0
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
    # Error: Return value str1 not found
    li $v0, 0
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
    # Error: Return value str2 not found
    li $v0, 0
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

getVehicle:
    # Function: getVehicle
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # level
    # Binary operation: t0 = level == 1.000000
    lw $t0, 4($fp)
    li $t1, 1
    seq $t2, $t0, $t1
    # Temporal t0 result stored in $t2
    # IF t0 == 0 GOTO L0
    beq $t2, $zero, L0
    # ALLOCATE Vehicle -> t2
    li $t2, 1
    # Object of type Vehicle allocated with ID 1
    # SETATTR t2.vtable = vtable_Vehicle
    # VTable set for object t2
    # Temporal assignment: t1 = t2 (register only)
    j L1
L0:
    # Binary operation: t3 = level == 2.000000
    lw $t0, 4($fp)
    li $t1, 2
    seq $t2, $t0, $t1
    # Temporal t3 result stored in $t2
    # IF t3 == 0 GOTO L2
    beq $t2, $zero, L2
    # ALLOCATE Car -> t5
    li $t2, 2
    # Object of type Car allocated with ID 2
    # SETATTR t5.vtable = vtable_Car
    # VTable set for object t5
    # Temporal assignment: t4 = t5 (register only)
    j L3
L2:
    # ALLOCATE SportsCar -> t6
    li $t2, 3
    # Object of type SportsCar allocated with ID 3
    # SETATTR t6.vtable = vtable_SportsCar
    # VTable set for object t6
    # Temporal assignment: t4 = t6 (register only)
L3:
    # Temporal assignment: t1 = t4 (register only)
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
    jal getVehicle
    # VCALL: t8 = t7.move()
    # Object in temporal t7, using previous result
    move $t0, $v0
    # Polymorphic call to move method
    # Check object type and call appropriate method
    move $a0, $t0
    jal move_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 2
    jal getVehicle
    # VCALL: t11 = t10.move()
    # Object in temporal t10, using previous result
    move $t0, $v0
    # Polymorphic call to move method
    # Check object type and call appropriate method
    move $a0, $t0
    jal move_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 3
    jal getVehicle
    # VCALL: t14 = t13.move()
    # Object in temporal t13, using previous result
    move $t0, $v0
    # Polymorphic call to move method
    # Check object type and call appropriate method
    move $a0, $t0
    jal move_polymorphic
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
move_polymorphic:
    # Polymorphic dispatcher for move method
    # $a0 contains object with type info
    
    # Prolog for dispatcher
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    move $fp, $sp
    
    # Check object type
    li $t1, 1
    beq $a0, $t1, move_call_f0
    li $t1, 2
    beq $a0, $t1, move_call_f1
    li $t1, 3
    beq $a0, $t1, move_call_f2
    
    # Default case - should not happen
    li $v0, 0
    j move_dispatcher_end
    
move_call_f0:
    jal f0
    j move_dispatcher_end
    
move_call_f1:
    jal f1
    j move_dispatcher_end
    
move_call_f2:
    jal f2
    j move_dispatcher_end
    
move_dispatcher_end:
    # Epilog for dispatcher
    lw $ra, 12($sp)
    lw $fp, 8($sp)
    addi $sp, $sp, 16
    jr $ra

