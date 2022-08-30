#include <stdio.h>
#include "numtheory.h"
#include "rsa.h"
#include "randstate.h"
#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#define OPTIONS "b:i:n:d:s:vh"

int main(int argc, char **argv) {
    uint64_t nbits = 256;
    uint64_t iters = 50;
    FILE *pbfile = NULL;
    FILE *pvfile = NULL;

    uint64_t seed = time(NULL);
    bool verbose = false;

    char *pbfile_ch = NULL;
    char *pvfile_ch = NULL;

    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

        case 'b':
            if (sscanf(optarg, "%" SCNu64, &nbits) <= 0) {

                fprintf(stderr, "invalid value");
                free(pbfile);
                free(pvfile);
                exit(EXIT_FAILURE);
            }
            break;
        case 'i':
            if (sscanf(optarg, "%" SCNu64, &iters) <= 0) {

                fprintf(stderr, "invalid value");
                free(pbfile);
                free(pvfile);
                exit(EXIT_FAILURE);
            }
            break;
        case 'n': pbfile_ch = strdup(optarg); break;
        case 'd': pvfile_ch = strdup(optarg); break;
        case 's':
            if (sscanf(optarg, "%" SCNu64, &seed) <= 0) {

                fprintf(stderr, "invalid value");
                free(pbfile);
                free(pvfile);
                exit(EXIT_FAILURE);
            }
            break;
        case 'v': verbose = true; break;
        case 'h':
            printf("SYNOPSIS\n   Generates an RSA public/private key pair.\n\nUSAGE\n   ./keygen "
                   "[-hv] [-b bits] -n pbfile -d pvfile\n\nOPTIONS\n   -h              Display "
                   "program help and usage.\n   -v              Display verbose program output.\n  "
                   " -b bits         Minimum bits needed for public key n (default: 256).\n   -i "
                   "confidence   Miller-Rabin iterations for testing primes (default: 50).\n   -n "
                   "pbfile       Public key file (default: rsa.pub).\n   -d pvfile       Private "
                   "key file (default: rsa.priv).\n   -s seed         Random seed for testing.");
            break;
        default:
            fprintf(stderr, "inserted item is not avalible");
            exit(EXIT_FAILURE);
            break;
        }
    }
    if (nbits == 0) {
        fprintf(stderr, "bytes must be more than 0");
        free(pbfile);
        free(pvfile);
        exit(EXIT_FAILURE);
    }

    randstate_init(seed);

    if (pbfile_ch != NULL) {
        pbfile = fopen(pbfile_ch, "w");
    }
    if (pbfile == NULL) {
        pbfile = fopen("rsa.pub", "w");

        //        fprintf(stderr, "Failed to open key");
        //        randstate_clear();
        //        free(pvfile);
        //       exit(EXIT_FAILURE);
    } // else {
    // pbfile = fopen(pbfile, "w");
    // }
    if (pvfile_ch != NULL) {
        pvfile = fopen(pvfile_ch, "w");
    }
    if (pvfile == NULL) {

        pvfile = fopen("rsa.priv", "w");

        //	    fprintf(stderr, "Failed to open private key");
        //        fclose(pbfile);
        //       randstate_clear();
        //        exit(EXIT_FAILURE);

    } else {
        fchmod(fileno(pvfile), 0600);
    }

    mpz_t p, q, n, e;
    mpz_inits(q, p, n, e, NULL);
    rsa_make_pub(p, q, n, e, nbits, iters);

    mpz_t d;
    mpz_init(d);
    rsa_make_priv(d, e, p, q);

    char *username = getenv("USER");

    mpz_t m;
    mpz_init_set_str(m, username, 62);
    mpz_t s;
    mpz_init(s);
    rsa_sign(s, m, d, n);

    rsa_write_pub(n, e, s, username, pbfile);
    rsa_write_priv(n, d, pvfile);

    if (verbose == true) {

        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("p (%zu bits) = %Zd\n", mpz_sizeinbase(p, 2), p);
        gmp_printf("q (%zu bits) = %Zd\n", mpz_sizeinbase(q, 2), q);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
        gmp_printf("d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    fclose(pvfile);
    fclose(pbfile);
    mpz_clears(s, p, q, n, e, d, m, NULL);
    randstate_clear();
    free(pvfile_ch);
    free(pbfile_ch);
    return EXIT_SUCCESS;
}
