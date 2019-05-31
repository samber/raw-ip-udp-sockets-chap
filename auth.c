
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <openssl/sha.h>
#include "mychap.h"

int phase_1_hello(struct t_client *client)
{
    struct t_data data =
        {
            "client hello",
            strlen("client hello"),
        };

    return send_datagram_over_socket(client, &data);
}

int phase_2_challenge(struct t_client *client, const struct t_data *recv_data)
{
    if (recv_data->len != 10)
    {
        printf("Challenge is malformed: 10 bytes expected");
        return -1;
    }

    client->auth.challenge = strndup(recv_data->data, recv_data->len);

    return phase_3_challenge(client);
}

int phase_3_challenge(struct t_client *client)
{
    char *raw = malloc(10 + strlen(client->auth.password) + 1);
    if (raw == NULL)
        return -1;
    strncpy(raw, client->auth.challenge, 10);
    strncpy(raw + 10, client->auth.password, strlen(client->auth.password));
    raw[10 + strlen(client->auth.password)] = '\0';

    char hash[SHA256_DIGEST_LENGTH * 2 + 1];
    hash_sha256(raw, hash);

    struct t_data data =
        {
            hash,
            SHA256_DIGEST_LENGTH * 2,
        };

    free(raw);
    return send_datagram_over_socket(client, &data);
}

int phase_4_challenge(struct t_client *client, const struct t_data *data)
{
    client->auth.authed = true;
    printf("SERVER SECRET IS \"%*.*s\"\n\n", data->len, data->len, (char *)data->data);
    return -1; // force client exit
}
