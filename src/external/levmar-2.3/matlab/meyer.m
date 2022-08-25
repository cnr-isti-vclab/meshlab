function x = meyer(p, data1, data2)
  n=16;

% data1, data2 are actually unused

  for i=1:n
    ui=0.45+0.05*i;
    x(i)=p(1)*exp(10.0*p(2)/(ui+p(3)) - 13.0);
  end
