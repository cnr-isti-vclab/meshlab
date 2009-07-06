function jac = hs01_jac(p)
  m=2;

  jac(1, 1:m)=[-20.0*p(1), 10.0];
  jac(2, 1:m)=[-1.0, 0.0];
