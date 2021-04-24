#!/bin/bash
DIRNAME=`dirname "$0"`
cd "$DIRNAME"
mkdir -p builddir

echo -e "### Building translator ... ###"
rm builddir/lpmt_lab04_var07 builddir/main.o 2>/dev/null
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/main.cpp -o builddir/main.o || exit 1
if [ ! -f "builddir/lexeme.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/lexeme.cpp -o builddir/lexeme.o || exit 1
fi
if [ ! -f "builddir/table_var.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/table_var.cpp -o builddir/table_var.o || exit 1
fi
if [ ! -f "builddir/transl_codegen.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_codegen.cpp -o builddir/transl_codegen.o || exit 1
fi
if [ ! -f "builddir/transl_common.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_common.cpp -o builddir/transl_common.o || exit 1
fi
if [ ! -f "builddir/transl_lexical.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_lexical.cpp -o builddir/transl_lexical.o || exit 1
fi
if [ ! -f "builddir/transl_optimize.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_optimize.cpp -o builddir/transl_optimize.o || exit 1
fi
if [ ! -f "builddir/transl_syntactical.o" ] ; then
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_syntactical.cpp -o builddir/transl_syntactical.o || exit 1
fi
g++ -o builddir/lpmt_lab04_var07 builddir/lexeme.o builddir/main.o builddir/table_var.o builddir/transl_codegen.o builddir/transl_common.o builddir/transl_lexical.o builddir/transl_optimize.o builddir/transl_syntactical.o || exit 1

echo -e "### Translator output: ###"
cd lpmt_lab04_var07
../builddir/lpmt_lab04_var07 || exit 1
cd ..

echo -e "\n### Assembling generated code ... ###"
nasm -f elf32 -o builddir/asm.o asm/asm.asm && 
gcc -m32 -o builddir/asm builddir/asm.o && 
rm builddir/asm.o || exit 1

builddir/asm || exit 1

echo -e "\n### Assembler program ended correctly! ###"
exit 0


