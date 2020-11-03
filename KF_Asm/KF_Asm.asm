.data

deltat dd 0.1

A dd 1.0, 0.0, 0.0, 1.0
B dd 0.0, 0.0
C dd 1.0, 0.0

std_dev_v dd 4.0
std_dev_w dd 3.0

v dd 0.0, 0.0, 0.0, 0.0
w dd 0.0

x0 dd 0.0, 0.0 ;stan poczatkowy
P0 dd 1.0, 0.0, 0.0, 1.0 ;
xpri dd  0.0, 0.0
xpost dd 0.0, 0.0
Ppri dd 1.0, 0.0, 0.0, 1.0
Ppost dd 1.0, 0.0, 0.0, 1.0


.code 

filter proc
	;RCX: raw Gyro Data
	;RDX raw Acc Angle
	;R8: filtered Data
	;R9: data Size

;przygotowanie macierzy stanu
xorps xmm0, xmm0 
movss xmm1, [deltat]
subss xmm0, xmm1
movd [A+1], xmm0

;przygotowanie macierzy wejscia
mov eax, [deltat]
mov [B], eax

mov ebx, B

;przygotowanie macierzy kowarniacji szumow
movss xmm0, [deltat]
movss xmm1, [std_dev_v]
mulss xmm0, xmm1
mulss xmm0, xmm1
movd [v], xmm0
movd [V+3], xmm0

movss xmm0, [std_dev_w]
mulss xmm0, xmm0
movd [w], xmm0


mov rbx, 1 ; przygotowanie licznika

for_loop:
	cmp rbx, r9 ;warunek konca petli
	je end_loop

	;mov QWORD PTR [rdx+8*rcx], 100

;aktualizacja czasu
	;movss A

;aktualizacja pomiarow

;zapis wyniku
	

	inc rbx 
	jmp for_loop

end_loop:
mov eax, 0

ret
filter endp

END