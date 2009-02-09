echo q_test for Windows DOS box 2002/6/1 >q_test.x
echo === errors if 'user_eg' and 'user_eg2' not found ===
echo === check user_eg === >>q_test.x
echo "user_eg 'QR1 p n Qt' 'v p' Fp" >>q_test.x
user_eg "QR1 p n Qt" "v p" Fp >>q_test.x
echo "user_eg2 'QR1 p' 'v p' Fp" >>q_test.x
user_eg2 "QR1 p" "v p" Fp >>q_test.x
echo === check front ends ========================================================== >>q_test.x
echo "qconvex -" >>q_test.x
qconvex - >>q_test.x
echo "qconvex ." >>q_test.x
qconvex . >>q_test.x
echo "qconvex" >>q_test.x
qconvex >>q_test.x
echo "rbox c D3 | qconvex s n Qt" >>q_test.x
rbox c D3 | qconvex s n Qt >>q_test.x
echo "rbox c D2 | qconvex i Qt " >>q_test.x
rbox c D2 | qconvex i Qt  >>q_test.x
echo "rbox c D2 | qconvex o" >>q_test.x
rbox c D2 | qconvex o >>q_test.x
echo "rbox 1000 s | qconvex s Tv FA" >>q_test.x
rbox 1000 s | qconvex s Tv FA >>q_test.x
echo "rbox c d D2 | qconvex s Qc Fx" >>q_test.x
rbox c d D2 | qconvex s Qc Fx >>q_test.x
echo "rbox y 1000 W0 | qconvex s n " >>q_test.x
rbox y 1000 W0 | qconvex s n  >>q_test.x
echo "rbox y 1000 W0 | qconvex s QJ" >>q_test.x
rbox y 1000 W0 | qconvex s QJ >>q_test.x
echo "rbox d G1 D12 | qconvex QR0 FA" >>q_test.x
rbox d G1 D12 | qconvex QR0 FA >>q_test.x
echo "rbox c D6 | qconvex FA TF500" >>q_test.x
rbox c D6 | qconvex FA TF500 >>q_test.x
echo "rbox c P0 d D2 | qconvex p Fa Fc FP FI Fn FN FS Fv Fx" >>q_test.x
rbox c P0 d D2 | qconvex p Fa Fc FP FI Fn FN FS Fv Fx >>q_test.x
echo "rbox c d D2 | qconvex s i QV0" >>q_test.x
rbox c d D2 | qconvex s i QV0 >>q_test.x
echo "rbox c | qconvex Q0" >>q_test.x
rbox c | qconvex Q0 >>q_test.x
echo "qvoronoi -" >>q_test.x
qvoronoi - >>q_test.x
echo "qvoronoi ." >>q_test.x
qvoronoi . >>q_test.x
echo "qvoronoi" >>q_test.x
qvoronoi >>q_test.x
echo "rbox c P0 D2 | qvoronoi s o" >>q_test.x
rbox c P0 D2 | qvoronoi s o >>q_test.x
echo "rbox c P0 D2 | qvoronoi Fi" >>q_test.x
rbox c P0 D2 | qvoronoi Fi >>q_test.x
echo "rbox c P0 D2 | qvoronoi Fo" >>q_test.x
rbox c P0 D2 | qvoronoi Fo >>q_test.x
echo "rbox c P0 D2 | qvoronoi Fv" >>q_test.x
rbox c P0 D2 | qvoronoi Fv >>q_test.x
echo "rbox c P0 D2 | qvoronoi s Qu Qt Fv" >>q_test.x
rbox c P0 D2 | qvoronoi s Qu Qt Fv >>q_test.x
echo "rbox c P0 D2 | qvoronoi Qu Fo" >>q_test.x
rbox c P0 D2 | qvoronoi Qu Fo >>q_test.x
echo "rbox c G1 d D2 | qvoronoi s p " >>q_test.x
rbox c G1 d D2 | qvoronoi s p  >>q_test.x
echo "rbox c G1 d D2 | qvoronoi QJ p " >>q_test.x
rbox c G1 d D2 | qvoronoi QJ p  >>q_test.x
echo "rbox c P-0.1 P+0.1 P+0.1 D2 | qvoronoi s Fc FP FQ Fn FN " >>q_test.x
rbox c P-0.1 P+0.1 P+0.1 D2 | qvoronoi s Fc FP FQ Fn FN  >>q_test.x
echo "rbox P0 c D2 | qvoronoi s Fv QV0" >>q_test.x
rbox P0 c D2 | qvoronoi s Fv QV0 >>q_test.x
echo "qdelaunay -" >>q_test.x
qdelaunay - >>q_test.x
echo "qdelaunay ." >>q_test.x
qdelaunay . >>q_test.x
echo "qdelaunay" >>q_test.x
qdelaunay >>q_test.x
echo "rbox c P0 D2 | qdelaunay s o" >>q_test.x
rbox c P0 D2 | qdelaunay s o >>q_test.x
echo "rbox c P0 D2 | qdelaunay i" >>q_test.x
rbox c P0 D2 | qdelaunay i >>q_test.x
echo "rbox c P0 D2 | qdelaunay Fv" >>q_test.x
rbox c P0 D2 | qdelaunay Fv >>q_test.x
echo "rbox c P0 D2 | qdelaunay s Qu Qt Fv" >>q_test.x
rbox c P0 D2 | qdelaunay s Qu Qt Fv >>q_test.x
echo "rbox c G1 d D2 | qdelaunay s i" >>q_test.x
rbox c G1 d D2 | qdelaunay s i >>q_test.x
echo "rbox c G1 d D2 | qdelaunay Ft" >>q_test.x
rbox c G1 d D2 | qdelaunay Ft >>q_test.x
echo "rbox c G1 d D2 | qdelaunay QJ s Ft" >>q_test.x
rbox c G1 d D2 | qdelaunay QJ s Ft >>q_test.x
echo "rbox M3,4 z 100 D2 | qdelaunay s" >>q_test.x
rbox M3,4 z 100 D2 | qdelaunay s >>q_test.x
echo "rbox c P-0.1 P+0.1 P+0.1 D2 | qdelaunay s Fx Fa Fc FP FQ Fn FN" >>q_test.x
rbox c P-0.1 P+0.1 P+0.1 D2 | qdelaunay s Fx Fa Fc FP FQ Fn FN >>q_test.x
echo "rbox P0 P0 c D2 | qdelaunay s FP QV0" >>q_test.x
rbox P0 P0 c D2 | qdelaunay s FP QV0 >>q_test.x
echo "qhalf -" >>q_test.x
qhalf - >>q_test.x
echo "qhalf ." >>q_test.x
qhalf . >>q_test.x
echo "qhalf" >>q_test.x
qhalf >>q_test.x
echo "rbox d | qhull FQ n | qhalf s Qt H0,0,0 Fp" >>q_test.x
rbox d | qhull FQ n | qhalf s H0,0,0 Fp >>q_test.x
echo "rbox c | qhull FQ FV n | qhalf s i" >>q_test.x
rbox c | qhull FQ FV n | qhalf s i >>q_test.x
echo "rbox c | qhull FQ FV n | qhalf o" >>q_test.x
rbox c | qhull FQ FV n | qhalf o >>q_test.x
echo "rbox d D2 | qhull FQ n | qhalf s H0 Fc FP Fn FN FQ Fv Fx" >>q_test.x
rbox d D2 | qhull FQ n | qhalf s H0 Fc FP Fn FN FQ Fv Fx >>q_test.x

echo === check quality of merges for ${d:-`hostname`} ${d:-`date`} >>q_test.x
echo "rbox 1000 W0 t | qhull QR2 QJ s Fs Tv" >>q_test.x
rbox 1000 W0 t | qhull QR2 QJ s Fs Tv >>q_test.x
echo "rbox 1000 W0 t | qhull QR2 s Fs Tv" >>q_test.x
rbox 1000 W0 t | qhull QR2 s Fs Tv >>q_test.x
echo "rbox 1000 s t | qhull C0.02 Qc Tv" >>q_test.x
rbox 1000 s t | qhull C0.02 Qc Tv >>q_test.x
echo "rbox 500 s D4 t | qhull C0.01 Qc Tv" >>q_test.x
rbox 500 s D4 t | qhull C0.01 Qc Tv >>q_test.x
echo "rbox 1000 s t | qhull C-0.02 Qc Tv" >>q_test.x
rbox 1000 s t | qhull C-0.02 Qc Tv >>q_test.x
echo "rbox 1000 s t | qhull C-0.02 Qc Tv" >>q_test.x
rbox 1000 s t | qhull C-0.02 Qc Tv >>q_test.x
echo "rbox 1000 s D4 t | qhull C-0.01 Qc Tv" >>q_test.x
rbox 1000 s D4 t | qhull C-0.01 Qc Tv >>q_test.x
echo "rbox 200 s D5 t | qhull C-0.01 Qx Qc Tv" >>q_test.x
rbox 200 s D5 t | qhull C-0.01 Qx Qc Tv >>q_test.x
echo "rbox 100 s D6 t | qhull C-0.001 Qx Qc Tv" >>q_test.x
rbox 100 s D6 t | qhull C-0.001 Qx Qc Tv >>q_test.x
echo "rbox 1000 W1e-4 t | qhull C-1e-6 Qc Tv" >>q_test.x
rbox 1000 W1e-4 t | qhull C-1e-6 Qc Tv >>q_test.x
echo "rbox 1000 W5e-4 D4 t | qhull C-1e-5 Qc Tv" >>q_test.x
rbox 1000 W5e-4 D4 t | qhull C-1e-5 Qc Tv >>q_test.x
echo "rbox 400 W1e-3 D5 t | qhull C-1e-5 Qx Qc Tv" >>q_test.x
rbox 400 W1e-3 D5 t | qhull C-1e-5 Qx Qc Tv >>q_test.x

echo === check input format etc. ${d:-`date`} >>q_test.x
echo "rbox d h | qhull Fd FV n FD  Tcv | qhull Fd H Fp Tcv" >>q_test.x
rbox d h | qhull Fd FV n FD Tcv | qhull Fd H Fp Tcv >>q_test.x
echo "rbox 10 h | qhull Fd FD p Tcv | qhull Fd d Tcv " >>q_test.x
rbox 10 h | qhull Fd FD p Tcv | qhull Fd d Tcv  >>q_test.x

echo === check rbox ${d:-`date`} >>q_test.x
echo "rbox 3 n D2" >>q_test.x
rbox 3 n D2 >>q_test.x
echo "rbox 3 D2" >>q_test.x
rbox 3 D2 >>q_test.x
echo "rbox 3 h D2" >>q_test.x
rbox 3 h D2 >>q_test.x
echo "rbox 3 z D2" >>q_test.x
rbox 3 z D2 >>q_test.x
echo "rbox 3 z h D2" >>q_test.x
rbox 3 z h D2 >>q_test.x
echo "rbox 3 B10 D2" >>q_test.x
rbox 3 B10 D2 >>q_test.x
echo "rbox 3 z B10 D2" >>q_test.x
rbox 3 z B10 D2 >>q_test.x
echo "rbox 4 L2 r D2" >>q_test.x
rbox 4 L2 r D2 >>q_test.x
echo "rbox 8 L2 D2" >>q_test.x
rbox 8 L2 D2 >>q_test.x
echo "rbox 4 L4 r D3" >>q_test.x
rbox 4 L4 r D3 >>q_test.x
echo "rbox y" >>q_test.x
rbox y >>q_test.x
echo "rbox 10 M3,4" >>q_test.x
rbox 10 M3,4 >>q_test.x
echo "rbox 4 L4 s D5 W1e-3 " >>q_test.x
rbox 4 L4 s D5 W1e-3  >>q_test.x
echo "rbox 10 L2 s D3 | qhull Tcv" >>q_test.x
rbox 10 L2 s D3 | qhull Tcv >>q_test.x
echo "rbox 10 L4 s W1e-3 D3 | qhull Tcv" >>q_test.x
rbox 10 L4 s W1e-3 D3 | qhull Tcv >>q_test.x
echo "rbox 10 L6 D3 | qhull Tcv" >>q_test.x
rbox 10 L6 D3 | qhull Tcv >>q_test.x
echo "rbox 10 L1.1 s D4 | qhull Tcv" >>q_test.x
rbox 10 L1.1 s D4 | qhull Tcv >>q_test.x
echo "rbox y r 100 W0 O0.5 | qhull s p Tcv" >>q_test.x
rbox y r 100 W0 O0.5 | qhull s p Tcv >>q_test.x
echo "rbox x r 100 W0 O0.5 | qhull s p Tcv" >>q_test.x
rbox x r 100 W0 O0.5 | qhull s p Tcv >>q_test.x
echo "rbox 12 D8 | qhull Tcv" >>q_test.x
rbox 12 D8 | qhull Tcv >>q_test.x
echo "rbox 12 D9 | qhull Tcv" >>q_test.x
rbox 12 D9 | qhull Tcv >>q_test.x
echo "rbox 1000 D4 | qhull s i A-0.97 C0.2 A0.7 Tcv" >>q_test.x
rbox 1000 D4 | qhull s i A-0.97 C0.2 A0.7 Tcv >>q_test.x
echo "rbox 3 D2 | qhull Qb0B1:-2 p " >>q_test.x
rbox 3 D2 | qhull Qb0B1:-2 p  >>q_test.x
echo "rbox 100 r D2 | qhull Pd0:0.7 PD0:0.8 n Tcv" >>q_test.x
rbox 100 r D2 | qhull Pd0:0.7 PD0:0.8 n Tcv >>q_test.x
echo "rbox 1000 s | qhull  C0.05 Tcv" >>q_test.x
rbox 1000 s | qhull  C0.05 Tcv >>q_test.x
echo "rbox 1000 s t | qhull Qm C0.05 Tcv" >>q_test.x
rbox 1000 s t | qhull Qm C0.05 Tcv >>q_test.x
echo "rbox 500 D2 | qhull n A-0.95 C0.1 Tcv" >>q_test.x
rbox 500 D2 | qhull n A-0.95 C0.1 Tcv >>q_test.x
echo "rbox 500 s P1,1,1 | qhull QgG0 Pp Tcv" >>q_test.x
rbox 500 s P1,1,1 | qhull QgG0 Pp Tcv >>q_test.x

echo "rbox d | qhull m" >>q_test.x
rbox d | qhull m >>q_test.x
echo "rbox d | qhull FM" >>q_test.x
rbox d | qhull FM >>q_test.x
echo "rbox c D2 | qhull Q0 Tcv" >>q_test.x
rbox c D2 | qhull Q0 Tcv >>q_test.x
echo "rbox d D2 | qhull Tcv" >>q_test.x
rbox d D2 | qhull Tcv >>q_test.x
echo "rbox c D3 | qhull Q0 Tcv" >>q_test.x
rbox c D3 | qhull Q0 Tcv >>q_test.x
echo "rbox d D3 | qhull Tcv" >>q_test.x
rbox d D3 | qhull Tcv >>q_test.x
echo "rbox c D4 | qhull Q0 Tcv" >>q_test.x
rbox c D4 | qhull Q0 Tcv >>q_test.x
echo "rbox d D4 | qhull Tcv" >>q_test.x
rbox d D4 | qhull Tcv >>q_test.x
echo "rbox c D5 | qhull Q0 Tcv" >>q_test.x
rbox c D5 | qhull Q0 Tcv >>q_test.x
echo "rbox d D5 | qhull Tcv" >>q_test.x
rbox d D5 | qhull Tcv >>q_test.x
echo "rbox c D6 | qhull Q0 Tcv" >>q_test.x
rbox c D6 | qhull Q0 Tcv >>q_test.x
echo "rbox d D6 | qhull Tcv" >>q_test.x
rbox d D6 | qhull Tcv >>q_test.x
echo "rbox d D7 | qhull Tcv" >>q_test.x
rbox d D7 | qhull Tcv >>q_test.x
echo "rbox c D2 | qhull Tcv C-0" >>q_test.x
rbox c D2 | qhull Tcv C-0 >>q_test.x
echo "rbox c D3 | qhull Tcv C-0" >>q_test.x
rbox c D3 | qhull Tcv C-0 >>q_test.x
echo "rbox c D4 | qhull Tcv C-0" >>q_test.x
rbox c D4 | qhull Tcv C-0 >>q_test.x
echo "rbox c D5 | qhull Tcv C-0" >>q_test.x
rbox c D5 | qhull Tcv C-0 >>q_test.x
echo "rbox c D6 | qhull Tcv C-0" >>q_test.x
rbox c D6 | qhull Tcv C-0 >>q_test.x
echo "rbox c D7 | qhull Tv C-0" >>q_test.x
rbox c D7 | qhull Tv C-0 >>q_test.x
echo "rbox 20 l D3 | qhull Tcv" >>q_test.x
rbox 20 l D3 | qhull Tcv >>q_test.x
echo "rbox 100 s D2 | qhull Tcv" >>q_test.x
rbox 100 s D2 | qhull Tcv >>q_test.x
echo "rbox 100 s D3 | qhull Tcv" >>q_test.x
rbox 100 s D3 | qhull Tcv >>q_test.x
echo "rbox 100 s D4 | qhull Tcv" >>q_test.x
rbox 100 s D4 | qhull Tcv >>q_test.x
echo "rbox 100 s c D4 | qhull Tcv" >>q_test.x
rbox 100 s c D4 | qhull Tcv >>q_test.x
echo "rbox 100 s d G1.5 D4 | qhull Tcv" >>q_test.x
rbox 100 s d G1.5 D4 | qhull Tcv >>q_test.x
echo "rbox 100 s W1e-2 | qhull Tcv" >>q_test.x
rbox 100 s W1e-2 | qhull Tcv >>q_test.x
echo "rbox 100 | qhull Tcv" >>q_test.x
rbox 100 | qhull Tcv >>q_test.x
echo "rbox 100 W1e-3 | qhull Tcv" >>q_test.x
rbox 100 W1e-3 | qhull Tcv >>q_test.x
echo "rbox 100 r D2 | qhull Tcv" >>q_test.x
rbox 100 r D2 | qhull Tcv >>q_test.x
echo "rbox 100 r s Z1 | qhull Tcv" >>q_test.x
rbox 100 r s Z1 | qhull Tcv >>q_test.x
echo "rbox 100 r s Z1 G0.1 | qhull Tcv C-0" >>q_test.x
rbox 100 r s Z1 G0.1 | qhull Tcv C-0 >>q_test.x
echo "rbox 100 s Z1 G0.1 | qhull Tcv " >>q_test.x
rbox 100 s Z1 G0.1 | qhull Tcv  >>q_test.x
echo "rbox 100 s Z1e-5 G0.1 | qhull Tc Pp" >>q_test.x
rbox 100 s Z1e-5 G0.1 | qhull Tc Pp >>q_test.x

echo === check qhull output formats ${d:-`date`} >>q_test.x
echo "rbox 5 r s D2 | qhull Tcv" >>q_test.x
rbox 5 r s D2 | qhull Tcv >>q_test.x
echo "rbox 5 r s D2 | qhull s " >>q_test.x
rbox 5 r s D2 | qhull s  >>q_test.x
echo "rbox 5 r s D2 | qhull s o " >>q_test.x
rbox 5 r s D2 | qhull s o  >>q_test.x
echo "rbox 5 r s D2 | qhull f" >>q_test.x
rbox 5 r s D2 | qhull f >>q_test.x
echo "rbox 5 r s D2 | qhull i " >>q_test.x
rbox 5 r s D2 | qhull i  >>q_test.x
echo "rbox 5 r s D2 | qhull m " >>q_test.x
rbox 5 r s D2 | qhull m  >>q_test.x
echo "rbox 5 r s D2 | qhull FM " >>q_test.x
rbox 5 r s D2 | qhull FM  >>q_test.x
echo "rbox 5 r s D2 | qhull n " >>q_test.x
rbox 5 r s D2 | qhull n  >>q_test.x
echo "rbox 5 r s D2 | qhull p " >>q_test.x
rbox 5 r s D2 | qhull p  >>q_test.x
echo "rbox 5 r s D2 | qhull o " >>q_test.x
rbox 5 r s D2 | qhull o  >>q_test.x
echo "rbox 5 r s D2 | qhull Fx" >>q_test.x
rbox 5 r s D2 | qhull Fx >>q_test.x
echo "rbox 5 r s D2 | qhull p n i p p" >>q_test.x
rbox 5 r s D2 | qhull p n i p p >>q_test.x
echo "rbox 10 D3 | qhull f Tcv" >>q_test.x
rbox 10 D3 | qhull f Tcv >>q_test.x
echo "rbox 10 D3 | qhull i" >>q_test.x
rbox 10 D3 | qhull i >>q_test.x
echo "rbox 10 D3 | qhull p " >>q_test.x
rbox 10 D3 | qhull p  >>q_test.x
echo "rbox 10 D3 | qhull o " >>q_test.x
rbox 10 D3 | qhull o  >>q_test.x
echo "rbox 10 D3 | qhull Fx " >>q_test.x
rbox 10 D3 | qhull Fx  >>q_test.x
echo "rbox 27 M1,0,1 | qhull Qc" >>q_test.x
rbox 27 M1,0,1 | qhull Qc >>q_test.x
echo "rbox 50 D3 s | qhull C0.1 Qc Pd0d1d2 s p Tcv" >>q_test.x
rbox 50 D3 s | qhull C0.1 Qc Pd0d1d2 s p Tcv >>q_test.x
echo "rbox 10 D2 P0 P1e-15 | qhull d Qc FP s Tcv" >>q_test.x
rbox 10 D2 P0 P1e-15 | qhull d Qc FP s Tcv >>q_test.x
echo "rbox 100 s | qhull C-0.003 Qc FP s" >>q_test.x
rbox 100 s | qhull C-0.003 Qc FP s >>q_test.x
echo "rbox 100 s D2 | qhull C0.1 i Fx Tcv" >>q_test.x
rbox 100 s D2 | qhull C0.1 i Fx Tcv >>q_test.x
echo "rbox 4 s D3 | qhull Qc Ghipv Tcv " >>q_test.x
rbox 4 s D3 | qhull Qc Ghipv Tcv  >>q_test.x
echo "rbox 6 D4 | qhull f Tcv" >>q_test.x
rbox 6 D4 | qhull f Tcv >>q_test.x
echo "rbox 6 D4 | qhull i" >>q_test.x
rbox 6 D4 | qhull i >>q_test.x
echo "rbox 6 D4 | qhull p " >>q_test.x
rbox 6 D4 | qhull p  >>q_test.x
echo "rbox 6 D4 | qhull o" >>q_test.x
rbox 6 D4 | qhull o >>q_test.x
echo "rbox 1000 s D2 | qhull FA Tcv" >>q_test.x
rbox 1000 s D2 | qhull FA Tcv >>q_test.x
echo "rbox 1000 s | qhull FA Tcv" >>q_test.x
rbox 1000 s | qhull FA Tcv >>q_test.x
echo "rbox c D4 | qhull FA Tcv" >>q_test.x
rbox c D4 | qhull FA Tcv >>q_test.x
echo "rbox c D5 | qhull FA Tcv" >>q_test.x
rbox c D5 | qhull FA Tcv >>q_test.x
echo "rbox 10 D2 | qhull d FA Tcv" >>q_test.x
rbox 10 D2 | qhull d FA Tcv >>q_test.x
echo "rbox 10 D2 | qhull d Qu FA Tcv" >>q_test.x
rbox 10 D2 | qhull d Qu FA Tcv >>q_test.x
echo "rbox 10 D2 | qhull FA Tcv" >>q_test.x
rbox 10 D2 | qhull FA Tcv >>q_test.x
echo "rbox 10 c D2 | qhull Fx Tcv" >>q_test.x
rbox 10 c D2 | qhull Fx Tcv >>q_test.x
echo "rbox 1000 s | qhull FS Tcv" >>q_test.x
rbox 1000 s | qhull FS Tcv >>q_test.x
echo "rbox 10 W0 D2 | qhull p Qc FcC Tcv" >>q_test.x
rbox 10 W0 D2 | qhull p Qc FcC Tcv >>q_test.x
echo "rbox 4 z h s D2 | qhull Fd s n FD Tcv" >>q_test.x
rbox 4 z h s D2 | qhull Fd s n FD Tcv >>q_test.x
echo "rbox 6 s D3 | qhull C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv" >>q_test.x
rbox 6 s D3 | qhull C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv >>q_test.x
echo "rbox P0.5,0.5 P0.5,0.5 W0 5 D2 | qhull d FN Qc" >>q_test.x
rbox P0.5,0.5 P0.5,0.5 W0 5 D2 | qhull d FN Qc >>q_test.x
echo "rbox 10 D3 | qhull Fa PA5" >>q_test.x
rbox 10 D3 | qhull Fa PA5 >>q_test.x
echo "rbox 10 D3 | qhull Fa PF0.4" >>q_test.x
rbox 10 D3 | qhull Fa PF0.4 >>q_test.x

echo === test Qt >>q_test.x
echo "rbox c | qhull Qt s o Tcv" >>q_test.x
rbox c | qhull Qt s o Tcv >>q_test.x
echo "rbox c | qhull Qt f i" >>q_test.x
rbox c | qhull Qt f i >>q_test.x
echo "rbox c | qhull Qt m FM n" >>q_test.x
rbox c | qhull Qt m FM n >>q_test.x
echo "rbox c | qhull Qt p o" >>q_test.x
rbox c | qhull Qt p o >>q_test.x
echo "rbox c | qhull Qt Fx" >>q_test.x
rbox c | qhull Qt Fx >>q_test.x
echo "rbox c | qhull Qt FA s Fa" >>q_test.x
rbox c | qhull Qt FA s Fa >>q_test.x
echo "rbox 6 r s c G0.1 D2 | qhull Qt d FA Tcv" >>q_test.x
rbox 6 r s c G0.1 D2 | qhull Qt d FA Tcv >>q_test.x
echo "rbox 6 r s c G0.1 D2 | qhull d FA Tcv" >>q_test.x
rbox 6 r s c G0.1 D2 | qhull d FA Tcv >>q_test.x
echo "rbox 6 r s c G0.1 D2 | qhull Qt v p Tcv" >>q_test.x
rbox 6 r s c G0.1 D2 | qhull Qt v p Tcv >>q_test.x
echo "rbox c | qhull Qt C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv" >>q_test.x
rbox c | qhull Qt C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv >>q_test.x
echo "rbox 6 r s c G0.1 D2 P0.1,0.1 | qhull s FP d FO Qt" >>q_test.x
rbox 6 r s c G0.1 D2 P0.1,0.1 | qhull s FP d FO Qt >>q_test.x
echo "RBOX 100 W0 | QHULL Tv Q11 FO" >>q_test.x
RBOX 100 W0 | QHULL Tv Q11 FO >>q_test.x

echo === test unbounded intersection >>q_test.x
echo "rbox c | qhull PD0:0.5 n | qhull H0 Fp Tcv" >>q_test.x
rbox c | qhull PD0:0.5 n | qhull H0 Fp Tcv >>q_test.x
echo "rbox 1000 W1e-3 D3 | qhull PA8 Fa FS s n Tcv " >>q_test.x
rbox 1000 W1e-3 D3 | qhull PA8 Fa FS s n Tcv  >>q_test.x
echo "rbox 1000 W1e-3 D3 | qhull C-0.01 PM10 Fm n Tcv Qc" >>q_test.x
rbox 1000 W1e-3 D3 | qhull C-0.01 PM10 Fm n Tcv Qc >>q_test.x
echo "rbox 1000 W1e-3 D3 | qhull C-0.01 PA8 PG n Tcv Qc" >>q_test.x
rbox 1000 W1e-3 D3 | qhull C-0.01 PA8 PG n Tcv Qc >>q_test.x
rbox 10 | qhull FO Tz TO q_test.log.1 
echo "type q_test.log.1" >>q_test.x
type q_test.log.1 >>q_test.x

echo === check Delaunay/Voronoi ${d:-`date`} >>q_test.x
echo "rbox 10 D2 | qhull d Tcv" >>q_test.x
rbox 10 D2 | qhull d Tcv >>q_test.x
echo "rbox 10 D2 | qhull d Qz Tcv" >>q_test.x
rbox 10 D2 | qhull d Qz Tcv >>q_test.x
echo "rbox 10 D3 | qhull d Tcv" >>q_test.x
rbox 10 D3 | qhull d Tcv >>q_test.x
echo "rbox c | qhull d Qz Ft Tcv" >>q_test.x
rbox c | qhull d Qz Ft Tcv >>q_test.x
echo "rbox 10 s D2 c | qhull d Tcv" >>q_test.x
rbox 10 s D2 c | qhull d Tcv >>q_test.x
echo "rbox 10 s D2 | qhull d Tcv Qz Q8" >>q_test.x
rbox 10 s D2 | qhull d Tcv Qz Q8 >>q_test.x
echo "rbox 10 D2 | qhull d Tcv p" >>q_test.x
rbox 10 D2 | qhull d Tcv p >>q_test.x
echo "rbox 10 D2 | qhull d Tcv i" >>q_test.x
rbox 10 D2 | qhull d Tcv i >>q_test.x
echo "rbox 10 D2 | qhull d Tcv o" >>q_test.x
rbox 10 D2 | qhull d Tcv o >>q_test.x
echo "rbox 10 D2 | qhull v Tcv o" >>q_test.x
rbox 10 D2 | qhull v Tcv o >>q_test.x
echo "rbox 10 D2 | qhull v Tcv p" >>q_test.x
rbox 10 D2 | qhull v Tcv p >>q_test.x
echo "rbox 10 D2 | qhull v Tcv G" >>q_test.x
rbox 10 D2 | qhull v Tcv G >>q_test.x
echo "rbox 10 D2 | qhull v Tcv Fv" >>q_test.x
rbox 10 D2 | qhull v Tcv Fv >>q_test.x
echo "rbox 10 D2 | qhull v Tcv Fi" >>q_test.x
rbox 10 D2 | qhull v Tcv Fi >>q_test.x
echo "rbox 10 D2 | qhull v Tcv Fo" >>q_test.x
rbox 10 D2 | qhull v Tcv Fo >>q_test.x
echo "rbox 10 D2 | qhull v Qu Tcv o Fv Fi Fo" >>q_test.x
rbox 10 D2 | qhull v Qu Tcv o Fv Fi Fo >>q_test.x
echo "rbox 10 D3 | qhull v Tcv Fv" >>q_test.x
rbox 10 D3 | qhull v Tcv Fv >>q_test.x
echo "rbox 10 D3 | qhull v Tcv Fi" >>q_test.x
rbox 10 D3 | qhull v Tcv Fi >>q_test.x
echo "rbox 10 D3 | qhull v Tcv Fo" >>q_test.x
rbox 10 D3 | qhull v Tcv Fo >>q_test.x
echo "rbox 10 D3 | qhull v Qu Tcv o Fv Fi Fo" >>q_test.x
rbox 10 D3 | qhull v Qu Tcv o Fv Fi Fo >>q_test.x
echo "rbox 5 D2 | qhull v f FnN o" >>q_test.x
rbox 5 D2 | qhull v f FnN o >>q_test.x

echo === check Halfspace ${d:-`date`} >>q_test.x
echo "rbox 100 s D4 | qhull FA FV n s Tcv | qhull H Fp Tcv | qhull FA  Tcv" >>q_test.x
rbox 100 s D4 | qhull FA s Tcv >>q_test.x
rbox 100 s D4 | qhull FA FV n s Tcv | qhull H Fp Tcv | qhull FA  Tcv >>q_test.x
echo "rbox d D3 | qhull FQ n s FD Tcv | qhull Fd H0.1,0.1 Fp FQ Tcv" >>q_test.x
rbox d D3 | qhull s >>q_test.x
rbox d D3 | qhull FQ n FD Tcv | qhull Fd H0.1,0.1 Fp FQ Tcv >>q_test.x
echo "rbox 5 r D2 | qhull s n Tcv | qhull H0 Fp Tcv" >>q_test.x
rbox 5 r D2 | qhull >>q_test.x
rbox 5 r D2 | qhull n Tcv | qhull H0 Fp Tcv >>q_test.x

echo === check qhull ${d:-`date`} >>q_test.x
echo "rbox 10 s D3 | qhull Tcv" >>q_test.x
rbox 10 s D3 | qhull Tcv >>q_test.x
echo "rbox 10 s D3 | qhull f Pd0:0.5 Pd2 Tcv" >>q_test.x
rbox 10 s D3 | qhull f Pd0:0.5 Pd2 Tcv >>q_test.x
echo "rbox 10 s D3 | qhull f Tcv PD2:-0.5" >>q_test.x
rbox 10 s D3 | qhull f Tcv PD2:-0.5 >>q_test.x
echo "rbox 10 s D3 | qhull QR-1" >>q_test.x
rbox 10 s D3 | qhull QR-1 >>q_test.x
echo "rbox 10 s D3 | qhull QR-40" >>q_test.x
rbox 10 s D3 | qhull QR-40 >>q_test.x
echo "rbox 1000 D3 | qhull Tcvs" >>q_test.x
rbox 1000 D3 | qhull Tcvs >>q_test.x
echo "rbox 100 s D3 | qhull TcvV-2" >>q_test.x
rbox 100 s D3 | qhull TcvV-2 >>q_test.x
echo "rbox 100 s D3 | qhull TcvC2" >>q_test.x
rbox 100 s D3 | qhull TcvC2 >>q_test.x
echo "rbox 100 s D3 | qhull TcvV2" >>q_test.x
rbox 100 s D3 | qhull TcvV2 >>q_test.x
echo "rbox 100 s D3 | qhull T1cvV2P2" >>q_test.x
rbox 100 s D3 | qhull T1cvV2P2 >>q_test.x
echo "rbox 100 s D3 | qhull TcvF100" >>q_test.x
rbox 100 s D3 | qhull TcvF100 >>q_test.x
echo "rbox 100 s D3 | qhull Qf Tcv" >>q_test.x
rbox 100 s D3 | qhull Qf Tcv >>q_test.x
echo "rbox 100 D3 | qhull Tcv" >>q_test.x
rbox 100 D3 | qhull Tcv >>q_test.x
echo "rbox 100 D3 | qhull Qs Tcv" >>q_test.x
rbox 100 D3 | qhull Qs Tcv >>q_test.x
echo "rbox 100 D5 | qhull Qs Tcv" >>q_test.x
rbox 100 D5 | qhull Qs Tcv >>q_test.x
echo "rbox 100 D3 | qhull Qr Tcv" >>q_test.x
rbox 100 D3 | qhull Qr Tcv >>q_test.x
echo "rbox 100 D3 | qhull Qxv Tcv" >>q_test.x
rbox 100 D3 | qhull Qxv Tcv >>q_test.x
echo "rbox 100 D3 | qhull Qi f Pd0 Pd1 Pd2 Tcv" >>q_test.x
rbox 100 D3 | qhull Qi f Pd0 Pd1 Pd2 Tcv >>q_test.x
echo "rbox c d | qhull Qc f Tcv" >>q_test.x
rbox c d | qhull Qc f Tcv >>q_test.x
echo "rbox c d | qhull Qc p Tcv" >>q_test.x
rbox c d | qhull Qc p Tcv >>q_test.x
echo "rbox 100 D3 | qhull QbB FO Tcv" >>q_test.x
rbox 100 D3 | qhull QbB FO Tcv >>q_test.x
echo "rbox 1000 D2 B1e6 | qhull d Qbb FO Tcv" >>q_test.x
rbox 1000 D2 B1e6 | qhull d Qbb FO Tcv >>q_test.x
echo "rbox 10 D3 | qhull QbB p Tcv" >>q_test.x
rbox 10 D3 | qhull QbB p Tcv >>q_test.x
echo "rbox 10 D3 | qhull Qbb p Tcv" >>q_test.x
rbox 10 D3 | qhull Qbb p Tcv >>q_test.x
echo "rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv" >>q_test.x
rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv >>q_test.x
echo "rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv | qhull QbB p Tcv" >>q_test.x
rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv | qhull QbB p Tcv >>q_test.x
echo "rbox 10 D3 | qhull Qb1:0B1:0 d Tcv Q8" >>q_test.x
rbox 10 D3 | qhull Qb1:0B1:0 d Tcv Q8 >>q_test.x
echo "rbox 10 D3 | qhull Qb1:0B1:0B2:0 d Tcv Q8" >>q_test.x
rbox 10 D3 | qhull Qb1:0B1:0B2:0 d Tcv Q8 >>q_test.x
echo "rbox 10 D3 | qhull Qb1:0 d Tcv " >>q_test.x
rbox 10 D3 | qhull Qb1:0 d Tcv  >>q_test.x
echo "rbox 10 D3 | qhull Qb1:0B1:0  Tcv" >>q_test.x
rbox 10 D3 | qhull Qb1:0B1:0  Tcv >>q_test.x
echo "== next command will error ==" >>q_test.x
echo "rbox 10 D2 | qhull Qb1:1B1:1 Tcv" >>q_test.x
rbox 10 D2 | qhull Qb1:1B1:1 Tcv >>q_test.x
echo "rbox 200 L20 D2 t | qhull FO Tcv C-0" >>q_test.x
rbox 200 L20 D2 t | qhull FO Tcv C-0 >>q_test.x
echo "rbox 1000 L20 t | qhull FO Tcv C-0" >>q_test.x
rbox 1000 L20 t | qhull FO Tcv C-0 >>q_test.x
echo "rbox 200 L20 D4 t | qhull FO Tcv C-0" >>q_test.x
rbox 200 L20 D4 t | qhull FO Tcv C-0 >>q_test.x
echo "rbox 200 L20 D5 t | qhull FO Tcv Qx" >>q_test.x
rbox 200 L20 D5 t | qhull FO Tcv Qx >>q_test.x
echo "rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu Q0" >>q_test.x
rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu Q0 >>q_test.x
echo "rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu C-0" >>q_test.x
rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu C-0 >>q_test.x

echo === check joggle and TRn ${d:-`date`} >>q_test.x
echo "rbox 100 W0 | qhull QJ1e-14 Qc TR100 Tv" >>q_test.x
rbox 100 W0 | qhull QJ1e-14 Qc TR100 Tv >>q_test.x
echo "rbox 100 W0 | qhull QJ1e-13 Qc TR100 Tv" >>q_test.x
rbox 100 W0 | qhull QJ1e-13 Qc TR100 Tv >>q_test.x
echo "rbox 100 W0 | qhull QJ1e-12 Qc TR100 Tv" >>q_test.x
rbox 100 W0 | qhull QJ1e-12 Qc TR100 Tv >>q_test.x
echo "rbox 100 W0 | qhull QJ1e-11 Qc TR100 Tv" >>q_test.x
rbox 100 W0 | qhull QJ1e-11 Qc TR100 Tv >>q_test.x
echo "rbox 100 W0 | qhull QJ1e-10 Qc TR100 Tv" >>q_test.x
rbox 100 W0 | qhull QJ1e-10 Qc TR100 Tv >>q_test.x
echo "rbox 100 | qhull d QJ Qb0:1e4 QB0:1e5 Qb1:1e4 QB1:1e6 Qb2:1e5 QB2:1e7 FO Tv" >>q_test.x
rbox 100 | qhull d QJ Qb0:1e4 QB0:1e5 Qb1:1e4 QB1:1e6 Qb2:1e5 QB2:1e7 FO Tv >>q_test.x

echo === check precision options ${d:-`date`} >>q_test.x
echo "rbox 100 D3 s | qhull E0.01 Qx Tcv FO" >>q_test.x
rbox 100 D3 s | qhull E0.01 Qx Tcv FO >>q_test.x
echo "rbox 100 D3 W1e-1 | qhull W1e-3 Tcv" >>q_test.x
rbox 100 D3 W1e-1 | qhull W1e-3 Tcv >>q_test.x
echo "rbox 100 D3 W1e-1 | qhull W1e-2 Tcv Q0" >>q_test.x
rbox 100 D3 W1e-1 | qhull W1e-2 Tcv Q0 >>q_test.x
echo "rbox 100 D3 W1e-1 | qhull W1e-2 Tcv" >>q_test.x
rbox 100 D3 W1e-1 | qhull W1e-2 Tcv >>q_test.x
echo "rbox 100 D3 W1e-1 | qhull W1e-1 Tcv" >>q_test.x
rbox 100 D3 W1e-1 | qhull W1e-1 Tcv >>q_test.x
echo "rbox 15 D2 P0 P1e-14,1e-14 | qhull d Quc Tcv" >>q_test.x
rbox 15 D2 P0 P1e-14,1e-14 | qhull d Quc Tcv >>q_test.x
echo "rbox 15 D3 P0 P1e-12,1e-14,1e-14 | qhull d Qcu Tcv" >>q_test.x
rbox 15 D3 P0 P1e-12,1e-14,1e-14 | qhull d Qcu Tcv >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Tcv Qc >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 V0 Qc Tcv" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 V0 Qc Tcv >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 U0 Qc Tcv" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 U0 Qc Tcv >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 V0 Qcm Tcv" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 V0 Qcm Tcv >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Qcm Tcv" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Qcm Tcv >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q1 FO Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q1 FO Tcv Qc >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q2 FO Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q2 FO Tcv Qc >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q3 FO Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q3 FO Tcv Qc >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q4 FO Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q4 FO Tcv Qc >>q_test.x
echo === this may generate an error ${d:-`date`} >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q5 FO Tcv " >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q5 FO Tcv  >>q_test.x
echo === this should generate an error ${d:-`date`} >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q6 FO Po Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q6 FO Po Tcv Qc >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Q7 FO Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Q7 FO Tcv Qc >>q_test.x
echo "rbox 1000 s D3 | qhull C-0.01 Qx Tcv Qc" >>q_test.x
rbox 1000 s D3 | qhull C-0.01 Qx Tcv Qc >>q_test.x
echo "rbox 100 s D3 t | qhull R1e-3 Tcv Qc" >>q_test.x
rbox 100 s D3 t | qhull R1e-3 Tcv Qc >>q_test.x
echo "rbox 100 s D3 t | qhull R1e-2 Tcv Qc" >>q_test.x
rbox 100 s D3 t | qhull R1e-2 Tcv Qc >>q_test.x
echo "rbox 500 s D3 t | qhull R0.05 A-1 Tcv Qc" >>q_test.x
rbox 500 s D3 t | qhull R0.05 A-1 Tcv Qc >>q_test.x
echo "rbox 100 W0 D3 t | qhull R1e-3 Tcv Qc" >>q_test.x
rbox 100 W0 D3 t | qhull R1e-3 Tcv Qc >>q_test.x
echo "rbox 100 W0 D3 t | qhull R1e-3 Qx Tcv Qc" >>q_test.x
rbox 100 W0 D3 t | qhull R1e-3 Qx Tcv Qc >>q_test.x
echo "rbox 100 W0 D3 t | qhull R1e-2 Tcv Qc" >>q_test.x
rbox 100 W0 D3 t | qhull R1e-2 Tcv Qc >>q_test.x
echo "rbox 100 W0 D3 t | qhull R1e-2 Qx Tcv Qc" >>q_test.x
rbox 100 W0 D3 t | qhull R1e-2 Qx Tcv Qc >>q_test.x
echo "rbox 500 W0 D3 t | qhull R0.05 A-1 Tcv Qc" >>q_test.x
rbox 500 W0 D3 t | qhull R0.05 A-1 Tcv Qc >>q_test.x
echo "rbox 500 W0 D3 t | qhull R0.05 Qx Tcv Qc" >>q_test.x
rbox 500 W0 D3 t | qhull R0.05 Qx Tcv Qc >>q_test.x
echo "rbox 1000 W1e-20 t | qhull Tcv Qc" >>q_test.x
rbox 1000 W1e-20 t | qhull Tcv Qc >>q_test.x
echo "rbox 1000 W1e-20 D4 t | qhull Tcv Qc" >>q_test.x
rbox 1000 W1e-20 D4 t | qhull Tcv Qc >>q_test.x
echo "rbox 500 W1e-20 D5 t | qhull Tv Qc" >>q_test.x
rbox 500 W1e-20 D5 t | qhull Tv Qc >>q_test.x
echo "rbox 100 W1e-20 D6 t | qhull Tv Qc" >>q_test.x
rbox 100 W1e-20 D6 t | qhull Tv Qc >>q_test.x
echo "rbox 50 W1e-20 D6 t | qhull Qv Tv Qc" >>q_test.x
rbox 50 W1e-20 D6 t | qhull Qv Tv Qc >>q_test.x
echo "rbox 10000 D4 t | qhull QR0 Qc C-0.01 A0.3 Tv" >>q_test.x
rbox 10000 D4 t | qhull QR0 Qc C-0.01 A0.3 Tv >>q_test.x
echo "rbox 1000 D2 t | qhull d QR0 Qc C-1e-8 Qu Tv" >>q_test.x
rbox 1000 D2 t | qhull d QR0 Qc C-1e-8 Qu Tv >>q_test.x
echo "rbox 300 D5 t |qhull A-0.999 Qc Tcv" >>q_test.x
rbox 300 D5 t |qhull A-0.999 Qc Tcv >>q_test.x
echo "rbox 100 D6 t |qhull A-0.9999 Qc Tcv" >>q_test.x
rbox 100 D6 t |qhull A-0.9999 Qc Tcv >>q_test.x
echo "rbox 50 D7 t |qhull A-0.99999 Qc Tcv W0.1" >>q_test.x
rbox 50 D7 t |qhull A-0.99999 Qc Tcv W0.1 >>q_test.x

echo === check bad cases for Qhull.  May cause errors ${d:-`date`} >>q_test.x
echo "RBOX 1000 L100000 s G1e-6 t | QHULL Tv" >>q_test.x
RBOX 1000 L100000 s G1e-6 t | QHULL Tv >>q_test.x
echo "RBOX 1000 L100000 s G1e-6 t | QHULL Tv Q10" >>q_test.x
RBOX 1000 L100000 s G1e-6 t | QHULL Tv Q10 >>q_test.x
echo "rbox 1000 s Z1 G1e-13 t | qhull Tv" >>q_test.x
rbox 1000 s Z1 G1e-13 t | qhull Tv >>q_test.x
echo "RBOX 1000 s W1e-13 P0 t | QHULL d Qbb Qc Tv" >>q_test.x
RBOX 1000 s W1e-13 P0 t | QHULL d Qbb Qc Tv >>q_test.x
echo "RBOX 1000 s W1e-13 t | QHULL d Tv" >>q_test.x
RBOX 1000 s W1e-13 t | QHULL d Tv >>q_test.x
echo "RBOX 1000 s W1e-13 t D2 | QHULL d Tv" >>q_test.x
RBOX 1000 s W1e-13 t D2 | QHULL d Tv >>q_test.x

echo ======================================================= >>q_test.x
echo ======================================================= >>q_test.x
echo === The following commands may cause errors =========== >>q_test.x
echo ======================================================= >>q_test.x
echo ======================================================= >>q_test.x
echo "rbox c D7 | qhull Q0 Tcv" >>q_test.x
rbox c D7 | qhull Q0 Tcv >>q_test.x
echo "rbox 100 s D3 | qhull Q0 E1e-3 Tc Po" >>q_test.x
rbox 100 s D3 | qhull Q0 E1e-3 Tc Po >>q_test.x
echo "rbox 100 s D3 | qhull Q0 E1e-2 Tc Po" >>q_test.x
rbox 100 s D3 | qhull Q0 E1e-2 Tc Po >>q_test.x
echo "rbox 100 s D3 | qhull Q0 E1e-1 Tc Po" >>q_test.x
rbox 100 s D3 | qhull Q0 E1e-1 Tc Po >>q_test.x
echo "rbox 100 s D3 | qhull Q0 R1e-3 Tc Po" >>q_test.x
rbox 100 s D3 | qhull Q0 R1e-3 Tc Po >>q_test.x
echo "rbox 100 s D3 | qhull Q0 R1e-2 Tc Po" >>q_test.x
rbox 100 s D3 | qhull Q0 R1e-2 Tc Po >>q_test.x
echo "rbox 100 s D3 | qhull Q0 R0.05 Tc" >>q_test.x
rbox 100 s D3 | qhull Q0 R0.05 Tc >>q_test.x
echo "rbox 100 s D3 | qhull Q0 R0.05 Tc Po" >>q_test.x
rbox 100 s D3 | qhull Q0 R0.05 Tc Po >>q_test.x
echo "rbox 1000 W1e-7 | qhull Q0 Tc Po" >>q_test.x
rbox 1000 W1e-7 | qhull Q0 Tc Po >>q_test.x
echo "rbox 50 s | qhull Q0 V0.05 W0.01 Tc Po" >>q_test.x
rbox 50 s | qhull Q0 V0.05 W0.01 Tc Po >>q_test.x
echo "rbox 100 s D5 | qhull Q0 R1e-2 Tc Po" >>q_test.x
rbox 100 s D5 | qhull Q0 R1e-2 Tc Po >>q_test.x
echo "qhull" >>q_test.x
qhull >>q_test.x
echo "qhull ." >>q_test.x
qhull . >>q_test.x
echo "qhull -" >>q_test.x
qhull - >>q_test.x
echo "rbox" >>q_test.x
rbox >>q_test.x

echo "# end of q_test" >>q_test.x
