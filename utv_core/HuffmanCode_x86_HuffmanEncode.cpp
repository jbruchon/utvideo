/* �����R�[�h�͂r�i�h�r ���s�R�[�h�͂b�q�k�e */
/* $Id$ */

	size_t ret;
	void* clobber;
#if defined(__GNUC__) || defined(__clang__)
	asm volatile (
	R"(
	.intel_syntax noprefix

.macro HUFFMAN_ENCODE symtype, symsize

	# edi = pDstBegin
	# esi = pSrcBegin
	# eax = pSrcEnd
	# edx = pEncodeTable

	push		edi
	mov			ebp, eax
	cmp			dword ptr [edx], 0
	je			3f

	mov			bl, -32
	mov			cl, 0

	mov			eax, ebp
	sub			eax, esi
	and			eax, 3 * \symsize

	cmp			eax, 3 * \symsize
	jne			1f
	sub			esi, 1 * \symsize
	jmp			91f

1:
	cmp			eax, 2 * \symsize
	jne			1f
	sub			esi, 2 * \symsize
	jmp			92f

1:
	cmp			eax, 1 * \symsize
	jne			1f
	sub			esi, 3 * \symsize
	jmp			93f

	.balign		64
1:
.irp offset, 0, 1, 2, 3
9\offset:
	shld		eax, ecx, cl
.if \offset == 0
	cmp			esi, ebp
	jnb			4f
.endif

	movzx		ecx, \symtype ptr [esi+\offset * \symsize]
.if \offset == 3
	add			esi, 4 * \symsize
.endif
	mov			ecx, dword ptr [edx+ecx*4]
	add			bl, cl
.if \offset != 3
	jnc			2f
.else
	jnc			1b
.endif
	sub			cl, bl
	shld		eax, ecx, cl
	mov			dword ptr [edi], eax
	add			edi, 4
	add			cl, bl
	sub			bl, 32
.if \offset == 3
	jmp			1b
.endif
2:
.endr

4:
	test		bl, 0x1f
	jz			3f
	neg			bl
	mov			cl, bl
	shl			eax, cl
	mov			dword ptr [edi], eax
	add			edi, 4
3:
	mov			eax, edi
	pop			edi
	sub			eax, edi
.endm
	HUFFMAN_ENCODE )" ARGS R"(
.purgem HUFFMAN_ENCODE
	)"
		: "=a"(ret), "=D"(clobber), "=S"(clobber), "=d"(clobber)
		: "D"(pDstBegin), "S"(pSrcBegin), "a"(pSrcEnd), "d"(pEncodeTable)
		: "ebx", "ecx", "ebp");
#endif
	return ret;

#undef ARGS
