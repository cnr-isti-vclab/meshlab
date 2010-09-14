#ifndef BIGFRAC_H
#define BIGFRAC_H

#include <iostream>
#include <gmpxx.h>

namespace vcg {
    namespace math {
        class fraction {
        public:
            fraction() : f(0) { }
            fraction(int n) : f(n, 1) {}
            fraction(int n, int d) : f(n, d) { f.canonicalize(); }
            fraction(const fraction &from) : f(from.f) {}
            ~fraction() {}

            inline fraction& operator=(const fraction &y) { f = y.f; return *this; }
            inline fraction& operator+=(const fraction &y) { f += y.f; return *this; }
            inline fraction& operator-=(const fraction &y) { f -= y.f; return *this; }
            inline fraction& operator*=(const fraction &y) { f *= y.f; return *this; }
            inline fraction& operator/=(const fraction &y) { f /= y.f; return *this; }

            inline fraction operator+(const fraction &y) const { return fraction(*this) += y; }
            inline fraction operator-(const fraction &y) const { return fraction(*this) -= y; }
            inline fraction operator*(const fraction &y) const { return fraction(*this) *= y; }
            inline fraction operator/(const fraction &y) const { return fraction(*this) /= y; }

            inline bool operator==(const fraction &y) const { return f == y.f; }
            inline bool operator!=(const fraction &y) const { return f != y.f; }
            inline bool operator<=(const fraction &y) const { return f <= y.f; }
            inline bool operator>=(const fraction &y) const { return f >= y.f; }
            inline bool operator<(const fraction &y) const { return f < y.f; }
            inline bool operator>(const fraction &y) const { return f > y.f; }

            inline double toFloat() const { return f.get_d(); }

            friend std::ostream& operator<<(std::ostream &out, const fraction &x) {
                //return out << x.f.get_d() << "[" << x.f << "]";
                return out << x.f;
            }

            friend inline long floor(const fraction &x) {
                mpz_class q;
                mpz_fdiv_q (q.get_mpz_t(), x.f.get_num_mpz_t(), x.f.get_den_mpz_t());
                return q.get_si();
            }

            friend inline long ceil(const fraction &x) {
                mpz_class q;
                mpz_cdiv_q (q.get_mpz_t(), x.f.get_num_mpz_t(), x.f.get_den_mpz_t());
                return q.get_si();
            }

        private:
            mpq_class f;
        };
    }
}

#endif // BIGFRAC_H
