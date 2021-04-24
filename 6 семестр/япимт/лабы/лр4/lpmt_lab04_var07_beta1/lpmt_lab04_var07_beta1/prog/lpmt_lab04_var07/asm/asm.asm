section .data
	a:	dd	0
	b:	dq	0.0
	const_48_0_IMQMS80QHT:	dd	0
	tmp_var_int_IMQMS80QHT:	dd	0
section .text
	global main
main:
	finit
	fld		qword [b]
	fild	dword [const_48_0_IMQMS80QHT]
	fcomp
	fistp	dword [tmp_var_int_IMQMS80QHT]
	fstsw	ax
	sahf
	ja lbl_lt_11_IMQMS80QHT
	fldz
	jmp lbl_ex_11_IMQMS80QHT
lbl_lt_11_IMQMS80QHT:
	fld1
lbl_ex_11_IMQMS80QHT:
	fistp	dword [a]
	mov		eax, 0
	ret
