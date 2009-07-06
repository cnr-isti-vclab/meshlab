% Unconstrained minimization

% fitting the exponential model x_i=p(1)*exp(-p(2)*i)+p(3) of expfit.c to noisy measurements obtained with (5.0 0.1 1.0)
p0=[1.0, 0.0, 0.0];
x=[5.8728, 5.4948, 5.0081, 4.5929, 4.3574, 4.1198, 3.6843, 3.3642, 2.9742, 3.0237, 2.7002, 2.8781,...
   2.5144, 2.4432, 2.2894, 2.0938, 1.9265, 2.1271, 1.8387, 1.7791, 1.6686, 1.6232, 1.571, 1.6057,...
   1.3825, 1.5087, 1.3624, 1.4206, 1.2097, 1.3129, 1.131, 1.306, 1.2008, 1.3469, 1.1837, 1.2102,...
   0.96518, 1.2129, 1.2003, 1.0743];

options=[1E-03, 1E-15, 1E-15, 1E-20, 1E-06];
% arg demonstrates additional data passing to expfit/jacexpfit
arg=[40];

[ret, popt, info]=levmar('expfit', 'jacexpfit', p0, x, 200, options, arg);
disp('Exponential model fitting (see also ../expfit.c)');
popt


% Meyer's (reformulated) problem
p0=[8.85, 4.0, 2.5];

x=[];
x(1:4)=[34.780, 28.610, 23.650, 19.630];
x(5:8)=[16.370, 13.720, 11.540, 9.744];
x(9:12)=[8.261, 7.030, 6.005, 5.147];
x(13:16)=[4.427, 3.820, 3.307, 2.872];

options=[1E-03, 1E-15, 1E-15, 1E-20, 1E-06];
% arg1, arg2 demonstrate additional dummy data passing to meyer/jacmeyer
arg1=[17];
arg2=[27];

%[ret, popt, info]=levmar('meyer', 'jacmeyer', p0, x, 200, options, arg1, arg2);

%[ret, popt, info, covar]=levmar('meyer', 'jacmeyer', p0, x, 200, options, arg1, arg2);
[ret, popt, info, covar]=levmar('meyer', p0, x, 200, options, 'unc', arg1, arg2);
disp('Meyer''s (reformulated) problem');
popt


% Linear constraints

% Boggs-Tolle problem 3
p0=[2.0, 2.0, 2.0, 2.0, 2.0];
x=[0.0, 0.0, 0.0, 0.0, 0.0];
options=[1E-03, 1E-15, 1E-15, 1E-20];
adata=[];

A=[1.0, 3.0, 0.0, 0.0, 0.0;
   0.0, 0.0, 1.0, 1.0, -2.0;
   0.0, 1.0, 0.0, 0.0, -1.0];
b=[0.0, 0.0, 0.0]';

[ret, popt, info, covar]=levmar('bt3', 'jacbt3', p0, x, 200, options, 'lec', A, b, adata);
disp('Boggs-Tolle problem 3');
popt


% Box constraints

% Hock-Schittkowski problem 01
p0=[-2.0, 1.0];
x=[0.0, 0.0];
lb=[-realmax, -1.5];
ub=[realmax, realmax];
options=[1E-03, 1E-15, 1E-15, 1E-20];

[ret, popt, info, covar]=levmar('hs01', 'jachs01', p0, x, 200, options, 'bc', lb, ub);
disp('Hock-Schittkowski problem 01');
popt


% Box and linear constraints

% Hock-Schittkowski modified problem 52
p0=[2.0, 2.0, 2.0, 2.0, 2.0];
x=[0.0, 0.0, 0.0, 0.0];
lb=[-0.09, 0.0, -realmax, -0.2, 0.0];
ub=[realmax, 0.3, 0.25, 0.3, 0.3];
A=[1.0, 3.0, 0.0, 0.0, 0.0;
   0.0, 0.0, 1.0, 1.0, -2.0;
   0.0, 1.0, 0.0, 0.0, -1.0];
b=[0.0, 0.0, 0.0]';
options=[1E-03, 1E-15, 1E-15, 1E-20];

[ret, popt, info, covar]=levmar('modhs52', 'jacmodhs52', p0, x, 200, options, 'blec', lb, ub, A, b);
disp('Hock-Schittkowski modified problem 52');
popt

% Hock-Schittkowski modified problem 235
p0=[-2.0, 3.0, 1.0];
x=[0.0, 0.0];
lb=[-realmax, 0.1, 0.7];
ub=[realmax, 2.9, realmax];
A=[1.0, 0.0, 1.0;
   0.0, 1.0, -4.0];
b=[-1.0, 0.0]';
options=[1E-03, 1E-15, 1E-15, 1E-20];

[ret, popt, info, covar]=levmar('mods235', p0, x, 200, options, 'blec', lb, ub, A, b);
disp('Hock-Schittkowski modified problem 235');
popt

