#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "rsa.h"
#include <gmp.h>
#include <inttypes.h>
#include <stdlib.h>
#include "numtheory.h"
#include "randstate.h"

void rsa_make_pub(mpz_t p, mpz_t q, mpz_t n, mpz_t e, uint64_t nbits, uint64_t iters) {

    mpz_t phi_q, phi_p, totient;
    mpz_inits(phi_p, phi_q, totient, NULL);
    uint32_t pbits = (rand() % ((2 * nbits / 4) + 1)) + nbits / 4;
    uint32_t qbits = nbits - pbits;
    pbits++;
    qbits++;
    make_prime(p, pbits, iters);
    make_prime(q, qbits, iters);
    mpz_mul(n, p, q);
    mpz_sub_ui(phi_p, p, 1);
    mpz_sub_ui(phi_q, q, 1);
    mpz_mul(totient, phi_p, phi_q);
    mpz_clears(phi_q, phi_p, NULL);

    mpz_t temp;
    mpz_init(temp);
    bool x = true;
    while (x) {

        mpz_urandomb(e, state, nbits);
        gcd(temp, e, totient);
        if (mpz_cmp_ui(temp, 1) == 0) {
            x = false;
        }
    }
    mpz_clears(temp, totient, NULL);
}

void rsa_write_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {

    gmp_fprintf(pbfile, "%Zx\n", n);
    gmp_fprintf(pbfile, "%Zx\n", e);
    gmp_fprintf(pbfile, "%Zx\n", s);
    fprintf(pbfile, "%s\n", username);
}

void rsa_read_pub(mpz_t n, mpz_t e, mpz_t s, char username[], FILE *pbfile) {

    gmp_fscanf(pbfile, "%Zx\n", n);
    gmp_fscanf(pbfile, "%Zx\n", e);
    gmp_fscanf(pbfile, "%Zx\n", s);
    fscanf(pbfile, "%s\n", username);
}

void rsa_make_priv(mpz_t d, mpz_t e, mpz_t p, mpz_t q) {

    mpz_t phi_p, phi_q, totient;
    mpz_inits(phi_p, phi_q, totient, NULL);
    mpz_sub_ui(phi_p, p, 1);
    mpz_sub_ui(phi_q, q, 1);
    mpz_mul(totient, phi_p, phi_q);

    mod_inverse(d, e, totient);
    mpz_clears(phi_p, totient, phi_q, NULL);
}

void rsa_write_priv(mpz_t n, mpz_t d, FILE *pvfile) {

    gmp_fprintf(pvfile, "%Zx\n", n);
    gmp_fprintf(pvfile, "%Zx\n", d);
}

void rsa_read_priv(mpz_t n, mpz_t d, FILE *pvfile) {

    gmp_fscanf(pvfile, "%Zx\n", n);
    gmp_fscanf(pvfile, "%Zx\n", d);
}
void rsa_encrypt(mpz_t c, mpz_t m, mpz_t e, mpz_t n) {

    pow_mod(c, m, e, n);
}

void rsa_encrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t e) {

    size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));
    block[0] = 0xFF;

    mpz_t m;
    mpz_init(m);
    mpz_t c;
    mpz_init(c);

    size_t j = 1;
    while ((j = fread(&block[1], sizeof(uint8_t), k - 1, infile)) > 0) {

        mpz_import(m, j + 1, 1, sizeof(uint8_t), 1, 0, block);

        rsa_encrypt(c, m, e, n);

        gmp_fprintf(outfile, "%Zx\n", c);
    }
    mpz_clears(c, m, NULL);
    free(block);
}
void rsa_decrypt(mpz_t m, mpz_t c, mpz_t d, mpz_t n) {

    pow_mod(m, c, d, n);
}
void rsa_decrypt_file(FILE *infile, FILE *outfile, mpz_t n, mpz_t d) {

    size_t k = (mpz_sizeinbase(n, 2) - 1) / 8;
    uint8_t *block = (uint8_t *) calloc(k, sizeof(uint8_t));

    mpz_t c;
    mpz_t m;
    mpz_init(c);
    mpz_init(m);
    size_t j = 1;
    while (gmp_fscanf(infile, "%Zx\n", c) > 0) {

        rsa_decrypt(m, c, d, n);
        mpz_export(block, &j, 1, sizeof(uint8_t), 1, 0, m);

        fwrite(block + 1, sizeof(uint8_t), j - 1, outfile);
    }

    mpz_clears(c, m, NULL);
    free(block);
}

void rsa_sign(mpz_t s, mpz_t m, mpz_t d, mpz_t n) {

    pow_mod(s, m, d, n);
}
bool rsa_verify(mpz_t m, mpz_t s, mpz_t e, mpz_t n) {

    mpz_t t;
    mpz_init(t);
    pow_mod(t, s, e, n);
    if (mpz_cmp(t, m) == 0) {
        mpz_clear(t);
        return true;
    }
    mpz_clear(t);
    return false;
}
