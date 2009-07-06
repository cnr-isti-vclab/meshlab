function [ret, popt, info, covar]=levmar(fname, jacname, p0, x, itmax, opts, type)
% LEVMAR  matlab MEX interface to the levmar non-linear least squares minimization
% library available from http://www.ics.forth.gr/~lourakis/levmar/
% 
% levmar can be used in any of the 4 following ways:
% [ret, popt, info, covar]=levmar(fname, jacname, p0, x, itmax, opts, 'unc', ...)
% [ret, popt, info, covar]=levmar(fname, jacname, p0, x, itmax, opts, 'bc', lb, ub, ...)
% [ret, popt, info, covar]=levmar(fname, jacname, p0, x, itmax, opts, 'lec', A, b, ...)
% [ret, popt, info, covar]=levmar(fname, jacname, p0, x, itmax, opts, 'blec', lb, ub, A, b, wghts, ...)
%  
% The dots at the end denote any additional, problem specific data that are passed uniterpreted to
% all invocations of fname and jacname, see below for details.
%
% In the following, the word "vector" is meant to imply either a row or a column vector.
%
% required input arguments:
% - fname: String defining the name of a matlab function implementing the function to be minimized.
%      fname will be called as fname(p, ...), where p denotes the parameter vector and the dots any
%      additional data passed as extra arguments during the invocation of levmar (refer to Meyer's
%      problem in lmdemo.m for an example).
%
% - p0: vector of doubles holding the initial parameters estimates.
%
% - x: vector of doubles holding the measurements vector.
%
% - itmax: maximum number of iterations.
%
% - opts: vector of doubles specifying the minimization parameters, as follows:
%      opts(1) scale factor for the initial damping factor
%      opts(2) stopping threshold for ||J^T e||_inf
%      opts(3) stopping threshold for ||Dp||_2
%      opts(4) stopping threshold for ||e||_2
%      opts(5) step used in finite difference approximation to the Jacobian.
%      If an empty vector (i.e. []) is specified, defaults are used.
%  
% optional input arguments:
% - jacname: String defining the name of matlab function implementing the Jacobian of function fname.
%      jacname will be called as jacname(p, ...) where p is again the parameter vector and the dots
%      denote any additional data passed as extra arguments to the invocation of levmar. If omitted,
%      the Jacobian is approximated with finite differences through repeated invocations of fname.
%
% - type: String defining the minimization type. It should be one of the following:
%      'unc' specifies unconstrained minimization.
%      'bc' specifies minimization subject to box constraints.
%      'lec' specifies minimization subject to linear equation constraints.
%      'blec' specifies minimization subject to box and linear equation constraints.
%      If omitted, a default of 'unc' is assumed. Depending on the minimization type, the MEX
%      interface will invoke one of dlevmar_XXX, dlevmar_bc_XXX, dlevmar_lec_XXX or dlevmar_blec_XXX
%
% - lb, ub: vectors of doubles specifying lower and upper bounds for p, respectively
%
% - A, b: k x m matrix and k vector specifying linear equation constraints for p, i.e. A*p=b
%      A should have full rank.
%
% - wghts: vector of doubles specifying the weights for the penalty terms corresponding to
%      the box constraints, see lmblec_core.c for more details. If omitted and a 'blec' type
%      minimization is to be carried out, default weights are used.
%  
%
% output arguments
% - ret: return value of levmar, corresponding to the number of iterations if successful, -1 otherwise.
%
% - popt: estimated minimizer, i.e. minimized parameters vector.
%
% - info: optional array of doubles, which upon return provides information regarding the minimization.
%      See lm_core.c for more details.
%
% - covar: optional covariance matrix corresponding to the estimated minimizer.
%
 
error('levmar.m is used only for providing documentation to levmar; make sure that levmar.c has been compiled using mex');
