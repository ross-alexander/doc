	.file	1 "poisson.c"
	.option pic2

 # GNU C 2.5.8 [AL 1.1, MM 40] SGI running IRIX 5.0 compiled by GNU C

 # Cc1 defaults:
 # -mabicalls

 # Cc1 arguments (-G value = 8, Cpu = default, ISA = 1):
 # -quiet -dumpbase -o

gcc2_compiled.:
__gnu_compiled_c:
	.rdata

	.byte	0x24,0x52,0x65,0x76,0x69,0x73,0x69,0x6f
	.byte	0x6e,0x3a,0x20,0x31,0x2e,0x34,0x36,0x20
	.byte	0x24,0x0

	.byte	0x24,0x52,0x65,0x76,0x69,0x73,0x69,0x6f
	.byte	0x6e,0x3a,0x20,0x31,0x2e,0x38,0x20,0x24
	.byte	0x0
	.text
	.align	2
	.globl	factorial

	.loc	1 6
LM1:
	.ent	factorial
factorial:
	.frame	$fp,24,$31		# vars= 8, regs= 2/0, args= 0, extra= 8
	.mask	0x50000000,-4
	.fmask	0x00000000,0
	.set	noreorder
	.cpload	$25
	.set	reorder
	subu	$sp,$sp,24
	.cprestore 0
	sw	$fp,20($sp)
	sw	$28,16($sp)
	move	$fp,$sp
	s.d	$f12,24($fp)
	li.d	$f0,1.00000000000000000000e+00
	s.d	$f0,8($fp)
	li	$2,0x00000001		# 1
$L2:
	mtc1	$2,$f0
	cvt.d.w	$f0,$f0
	l.d	$f2,24($fp)
	c.le.d	$f0,$f2
	bc1f	$L3
	mtc1	$2,$f0
	cvt.d.w	$f0,$f0
	l.d	$f2,8($fp)
	mul.d	$f0,$f2,$f0
	s.d	$f0,8($fp)
$L4:
	addu	$2,$2,1
	j	$L2
$L3:
	l.d	$f0,8($fp)
	j	$L1
$L1:
	move	$sp,$fp			# sp not trusted here
	lw	$fp,20($sp)
	addu	$sp,$sp,24
	j	$31
	.end	factorial
	.sdata
	.align	2
$LC0:

	.byte	0x6c,0x3a,0x6d,0x3a,0x0
	.align	2
$LC1:

	.byte	0x25,0x64,0x20,0x25,0x6c,0x66,0xa,0x0
	.text
	.align	2
	.globl	main

	.loc	1 18
LM2:
	.ent	main
main:
	.frame	$fp,96,$31		# vars= 40, regs= 3/1, args= 24, extra= 8
	.mask	0xd0000000,-16
	.fmask	0x00300000,-4
	.set	noreorder
	.cpload	$25
	.set	reorder
	subu	$sp,$sp,96
	.cprestore 24
	sw	$31,80($sp)
	sw	$fp,76($sp)
	sw	$28,72($sp)
	s.d	$f20,88($sp)
	move	$fp,$sp
	sw	$4,96($fp)
	sw	$5,100($fp)
	jal	__main
	li.d	$f0,1.40000000000000000000e+01
	s.d	$f0,40($fp)
	li	$2,0x00000014		# 20
	sw	$2,48($fp)
$L6:
	lw	$4,96($fp)
	lw	$5,100($fp)
	la	$6,$LC0
	jal	getopt
	sw	$2,32($fp)
	lw	$2,32($fp)
	li	$3,-1			# 0xffffffff
	beq	$2,$3,$L7
	lw	$2,32($fp)
	li	$3,0x0000006c		# 108
	beq	$2,$3,$L9
	li	$3,0x0000006d		# 109
	beq	$2,$3,$L10
	j	$L12
$L9:
	lw	$4,optarg
	jal	atoi
	mtc1	$2,$f0
	cvt.d.w	$f0,$f0
	s.d	$f0,40($fp)
	j	$L8
$L10:
	lw	$4,optarg
	jal	atoi
	sw	$2,48($fp)
	j	$L8
$L12:
$L8:
	j	$L6
$L7:
	.set	noreorder
	nop
	.set	reorder
	sw	$0,36($fp)
$L13:
	lw	$2,36($fp)
	lw	$3,48($fp)
	slt	$2,$3,$2
	bne	$2,$0,$L14
	l.s	$f0,36($fp)
	cvt.d.w	$f0,$f0
	s.d	$f0,56($fp)
	l.d	$f12,56($fp)
	jal	factorial
	s.d	$f0,64($fp)
	l.d	$f2,40($fp)
	neg.d	$f0,$f2
	mov.d	$f12,$f0
	jal	exp
	mov.d	$f20,$f0
	l.d	$f12,40($fp)
	l.d	$f14,56($fp)
	jal	pow
	mul.d	$f2,$f20,$f0
	l.d	$f4,64($fp)
	div.d	$f0,$f2,$f4
	s.d	$f0,16($sp)
	la	$4,__iob+16
	la	$5,$LC1
	lw	$6,36($fp)
	jal	fprintf
$L15:
	lw	$3,36($fp)
	addu	$2,$3,1
	move	$3,$2
	sw	$3,36($fp)
	j	$L13
$L14:
$L5:
	move	$sp,$fp			# sp not trusted here
	lw	$31,80($sp)
	lw	$fp,76($sp)
	l.d	$f20,88($sp)
	addu	$sp,$sp,96
	j	$31
	.end	main
