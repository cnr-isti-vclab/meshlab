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

#ifndef BIGINT_H
#define BIGINT_H

#include <iostream>
#include <vector>
#include <utility>

/*
Bignum unsigned integers
Represented by a vector of unsigned integers, with positional encoding.
The vector v (with elements v[0], v[1], ...) represents the number
Sum(v[i] * b^i) where b is the maximum number that the type of the elements
of the vector can represent
Supported operations are +, *, <<, >> and comparisons.
Please notice division is NOT included.
*/

namespace vcg {
    namespace math {
        using namespace std;
        using namespace std::rel_ops;

        template<const size_t tbits, typename T, typename TT>
                class bigint {
        private:
            inline bigint(const vector<T> &v) : v(v) {}

            inline bigint lnshift(size_t n) const {
                vector<T> r(n);
                r.insert(r.end(), v.begin(), v.end());
                return r;
            }

            inline bigint rnshift(size_t n) const {
                return vector<T>(v.begin() + n, v.end());
            }

            inline bigint<tbits,T,TT> lo() const { return vector<T>(v.begin(), v.begin() + (v.size()+1)/2); }

            inline bigint<tbits,T,TT> hi() const { return rnshift((v.size()+1)/2); }

        public:
            inline bigint() : v(0) {}

            template <typename S>
                    inline bigint(const S &x) : v(1) { v[0] = x; }

            inline bool operator<(const bigint &y) const {
                for (size_t i = y.v.size(); i > v.size(); --i)
                    if (y.v[i-1] != 0)
                        return true;
                for (size_t i = v.size(); i > y.v.size(); --i)
                    if (v[i-1] != 0)
                        return false;
                for (size_t i = min(v.size(), y.v.size()); i > 0; --i)
                    if (v[i-1] != y.v[i-1])
                        return v[i-1] < y.v[i-1];
                return false;
            }

            inline bool operator==(const bigint &y) const {
                for (size_t i = 0; i < min(v.size(), y.v.size()); ++i)
                    if (v[i] != y.v[i])
                        return false;
                for (size_t i = v.size(); i < y.v.size(); ++i)
                    if (y.v[i] != T(0))
                        return false;
                for (size_t i = y.v.size(); i < v.size(); ++i)
                    if (v[i] != T(0))
                        return false;
                return true;
            }

            inline bigint& operator+=(const bigint &y) {
                if (this == &y) {
                    *this += bigint(y.v);
                    return *this;
                }

                if (v.size() < y.v.size())
                    v.resize(y.v.size());

                bool carry = false;
                size_t i;
                for (i = 0; i < y.v.size(); ++i) {
                    v[i] += y.v[i];
                    if (carry) {
                        v[i]++;
                        carry = v[i] <= y.v[i];
                    } else
                        carry = v[i] < y.v[i];
                }
                while (carry) {
                    if (i == v.size())
                        v.push_back(T(1));
                    else
                        v[i]++;
                    carry = v[i] == T(0);
                    i++;
                }
                return *this;
            }

            inline bigint& operator-=(const bigint &y) {
                if (this == &y) {
                    v.resize(0);
                    return *this;
                }

                if (v.size() < y.v.size())
                    v.resize(y.v.size());

                bool carry = false;
                size_t i;
                for (i = 0; i < y.v.size(); ++i) {
                    T tmp = v[i];
                    v[i] -= y.v[i];
                    if (carry) {
                        v[i]--;
                        carry = v[i] >= tmp;
                    } else
                        carry = v[i] > tmp;
                }
                while (carry && i < v.size()) {
                    T tmp = v[i];
                    v[i]--;
                    carry = v[i] >= tmp;
                    ++i;
                }
                assert (!carry);
                return *this;
            }

            inline bigint& operator>>=(size_t n) {
                size_t div = n / tbits, mod = n % tbits;
                if (div >= v.size()) {
                    v.resize(0);
                    return *this;
                }

                v.erase(v.begin(), v.begin() + div);
                if (mod != 0) {
                    for (size_t i = 1; i < v.size(); ++i)
                        v[i-1] = (v[i] << (tbits - mod)) | (v[i-1] >> mod);
                    v[v.size() - 1] >>= mod;
                }
                return *this;
            }

            inline bigint& operator<<=(size_t n) {
                v.insert(v.begin(), (n + tbits - 1) / tbits, T(0));
                if (n % tbits != 0)
                    *this >>= (tbits - (n % tbits));
                return *this;
            }

            inline bigint operator*=(const bigint &y) {
                if (v.empty() || y.v.empty()) {
                    this->v.resize(0);
                } else if (v.size() == 1 && y.v.size() == 1) {
                    TT m = TT(v[0]) * TT(y.v[0]);
                    this->v.resize(2);
                    v[0] = m;
                    v[1] = m >> tbits;
                } else {
                    bigint xl = lo();
                    bigint xh = hi();
                    bigint yl = y.lo();
                    bigint yh = y.hi();

                    bigint ll = (xl * yl);
                    bigint lh = (xl * yh).lnshift(yl.v.size());
                    bigint hl = (xh * yl).lnshift(xl.v.size());
                    bigint hh = (xh * yh).lnshift(xl.v.size() + yl.v.size());

                    // Karatsuba would be nice here, but carries are more difficult to handle
                    *this = ll + lh + hl + hh;
                }
                return *this;
            }

            inline bigint operator+(const bigint &y) const { return bigint(*this) += y; }

            inline bigint operator-(const bigint &y) const { return bigint(*this) -= y; };

            inline bigint operator*(const bigint &y) const { return bigint(*this) *= y; };

            inline bigint operator<<(size_t n) const { return bigint(*this) <<= n; };

            inline bigint operator>>(size_t n) const { return bigint(*this) >>= n; };

            friend ostream& operator<<(ostream &out, const bigint<tbits,T,TT> &x) {
                if (x.v.empty())
                    return out << '0';

                ios_base::fmtflags flags = out.flags();
                std::streamsize width = out.width();
                ostream::char_type fill = out.fill();

                size_t i = x.v.size();
                while (x.v[i-1] == 0 && i > 1)
                    --i;

                out << hex << TT(x.v[i-1]);
                for(--i; i>0; --i) {
                    out.width(tbits/4);
                    out.fill('0');
                    out << hex << right << TT(x.v[i-1]);
                }
                out.flags(flags);
                out.width(width);
                out.fill(fill);
                return out;
            }

        private:
            vector<T> v;
        };

        typedef bigint<8,uint8_t,uint16_t> bigint8;
        typedef bigint<16,uint16_t,uint32_t> bigint16;
        typedef bigint<32,uint32_t,uint64_t> bigint32;
        //typedef bigint<64,uint64_t,uint128_t> bigint64;
    }
}
#endif // BIGINT_H
