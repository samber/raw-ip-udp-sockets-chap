
#include <openssl/sha.h>
#include <stdio.h>
#include <string.h>

void hash_sha256(char *input, char *output)
{
    unsigned char hash[SHA256_DIGEST_LENGTH];

    SHA256_CTX sha256;
    SHA256_Init(&sha256);
    SHA256_Update(&sha256, input, strlen(input));
    SHA256_Final(hash, &sha256);

    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        sprintf(output + (i * 2), "%02x", (unsigned char)hash[i]);

    output[SHA256_DIGEST_LENGTH * 2] = 0;
}