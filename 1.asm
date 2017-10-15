.data 
var1: .word 4 
var2: .word 5 
.text 
main: 
lw  $t0, var1 
lw  $t1, var2 
add $s0, $t0, $t1 
end: 
li $v0, 10 
syscall 