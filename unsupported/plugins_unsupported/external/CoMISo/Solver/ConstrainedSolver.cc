/*===========================================================================*\
 *                                                                           *
 *                               CoMISo                                      *
 *      Copyright (C) 2008-2009 by Computer Graphics Group, RWTH Aachen      *
 *                           www.rwth-graphics.de                            *
 *                                                                           *
 *---------------------------------------------------------------------------* 
 *  This file is part of CoMISo.                                             *
 *                                                                           *
 *  CoMISo is free software: you can redistribute it and/or modify           *
 *  it under the terms of the GNU General Public License as published by     *
 *  the Free Software Foundation, either version 3 of the License, or        *
 *  (at your option) any later version.                                      *
 *                                                                           *
 *  CoMISo is distributed in the hope that it will be useful,                *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of           *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            *
 *  GNU General Public License for more details.                             *
 *                                                                           *
 *  You should have received a copy of the GNU General Public License        *
 *  along with CoMISo.  If not, see <http://www.gnu.org/licenses/>.          *
 *                                                                           *
\*===========================================================================*/ 


#include "ConstrainedSolver.hh"

#include <gmm/gmm.h>

namespace COMISO {

//-----------------------------------------------------------------------------

int 
ConstrainedSolver::
find_gcd(std::vector<int>& _v_gcd, int& _n_ints)
{
  bool found_gcd = false;
  bool done      = false;
  bool all_same  = true;
  int i_gcd = -1;
  int prev_val   = -1;
  // check integer coefficient pairwise
  while( !done)
  {
    // assume gcd of all pairs is the same
    all_same = true;
    for( int k=0; k<_n_ints-1 && !done; ++k)
    {
      // use abs(.) to get same sign needed for all_same
      _v_gcd[k] = abs(gcd(_v_gcd[k],_v_gcd[k+1]));

      if( k>0 && prev_val != _v_gcd[k])
        all_same = false;

      prev_val = _v_gcd[k];

      // if a 2 was found, all other entries have to be divisible by 2
      if(_v_gcd[k] == 2)
      {
        bool all_ok=true;
        for( int l=0; l<_n_ints; ++l)
          if( abs(_v_gcd[l]%2) != 0)
          {
            all_ok = false;
            break;
          }
        done      = true;
        if( all_ok )
        {
          found_gcd = true;
          i_gcd     = 2;
        } 
        else
        {
          found_gcd = false;
        }
      }
    }
    // already done (by successfull "2"-test)?
    if(!done)
    {
      // all gcds the same? 
      // we just need to check one final gcd between first 2 elements
      if( all_same && _n_ints >1)
      {
        _v_gcd[0]  = abs(gcd(_v_gcd[0],_v_gcd[1]));
        // we are done
        _n_ints = 1;
      }

      // only one value left, either +-1 or gcd
      if( _n_ints == 1)
      {
        done = true;
        if( (_v_gcd[0])*(_v_gcd[0]) == 1)
        {
          found_gcd = false;
          //std::cerr << __FUNCTION__ << " Info: No gcd found!" << std::endl;
        }
        else
        {
          i_gcd = _v_gcd[0];
          found_gcd = true;
          //std::cerr << __FUNCTION__ << " Info: Found gcd = " << i_gcd << std::endl;
        }
      }
    }
    // we now have n_ints-1 gcds to check next iteration
    --_n_ints;
  }
  return i_gcd;
}

//-----------------------------------------------------------------------------

}

