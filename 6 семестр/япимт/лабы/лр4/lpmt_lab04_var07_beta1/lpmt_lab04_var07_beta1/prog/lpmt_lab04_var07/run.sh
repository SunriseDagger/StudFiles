#!/bin/bash
DIRNAME=`dirname "$0"`
cd "$DIRNAME"
mkdir -p builddir

if [ ! -f "./builddir/lpmt_lab04_var07" ] ; then
echo -e "### Translator is not builded, build ... ###"
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/lexeme.cpp -o builddir/lexeme.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/main.cpp -o builddir/main.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/table_var.cpp -o builddir/table_var.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_codegen.cpp -o builddir/transl_codegen.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_common.cpp -o builddir/transl_common.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_lexical.cpp -o builddir/transl_lexical.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_optimize.cpp -o builddir/transl_optimize.o && 
g++ -O2 -Wall -Wextra -c lpmt_lab04_var07/transl_syntactical.cpp -o builddir/transl_syntactical.o && 
g++ -o builddir/lpmt_lab04_var07 builddir/lexeme.o builddir/main.o builddir/table_var.o builddir/transl_codegen.o builddir/transl_common.o builddir/transl_lexical.o builddir/transl_optimize.o builddir/transl_syntactical.o &&
rm builddir/*.o || exit 1
echo ""
fi

echo -e "### Translator output: ###"
cd lpmt_lab04_var07
../builddir/lpmt_lab04_var07 || exit 1
cd ..

echo -e "\n### Assembling generated code ... ###"
nasm -f elf32 -o builddir/asm.o asm/asm.asm && 
gcc -m32 -o builddir/asm builddir/asm.o && 
rm builddir/*.o || exit 1

builddir/asm || exit 1

echo -e "\n### Assembler program ended correctly! ###"
exit 0


