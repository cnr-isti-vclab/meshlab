function x = hs01(p)
  n=2;

  t=p(1)*p(1);
  x(1)=10.0*(p(2)-t);
  x(2)=1.0-p(1);
