function x = bt3(p, adata)
  n=5;

  t1=p(1)-p(2);
  t2=p(2)+p(3)-2.0;
  t3=p(4)-1.0;
  t4=p(5)-1.0;

  for i=1:n
    x(i)=t1*t1 + t2*t2 + t3*t3 + t4*t4;
  end
