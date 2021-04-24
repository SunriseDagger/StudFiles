.386
.MODEL FLAT, STDCALL

EXTRN	ExitProcess@4:NEAR


;сегмент данных
.data
	tmp_var	dd	? ;переменная, для преобразования типов
	tmp_var_eq	dd	? ;переменная, для сравнения
	a	real8 ?
	b	real8 ?
	b1	dd ?
	bool1	dw ?
	bool2	dw ?
	bool3	dw ?
	bool4	dw ?
	rez1	dd ?
	rez2	dd ?
	total_rez	real8 ?
	var1	real8 ?
	var2	real8 ?

	;Вещественные константы
	constname01	real8	0.1
	constname_minus_14	real8	-1.4
	constname34	real8	3.4
	constname46	real8	4.6


.code
	START:
;Инициализпция математического со процессора
	finit


;Начало выражения
	mov tmp_var, 20
	fild tmp_var
	fstp a


;Конец выражения


;Начало выражения
	fld constname01
	fstp b


;Конец выражения


;Начало выражения
	fld constname_minus_14
	fstp var2


;Конец выражения


;Начало выражения
	mov tmp_var, 0
	fild tmp_var
	fistp rez1


;Конец выражения


;Начало выражения
	mov tmp_var, 2
	fild tmp_var
	fistp rez2


;Конец выражения


;Начало выражения
	fld b
	fistp b1


;Конец выражения


;Начало выражения
	fild rez2
	fstp var1


;Конец выражения


;Начало выражения
	fld a
	fld b
	fstp tmp_var_eq
	fcom tmp_var_eq
	fstp tmp_var_eq
	fstsw ax
	sahf
	je jmp_mark_eq_ab
	;else
		fldz
		jmp jmp_mark_eq_ab_end
	;then
	jmp_mark_eq_ab:
		fld1

	jmp_mark_eq_ab_end:

	fistp bool1


;Конец выражения


;Начало выражения
	fild rez1
	mov tmp_var, 0
	fild tmp_var
	fstp tmp_var_eq
	fcom tmp_var_eq
	fstp tmp_var_eq
	fstsw ax
	sahf
	jne jmp_mark_neq_rez10
	;else
		fldz
		jmp jmp_mark_neq_rez10_end
	;then
	jmp_mark_neq_rez10:
		fld1

	jmp_mark_neq_rez10_end:

	fistp bool2


;Конец выражения


;Начало выражения
	fld b
	fld var2
	fstp tmp_var_eq
	fcom tmp_var_eq
	fstp tmp_var_eq
	fstsw ax
	sahf
	jb jmp_mark_l_bvar2
	;else
		fldz
		jmp jmp_mark_l_bvar2_end
	;then
	jmp_mark_l_bvar2:
		fld1

	jmp_mark_l_bvar2_end:

	fistp bool3


;Конец выражения


;Начало выражения
	fild bool3
	fild bool2
	fstp tmp_var_eq
	fcom tmp_var_eq
	fstp tmp_var_eq
	fstsw ax
	sahf
	jl jmp_mark_l_bool3bool2
	;else
		fldz
		jmp jmp_mark_l_bool3bool2_end
	;then
	jmp_mark_l_bool3bool2:
		fld1

	jmp_mark_l_bool3bool2_end:

	fistp bool4


;Конец выражения


;Начало выражения
	fild rez2
	fld b
	mov tmp_var, 1
	fild tmp_var
	fld a
	fld var2
	fsub
	fsub
	fmul
	fadd
	fistp b1


;Конец выражения


;Начало выражения
	fild b1
	fld constname34
	fsub
	fistp rez1


;Конец выражения


;Начало выражения
	fld constname46
	fld var2
	fadd
	fstp var2


;Конец выражения


;Начало выражения
	fld a
	mov tmp_var, 5
	fild tmp_var
	fstp tmp_var_eq
	fcom tmp_var_eq
	fstp tmp_var_eq
	fstsw ax
	sahf
	jne jmp_mark_neq_a5
	;else
		fldz
		jmp jmp_mark_neq_a5_end
	;then
	jmp_mark_neq_a5:
		fld1

	jmp_mark_neq_a5_end:

	fld b
	mov tmp_var, 1
	fild tmp_var
	fstp tmp_var_eq
	fcom tmp_var_eq
	fstp tmp_var_eq
	fstsw ax
	sahf
	je jmp_mark_eq_b1
	;else
		fldz
		jmp jmp_mark_eq_b1_end
	;then
	jmp_mark_eq_b1:
		fld1

	jmp_mark_eq_b1_end:

	mov tmp_var, 4
	fild tmp_var
	fmul
	fsub
	fstp total_rez


;Конец выражения


	CALL ExitProcess@4
END START
