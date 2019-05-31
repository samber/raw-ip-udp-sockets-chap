
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>
#include "mychap.h"

int prepare_client(struct t_client *clt)
{
    if (inet_pton(AF_INET, clt->addr.saddr, &(clt->addr.encoded_saddr)) != 1)
        return 1;
    if (inet_pton(AF_INET, clt->addr.daddr, &(clt->addr.encoded_daddr)) != 1)
        return 1;

    clt->addr.encoded_sport = htons(clt->addr.sport);
    clt->addr.encoded_dport = htons(clt->addr.dport);

    return 0;
}

// int prepare_client(int sock)
// {
//     struct t_auth auth =
//         {
//             false,
//             {0},
//             "1234567890",
//         };

//     struct t_client client =
//         {
//             auth,
//             sock,
//         };

//     return client;
// }

int main()
{
    printf("Starting client\n\n");

    int sock = init_socket();
    if (sock == -1)
    {
        printf("Failed to create socket");
        return 84;
    }

    struct t_client client =
        {
            {
                "127.0.0.1",
                "127.0.0.1",
                4242,
                14242,
                0,
                0,
                0,
                0,
            },
            {
                false,
                NULL,
                "epitech",
            },
            sock,
        };

    if (prepare_client(&client) != 0)
    {
        printf("Malformed addresses");
        close(sock);
        return 84;
    }

    int retval = phase_1_hello(&client);
    if (retval < 0)
    {
        close(sock);
        return 84;
    }

    while (1)
    {
        struct t_data data =
            {
                NULL,
                0,
            };

        // loops while packet is in idle mode
        retval = socket_is_readable(sock, POLL_TIMEOUT);
        if (retval <= 0)
            continue;

        retval = receive_datagram_over_socket(&client, &data);
        if (retval < 0)
        {
            close(sock);
            return 84;
        }
        // loops while packet is dropped
        if (retval == 0)
            continue;

        retval = match_command(&client, &data);

        free(data.data);

        if (retval == -1)
            break;
    }

    printf("Bye\n");

    close(sock);
    return 0;
}
