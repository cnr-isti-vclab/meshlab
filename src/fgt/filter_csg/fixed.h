/****************************************************************************
* MeshLab                                                           o o     *
* A versatile mesh processing toolbox                             o     o   *
*                                                                _   O  _   *
* Copyright(C) 2010                                                \/)\/    *
* Visual Computing Lab                                            /\/|      *
* ISTI - Italian National Research Council                           |      *
*                                                                    \      *
* All rights reserved.                                                      *
*                                                                           *
* This program is free software; you can redistribute it and/or modify      *   
* it under the terms of the GNU General Public License as published by      *
* the Free Software Foundation; either version 2 of the License, or         *
* (at your option) any later version.                                       *
*                                                                           *
* This program is distributed in the hope that it will be useful,           *
* but WITHOUT ANY WARRANTY; without even the implied warranty of            *
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             *
* GNU General Public License (http://www.gnu.org/licenses/gpl.txt)          *
* for more details.                                                         *
*                                                                           *
****************************************************************************/

#ifndef FIXED_H
#define FIXED_H

#include <stdint.h>
#include <utility>
#include "bigint.h"

namespace vcg {
    namespace math {
        using namespace std::rel_ops;

        template <typename T, typename TT>
                class arith_type {
        public:
            typedef T type;
            typedef TT mul_type;
        };

        template <typename T>
                class closed_arith_type : public arith_type<T, closed_arith_type<T> > {};

        typedef closed_arith_type<bigint32> big32;
        typedef arith_type<uint32_t, big32> base32;

        template<const int frac_bits, typename a_T=big32>
                                                   class fixed {
        private:
            typedef typename a_T::type T;
            static inline fixed MakeFixed(const T &x) { fixed r; r.x = x; return r; }

            inline fixed(const T &a, const T &b) : x(a * b) {}

            inline static T ONE() { return T(1) << frac_bits; }

        public:
            static inline fixed MakeFixed(const T &a, const T &b) { fixed r; r.x = a * b; return r; }

            inline fixed() {}

            inline fixed(const double &v) : x(ONE() * v) {}

            inline bool operator<(const fixed &y) const { return x < y.x; }

            inline bool operator==(const fixed &y) const { return x == y.x; }

            inline fixed operator+(const fixed &y) const { return MakeFixed(x + y.x); }

            inline fixed operator-(const fixed &y) const { return MakeFixed(x - y.x); }

            inline fixed operator-() const { return MakeFixed(-x); }

            friend inline T floor(const fixed &x) { return x.x >> frac_bits; }

            friend inline T ceil(const fixed &x) { return floor(x - MakeFixed(ONE() - T(1))); }

            template<const int sbits, typename a_S>
                   inline fixed<frac_bits+sbits,typename a_T::mul_type> operator*(const fixed<sbits, a_S> &y) { return fixed<frac_bits+sbits,typename a_T::mul_type>::MakeFixed(x, y.x); }

        private:
            T x;
        };
    };
};

#endif // FIXED_H
