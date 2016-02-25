.def COUNT=r25			;counter
.def dividend=r22		;dividend register
.def number=r12			;number to be added is divided
.def SUM_7H=r20			;high of sum of 7
.def SUM_7L=r21			;low of sum of 7
.def SUM_3H=r23			;high of sum of 3
.def SUM_3L=r24			;low of sum of 3
.def OVERFLOW=r7		;overflow register for sum

.macro STACK
	ldi @0, high(@1)
	out SPH, @0
	ldi @0, low(@1)
	out SPL, @0
.endmacro

STACK r16, RAMEND

ldi XH, high(RAMEND/2)		;set X pointer to high bits of middle of ramend
ldi XL, low(RAMEND/2)		;set X pointer to low bits of middle of ramend
ldi COUNT, 0				;set counter to 0

loop:
;loop to store numbers in to RAMEND/2 location
	ldi r17, low(RAMEND/2)
	add r17, COUNT
	st X+, r17 
	inc COUNT
	cpi COUNT, 25
	brne loop

;set the X,Y,Z pointers to the first number on the stack
	ldi XH, high(RAMEND/2)
	ldi XL, low(RAMEND/2)
	ldi YH, high(RAMEND/2)
	ldi YL, low(RAMEND/2)
	ldi ZH, high(RAMEND/2)
	ldi ZL, low(RAMEND/2)

again:
	ld number, Z+			;loads number in to the number var
	ld dividend, X+			;loads number to the dividend to be divided
division7:
;loop to divide number by 7
	subi dividend, 7
	cpi dividend, 7
	brsh division7
	cpi dividend, 0
	ld dividend, Y+			;if remainder is 0, then the number is divisible by 7
	breq sum_7
division3:
;loop to divide number by 3
	subi dividend, 3
	cpi dividend, 3
	brsh division3
	cpi dividend, 0			;if remainder is 0, then the number is divisible by 3
	breq sum_3
div_lp:
	dec COUNT
	cpi COUNT, 0			;count of the numbers already used
	brne again
	jmp done

sum_7:
;calculates the sum for division by 7
	add SUM_7L, number
	brvs ovr_flw7
	jmp division3
sum_3:
;calculates the sum for division by 3
	add SUM_3L, number
	brvs ovr_flw3
	jmp div_lp

ovr_flw7:
;both labels will set overflow register is the sum is greater that 8 bits
	ldi r17, 0x08
	mov OVERFLOW, r17		;copies r17 to OVERFLOW(r7) register and set bit 3
	subi SUM_7H, -1
	jmp division3
ovr_flw3:
	ldi r17, 0x08
	mov OVERFLOW, r17 
	subi SUM_3H, -1
	jmp div_lp

done:
	