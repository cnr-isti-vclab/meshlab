#ifndef GMPFRAC_H
#define GMPFRAC_H

#include <iostream>
#include <mpirxx.h>

inline long floor(const mpq_class &x) {
    mpz_class q;
    mpz_fdiv_q (q.get_mpz_t(), x.get_num_mpz_t(), x.get_den_mpz_t());
    return q.get_si();
}

inline long ceil(const mpq_class &x) {
    mpz_class q;
    mpz_cdiv_q (q.get_mpz_t(), x.get_num_mpz_t(), x.get_den_mpz_t());
    return q.get_si();
}

inline double toFloat(const mpq_class &x) {
    return x.get_d();
}

inline mpq_class makeFraction(int num, int den) {
    mpq_class r(num, den);
    r.canonicalize();
    return r;
}

#endif // GMPFRAC_H
