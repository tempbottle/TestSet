

PUBLIC GetArgument2			; �൱��c��extern

.486						; create 32 bit code

.model flat,stdcall			; 32 bit memory model

;option casemap :none		; case sensitive

GetArgument PROTO :DWORD	; ��������

_TEXT SEGMENT PUBLIC 'CODE'

GetArgument2 PROC dwArg:DWORD

    INVOKE GetArgument, dwArg	; ����C++��GetArgument(DWORD)

    MOV EAX, dwArg			; return x

    POP EBP					; ƽ���ջ

    RETN 4

GetArgument2 ENDP

_TEXT ENDS

END
