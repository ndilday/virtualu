;Filename    : IP_TD.ASM
;Description : Get a pixel of transparent decompressed bitmap


INCLUDE IMGFUN.INC
INCLUDE COLCODE.INC
INCLUDE I_EFFECT.INC

.CODE


;----------- BEGIN OF FUNCTION IMGgetTransDecompress ------
;
; get a pixel of a compressed image
;
; Syntax : int IMGgetTransDecompress( bitmapBuf, x1, y1 )
;
; char *bitmapPtr  - the pointer to the bitmap buffer
; int  x1,y1       - co-orindate relative to bitmap
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

		PUBLIC    IMGgetTransDecompress

SeekForward	PROTO	STDCALL			; see IB_ATRD.ASM

IMGgetTransDecompress PROC bitmapPtr,x1,y1
		LOCAL	bitmapWidth:DWORD, bitmapHeight:DWORD
		STARTPROC

		;------ get the bitmap width and height -----;

		MOV     AX , DS
		MOV	ES , AX
		MOV     ESI, bitmapPtr

		;------ calculate clipWidth - no. of points ploted on each row --;
		XOR	EAX, EAX
		LODSW                            ; get bitmap width
		MOV     bitmapWidth, EAX

		;----- calculate number of rows to be ploted on the screen
		LODSW                            ; get bitmap height
		MOV	bitmapHeight, EAX

		; ----- position ESI to the first point to draw -------
		; AH contain the bytes remained in decompression
		; position to row y1 and column x1
		; ECX = y1 * bitmapWidth + x1
		MOV	EAX, y1
		MUL	bitmapWidth
		ADD	EAX, x1		; DROP EDX
		MOV	ECX, EAX
		MOV	AH,0
		CALL	SeekForward

		MOV	AL, BYTE PTR [ESI]
		JUMP_IF_NOT_TRANS AL, @@nonTrans
		MOV	AL, TRANSPARENT_CODE
@@nonTrans:
		MOVZX	EAX, AL

@@end:          ENDPROC
IMGgetTransDecompress   	ENDP

;----------- END OF FUNCTION IMGgetTransDecompress ----------

END
