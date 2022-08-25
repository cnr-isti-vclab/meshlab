function jac = bt3_jac(p, adata)
  n=5;
  m=5;

  t1=p(1)-p(2);
  t2=p(2)+p(3)-2.0;
  t3=p(4)-1.0;
  t4=p(5)-1.0;

  for i=1:n
    jac(i, 1:m)=[2.0*t1, 2.0*(t2-t1), 2.0*t2, 2.0*t3, 2.0*t4];
  end

