function jac = expfit_jac(p, data)
  n=data;
  m=max(size(p));

  for i=1:n
    jac(i, 1:m)=[exp(-p(2)*i), -p(1)*i*exp(-p(2)*i), 1.0];
  end
