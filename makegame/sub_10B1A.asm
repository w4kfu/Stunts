; =============== S U B	R O U T	I N E =======================================

; Attributes: bp-based frame

; int __cdecl __far sub_10B1A(int type_graphic)
sub_10B1A	proc far		; CODE XREF: _main+5E9p

HandleFileCOD	= word ptr -12h
allocmem_seg	= word ptr -10h
allocmem_offset	= word ptr -0Eh
FileNameCod	= word ptr -0Ch
FileNameDif	= word ptr -0Ah
actualvalue	= word ptr -8
FileHandle	= word ptr -6
Segment		= word ptr -4
Offset		= word ptr -2
type_graphic	= word ptr  6

		push	bp
		mov	bp, sp
		sub	sp, 14h
		cmp	[bp+type_graphic], 1

loc_10B24:
		jnz	short loc_10B3A
		mov	ax, offset aCga_dif ; "cga.dif"
		push	ax		; char *
		call	_strdup
		add	sp, 2
		mov	[bp+FileNameDif], ax
		mov	ax, offset aCga_cod ; "cga.cod"
		jmp	short loc_10B66
; ---------------------------------------------------------------------------

loc_10B3A:				; CODE XREF: sub_10B1A:loc_10B24j
		cmp	[bp+type_graphic], 2
		jnz	short loc_10B54
		mov	ax, offset aTdy_dif ; "tdy.dif"
		push	ax		; char *
		call	_strdup
		add	sp, 2
		mov	[bp+FileNameDif], ax
		mov	ax, offset aTdy_cod ; "tdy.cod"
		jmp	short loc_10B66
; ---------------------------------------------------------------------------

loc_10B54:				; CODE XREF: sub_10B1A+24j
		mov	ax, offset aMcga_dif ; "mcga.dif"
		push	ax		; char *
		call	_strdup
		add	sp, 2
		mov	[bp+FileNameDif], ax
		mov	ax, offset aMcga_cod ; "mcga.cod"

loc_10B66:				; CODE XREF: sub_10B1A+1Ej
					; sub_10B1A+38j
		push	ax		; char *
		call	_strdup
		add	sp, 2
		mov	[bp+FileNameCod], ax
		push	[bp+FileNameDif] ; char	*
		push	cs
		call	near ptr SearchFileOnDisk
		add	sp, 2
		cmp	uncompress_option, 0
		jz	short loc_10B9C
		push	cs
		call	near ptr mmalloc
		mov	[bp+allocmem_seg], ax
		mov	[bp+allocmem_offset], dx
		push	dx		; int
		push	ax		; int
		push	[bp+FileNameDif] ; FileName
		call	HuffmanDecompress
		add	sp, 6
		jmp	short loc_10BC0
; ---------------------------------------------------------------------------

loc_10B9C:				; CODE XREF: sub_10B1A+67j
		mov	ax, offset aRB	; "r+b"
		push	ax
		push	[bp+FileNameDif] ; char	*
		call	_fopen
		add	sp, 4
		mov	[bp+FileHandle], ax
		or	ax, ax
		jnz	short loc_10BC0
		push	[bp+FileNameDif]
		mov	ax, offset aUnableToOpen_0 ; "Unable to	open %s."
		push	ax		; eMessage
		push	cs
		call	near ptr ExitMessage
; ---------------------------------------------------------------------------
		db  83h	; .
		db 0C4h	; Ä
		db    4
; ---------------------------------------------------------------------------

loc_10BC0:				; CODE XREF: sub_10B1A+80j
					; sub_10B1A+96j
		mov	ax, Segment
		mov	dx, Offset
		mov	[bp+Segment], ax
		mov	[bp+Offset], dx
		or	ax, ax
		jnz	short loc_10BD6
		sub	[bp+Offset], 1000h

loc_10BD6:				; CODE XREF: sub_10B1A+B5j
		dec	[bp+Segment]
		sub	ax, ax
		push	ax
		push	ax
		push	[bp+Offset]
		push	[bp+Segment]
		push	cs
		call	near ptr ComputeOffset
		add	sp, 8
		mov	[bp+Segment], ax
		mov	[bp+Offset], dx
		cmp	uncompress_option, 0
		jnz	short loc_10BFA
		jmp	loc_10CDD
; ---------------------------------------------------------------------------

loc_10BFA:				; CODE XREF: sub_10B1A+DBj
					; sub_10B1A+133j ...
		lea	ax, [bp+allocmem_seg]
		push	ax
		push	cs
		call	near ptr GetShortM
		add	sp, 2
		mov	[bp+actualvalue], ax
		or	ax, ax
		jnz	short loc_10C0F
		jmp	loc_10CF9
; ---------------------------------------------------------------------------

loc_10C0F:				; CODE XREF: sub_10B1A+F0j
		sub	dx, dx
		and	ah, 7Fh
		push	dx
		push	ax
		push	[bp+Offset]
		push	[bp+Segment]
		push	cs
		call	near ptr ComputeOffset
		add	sp, 8
		mov	[bp+Segment], ax
		mov	[bp+Offset], dx
		les	bx, dword ptr [bp+allocmem_seg]
		inc	[bp+allocmem_seg]
		mov	al, es:[bx]
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx], al
		les	bx, dword ptr [bp+allocmem_seg]
		inc	[bp+allocmem_seg]
		mov	al, es:[bx]
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx+1], al
		test	[bp+actualvalue], 8000h
		jz	short loc_10BFA
		les	bx, dword ptr [bp+allocmem_seg]
		inc	[bp+allocmem_seg]
		mov	al, es:[bx]
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx+2], al
		les	bx, dword ptr [bp+allocmem_seg]
		inc	[bp+allocmem_seg]
		mov	al, es:[bx]
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx+3], al
		jmp	short loc_10BFA
; ---------------------------------------------------------------------------
		align 2

loc_10C72:				; CODE XREF: sub_10B1A+1D2j
		mov	ax, [bp+actualvalue]
		sub	dx, dx
		and	ah, 7Fh
		push	dx
		push	ax
		push	[bp+Offset]
		push	[bp+Segment]
		push	cs
		call	near ptr ComputeOffset
		add	sp, 8
		mov	[bp+Segment], ax
		mov	[bp+Offset], dx
		push	[bp+FileHandle]	; FILE *
		call	_fgetc
		add	sp, 2
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx], al
		push	[bp+FileHandle]	; FILE *
		call	_fgetc
		add	sp, 2
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx+1], al
		test	[bp+actualvalue], 8000h
		jz	short loc_10CDD
		push	[bp+FileHandle]	; FILE *
		call	_fgetc
		add	sp, 2
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx+2], al
		push	[bp+FileHandle]	; FILE *
		call	_fgetc
		add	sp, 2
		les	bx, dword ptr [bp+Segment]
		mov	es:[bx+3], al

loc_10CDD:				; CODE XREF: sub_10B1A+DDj
					; sub_10B1A+19Dj
		push	[bp+FileHandle]	; FileHandle
		push	cs
		call	near ptr ReadShortF
		add	sp, 2
		mov	[bp+actualvalue], ax
		or	ax, ax
		jnz	short loc_10C72
		push	[bp+FileHandle]	; FILE *
		call	_fclose
		add	sp, 2

loc_10CF9:				; CODE XREF: sub_10B1A+F2j
		mov	ax, word_15810
		mov	dx, word_15812
		mov	[bp+Segment], ax
		mov	[bp+Offset], dx
		cmp	uncompress_option, 0
		jz	short loc_10D17
		push	[bp+allocmem_offset]
		push	cs
		call	near ptr FreeMemory
		add	sp, 2

loc_10D17:				; CODE XREF: sub_10B1A+1F1j
		push	[bp+FileNameCod] ; char	*
		push	cs
		call	near ptr SearchFileOnDisk
		add	sp, 2
		cmp	word_15800, 0
		jnz	short loc_10D2F
		cmp	uncompress_option, 0
		jnz	short loc_10D7C

loc_10D2F:				; CODE XREF: sub_10B1A+20Cj
		mov	ax, 8000h
		push	ax
		push	[bp+FileNameCod] ; char	*
		call	_open
		add	sp, 4
		mov	[bp+HandleFileCOD], ax
		inc	ax
		jnz	short loc_10D52
		push	[bp+FileNameCod]
		mov	ax, offset aUnableToOpenS_ ; "Unable to	open %s."
		push	ax		; eMessage
		push	cs
		call	near ptr ExitMessage
; ---------------------------------------------------------------------------
		db  83h	; .
		db 0C4h	; Ä
		db    4
; ---------------------------------------------------------------------------

loc_10D52:				; CODE XREF: sub_10B1A+228j
		push	[bp+FileNameCod]
		push	[bp+HandleFileCOD]
		mov	ax, 0FFFFh
		mov	dx, 7FFFh
		push	dx
		push	ax
		push	[bp+Offset]
		push	[bp+Segment]
		push	cs
		call	near ptr MemcpyFromFile
		add	sp, 0Ch
		push	[bp+HandleFileCOD] ; int
		call	_close
		add	sp, 2
		mov	sp, bp
		pop	bp
		retf
; ---------------------------------------------------------------------------

loc_10D7C:				; CODE XREF: sub_10B1A+213j
		push	[bp+Offset]	; int
		push	[bp+Segment]	; int
		push	[bp+FileNameCod] ; FileName
		call	HuffmanDecompress
		add	sp, 6
		mov	sp, bp
		pop	bp
		retf
sub_10B1A	endp

; ---------------------------------------------------------------------------
