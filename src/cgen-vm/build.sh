#/bin/bash
CC="gcc"

BIN_DIR="bin"
BIN="cgen-vm"

CGEN_VM_DIR="src/cgen-vm"

INCLUDE="include"
$CC -o $BIN_DIR/$BIN $CGEN_VM_DIR/instr.c $CGEN_VM_DIR/vm.c $CGEN_VM_DIR/main.c -I$INCLUDE
