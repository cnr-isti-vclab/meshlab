echo off
echo q_test.bat -- writes output to q_test.x for comparison with eg/q_test-ok.txt
echo
echo Instructions:
echo
echo Launch QHULL-GO.bat
echo "  ..\eg\q_test.bat >q_test.x 2>&1  "
echo
echo Compare bin\q_test.x with ..\eg\q_test-ok.txt

echo "========================" >q_test.x
echo "== Run q_test.bat for Windows from the bin/ directory."
echo "== Skips 'make test' and 'make qtest'"
echo "== Align the next block with 'eg/q_test'"
echo "========================"
echo
echo "=============================="
echo "========= eg/q_test =========="
echo "== Check qhull programs ======"
echo "=============================="
echo
echo "=============================="
echo "== check user_eg, user_eg2, and user_eg3"
echo "== errors if 'user_eg' and 'user_eg2' not found"
echo "=============================="
echo user_eg
user_eg
echo "user_eg 'QR1 p n Qt' 'v p' Fp"
user_eg "QR1 p n Qt" "v p" Fp
echo user_eg2
user_eg2
echo "user_eg2 'QR1 p' 'v p' Fp"
user_eg2 "QR1 p" "v p" Fp
echo user_eg3
user_eg3
echo user_eg3 eg-100
user_eg3 eg-100
echo user_eg3 eg-convex
user_eg3 eg-convex
echo user_eg3 eg-delaunay
user_eg3 eg-delaunay
echo user_eg3 eg-voronoi
user_eg3 eg-voronoi
echo user_eg3 eg-fifo
user_eg3 eg-fifo
echo user_eg3 rbox qhull
user_eg3 rbox qhull
echo user_eg3 rbox qhull T1
user_eg3 rbox qhull T1
echo user_eg3 rbox qhull d
user_eg3 rbox qhull d
echo user_eg3 rbox y c
user_eg3 rbox y c
echo 'user_eg3 rbox D2 10 2 "s r 5"  qhull 's p' facets'
user_eg3 rbox D2 10 2 "s r 5"  qhull 's p' facets
echo user_eg3 rbox "10 D2" eg-convex
user_eg3 rbox "10 D2" eg-convex
echo 'user_eg3 rbox "10 D2" qhull s eg-convex'
user_eg3 rbox "10 D2" qhull s eg-convex
echo 'user_eg3 rbox 10 eg-delaunay qhull "d o"'
user_eg3 rbox 10 eg-delaunay qhull "d o"
echo user_eg3 rbox D5 c P2 qhull d eg-delaunay
user_eg3 rbox D5 c P2 qhull d eg-delaunay
echo user_eg3 rbox "10 D2" eg-voronoi
user_eg3 rbox "10 D2" eg-voronoi
echo user_eg3 rbox "D5 c P2" qhull v eg-voronoi o
user_eg3 rbox "D5 c P2" qhull v eg-voronoi o
echo user_eg3 rbox "10 D2" eg-fifo
user_eg3 rbox "10 D2" eg-fifo
echo user_eg3 rbox "10 D2" qhull v eg-fifo p Fi Fo
user_eg3 rbox "10 D2" qhull v eg-fifo p Fi Fo

echo === check front ends ==========================================================
echo "qconvex -"
qconvex -
echo "qconvex ."
qconvex .
echo "qconvex -?"
qconvex -? # [mar'19] isatty does not work for Git for Windows
echo "rbox c D3 | qconvex s n Qt"
rbox c D3 | qconvex s n Qt
echo "rbox c D2 | qconvex s i"
rbox c D2 | qconvex s i 
echo "rbox c D2 | qconvex s n"
rbox c D2 | qconvex s n 
echo "rbox c D2 | qconvex o"
rbox c D2 | qconvex o
echo "rbox 1000 s | qconvex s Tv FA"
rbox 1000 s | qconvex s Tv FA
echo "rbox c d D2 | qconvex s Qc Fx"
rbox c d D2 | qconvex s Qc Fx
echo "rbox y 1000 W0 | qconvex Qc s n"
rbox y 1000 W0 | qconvex Qc s n 
echo "rbox y 1000 W0 | qconvex s QJ"
rbox y 1000 W0 | qconvex s QJ
echo "rbox d G1 D12 | qconvex QR0 FA"
rbox d G1 D12 | qconvex QR0 FA
echo "rbox c D6 | qconvex s FA TF500"
rbox c D6 | qconvex s FA TF500
echo "rbox c D7 | qconvex s FA TF1000"
rbox c D7 | qconvex s FA TF1000
echo "rbox d G1 D12 | qconvex QR0 FA Pp"
rbox d G1 D12 | qconvex QR0 FA Pp
echo "rbox c P0 d D2 | qconvex p Fa Fc FP FI Fn FN FS Fv Fx"
rbox c P0 d D2 | qconvex p Fa Fc FP FI Fn FN FS Fv Fx
echo "rbox c d D2 | qconvex s i QV0"
rbox c d D2 | qconvex s i QV0
echo "rbox c | qconvex Q0"
rbox c | qconvex Q0
echo "qvoronoi -"
qvoronoi -
echo "qvoronoi ."
qvoronoi .
echo "qvoronoi -?"
qvoronoi -? # [mar'19] isatty does not work for Git for Windows
echo "rbox c P0 D2 | qvoronoi s o"
rbox c P0 D2 | qvoronoi s o
echo "rbox c P0 D2 | qvoronoi Fi Tv"
rbox c P0 D2 | qvoronoi Fi Tv
echo "rbox c P0 D2 | qvoronoi Fo Tv"
rbox c P0 D2 | qvoronoi Fo Tv
echo "rbox c P0 D2 | qvoronoi Fv"
rbox c P0 D2 | qvoronoi Fv
echo "rbox c P0 D2 | qvoronoi s Qu Fv"
rbox c P0 D2 | qvoronoi s Qu Fv
echo "rbox c P0 D2 | qvoronoi s Qu Qt Fv"
rbox c P0 D2 | qvoronoi s Qu Qt Fv
echo "rbox c P0 D2 | qvoronoi Qu Fo"
rbox c P0 D2 | qvoronoi Qu Fo
echo "rbox c G1 d D2 | qvoronoi s p"
rbox c G1 d D2 | qvoronoi s p 
echo "rbox c G1 d D2 | qvoronoi QJ p"
rbox c G1 d D2 | qvoronoi QJ p 
echo "rbox c P-0.1 P+0.1 P+0.1 D2 | qvoronoi s Fc FP FQ Fn FN"
rbox c P-0.1 P+0.1 P+0.1 D2 | qvoronoi s Fc FP FQ Fn FN 
echo "rbox P0 c D2 | qvoronoi s Fv QV0"
rbox P0 c D2 | qvoronoi s Fv QV0
echo "qdelaunay -"
qdelaunay -
echo "qdelaunay ."
qdelaunay .
echo "qdelaunay -?"
qdelaunay -? # [mar'19] isatty does not work for Git for Windows
echo "rbox c P0 D2 | qdelaunay s o"
rbox c P0 D2 | qdelaunay s o
echo "rbox c P0 D2 | qdelaunay i"
rbox c P0 D2 | qdelaunay i
echo "rbox c P0 D2 | qdelaunay Fv"
rbox c P0 D2 | qdelaunay Fv
echo "rbox c P0 D2 | qdelaunay s Qu Qt Fv"
rbox c P0 D2 | qdelaunay s Qu Qt Fv
echo "rbox c G1 d D2 | qdelaunay s i"
rbox c G1 d D2 | qdelaunay s i
echo "rbox c G1 d D2 | qhull d Qbb Ft"
rbox c G1 d D2 | qhull d Qbb Ft
echo "rbox c G1 d D2 | qhull d Qbb QJ s Ft"
rbox c G1 d D2 | qhull d Qbb QJ s Ft
echo "rbox M3,4 z 100 D2 | qdelaunay s"
rbox M3,4 z 100 D2 | qdelaunay s
echo "rbox c P-0.1 P+0.1 P+0.1 D2 | qdelaunay s Fx Fa Fc FP FQ Fn FN"
rbox c P-0.1 P+0.1 P+0.1 D2 | qdelaunay s Fx Fa Fc FP FQ Fn FN
echo "rbox P0 P0 c D2 | qdelaunay s FP QV0"
rbox P0 P0 c D2 | qdelaunay s FP QV0
echo "qhalf -"
qhalf -
echo "qhalf ."
qhalf .
echo "qhalf -?"
qhalf -? # [mar'19] isatty does not work for Git for Windows
echo "rbox d | qhull FQ n | qhalf s Qt H0,0,0 Fp"
rbox d | qhull FQ n | qhalf s Qt H0,0,0 Fp
echo "rbox c | qhull FQ FV n | qhalf s i"
rbox c | qhull FQ FV n | qhalf s i
echo "rbox c | qhull FQ FV n | qhalf o"
rbox c | qhull FQ FV n | qhalf o
echo "rbox d D2 | qhull FQ n | qhalf s H0 Fc FP Fn FN FQ Fv Fx"
rbox d D2 | qhull FQ n | qhalf s H0 Fc FP Fn FN FQ Fv Fx
echo "qhull - printed at end"
echo "qhull ."
qhull .
echo "qhull -?"
qhull -? # [mar'19] isatty does not work for Git for Windows
echo "rbox 1000 s | qhull Tv s FA"
rbox 1000 s | qhull Tv s FA
echo "rbox 10 D2 | qhull d QJ s i TO q_test.log.1"
rbox 10 D2 | qhull d QJ s i TO q_test.log.1
echo "type q_test.log.1"
type q_test.log.1
echo "rbox 10 D2 | qhull v Qbb Qt p"
rbox 10 D2 | qhull v Qbb Qt p
echo "rbox 10 D2 | qhull d Qu QJ m" 
rbox 10 D2 | qhull d Qu QJ m  
echo "rbox 10 D2 | qhull v Qu QJ o"
rbox 10 D2 | qhull v Qu QJ o
echo "rbox c d D2 | qhull Qc s f Fx"
rbox c d D2 | qhull Qc s f Fx
echo "rbox c | qhull FV n | qhull H Fp"
rbox c | qhull FV n | qhull H Fp
echo "rbox d D12 | qhull QR0 FA"
rbox d D12 | qhull QR0 FA
echo "rbox c D7 | qhull FA TF1000"
rbox c D7 | qhull FA TF1000
echo "rbox y 1000 W0 | qhull Qc"   
rbox y 1000 W0 | qhull Qc    
echo "rbox c | qhull n"
rbox c | qhull n
echo "rbox c | qhull TA1"
rbox c | qhull TA1
echo "rbox 10 s | qhull C1e-5 T1P-1f"
rbox 10 s | qhull C1e-5 T1P-1f
echo === check quality of Qhull for ${d:-`hostname`} ${d:-`date`}
echo "rbox 1000 W0 | qhull QR2 QJ s Fs Tv"
rbox 1000 W0 | qhull QR2 QJ s Fs Tv
echo "rbox 1000 W0 | qhull QR2 s Fs Tv"
rbox 1000 W0 | qhull QR2 s Fs Tv
echo "rbox 1000 s | qhull C0.02 Qc Tv"
rbox 1000 s | qhull C0.02 Qc Tv
echo "rbox 500 s D4 | qhull C0.01 Qc Tv"
rbox 500 s D4 | qhull C0.01 Qc Tv
echo "rbox 1000 s | qhull C-0.02 Qc Tv"
rbox 1000 s | qhull C-0.02 Qc Tv
echo "rbox 1000 s D4 | qhull C-0.01 Qc Tv"
rbox 1000 s D4 | qhull C-0.01 Qc Tv
echo "rbox 200 s D5 | qhull C-0.01 Qx Qc Tv"
rbox 200 s D5 | qhull C-0.01 Qx Qc Tv
echo "rbox 100 s D6 | qhull C-0.001 Qx Qc Tv"
rbox 100 s D6 | qhull C-0.001 Qx Qc Tv
echo "rbox 1000 W1e-4 | qhull C-1e-6 Qc Tv"
rbox 1000 W1e-4 | qhull C-1e-6 Qc Tv
echo "rbox 1000 W5e-4 D4 | qhull C-1e-5 Qc Tv"
rbox 1000 W5e-4 D4 | qhull C-1e-5 Qc Tv
echo "rbox 400 W1e-3 D5 | qhull C-1e-5 Qx Qc Tv"
rbox 400 W1e-3 D5 | qhull C-1e-5 Qx Qc Tv

echo === check input format etc. ${d:-`date`}
echo "=== test long and short input to Qhull by hand, see eg/q_test ==="
echo "rbox d h | qhull Fd FV n FD  Tcv | qhull Fd H Fp Tcv"
rbox d h | qhull Fd FV n FD Tcv | qhull Fd H Fp Tcv
echo "rbox 10 h | qhull Fd FD p Tcv | qhull Fd d Tcv"
rbox 10 h | qhull Fd FD p Tcv | qhull Fd d Tcv 

echo === check rbox ${d:-`date`}
echo "rbox 3 n D2"
rbox 3 n D2
echo "rbox 3 D2"
rbox 3 D2
echo "rbox 3 h D2"
rbox 3 h D2
echo "rbox 3 z D2"
rbox 3 z D2
echo "rbox 3 z h D2"
rbox 3 z h D2
echo "rbox 3 B10 D2"
rbox 3 B10 D2
echo "rbox 3 B10 D2 C2,1E-13,1"
rbox 3 B10 D2 C2,1E-13,1
echo "rbox 3 z B10 D2"
rbox 3 z B10 D2
echo "rbox 4 L2 r D2"
rbox 4 L2 r D2
echo "rbox 8 L2 r D2"
rbox 8 L2 r D2
echo "rbox 8 L2 D2 C1,1E-13,2"
rbox 8 L2 D2 C1,1E-13,2
echo "rbox 4 L4 r D3"
rbox 4 L4 r D3
echo "rbox 4 L4 s D5 W1e-3" 
rbox 4 L4 s D5 W1e-3 
echo "rbox 4 L4 s D5 W1e-3 C1,1E-12"
rbox 4 L4 s D5 W1e-3 C1,1E-12
echo "rbox y"
rbox y
echo "rbox 10 M3,4"
rbox 10 M3,4
echo "rbox 10 M3,4 C1,1E-14"
rbox 10 M3,4 C1,1E-14
echo "rbox 27 M1,0,1"
rbox 27 M1,0,1
echo "rbox 10 L4 r D3 | qhull Tcv"
rbox 10 L4 r D3 | qhull Tcv
echo "rbox 10 L4 s D5 W1e-3 | qhull Tcv"
rbox 10 L4 s D5 W1e-3 | qhull Tcv
echo "rbox 10 L4 s D5 W1e-3 C1,1E-12 | qhull Tcv"
rbox 10 L4 s D5 W1e-3 C1,1E-12 | qhull Tcv
echo "rbox 10 L6 D3 | qhull Tcv"
rbox 10 L6 D3 | qhull Tcv
echo "rbox 10 L1.1 s D4 | qhull Tcv"
rbox 10 L1.1 s D4 | qhull Tcv
echo "rbox y r 100 W0 O0.5 | qhull s p Tcv"
rbox y r 100 W0 O0.5 | qhull s p Tcv
echo "rbox x r 100 W0 O0.5 | qhull s Tcv"
rbox x r 100 W0 O0.5 | qhull s Tcv
echo "rbox 12 D8 | qhull Tcv"
rbox 12 D8 | qhull Tcv
echo "rbox 12 D9 | qhull Tcv"
rbox 12 D9 | qhull Tcv
echo "rbox 1000 D4 | qhull s i A-0.97 C0.2 A0.7 Tcv"
rbox 1000 D4 | qhull s i A-0.97 C0.2 A0.7 Tcv
echo "rbox 3 D2 | qhull Qb0B1:-2 p"
rbox 3 D2 | qhull Qb0B1:-2 p 
echo "rbox 100 r D2 | qhull Pd0:0.7 PD0:0.8 Pg n Tcv"
rbox 100 r D2 | qhull Pd0:0.7 PD0:0.8 Pg n Tcv
echo "rbox 1000 s | qhull  C0.05 Tcv"
rbox 1000 s | qhull  C0.05 Tcv
echo "rbox 1000 s t | qhull Qm C0.05 Tcv"
rbox 1000 s t | qhull Qm C0.05 Tcv
echo "rbox 500 D2 | qhull n A-0.95 C0.1 Tcv"
rbox 500 D2 | qhull n A-0.95 C0.1 Tcv
echo "rbox 500 s P1,1,1 | qhull QgG0 Pp Tcv"
rbox 500 s P1,1,1 | qhull QgG0 Pp Tcv
echo "rbox d | qhull m"
rbox d | qhull m
echo "rbox d | qhull FM"
rbox d | qhull FM
echo "rbox c D2 | qhull Tcv Q0"
rbox c D2 | qhull Tcv Q0
echo "rbox d D2 | qhull Tcv"
rbox d D2 | qhull Tcv
echo "rbox c D3 | qhull Tcv Q0"
rbox c D3 | qhull Tcv Q0
echo "rbox d D3 | qhull Tcv"
rbox d D3 | qhull Tcv
echo "rbox c D4 | qhull Tcv Q0"
rbox c D4 | qhull Tcv Q0
echo "rbox d D4 | qhull Tcv"
rbox d D4 | qhull Tcv
echo "rbox c D5 | qhull Tcv Q0"
rbox c D5 | qhull Tcv Q0
echo "rbox d D5 | qhull Tcv"
rbox d D5 | qhull Tcv
echo "rbox c D6 | qhull Tcv Q0"
rbox c D6 | qhull Tcv Q0
echo "rbox d D6 | qhull Tcv"
rbox d D6 | qhull Tcv
echo "rbox d D7 | qhull Tcv"
rbox d D7 | qhull Tcv
echo "rbox c D2 | qhull Tcv C-0"
rbox c D2 | qhull Tcv C-0
echo "rbox c D3 | qhull Tcv C-0"
rbox c D3 | qhull Tcv C-0
echo "rbox c D4 | qhull Tcv C-0"
rbox c D4 | qhull Tcv C-0
echo "rbox c D5 | qhull Tcv C-0"
rbox c D5 | qhull Tcv C-0
echo "rbox c D6 | qhull Tcv C-0"
rbox c D6 | qhull Tcv C-0
echo "rbox c D7 | qhull Tv C-0"
rbox c D7 | qhull Tv C-0
echo "rbox 20 l D3 | qhull Tcv"
rbox 20 l D3 | qhull Tcv
echo "rbox 100 s D2 | qhull Tcv"
rbox 100 s D2 | qhull Tcv
echo "rbox 100 s D3 | qhull Tcv"
rbox 100 s D3 | qhull Tcv
echo "rbox 100 s D4 | qhull Tcv"
rbox 100 s D4 | qhull Tcv
echo "rbox 100 s c D4 | qhull Tcv"
rbox 100 s c D4 | qhull Tcv
echo "rbox 100 s d G1.5 D4 | qhull Tcv"
rbox 100 s d G1.5 D4 | qhull Tcv
echo "rbox 100 s W1e-2 | qhull Tcv"
rbox 100 s W1e-2 | qhull Tcv
echo "rbox 100 | qhull Tcv"
rbox 100 | qhull Tcv
echo "rbox 100 W1e-3 | qhull Tcv"
rbox 100 W1e-3 | qhull Tcv
echo "rbox 100 r D2 | qhull Tcv"
rbox 100 r D2 | qhull Tcv
echo "rbox 100 r s Z1 | qhull Tcv"
rbox 100 r s Z1 | qhull Tcv
echo "rbox 100 r s Z1 G0.1 | qhull Tcv C-0"
rbox 100 r s Z1 G0.1 | qhull Tcv C-0
echo "rbox 100 s Z1 G0.1 | qhull Tcv"
rbox 100 s Z1 G0.1 | qhull Tcv 
echo "rbox 100 s Z1e-5 G0.1 | qhull Tc Pp"
rbox 100 s Z1e-5 G0.1 | qhull Tc Pp

echo === check qhull option errors ${d:-`date`}


echo "rbox 10 | qhull d H0"
rbox 10 | qhull d H0
echo "rbox 10 | qhull Zza"
rbox 10 | qhull Zza
echo "rbox 10 | qhull Zza Qw"
rbox 10 | qhull Zza Qw
echo "rbox 10 | qhull Q999"
rbox 10 | qhull Q999
echo "rbox 10 | qhull Q999 Qw"
rbox 10 | qhull Q999 Qw
echo "qhull TIxyzlkajdfhwh"
qhull TIxyzlkajdfhwh
echo "rbox 10 >r.x"
rbox 10 >r.x
echo "qhull TIr.x"
qhull TIr.x
echo "qhull p TI    r.x TO x.x"
qhull p TI    r.x TO x.x
echo "type x.x"
type x.x

echo === check qhull output formats ${d:-`date`}
echo "rbox 5 r s D2 | qhull Tcv"
rbox 5 r s D2 | qhull Tcv
echo "rbox 5 r s D2 | qhull s "
rbox 5 r s D2 | qhull s 
echo "rbox 5 r s D2 | qhull s o "
rbox 5 r s D2 | qhull s o 
echo "rbox 5 r s D2 | qhull f"
rbox 5 r s D2 | qhull f
echo "rbox 5 r s D2 | qhull i "
rbox 5 r s D2 | qhull i 
echo "rbox 5 r s D2 | qhull m "
rbox 5 r s D2 | qhull m 
echo "rbox 5 r s D2 | qhull FM "
rbox 5 r s D2 | qhull FM 
echo "rbox 5 r s D2 | qhull n "
rbox 5 r s D2 | qhull n 
echo "rbox 5 r s D2 | qhull p "
rbox 5 r s D2 | qhull p 
echo "rbox 5 r s D2 | qhull o "
rbox 5 r s D2 | qhull o 
echo "rbox 5 r s D2 | qhull Ft"
rbox 5 r s D2 | qhull Ft
echo "rbox 5 r s D2 | qhull Fx"
rbox 5 r s D2 | qhull Fx
echo "rbox 5 r s D2 | qhull p n i p p"
rbox 5 r s D2 | qhull p n i p p
echo "rbox 10 D3 | qhull f Tcv"
rbox 10 D3 | qhull f Tcv
echo "rbox 10 D3 | qhull i"
rbox 10 D3 | qhull i
echo "rbox 10 D3 | qhull p "
rbox 10 D3 | qhull p 
echo "rbox 10 D3 | qhull o "
rbox 10 D3 | qhull o 
echo "rbox 10 D3 | qhull Fx"
rbox 10 D3 | qhull Fx
echo "rbox 27 M1,0,1 | qhull Qc"
rbox 27 M1,0,1 | qhull Qc
echo "rbox 50 D3 s | qhull C0.1 Qc Pd0d1d2 Pg s p Tcv"
rbox 50 D3 s | qhull C0.1 Qc Pd0d1d2 Pg s p Tcv
echo "rbox 10 D2 P0 P1e-15 | qhull d Qc FP s Tcv"
rbox 10 D2 P0 P1e-15 | qhull d Qc FP s Tcv
echo "rbox 100 s | qhull C-0.003 Qc FP s"
rbox 100 s | qhull C-0.003 Qc FP s
echo "rbox 100 s D2 | qhull C0.1 i Fx Tcv"
rbox 100 s D2 | qhull C0.1 i Fx Tcv
echo "rbox 4 s D3 | qhull Qc Ghipv Tcv "
rbox 4 s D3 | qhull Qc Ghipv Tcv 
echo "rbox 6 D4 | qhull f Tcv"
rbox 6 D4 | qhull f Tcv
echo "rbox 6 D4 | qhull i"
rbox 6 D4 | qhull i
echo "rbox 6 D4 | qhull p "
rbox 6 D4 | qhull p 
echo "rbox 6 D4 | qhull o"
rbox 6 D4 | qhull o
echo "rbox 1000 s D2 | qhull FA Tcv"
rbox 1000 s D2 | qhull FA Tcv
echo "rbox 1000 s | qhull FA Tcv"
rbox 1000 s | qhull FA Tcv
echo "rbox c D4 | qhull FA Tcv"
rbox c D4 | qhull FA Tcv
echo "rbox c D5 | qhull FA Tcv"
rbox c D5 | qhull FA Tcv
echo "rbox c D5 | qhull FA Qt Tcv"
rbox c D5 | qhull FA Qt Tcv
echo "rbox 10 D2 | qhull d FA Tcv"
rbox 10 D2 | qhull d FA Tcv
echo "rbox 10 D2 | qhull d Qu FA Tcv"
rbox 10 D2 | qhull d Qu FA Tcv
echo "rbox 10 D2 | qhull FA Tcv"
rbox 10 D2 | qhull FA Tcv
echo "rbox 10 c D2 | qhull Fx Tcv"
rbox 10 c D2 | qhull Fx Tcv
echo "rbox 1000 s | qhull FS Tcv"
rbox 1000 s | qhull FS Tcv
echo "rbox 10 W0 D2 | qhull p Qc FcC Tcv"
rbox 10 W0 D2 | qhull p Qc FcC Tcv
echo "rbox 4 z h s D2 | qhull Fd s n FD Tcv"
rbox 4 z h s D2 | qhull Fd s n FD Tcv
echo "rbox 6 s D3 | qhull C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv"
rbox 6 s D3 | qhull C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv
echo "rbox P0.5,0.5 P0.5,0.5 W0 5 D2 | qhull d FN Qc"
rbox P0.5,0.5 P0.5,0.5 W0 5 D2 | qhull d FN Qc
echo "rbox 10 D3 | qhull Fa PA5"
rbox 10 D3 | qhull Fa PA5
echo "rbox 10 D3 | qhull Fa PF0.4"
rbox 10 D3 | qhull Fa PF0.4

echo === test Qt ${d:-`date`}
echo "rbox c | qhull Qt s o Tcv"
rbox c | qhull Qt s o Tcv
echo "rbox c | qhull Qt f i"
rbox c | qhull Qt f i
echo "rbox c | qhull Qt m FM n"
rbox c | qhull Qt m FM n
echo "rbox c | qhull Qt p o"
rbox c | qhull Qt p o
echo "rbox c | qhull Qt Fx"
rbox c | qhull Qt Fx
echo "rbox c | qhull Qt FA s Fa"
rbox c | qhull Qt FA s Fa
echo "rbox 6 r s c G0.1 D2 | qhull Qt d FA Tcv"
rbox 6 r s c G0.1 D2 | qhull Qt d FA Tcv
echo "rbox 6 r s c G0.1 D2 | qhull d FA Tcv"
rbox 6 r s c G0.1 D2 | qhull d FA Tcv
echo "rbox 6 r s c G0.1 D2 | qhull Qt v p Tcv"
rbox 6 r s c G0.1 D2 | qhull Qt v p Tcv
echo "rbox c | qhull Qt C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv"
rbox c | qhull Qt C-0.1 Qc FF s FQ Fi n Fo FQ FI Fm Fn FN FO FO FQ Fs FS FV Fv Tcv
echo "rbox 6 r s c G0.1 D2 P0.1,0.1 | qhull s FP d FO Qt"
rbox 6 r s c G0.1 D2 P0.1,0.1 | qhull s FP d FO Qt
echo "rbox 100 W0 | qhull Tv Q11 FO"
rbox 100 W0 | qhull Tv Q11 FO

echo === test unbounded intersection
echo "rbox c | qhull PD0:0.5 n | qhull H0 Fp Tcv"
rbox c | qhull PD0:0.5 n | qhull H0 Fp Tcv
echo "rbox 1000 W1e-3 D3 | qhull PA8 Fa FS s n Tcv"
rbox 1000 W1e-3 D3 | qhull PA8 Fa FS s n Tcv 
echo "rbox 1000 W1e-3 D3 | qhull C-0.01 PM10 Fm n Tcv Qc"
rbox 1000 W1e-3 D3 | qhull C-0.01 PM10 Fm n Tcv Qc
echo "rbox 1000 W1e-3 D3 | qhull C-0.01 PA8 PG n Tcv Qc"
rbox 1000 W1e-3 D3 | qhull C-0.01 PA8 PG n Tcv Qc
echo "rbox 10 | qhull FO Tz TO q_test.log.1"
rbox 10 | qhull FO Tz TO q_test.log.1
echo "type q_test.log.1"
type q_test.log.1

echo === check Delaunay/Voronoi ${d:-`date`}
echo "rbox 10 D2 | qhull d Tcv"
rbox 10 D2 | qhull d Tcv
echo "rbox 10 D2 | qhull d Qz Tcv"
rbox 10 D2 | qhull d Qz Tcv
echo "rbox 10 D3 | qhull d Tcv"
rbox 10 D3 | qhull d Tcv
echo "rbox c | qhull d Qz Ft Tcv"
rbox c | qhull d Qz Ft Tcv
echo "rbox 10 s D2 c | qhull d Tcv"
rbox 10 s D2 c | qhull d Tcv
echo "rbox 10 s D2 | qhull d Tcv Qz Q8"
rbox 10 s D2 | qhull d Tcv Qz Q8
echo "rbox 10 D2 | qhull d Tcv p"
rbox 10 D2 | qhull d Tcv p
echo "rbox 10 D2 | qhull d Tcv i"
rbox 10 D2 | qhull d Tcv i
echo "rbox 10 D2 | qhull d Tcv o"
rbox 10 D2 | qhull d Tcv o
echo "rbox 10 D2 | qhull v Tcv o"
rbox 10 D2 | qhull v Tcv o
echo "rbox 10 D2 | qhull v Tcv p"
rbox 10 D2 | qhull v Tcv p
echo "rbox 10 D2 | qhull v Tcv G"
rbox 10 D2 | qhull v Tcv G
echo "rbox 10 D2 | qhull v Tcv Fv"
rbox 10 D2 | qhull v Tcv Fv
echo "rbox 10 D2 | qhull v Tcv Fi"
rbox 10 D2 | qhull v Tcv Fi
echo "rbox 10 D2 | qhull v Tcv Fo"
rbox 10 D2 | qhull v Tcv Fo
echo "rbox 10 D2 | qhull v Qu o Fv Fi Fo Tcv"
rbox 10 D2 | qhull v Qu o Fv Fi Fo Tcv
echo "rbox 10 D3 | qhull v Fv Tcv"
rbox 10 D3 | qhull v Fv Tcv
echo "rbox 10 D3 | qhull v fi Tcv"
rbox 10 D3 | qhull v Fi Tcv
echo "rbox 10 D3 | qhull v Fo Tcv"
rbox 10 D3 | qhull v Fo Tcv
echo "rbox 10 D3 | qhull v Qu o Fv Fi Fo Tcv"
rbox 10 D3 | qhull v Qu o Fv Fi Fo Tcv
echo "rbox 5 D2 | qhull v f FnN o"
rbox 5 D2 | qhull v f FnN o

echo === check Halfspace, showing the input ${d:-`date`}
echo === the Qhull pipeline recreates 100 4-D cospherical points with the same area and volume
echo "rbox 100 s D4 | qhull FA FV s"
rbox 100 s D4 | qhull FA FV s
echo "rbox 100 s D4 | qhull FQ FA FV n | qhull s H"
rbox 100 s D4 | qhull FQ FA FV n s | qhull s H
echo "rbox 100 s D4 | qhull FQ FA FV n s Tcv | qhull FQ s H Fp Tcv | qhull FA Tcv"
rbox 100 s D4 | qhull FQ FA FV n s Tcv | qhull FQ s H Fp Tcv | qhull FA Tcv
echo === the Qhull pipeline recreates a 3-D tetrahedron
echo "rbox d D3 | qhull n FD"
rbox d D3 | qhull n FD
echo "rbox d D3 | qhull s n FD Tcv | qhull s Fd H0.1,0.1 Fp Tcv"
rbox d D3 | qhull s n FD Tcv | qhull s Fd H0.1,0.1 Fp Tcv
echo "rbox 5 r D2 | qhull s n Tcv"
echo === the Qhull pipeline recreates a regular 2-D pentagon
rbox 5 r D2 | qhull FQ n
echo "rbox 5 r D2 | qhull s FQ n Tcv | qhull s H0 Fp Tcv"
rbox 5 r D2 | qhull s FQ n Tcv | qhull s H0 Fp Tcv

echo === check qhull ${d:-`date`}
echo "rbox 10 s D3 | qhull Tcv"
rbox 10 s D3 | qhull Tcv
echo "rbox 10 s D3 | qhull f Pd0:0.5 Pd2 Pg Tcv"
rbox 10 s D3 | qhull f Pd0:0.5 Pd2 Pg Tcv
echo "rbox 10 s D3 | qhull f Tcv PD2:-0.5 Pg"
rbox 10 s D3 | qhull f Tcv PD2:-0.5 Pg
echo "rbox 10 s D3 | qhull QR-1"
rbox 10 s D3 | qhull QR-1
echo "rbox 10 s D3 | qhull QR-40"
rbox 10 s D3 | qhull QR-40
echo "rbox 1000 D3 | qhull Tcvs"
rbox 1000 D3 | qhull Tcvs
echo "Test tracing 'Tn', combine stderr/stdout 'Tz', flush fprintf 'Tf'"
echo "rbox 100 D3 | qhull T1 Tz Tf TA1 TO q_test.log.1"
rbox 100 D3 | qhull T1 Tz Tf TA1 TO q_test.log.1
echo "type q_test.log.1"
type q_test.log.1
echo "del q_test.log.1"
del q_test.log.1
echo "rbox 100 s D3 | qhull TcvA10"
rbox 100 s D3 | qhull TcvA10
echo "rbox 100 s D3 | qhull TcvV-2"
rbox 100 s D3 | qhull TcvV-2
echo "rbox 100 s D3 | qhull TcvC2"
rbox 100 s D3 | qhull TcvC2
echo "rbox 100 s D3 | qhull TcvV2"
rbox 100 s D3 | qhull TcvV2
echo "rbox 100 s D3 | qhull T1cvV2P2"
rbox 100 s D3 | qhull T1cvV2P2
echo "rbox 100 s D3 | qhull TcvF100"
rbox 100 s D3 | qhull TcvF100
echo "rbox 100 s D3 | qhull Qf Tcv"
rbox 100 s D3 | qhull Qf Tcv
echo "rbox 100 D3 | qhull Tcv"
rbox 100 D3 | qhull Tcv
echo "rbox 100 D3 | qhull Qs Tcv"
rbox 100 D3 | qhull Qs Tcv
echo "rbox 100 D5 | qhull Qs Tcv"
rbox 100 D5 | qhull Qs Tcv
echo "rbox 100 D3 | qhull Qr Tcv"
rbox 100 D3 | qhull Qr Tcv
echo "rbox 100 D3 | qhull Qxv Tcv"
rbox 100 D3 | qhull Qxv Tcv
echo "rbox 100 D3 | qhull Qi f Pd0 Pd1 Pd2 Pg Tcv"
rbox 100 D3 | qhull Qi f Pd0 Pd1 Pd2 Pg Tcv
echo "rbox c d | qhull Qc f Tcv"
rbox c d | qhull Qc f Tcv
echo "rbox c d | qhull Qc p Tcv"
rbox c d | qhull Qc p Tcv
echo "rbox 100 D3 | qhull QbB FO Tcv"
rbox 100 D3 | qhull QbB FO Tcv
echo "rbox 1000 D2 B1e6 | qhull d Qbb FO Tcv"
rbox 1000 D2 B1e6 | qhull d Qbb FO Tcv
echo "rbox 10 D3 | qhull QbB p Tcv"
rbox 10 D3 | qhull QbB p Tcv
echo "rbox 10 D3 | qhull Qbb p Tcv"
rbox 10 D3 | qhull Qbb p Tcv
echo "rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv"
rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv
echo "rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv | qhull QbB p Tcv"
rbox 10 D3 | qhull Qb0:-10B2:20 p Tcv | qhull QbB p Tcv
echo "rbox 10 D3 | qhull Qb1:0B1:0 d Tcv Q8"
rbox 10 D3 | qhull Qb1:0B1:0 d Tcv Q8
echo "rbox 10 D3 | qhull Qb1:0B1:0B2:0 d Tcv Q8"
rbox 10 D3 | qhull Qb1:0B1:0B2:0 d Tcv Q8
echo "rbox 10 D3 | qhull Qb1:0 d Tcv"
rbox 10 D3 | qhull Qb1:0 d Tcv 
echo "rbox 10 D3 | qhull Qb1:0B1:0  Tcv"
rbox 10 D3 | qhull Qb1:0B1:0  Tcv
echo "== next command will error ${d:-`date`} =="
echo "rbox 10 D2 | qhull Qb1:1B1:1 Tcv"
rbox 10 D2 | qhull Qb1:1B1:1 Tcv
echo "rbox 200 L20 D2 t | qhull FO Tcv C-0"
rbox 200 L20 D2 t | qhull FO Tcv C-0
echo "rbox 1000 L20 t | qhull FO Tcv C-0"
rbox 1000 L20 t | qhull FO Tcv C-0
echo "rbox 200 L20 D4 t | qhull FO Tcv C-0"
rbox 200 L20 D4 t | qhull FO Tcv C-0
echo "rbox 200 L20 D5 t | qhull FO Tcv Qx"
rbox 200 L20 D5 t | qhull FO Tcv Qx
echo "rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu Q0"
rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu Q0
echo "rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu C-0"
rbox 1000 W1e-3 s D2 t | qhull d FO Tcv Qu C-0

echo === check joggle and TRn ${d:-`date`}
echo "rbox 100 W0 | qhull QJ1e-14 Qc TR100 Tv"
rbox 100 W0 | qhull QJ1e-14 Qc TR100 Tv
echo "rbox 100 W0 | qhull QJ1e-13 Qc TR100 Tv"
rbox 100 W0 | qhull QJ1e-13 Qc TR100 Tv
echo "rbox 100 W0 | qhull QJ1e-12 Qc TR100 Tv"
rbox 100 W0 | qhull QJ1e-12 Qc TR100 Tv
echo "rbox 100 W0 | qhull QJ1e-11 Qc TR100 Tv"
rbox 100 W0 | qhull QJ1e-11 Qc TR100 Tv
echo "rbox 100 W0 | qhull QJ1e-10 Qc TR100 Tv"
rbox 100 W0 | qhull QJ1e-10 Qc TR100 Tv
echo "rbox 100 | qhull d QJ Qb0:1e4 QB0:1e5 Qb1:1e4 QB1:1e6 Qb2:1e5 QB2:1e7 FO Tv"
rbox 100 | qhull d QJ Qb0:1e4 QB0:1e5 Qb1:1e4 QB1:1e6 Qb2:1e5 QB2:1e7 FO Tv

echo === check precision options ${d:-`date`}
echo "rbox 100 D3 s | qhull E0.01 Qx Tcv FO"
rbox 100 D3 s | qhull E0.01 Qx Tcv FO
echo "rbox 100 D3 W1e-1 | qhull W1e-3 Tcv"
rbox 100 D3 W1e-1 | qhull W1e-3 Tcv
echo "rbox 100 D3 W1e-1 | qhull W1e-2 Tcv Q0"
rbox 100 D3 W1e-1 | qhull W1e-2 Tcv Q0
echo "rbox 100 D3 W1e-1 | qhull W1e-2 Tcv"
rbox 100 D3 W1e-1 | qhull W1e-2 Tcv
echo "rbox 100 D3 W1e-1 | qhull W1e-1 Tcv"
rbox 100 D3 W1e-1 | qhull W1e-1 Tcv
echo "rbox 15 D2 P0 P1e-14,1e-14 | qhull d Quc Tcv"
rbox 15 D2 P0 P1e-14,1e-14 | qhull d Quc Tcv
echo "rbox 15 D3 P0 P1e-12,1e-14,1e-14 | qhull d Qcu Tcv"
rbox 15 D3 P0 P1e-12,1e-14,1e-14 | qhull d Qcu Tcv
echo "rbox 1000 s D3 | qhull C-0.01 Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Tcv Qc
echo "rbox 1000 s D3 | qhull C-0.01 V0 Qc Tcv"
rbox 1000 s D3 | qhull C-0.01 V0 Qc Tcv
echo "rbox 1000 s D3 | qhull C-0.01 U0 Qc Tcv"
rbox 1000 s D3 | qhull C-0.01 U0 Qc Tcv
echo "rbox 1000 s D3 | qhull C-0.01 V0 Qcm Tcv"
rbox 1000 s D3 | qhull C-0.01 V0 Qcm Tcv
echo "rbox 1000 s D3 | qhull C-0.01 Qcm Tcv"
rbox 1000 s D3 | qhull C-0.01 Qcm Tcv
echo "rbox 1000 s D3 | qhull C-0.01 Q1 FO Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Q1 FO Tcv Qc
echo "rbox 1000 s D3 | qhull C-0.01 Q2 FO Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Q2 FO Tcv Qc
echo "rbox 1000 s D3 | qhull C-0.01 Q3 FO Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Q3 FO Tcv Qc
echo "rbox 1000 s D3 | qhull C-0.01 Q4 FO Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Q4 FO Tcv Qc
echo === this may generate an error ${d:-`date`}
echo "rbox 1000 s D3 | qhull C-0.01 Q5 FO Tcv"
rbox 1000 s D3 | qhull C-0.01 Q5 FO Tcv 
echo === this should generate an error ${d:-`date`}
echo "rbox 1000 s D3 | qhull C-0.01 Q6 FO Po Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Q6 FO Po Tcv Qc
echo "rbox 1000 s D3 | qhull C-0.01 Q7 FO Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Q7 FO Tcv Qc
echo "rbox 1000 s D3 | qhull C-0.01 Qx Tcv Qc"
rbox 1000 s D3 | qhull C-0.01 Qx Tcv Qc
echo "=== this may generate an error e.g., t1263080158 ${d:-`date`}"
echo "rbox 100 s D3 t | qhull R1e-3 Tcv Qc"
rbox 100 s D3 t | qhull R1e-3 Tcv Qc
echo "rbox 100 s D3 t | qhull R1e-2 Tcv Qc"
rbox 100 s D3 t | qhull R1e-2 Tcv Qc
echo "rbox 500 s D3 t | qhull R0.05 A-1 Tcv Qc"
rbox 500 s D3 t | qhull R0.05 A-1 Tcv Qc
echo "rbox 100 W0 D3 t | qhull R1e-3 Tcv Qc"
rbox 100 W0 D3 t | qhull R1e-3 Tcv Qc
echo "rbox 100 W0 D3 t | qhull R1e-3 Qx Tcv Qc"
rbox 100 W0 D3 t | qhull R1e-3 Qx Tcv Qc
echo "rbox 100 W0 D3 t | qhull R1e-2 Tcv Qc"
rbox 100 W0 D3 t | qhull R1e-2 Tcv Qc
echo "rbox 100 W0 D3 t | qhull R1e-2 Qx Tcv Qc"
rbox 100 W0 D3 t | qhull R1e-2 Qx Tcv Qc
echo "rbox 500 W0 D3 t | qhull R0.05 A-1 Tcv Qc"
rbox 500 W0 D3 t | qhull R0.05 A-1 Tcv Qc
echo "rbox 500 W0 D3 t | qhull R0.05 Qx Tcv Qc"
rbox 500 W0 D3 t | qhull R0.05 Qx Tcv Qc
echo "rbox 1000 W1e-20 t | qhull Tcv Qc"
rbox 1000 W1e-20 t | qhull Tcv Qc
echo "rbox 1000 W1e-20 D4 t | qhull Tcv Qc"
rbox 1000 W1e-20 D4 t | qhull Tcv Qc
echo "rbox 500 W1e-20 D5 t | qhull Tv Qc"
rbox 500 W1e-20 D5 t | qhull Tv Qc
echo "rbox 100 W1e-20 D6 t | qhull Tv Qc"
rbox 100 W1e-20 D6 t | qhull Tv Qc
echo "rbox 50 W1e-20 D6 t | qhull Qv Tv Qc"
rbox 50 W1e-20 D6 t | qhull Qv Tv Qc
echo "rbox 10000 D4 t | qhull QR0 Qc C-0.01 A0.3 Tv"
rbox 10000 D4 t | qhull QR0 Qc C-0.01 A0.3 Tv
echo "rbox 1000 D2 t | qhull d QR0 Qc C-1e-8 Qu Tv"
rbox 1000 D2 t | qhull d QR0 Qc C-1e-8 Qu Tv
echo "rbox 300 D5 t |qhull A-0.999 Qx Qc Tcv"
rbox 300 D5 t |qhull A-0.999 Qx Qc Tcv
echo "rbox 100 D6 t |qhull A-0.9999 Qx Qc Tcv"
rbox 100 D6 t |qhull A-0.9999 Qx Qc Tcv
echo "rbox 50 D7 t |qhull A-0.99999 Qx Qc Tcv W0.1"
rbox 50 D7 t |qhull A-0.99999 Qx Qc Tcv W0.1

echo === check bad cases for Qhull.  May cause errors ${d:-`date`}
echo "rbox 1000 L100000 s G1e-6 t | qhull Tv"
rbox 1000 L100000 s G1e-6 t | qhull Tv
echo "rbox 1000 L100000 s G1e-6 t | qhull Tv Q10"
rbox 1000 L100000 s G1e-6 t | qhull Tv Q10
echo "rbox 1000 s Z1 G1e-13 t | qhull Tv"
rbox 1000 s Z1 G1e-13 t | qhull Tv
echo "rbox 1000 s W1e-13 P0 t | qhull d Qbb Qc Q12 Tv"
rbox 1000 s W1e-13 P0 t | qhull d Qbb Qc Q12 Tv
echo "rbox 1000 s W1e-13 t | qhull d Q12 Tv"
rbox 1000 s W1e-13 t | qhull d Q12 Tv
echo "rbox 1000 s W1e-13 t D2 | qhull d Tv"
rbox 1000 s W1e-13 t D2 | qhull d Tv

echo =======================================================
echo =======================================================
echo === The following commands may cause errors ===========
echo =======================================================
echo =======================================================
echo "rbox c D7 | qhull Q0 Tcv"
rbox c D7 | qhull Q0 Tcv
echo "rbox 100 s D3 | qhull Q0 E1e-3 Tc Po"
rbox 100 s D3 | qhull Q0 E1e-3 Tc Po
echo "rbox 100 s D3 | qhull Q0 E1e-2 Tc Po"
rbox 100 s D3 | qhull Q0 E1e-2 Tc Po
echo "rbox 100 s D3 | qhull Q0 E1e-1 Tc Po"
rbox 100 s D3 | qhull Q0 E1e-1 Tc Po
echo "rbox 100 s D3 | qhull Q0 R1e-3 Tc Po"
rbox 100 s D3 | qhull Q0 R1e-3 Tc Po
echo "rbox 100 s D3 | qhull Q0 R1e-2 Tc Po"
rbox 100 s D3 | qhull Q0 R1e-2 Tc Po
echo "rbox 100 s D3 | qhull Q0 R0.05 Tc"
rbox 100 s D3 | qhull Q0 R0.05 Tc
echo "rbox 100 s D3 | qhull Q0 R0.05 Tc Po"
rbox 100 s D3 | qhull Q0 R0.05 Tc Po
echo "rbox 1000 W1e-7 | qhull Q0 Tc Po"
rbox 1000 W1e-7 | qhull Q0 Tc Po
echo "rbox 50 s | qhull Q0 V0.05 W0.01 Tc Po"
rbox 50 s | qhull Q0 V0.05 W0.01 Tc Po
echo "rbox 100 s D5 | qhull Q0 R1e-2 Tc Po"
rbox 100 s D5 | qhull Q0 R1e-2 Tc Po
echo "rbox L100 10000 D4 s C1,1e-13 t2 | qhull"
rbox L100 10000 D4 s C1,1e-13 t2 | qhull
echo "rbox L100 10000 D4 s C1,1e-13 t2 | qhull Q12"
rbox L100 10000 D4 s C1,1e-13 t2 | qhull Q12
echo "rbox 50 C1,1E-13 t1447644703 | qhull d"
rbox 50 C1,1E-13 t1447644703 | qhull d
echo "rbox 50 C1,1E-13 t1447644703 | qhull d Q12"
rbox 50 C1,1E-13 t1447644703 | qhull d Q12
echo "rbox 50 C1,1E-13 t1447644703 | qhull d Q14"
rbox 50 C1,1E-13 t1447644703 | qhull d Q14

echo "======================================================="
echo "=== Testing done.  Print documentation"
echo "======================================================="
echo "qhull -"
qhull -
echo "rbox"
rbox

echo "# end of q_test"
