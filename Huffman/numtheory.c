#include "numtheory.h"
#include "randstate.h"
#include <inttypes.h>

void gcd(mpz_t d, mpz_t a, mpz_t b) {
    mpz_t t, temp_a, temp_b;
    mpz_init_set(temp_a, a);
    mpz_init_set(temp_b, b);
    mpz_init(t);

    while (mpz_sgn(temp_b) != 0) {
        mpz_set(t, temp_b);
        mpz_mod(temp_b, temp_a, temp_b);
        mpz_set(temp_a, t);
    }
    mpz_clear(t);
    mpz_set(d, temp_a);
    mpz_clear(temp_b);
    mpz_clear(temp_a);
}

void mod_inverse(mpz_t i, mpz_t a, mpz_t n) {

    mpz_t r;
    mpz_init_set(r, n);

    mpz_t r_prime;
    mpz_init_set(r_prime, a);

    mpz_t t;
    mpz_init_set_ui(t, 0);

    mpz_t t_prime;
    mpz_init_set_ui(t_prime, 1);

    mpz_t q;
    mpz_init(q);
    while (mpz_sgn(r_prime) != 0) {

        mpz_fdiv_q(q, r, r_prime);

        mpz_t temp_r;
        mpz_init_set(temp_r, r);
        mpz_set(r, r_prime);
        mpz_submul(temp_r, q, r_prime);
        mpz_set(r_prime, temp_r);
        mpz_clear(temp_r);

        mpz_t temp_t;
        mpz_init_set(temp_t, t);
        mpz_set(t, t_prime);
        mpz_submul(temp_t, q, t_prime);
        mpz_set(t_prime, temp_t);
        mpz_clear(temp_t);
    }
    if (mpz_sgn(t) == -1) {

        mpz_add(t, t, n);
    }
    mpz_set(i, t);
    if (mpz_cmp_ui(r, 1) > 0) {

        mpz_set_ui(i, 0);
    }
    mpz_clears(t_prime, q, r, r_prime, t, NULL);
}

void pow_mod(mpz_t out, mpz_t base, mpz_t exponent, mpz_t modulus) {

    mpz_t v, d, n;
    mpz_t a, p;

    mpz_inits(v, d, n, a, p, NULL);
    mpz_set_ui(v, 1);
    mpz_set(d, exponent);
    mpz_set(n, modulus);
    mpz_set(a, base);
    mpz_set(p, a);

    while (mpz_sgn(d) == 1) {
        if (mpz_odd_p(d)) {
            mpz_mul(v, v, p);
            mpz_mod(v, v, n);
            //	out = (out * temp) % n;
        }
        mpz_mul(p, p, p);
        mpz_mod(p, p, n);

        mpz_fdiv_q_2exp(d, d, 1);
    }
    mpz_set(out, v);
    mpz_clears(v, d, n, a, p, NULL);
}
bool is_prime(mpz_t n, uint64_t iters) {
    if (mpz_cmp_ui(n, 1) <= 0) {
        return false;
    } else if (mpz_cmp_ui(n, 3) <= 0) {

        return true;
    } else if (mpz_even_p(n)) {

        return false;
    } else {

        mpz_t temp, s;
        mpz_inits(s, temp, NULL);
        mpz_sub_ui(temp, n, 1);

        //	int s = 0;
        mpz_set_ui(s, 0);
        //	mpz_t temp = n-1;

        while (mpz_even_p(temp)) {

            mpz_fdiv_q_2exp(temp, temp, 1);

            mpz_add_ui(s, s, 1);
        }

        for (uint64_t i = 1; i <= iters; i++) {

            //	mpz_t a = (rand() % (n - 2)) + 2
            //	            //	mpz_t y = 0;
            mpz_t range;
            mpz_init(range);
            mpz_sub_ui(range, n, 3);

            mpz_t a;
            mpz_init(a);
            mpz_urandomm(a, state, range);

            mpz_clear(range);

            mpz_add_ui(a, a, 2);

            mpz_t y;
            mpz_init(y);
            mpz_set_ui(y, 0);

            pow_mod(y, a, temp, n);

            mpz_clear(a);

            mpz_t n_prime;
            mpz_init(n_prime);
            mpz_sub_ui(n_prime, n, 1);

            if (mpz_cmp_ui(y, 1) != 0 && mpz_cmp(y, n_prime) != 0) {

                mpz_t j;
                mpz_init_set_ui(j, 1);

                mpz_t s_prime;
                mpz_init(s_prime);
                mpz_sub_ui(s_prime, s, 1);

                while (mpz_cmp(j, s_prime) <= 0 && mpz_cmp(y, n_prime) != 0) {

                    mpz_t exponent;
                    mpz_init_set_ui(exponent, 2);
                    pow_mod(y, y, exponent, n);
                    mpz_clear(exponent);
                    if (mpz_cmp_ui(y, 1) == 0) {
                        mpz_clears(y, j, s_prime, n_prime, temp, s, NULL);
                        return false;
                    }
                    mpz_add_ui(j, j, 1);
                }
                mpz_clear(j);
                if (mpz_cmp(y, n_prime) != 0) {
                    mpz_clears(y, s_prime, n_prime, temp, s, NULL);
                    return false;
                }
                mpz_clear(s_prime);
            }
            mpz_clears(n_prime, y, NULL);
        }
        mpz_clears(temp, s, NULL);
        return true;
    }
}
void make_prime(mpz_t p, uint64_t bits, uint64_t iters) {

    size_t shift = bits - 1;
    bool x = true;
    while (x) {

        mpz_set_ui(p, 1);
        mpz_mul_2exp(p, p, shift);

        mpz_t tail;
        mpz_init(tail);
        mpz_urandomb(tail, state, shift);
        mpz_ior(p, p, tail);
        mpz_clear(tail);
        if (is_prime(p, iters) == true) {

            x = false;
        }
    }
}
