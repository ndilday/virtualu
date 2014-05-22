;Filename    : IB_RM.ASM
;Description : Blt a bitmap to the display surface buffer with colour remapping
;              with color key transparency handling and mirroring


INCLUDE IMGFUN.INC
INCLUDE COLCODE.INC

.CODE


;----------- BEGIN OF FUNCTION IMGbltRemapHMirror ------------
;
; Put an non-compressed bitmap on image buffer.
; It does not handle color key transparency but colorRemapping
;
; Syntax : IMGbltRemapHMirror( imageBuf, pitch, x, y, bitmapBuf, colorTable )
;
; char *imageBuf - the pointer to the display surface buffer
; int  pitch        - pitch of the display surface buffer
; int  x,y       - where to put the image on the surface buffer
; char *bitmapPtr  - the pointer to the bitmap buffer
; short *colorTable - a 256-entry color remapping table
;
;-------------------------------------------------
;
; Format of the bitmap data :
;
; <short>  width
; <short>  height
; <char..> bitmap image
;
;-------------------------------------------------

		PUBLIC	IMGbltRemapHMirror

IMGbltRemapHMirror	PROC	imageBuf, pitch, x, y, bitmapPtr, colorTable
		LOCAL	drawWidth
		STARTPROC

		MOV	EAX, imageBuf		; store the address of the image buffer to a variable
		MOV	image_buf, EAX

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX
		MOV     ESI, bitmapPtr

		XOR	EAX, EAX
		LODSW                            ; get bitmap width
		MOV     EBX, EAX
		MOV	drawWidth, EAX

		LODSW                            ; get bitmap height
		MOV     ECX, EAX

		MOV	EDX, pitch	 ; EDX = lineDiff
		ADD	EDX, EBX		 ; lineDiff = pitch - 2*bitmap_width
		ADD	EDX, EBX

		CLD                              ; clear direction flag for MOVSB

		;------- pixels copying loop --------;

		CALC_ADDR_2 EDI, x, y, EBX, 0, pitch              ; Get the offset to the image buffer address
		MOV	EBX, colorTable

		ALIGN	4
@@putLine:
		PUSH	ECX
		MOV     ECX, drawWidth

		ALIGN	4
@@putPoint:
		MOVZX	EAX, BYTE PTR [ESI]
		SUB	EDI,2
		PRE_REMAP
		POST_REMAP
		INC	ESI
		MOV	[EDI], AX
		LOOP	@@putPoint

		ADD	EDI, EDX		 ; EDX = lineDiff

		POP	ECX
		LOOP    @@putLine                ; decrease the remain height and loop

@@end:          ENDPROC
IMGbltRemapHMirror	ENDP

;----------- END OF FUNCTION IMGbltRemapHMirror ----------


END