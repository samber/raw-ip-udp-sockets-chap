
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <netpacket/packet.h>

#include "mychap.h"

static char read_buffer[MAX_MTU_IPV4 * 2];
static struct t_data read_buffer_datagram = {
    read_buffer,
    0, // this is the offset in the ring buffer
};

void print_sent_info(const struct t_addr *addr, const struct t_data *data, size_t sent)
{
    if (CHECK_LOG_LEVEL(ll_info))
    {
        printf("Sent %lu bytes to %s..%d\nPayload: \"%*.*s\"\n\n", sent, addr->daddr, addr->dport, data->len, data->len, (char *)data->data);
    }
}

void print_received_info(const struct t_addr *addr, const struct t_data *data, size_t packet_size)
{
    if (CHECK_LOG_LEVEL(ll_info))
    {
        printf("Received %lu bytes from %s..%d\nPayload: \"%*.*s\"\n\n", packet_size, addr->daddr, addr->dport, data->len, data->len, (char *)data->data);
    }
}

int init_socket()
{
    int sock = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
    // int sock = socket(AF_INET, SOCK_RAW, IPPROTO_RAW);
    if (sock < 0)
        return -1;

    int optval = 1;
    if (setsockopt(sock, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0)
    {
        close(sock);
        return -1;
    }

    return sock;
}

// should send a void* msg instead of char*
int send_datagram_over_socket(struct t_client *client, const struct t_data *data)
{
    struct t_data datagram = {
        NULL,
        0,
    };

    if (datagram_serialize(&datagram, &client->addr, data) != 0)
    {
        printf("Failed to build datagram");
        return -1;
    }

    struct sockaddr_in sin;
    int sent = 0;

    sin.sin_family = AF_INET;
    sin.sin_port = client->addr.encoded_sport;
    sin.sin_addr.s_addr = client->addr.encoded_saddr;

    sent = sendto(client->sock, datagram.data, datagram.len, 0, (struct sockaddr *)&sin, sizeof(sin));
    // sent = sendto(client->sock, datagram.data, datagram.len, 0, NULL, 0);
    free(datagram.data);

    print_sent_info(&client->addr, data, sent);
    return sent;
}

// returns -1 in case of error
// returns 0 if nothing to read
// returns 1 if socket is readable
int socket_is_readable(int sock, unsigned int timeout_ms)
{
    struct timeval tv = {
        0,
        timeout_ms * 1000,
    };
    fd_set readfds;

    FD_ZERO(&readfds);
    FD_SET(sock, &readfds);

    return select(sock + 1, &readfds, NULL, NULL, &tv);
}

// returns data size if successful
// returns 0 if packet is dropped or incomplete (one more read expected)
// returns -1 in case of error
int receive_datagram_over_socket(struct t_client *client, struct t_data *data)
{
    // struct sockaddr_ll recvaddr;
    // unsigned int len = sizeof(struct sockaddr);
    // int retval = recvfrom(sock, &read_buffer_datagram.data + read_buffer_datagram.len, MAX_MTU_IPV4, 0, (struct sockaddr *)&recvaddr, &len);

    int retval = read(client->sock, read_buffer_datagram.data + read_buffer_datagram.len, MAX_MTU_IPV4);
    if (retval < 0)
    {
        printf("Failed to receive packet"); // tbh, this should not happen (udp 😅)
        return -1;
    }

    // ring buffer is now longer of `retval` bytes
    read_buffer_datagram.len += retval;

    // returns 0 if we need one more read for a full packet
    // or size of packet to remove in ring buffer
    // if packet is valid, then data.len > 0
    int packet_size = datagram_unserialize(&read_buffer_datagram, data, &client->addr);
    if (packet_size == 0)
        return 0;

    if (data->len > 0)
        print_received_info(&client->addr, data, packet_size);

    // moves offset of ring buffer
    // tbh, this is not a ring buffer, 'cause it's easier to map a header on a simple buffer
    // then we just move offset to 0
    read_buffer_datagram.len -= packet_size;
    memmove(read_buffer_datagram.data, read_buffer_datagram.data + packet_size, read_buffer_datagram.len);

    return data->len;
}
