function x = expfit(p, data)
  n=data;

% data1, data2 are actually unused

  for i=1:n
    x(i)=p(1)*exp(-p(2)*i) + p(3);
  end
