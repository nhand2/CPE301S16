.dseg
	.def XOR1=R24;

.cseg
.MACRO INITSTACK
	LDI @0,HIGH(@1)
	OUT SPH,@0
	LDI @0,LOW(@1)
	OUT SPL,@0
.ENDMACRO

	INITSTACK R16, RAMEND 	;use Macro here

	LDI XOR1,0x01
	SBI DDRC, 0 			;PC.0 as an output
	LDI R17,0
	OUT PORTC,R17 			;PORTC = 0
	LDI R17, 0xFF			
	OUT DDRB, R17			;loads R17(0xFF) to DDRB, to set as output
	LDI R17, 0x00
	OUT PORTB, R17			;initialize PORTB to 0
	OUT DDRD, R17			;sets DDRD as output for use with PD.4
	
	OUT TCNT0, R17			;initialize TCNT0 to 0
	OUT TCCR0A, R17			;normal mode operation counter
	LDI R17, 0x07			;sets counter to accept external clock (PD4)
	OUT TCCR0B, R17
BEGIN:
	RCALL DELAY				;calls delay subroutine
	EOR R17, XOR1			;toggle B0 of R17;
	OUT PORTC,R17 			;toggle PC.0
	IN R16, TCNT0			;loads TCNT0 to R16
	OUT PORTB, R16			;outputs TCNT0 to PORTB
	RJMP BEGIN

;TIMER1 DELAY
DELAY:
	LDI R20,0xF8
	STS TCNT1H,R20		 	;TCNT1H = 0xF8 timer1 high
	LDI R20,0x5F
	STS TCNT1L,R20 			;TCNT1L = 0x5F timer1 low
	LDI R20,0x00
	STS TCCR1A,R20 			;set normal mode
	LDI R20,0x05
	STS TCCR1B,R20 			;Normal mode, prescaler = 1024

AGAIN:
	IN R20,TIFR1 			;read TIFR1
	SBRS R20,TOV1		 	;if TOV1 is set skip next instruction
	RJMP AGAIN
	LDI R20,0x00			;logic 00 if fed to the register to stop the timer
	STS TCCR1B,R20 			;stop Timer1
	LDI R20,0x01			;a logic 1 in the TOV1 bit causes a reset/clear
	OUT TIFR1,R20 			;clear TOV1 flag

	RET
