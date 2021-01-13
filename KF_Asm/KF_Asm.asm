.data

deltat dd 0.1

A dd 1.0, 0.0, 0.0, 1.0
B dd 0.0, 0.0
H dd 1.0, 0.0

std_dev_v dd 1.0
std_dev_w dd 2.0

v dd 1.0, 0.0, 0.0, 1.0
w dd 0.0

xpri dd  0.0, 0.0
xpost dd 0.0, 0.0
Ppri dd 1.0, 0.0, 0.0, 1.0
Ppost dd 1.0, 0.0, 0.0, 1.0

one110 dd 1.0, 1.0, 1.0, 0.0
one000 dd 1.0, 0.0, 0.0, 0.0


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
shufps xmm0, xmm0, 3Ch
movups [v], xmm0

movups xmm0, [v]

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
	haddps xmm1, xmm1
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


	;Ppri = A * Ppost * A' + V;

	movups xmm0, DWORD PTR[Ppost]
	movups xmm1, DWORD PTR[v]
	addps xmm0, xmm1 ;Ppri[i] = Ppost[i]+V[i]

	movss xmm1, DWORD PTR[A+4]
	shufps xmm1, xmm1, 0h
	mulps xmm1, DWORD PTR[Ppost] ;Ppost[i] *= A[1]

	movaps xmm2, xmm1
	shufps xmm2, xmm2, 3Dh
	mulps xmm2, DWORD PTR[one110]
	addps xmm0, xmm2 ;Ppri[0] += Ppost[1]*A[1]; Ppri[1] += Ppost[3]*A[1]; Ppri[2] += Ppost[3]*A[1]; Ppri[3] += 0

	movaps xmm2, xmm1
	shufps xmm2, xmm2, 02h
	mulps xmm2, DWORD PTR[one000]
	addps xmm0, xmm2 ;Ppri[0] += Ppost[2]*A[2]

	shufps xmm1, xmm1, 03h
	mulps xmm1, DWORD PTR[one000]
	mulss xmm1, DWORD PTR[A+4] ;Ppost[3]*A[1]*A[1]
	addps xmm0, xmm1 ;Ppri[0] += Ppost[3]*A[1]*A[1]
	movups [Ppri], xmm0


;aktualizacja pomiarow

    ;eps = Y(i) - H*xpri;
	movss xmm1, DWORD PTR[RDX+rbx*4]
	subss xmm1, DWORD PTR[xpri] ;eps = rawAccAngle[i] - xpri[0];

	;float S = H * Ppri * H' + W;
	movss xmm2, DWORD PTR[Ppri]
	addss xmm2, DWORD PTR[w] ;S = Ppri00 + W

	;float K = Ppri * H' * (1 / S);
	movss xmm3, DWORD PTR[Ppri]
	divss xmm3, xmm2 ; k0 = Ppri0 / S
	movss xmm4, DWORD PTR[Ppri + 8]
	divss xmm4, xmm2 ; k1 = Ppri0 / S
	movlhps xmm3, xmm4
	shufps xmm3, xmm3, 08h ;xmm3 [- - K1 K0]

	;xpost = xpri + K * eps;
	movss xmm5, xmm1
	shufps xmm5, xmm5, 0h
	mulps xmm5, xmm3 ; eps * k[1]
	movups xmm4, DWORD PTR[xpri]
	addps xmm5, xmm4 ; xpost[j] = xpri[j] + k[j]*eps
	movlps QWORD PTR[xpost], xmm5

	;Ppost = Ppri - K * S * K';
	movups xmm6, DWORD PTR[Ppri]
	movaps xmm7, xmm3
	shufps xmm7, xmm7, 40h
	movaps xmm8, xmm3
	shufps xmm8, xmm8, 54h
	mulps xmm8, xmm7
	shufps xmm2, xmm2, 0h
	mulps xmm8, xmm2
	subps xmm6, xmm8
	movaps [Ppost], xmm6

;zapis wyniku
	movss xmm0, DWORD PTR[xpost] ;zaladuj xpost[0]
	movss DWORD PTR[r8 + rbx*4], xmm0 ;zapisz w odpowiednie miejsce tablicy filteredData

	inc rbx 
	jmp for_loop

end_loop:
mov eax, 0

ret
filter endp

END