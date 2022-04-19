;  2100, Tue 19 Apr 94
;
;  HWTIMER.ASM:  High-resolution time-of-day routines
;
;  Copyright (C) 1994,  Nevil Brownlee,
;  Computer Centre, The University of Auckland
;
;  void hwt_init(void);	 /* Initialise timer routines */
;  void hwt_fin(void);   /* Restore BIOS timer routines */
;
;  unsigned long hw_time(void);  /* Time in 1/256 clock tick units */
;
	include	ndos.mac	; Assembler Macros
	environ	test
	SETX
	begdata	hwtimer
;
t0_st	dw	0	; 0ABCH = initialised
;
	enddata	hwtimer
	begcode	hwtimer
;
old_t0_int  dd  ?	; Stored in code segment
;			;    This is because we don't know
;			;    where ds will be pointing when we
;			;    call the BIOS timer0 handler!
;
t0count  dw	0	; Nbr of timer interrupts
	db	0
;
t0int	proc	near
	inc	word ptr cs:t0count	; Our IRQ0 (timer 0) handling
	jnz	int_exit
	inc	byte ptr cs:t0count+2
;
int_exit:
	pushf
	call	cs:old_t0_int	; Goto BIOS time-of-day handler
	iret
t0int	endp
;
;
	subrt   hwt_init
;	=====	========
	cmp     [t0_st],0ABCH    ; Interrupts already initialised?
	je      hwtiset
;
	push    ds              ; Save ds
	mov     ax,0            ; Point to bottom of memory
	mov     ds,ax
	mov     bx,0020H	; Address for timer0 interrupt
	mov     cx,offset t0int
	mov     dx,cs
	cli                     ; Disable interrupts
	xchg    [bx],cx         ; Set kb interrupt offset
	xchg    [bx+2],dx       ;    and segment
	sti                     ; Enable interrupts
	pop     ds              ; Restore ds
	mov     word ptr cs:old_t0_int,cx	; Save kb interupt offset
	mov     word ptr cs:old_t0_int+2,dx	;    and segment
	mov     [t0_st],0ABCH
;
hwtiset:
	mov	al,24H	; Counter 0, MSB only, mode 2
	out	43H,al	;   Decr in ones, interrupt every cycle
	mov	al,0
	out	40H,al		; Set MSB
;
	xor	ax,ax		; Zero our counter
	mov     word ptr cs:t0count,ax
	mov	byte ptr cs:t0count+2,al
	endsub  hwt_init
;
;
	subrt   hwt_fin
;	=====	=======
	cmp     [t0_st],0ABCH    ; Interrupts initialised?
	jne     hwtrst
;
	mov     cx,word ptr cs:old_t0_int
	mov     dx,word ptr cs:old_t0_int+2
	push    ds              ; Save ds
	mov     ax,0            ; Point to bottom of memory
	mov     ds,ax
	mov     bx,0020H	; Address for timer0 interrupt
	cli                     ; Disable interrupts
	mov     [bx],cx         ; Restore MSDOS kb interrupt offset
	mov     [bx+2],dx       ;    and segment
	sti                     ; Reenable interrupts
	pop     ds              ; Restore ds
	mov     [t0_st],0000H
;
hwtrst:
	mov	al,36H	; Counter 0, LSB+MSB, mode 3
	out	43H,al	;   Decr in ones, interrupt every second cycle
	mov	al,0
	out	40H,al		; Set LSB
	out	40H,al		;    and MSB
	endsub  hwt_fin
;
;
	function  hw_time
;       ========  =======
	mov	al,00H		; Counter 0, Counter Latch
	cli			; Disable interrupts
	out	43H,al
	in	al,40H		; MSB in al
	mov	ah,byte ptr cs:t0count
	mov	dx,word ptr cs:t0count+1
	sti			; Enable interrupts
;
	mov	bl,al
	mov	al,0FFH		; Counter MSB in 4th byte
	sub	al,bl
	add	al,2
	endfn  hw_time		; Result returned in dx,ax
;
;
	endcode	hwtimer
	end
