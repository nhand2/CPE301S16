.org 0

.def rand_num = r16
.def sum = r17
.def counter = r18

.cseg
sbi DDRB, 2
ldi counter, 0x00
ldi sum, 0x00
ldi r19, 0
out PORTB, r19

random:
	ldi rand_num, random%31
	subi rand_num, -30
	add sum, rand_num

	ldi rand_num, ovrflw%31
	subi rand_num, -30
	add sum, rand_num

	ldi rand_num, ovrflw+random %31
	subi rand_num, -30
	add sum, rand_num

	ldi rand_num, ovrflw-random %31
	subi rand_num, -30
	add sum, rand_num

	ldi rand_num, done %31
	subi rand_num, -30
	add sum, rand_num
	brvs ovrflw
	jmp done

ovrflw:
	ldi r19, 4
	out PORTB, r19

done: