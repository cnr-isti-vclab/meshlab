#ifndef BIGFRAC_H
#define BIGFRAC_H

namespace vcg {
    namespace math {
        template<typename T>
        class fraction {
        public:
            fraction() : num(0), den(1) { }
            fraction(T n) : num(n), den(1) {}
            fraction(T n, T d) : num(n), den(d) {}
            fraction(const fraction &from) : num(from.num), den(from.den) {}
            ~fraction() {}

            fraction& operator=(const fraction &from) { num = from.num; den = from.den; return *this; }
            fraction& operator+=(const fraction &b) {
                num = num * y.den + y.num * den;
                den *= y.den;
                return *this;
            }

            inline fraction& operator-=(const fraction &y) {
                num = num * y.den - y.num * den;
                den *= y.den;
                return *this;
            }

            inline fraction& operator*=(const fraction &y) { num *= y.num; den *= y.den; return *this; }
            inline fraction& operator/=(const fraction &y) { num *= y.den; den *= y.num; return *this; }

            inline fraction operator+(const fraction &y) const { return fraction(*this) += y; }
            inline fraction operator-(const fraction &y) const { return fraction(*this) -= y; }
            inline fraction operator*(const fraction &y) const { return fraction(*this) *= y; }
            inline fraction operator/(const fraction &y) const { return fraction(*this) /= y; }

            inline bool operator==(const fraction &y) const { return num * y.den == den * y.num; }
            inline bool operator!=(const fraction &y) const { return num * y.den != den * y.num; }
            inline bool operator<=(const fraction &y) const { return num * y.den <= den * y.num; }
            inline bool operator>=(const fraction &y) const { return num * y.den >= den * y.num; }
            inline bool operator<(const fraction &y) const { return num * y.den < den * y.num; }
            inline bool operator>(const fraction &y) const { return num * y.den > den * y.num; }

        private:
            T num, den;
        };
    }
}

#endif // BIGFRAC_H
