;
;
;  Usage :
;	    -pktentry
;           _pktasminit( void far * buffers, int maxbufs, int buflen)
;
;  (c) 1991 University of Waterloo,
;           Faculty of Engineering,
;           Engineering Microcomputer Network Development Office
;
;  version
;
;    0.1    22-May-1992   E. P. Engelke
;
;
        include masmdefs.hsm
        include model.hsm

ORIGINAL  equ  0         ; AU

	cextrn	pkt_rcv_1
	cextrn	pkt_rcv_2
	cextrn	test1
	cextrn	test2
codedef ASMPKT
datadef

datastart
STACK_FILL      equ     0CC33H  ; Stack for AU interrupt handler
ISTACK_SIZE     equ     512     ; bytes
        dw      256 dup (STACK_FILL)
END_ISTACK      equ     $
dataend

cstart  ASMPKT

helper_ds	DW	?
helper_ss	DW	?
helper_sp	DW	?
interface	DW	?

cproc	pktentry0       ; AU: packet receive code from pcip
	pushf
	cli
	mov interface, 0
	jmp entry
cendp	pktentry0

cproc	pktentry1       ; AU: packet receive code from pcip
	pushf
	cli
	mov interface, 1
	jmp entry
cendp	pktentry1

proc	main
	ASSUME	DS:NOTHING, SS:NOTHING, ES:NOTHING
	; Squirrel away the packet driver's critical registers...

entry:	mov	helper_ds,ds
	mov	helper_ss,ss
	mov	helper_sp,sp

	or	ax,ax		; First or second call?
				; DON'T DO ANYTHING TO CHANGE FLAGS
				; UNTIL THE JNZ SECOND_CALL, BELOW

	; Get a stack and a way to reach our data segment...
	; MOV instructions don't affect the flags
	mov	ax,DGROUP
	mov	ss,ax
	ASSUME	SS:DGROUP
	mov	ds,ax
	ASSUME	DS:DGROUP
	mov	sp,OFFSET DGROUP: END_ISTACK

	; Save some of the packet driver's other registers...
	pushf			; Preserve direction-flag and int-flag

	cld			; Microsoft C needs this, doesn't
				; harm the flags set by the or ax,ax
				; above

	jnz	short second_call	; Check result of or ax,ax
					; way above
	push	cx		; len
	ccall	pkt_rcv_1	; First call returns a far pointer
				; in dx (segment) & ax (offset)
	add	sp,4		; Remove the parameters from the stack
	mov	es,dx		; segment of far ptr returned
	mov	di,ax		; offset of far ptr returned
help_ret:
	popf			; Restore int status and direction flag
	mov	ds,helper_ds
	ASSUME	DS:NOTHING
	mov	ss,helper_ss
	ASSUME	SS:NOTHING
	mov	sp,helper_sp

	popf
	retf

	ASSUME	DS:DGROUP, SS:DGROUP
second_call:
	push	helper_ds	; Segment part of buffer
	push	si		; Offset part of buffer
	push	cx		; len
	push	bx		; handle
	push	interface	; interface
	ccall	pkt_rcv_2	; Nothing returned
	add	sp,10		; Remove the parameters from the stack
	jmp	short help_ret
endp    main

cend    ASMPKT
        end
