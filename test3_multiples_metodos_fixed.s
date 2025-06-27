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

    # DEBUG: Discovered method: area
    # DEBUG: Discovered method: perimeter
    # DEBUG: Discovered method: area
    # DEBUG: Discovered method: perimeter
f0:
    # Function: f0
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
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
    li $v0, 20
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

f3:
    # Function: f3
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    li $v0, 18
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

f4:
    # Function: f4
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    li $v0, 314
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

f5:
    # Function: f5
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    # PARAM self at offset 4
    sw $a0, 4($fp)  # self
    li $v0, 628
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

createShape:
    # Function: createShape
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # isCircle
    # Binary operation: t0 = isCircle == 1.000000
    lw $t0, 4($fp)
    li $t1, 1
    seq $t2, $t0, $t1
    # Temporal t0 result stored in $t2
    # IF t0 == 0 GOTO L0
    beq $t2, $zero, L0
    # ALLOCATE Circle -> t2
    li $t2, 1
    # Object of type Circle allocated with ID 1
    # SETATTR t2.vtable = vtable_Circle
    # VTable set for object t2
    # Temporal assignment: t1 = t2 (register only)
    j L1
L0:
    # ALLOCATE Rectangle -> t3
    li $t2, 2
    # Object of type Rectangle allocated with ID 2
    # SETATTR t3.vtable = vtable_Rectangle
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
    li $a0, 0
    jal createShape
    # VCALL: t5 = t4.area()
    # Object in temporal t4, using previous result
    move $t0, $v0
    # Polymorphic call to area method
    # Check object type and call appropriate method
    move $a0, $t0
    jal area_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 0
    jal createShape
    # VCALL: t8 = t7.perimeter()
    # Object in temporal t7, using previous result
    move $t0, $v0
    # Polymorphic call to perimeter method
    # Check object type and call appropriate method
    move $a0, $t0
    jal perimeter_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 1
    jal createShape
    # VCALL: t11 = t10.area()
    # Object in temporal t10, using previous result
    move $t0, $v0
    # Polymorphic call to area method
    # Check object type and call appropriate method
    move $a0, $t0
    jal area_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 1
    jal createShape
    # VCALL: t14 = t13.perimeter()
    # Object in temporal t13, using previous result
    move $t0, $v0
    # Polymorphic call to perimeter method
    # Check object type and call appropriate method
    move $a0, $t0
    jal perimeter_polymorphic
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
area_polymorphic:
    # Polymorphic dispatcher for area method
    # $a0 contains object with type info
    
    # Prolog for dispatcher
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    move $fp, $sp
    
    # Check object type
    li $t1, 1
    beq $a0, $t1, area_call_f0
    li $t1, 2
    beq $a0, $t1, area_call_f2
    li $t1, 3
    beq $a0, $t1, area_call_f4
    
    # Default case - should not happen
    li $v0, 0
    j area_dispatcher_end
    
area_call_f0:
    jal f0
    j area_dispatcher_end
    
area_call_f2:
    jal f2
    j area_dispatcher_end
    
area_call_f4:
    jal f4
    j area_dispatcher_end
    
area_dispatcher_end:
    # Epilog for dispatcher
    lw $ra, 12($sp)
    lw $fp, 8($sp)
    addi $sp, $sp, 16
    jr $ra

perimeter_polymorphic:
    # Polymorphic dispatcher for perimeter method
    # $a0 contains object with type info
    
    # Prolog for dispatcher
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    move $fp, $sp
    
    # Check object type
    li $t1, 1
    beq $a0, $t1, perimeter_call_f1
    li $t1, 2
    beq $a0, $t1, perimeter_call_f3
    li $t1, 3
    beq $a0, $t1, perimeter_call_f5
    
    # Default case - should not happen
    li $v0, 0
    j perimeter_dispatcher_end
    
perimeter_call_f1:
    jal f1
    j perimeter_dispatcher_end
    
perimeter_call_f3:
    jal f3
    j perimeter_dispatcher_end
    
perimeter_call_f5:
    jal f5
    j perimeter_dispatcher_end
    
perimeter_dispatcher_end:
    # Epilog for dispatcher
    lw $ra, 12($sp)
    lw $fp, 8($sp)
    addi $sp, $sp, 16
    jr $ra

