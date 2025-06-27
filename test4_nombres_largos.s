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

    # DEBUG: Discovered method: calculateComplexResult
    # DEBUG: Discovered method: calculateComplexResult
    # DEBUG: Discovered method: calculateComplexResult
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
    li $v0, 42
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
    li $v0, 999
    # Epilog
    lw $ra, 60($sp)
    lw $fp, 56($sp)
    addi $sp, $sp, 64
    jr $ra

getCalculator:
    # Function: getCalculator
    # Prolog
    addi $sp, $sp, -64
    sw $ra, 60($sp)
    sw $fp, 56($sp)
    move $fp, $sp
    sw $a0, 4($fp)  # complexity
    # Binary operation: t0 = complexity > 10.000000
    lw $t0, 4($fp)
    li $t1, 10
    sgt $t2, $t0, $t1
    # Temporal t0 result stored in $t2
    # IF t0 == 0 GOTO L0
    beq $t2, $zero, L0
    # ALLOCATE UltraAdvancedCalculation -> t2
    li $t2, 1
    # Object of type UltraAdvancedCalculation allocated with ID 1
    # SETATTR t2.vtable = vtable_UltraAdvancedCalculation
    # VTable set for object t2
    # Temporal assignment: t1 = t2 (register only)
    j L1
L0:
    # Binary operation: t3 = complexity > 5.000000
    lw $t0, 4($fp)
    li $t1, 5
    sgt $t2, $t0, $t1
    # Temporal t3 result stored in $t2
    # IF t3 == 0 GOTO L2
    beq $t2, $zero, L2
    # ALLOCATE AdvancedCalculation -> t5
    li $t2, 2
    # Object of type AdvancedCalculation allocated with ID 2
    # SETATTR t5.vtable = vtable_AdvancedCalculation
    # VTable set for object t5
    # Temporal assignment: t4 = t5 (register only)
    j L3
L2:
    # ALLOCATE MathematicalOperation -> t6
    li $t2, 3
    # Object of type MathematicalOperation allocated with ID 3
    # SETATTR t6.vtable = vtable_MathematicalOperation
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
    li $a0, 3
    jal getCalculator
    # VCALL: t8 = t7.calculateComplexResult()
    # Object in temporal t7, using previous result
    move $t0, $v0
    # Polymorphic call to calculateComplexResult method
    # Check object type and call appropriate method
    move $a0, $t0
    jal calculateComplexResult_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 7
    jal getCalculator
    # VCALL: t11 = t10.calculateComplexResult()
    # Object in temporal t10, using previous result
    move $t0, $v0
    # Polymorphic call to calculateComplexResult method
    # Check object type and call appropriate method
    move $a0, $t0
    jal calculateComplexResult_polymorphic
    move $a0, $v0
    li $v0, 1
    syscall
    li $a0, 10
    li $v0, 11
    syscall
    li $a0, 15
    jal getCalculator
    # VCALL: t14 = t13.calculateComplexResult()
    # Object in temporal t13, using previous result
    move $t0, $v0
    # Polymorphic call to calculateComplexResult method
    # Check object type and call appropriate method
    move $a0, $t0
    jal calculateComplexResult_polymorphic
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
calculateComplexResult_polymorphic:
    # Polymorphic dispatcher for calculateComplexResult method
    # $a0 contains object with type info
    
    # Prolog for dispatcher
    addi $sp, $sp, -16
    sw $ra, 12($sp)
    sw $fp, 8($sp)
    move $fp, $sp
    
    # Check object type
    li $t1, 1
    beq $a0, $t1, calculateComplexResult_call_f0
    li $t1, 2
    beq $a0, $t1, calculateComplexResult_call_f1
    li $t1, 3
    beq $a0, $t1, calculateComplexResult_call_f2
    
    # Default case - should not happen
    li $v0, 0
    j calculateComplexResult_dispatcher_end
    
calculateComplexResult_call_f0:
    jal f0
    j calculateComplexResult_dispatcher_end
    
calculateComplexResult_call_f1:
    jal f1
    j calculateComplexResult_dispatcher_end
    
calculateComplexResult_call_f2:
    jal f2
    j calculateComplexResult_dispatcher_end
    
calculateComplexResult_dispatcher_end:
    # Epilog for dispatcher
    lw $ra, 12($sp)
    lw $fp, 8($sp)
    addi $sp, $sp, 16
    jr $ra

