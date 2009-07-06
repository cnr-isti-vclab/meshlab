function jac = meyer_jac(p, data1, data2)
  n=16;
  m=3;

  for i=1:n
    ui=0.45+0.05*i;
    tmp=exp(10.0*p(2)/(ui+p(3)) - 13.0);

    jac(i, 1:m)=[tmp, 10.0*p(1)*tmp/(ui+p(3)), -10.0*p(1)*p(2)*tmp/((ui+p(3))*(ui+p(3)))];
  end
