
#include <stdio.h>
#include <string.h>
#include "mychap.h"

int match_command_pre_auth(struct t_client *client, const struct t_data *data)
{
    if (client->auth.challenge == NULL)
        return phase_2_challenge(client, data);
    return phase_4_challenge(client, data);
}

int match_command_post_auth(struct t_client *client, const struct t_data *data)
{
    (void)client;
    (void)data;
    return 0;
}

int match_command(struct t_client *client, const struct t_data *data)
{
    // parse only if we read more than 0 bytes
    if (data->len == 0)
        return 0;

    // exit when server send "Protocol Mismatch"
    const char *PROTOCOL_MISMATCHED = "Protocol Mismatch";
    const size_t PROTOCOL_MISMATCHED_len = strlen("Protocol Mismatch");
    if (data->len == PROTOCOL_MISMATCHED_len && strncmp(data->data, PROTOCOL_MISMATCHED, PROTOCOL_MISMATCHED_len) == 0)
    {
        printf("Protocol mismatched :-(\n\n");
        return -1;
    }

    return client->auth.authed == true ? match_command_post_auth(client, data) : match_command_pre_auth(client, data);
}
