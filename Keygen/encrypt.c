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
    FILE *pubkey = NULL;
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
            pubkey = fopen(optarg, "r");
            if (pubkey == NULL) {

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

    if (pubkey == NULL) {
        pubkey = fopen("rsa.pub", "r");
        if (pubkey == NULL) {
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

    char *username = calloc(1024, sizeof(char));
    mpz_t n, e, s;

    mpz_inits(n, e, s, NULL);

    rsa_read_pub(n, e, s, username, pubkey);

    if (verbose) {

        gmp_printf("s (%zu bits) = %Zd\n", mpz_sizeinbase(s, 2), s);
        gmp_printf("n (%zu bits) = %Zd\n", mpz_sizeinbase(n, 2), n);
        gmp_printf("e (%zu bits) = %Zd\n", mpz_sizeinbase(e, 2), e);
    }

    mpz_t username_t;
    mpz_init_set_str(username_t, username, 62);
    if (rsa_verify(username_t, s, e, n) == false) {
        fprintf(stderr, "verification failed");
        exit(EXIT_FAILURE);
    }
    mpz_clear(username_t);
    free(username);

    rsa_encrypt_file(infile, outfile, n, e);

    mpz_clears(n, e, s, NULL);
    fclose(infile);
    fclose(outfile);
    fclose(pubkey);
    return EXIT_SUCCESS;
}
