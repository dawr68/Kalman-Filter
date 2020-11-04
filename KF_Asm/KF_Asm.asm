.data

deltat dd 0.1

A dd 1.0, 0, 0, 1.0
B dd 0, 0
C dd 1.0, 0

std_dev_v dd 4.0
std_dev_w dd 3.0

v dd 0, 0, 0, 0
w dd 0

xpri dd  0, 0
xpost dd 0, 0
Ppri dd 1.0, 0, 0, 1.0
Ppost dd 1.0, 0, 0, 1.0


.code 

filter proc
	;RCX: raw Gyro Data
	;RDX raw Acc Angle
	;R8: filtered Data
	;R9: data Size

	;XMM0: u

;przygotowanie macierzy stanu
xorps xmm0, xmm0 
movss xmm1, [deltat]
subss xmm0, xmm1
movd [A+4], xmm0

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
movd [V+12], xmm0

movss xmm0, [std_dev_w]
mulss xmm0, xmm0
movd [w], xmm0

mov eax, DWORD PTR[rdx] ;przypisanie wartosci poczatkowej
mov [xpost], eax ; xpost[0] = rawAccAngle[0];

mov rbx, 1 ; przygotowanie licznika

for_loop:
	cmp rbx, r9 ;warunek konca petli
	je end_loop

	movss xmm0, DWORD PTR[rcx+rbx*4] ;u = rawGyroData[i];

	;aktualizacja czasu
	;xpri[0] = A[0][0] * xpost[0] + A[0][1] * xpost[1] + B[0]*u;
	movlps xmm1, QWORD PTR[A]
	movlps xmm2, QWORD PTR[xpost]
	mulps xmm1, xmm2
	shufps	xmm2, xmm1, 01h
	addps xmm1, xmm2
	movss xmm2, DWORD PTR[B]
	mulss xmm2, xmm0
	addps xmm1, xmm2
	movd [xpri], xmm1

	;xpri[1] = A[1][1] * xpost[1] + B[1]*u;
	movss xmm1, DWORD PTR[A+12]
	mulss xmm1, DWORD PTR[xpost+4]
	movss xmm2, DWORD PTR[B+4]
	mulss xmm2, xmm0
	addss xmm1, xmm2
	movd [xpri+4], xmm1


;aktualizacja pomiarow

;zapis wyniku
	

	inc rbx 
	jmp for_loop

end_loop:
mov eax, 0

ret
filter endp

END