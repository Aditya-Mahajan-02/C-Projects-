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

#define OPTIONS "i:o:n:vh"

int main(int argc, char **argv) {
    FILE *infile = stdin;
    FILE *outfile = stdout;
    FILE *privkey = NULL;
    bool verbose = false;

    char *outfile_ch = NULL;

    int opt;
    while ((opt = getopt(argc, argv, OPTIONS)) != -1) {

        switch (opt) {

        case 'i':
            infile = fopen(optarg, "r");
            if (infile == NULL) {

                fprintf(stderr, "Failed to open input");
                exit(EXIT_FAILURE);
            }
            break;
        case 'o': outfile_ch = strdup(optarg); break;
        case 'n':
            privkey = fopen(optarg, "r");
            if (privkey == NULL) {

                fprintf(stderr, "Failed to open input");
                exit(EXIT_FAILURE);
            }
            break;
        case 'v': verbose = true; break;
        case 'h':
            printf(
                "SYNOPSIS\n   Encrypts data using RSA encryption.\n   Encrypted data is decrypted "
                "by the decrypt program.\n\nUSAGE\n   ./encrypt [-hv] [-i infile] [-o outfile] -n "
                "pubkey\n\nOPTIONS\n   -h              Display program help and usage.\n   -v      "
                "        Display verbose program output.\n   -i infile       Input file of data to "
                "encrypt (default: stdin).\n   -o outfile      Output file for encrypted data "
                "(default: stdout).\n   -n pbfile       Public key file (default: rsa.pub).");
            break;
        default:
            fprintf(stderr, "inserted item is not avalible");
            exit(EXIT_FAILURE);
            break;
        }
    }

    if (privkey == NULL) {

        privkey = fopen("rsa.priv", "r");
        if (privkey == NULL) {
            fprintf(stderr, "Failed to opem file");
            exit(EXIT_FAILURE);
        }
    }
    if (outfile_ch != NULL) {

        outfile = fopen(outfile_ch, "w");
        free(outfile_ch);
        if (outfile == NULL) {

            fprintf(stderr, "failed to open for writing");
            exit(EXIT_FAILURE);
        }
    }

    mpz_t n, d;

    mpz_inits(n, d, NULL);

    rsa_read_priv(n, d, privkey);

    if (verbose) {
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("d (%zu bits) = %Zd\n", mpz_sizeinbase(d, 2), d);
    }

    rsa_decrypt_file(infile, outfile, n, d);

    mpz_clears(n, d, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(privkey);
    return EXIT_SUCCESS;
}
