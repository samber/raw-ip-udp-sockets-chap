

#ifndef MYCHAP_H__
#define MYCHAP_H__

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

enum log_level
{
    ll_error = 0,
    ll_info = 1,
    ll_debug = 2,
};

#define __LOG_LEVEL__ ll_error
#define CHECK_LOG_LEVEL(lvl) ((__LOG_LEVEL__) >= (lvl))

#define MAX_MTU_IPV4 0xffff
#define POLL_TIMEOUT 100 // ms

struct t_addr
{
    char *saddr;
    char *daddr;
    unsigned short sport;
    unsigned short dport;

    uint32_t encoded_saddr;
    uint32_t encoded_daddr;
    uint16_t encoded_sport;
    uint16_t encoded_dport;
};

struct t_auth
{
    bool authed;
    char *challenge;
    char *password;
};

struct t_client
{
    struct t_addr addr;
    struct t_auth auth;
    int sock;
};

struct t_data
{
    void *data;
    unsigned int len;
};

struct lvl3_ipv4_header
{
    // uint8_t version : 4;
    // uint8_t ihl : 4;
    uint8_t version_and_ihl;
    uint8_t tos;
    uint16_t len;
    uint16_t id;
    // uint16_t flags : 3;
    // uint16_t frag_offset : 13;
    uint16_t flags_and_frag_offset;
    uint8_t ttl;
    uint8_t proto;
    uint16_t csum;
    uint32_t saddr;
    uint32_t daddr;
} __attribute__((packed));

struct lvl4_pseudo_header
{
    uint32_t saddr;
    uint32_t daddr;
    uint8_t placeholder;
    uint8_t proto;
    uint16_t data_len;
};

struct lvl4_udp_header
{
    uint16_t sport;
    uint16_t dport;
    short len;
    uint16_t csum;
};

/* socket stuffs */
int init_socket();
int send_datagram_over_socket(struct t_client *client, const struct t_data *data);
int receive_datagram_over_socket(struct t_client *client, struct t_data *data);
int socket_is_readable(int sock, unsigned int timeout_ms);

/* packet marshalling/unmarshalling */
int datagram_serialize(struct t_data *datagram, const struct t_addr *addr, const struct t_data *data);
int datagram_unserialize(const struct t_data *datagram, struct t_data *data, const struct t_addr *addr);
void lvl3_ipv4_serialize(struct lvl3_ipv4_header *hdr, const uint32_t saddr, const uint32_t daddr, const uint8_t proto, const size_t data_length);
void lvl4_pseudo_header_serialize(struct lvl4_pseudo_header *hdr, const uint32_t saddr, const uint32_t daddr, uint8_t proto, uint16_t data_len);
void lvl4_udp_serialize(struct lvl4_udp_header *hdr, const uint16_t sport, const uint16_t dport, const struct t_data *data);
uint16_t checksum(void *addr, int nbytes);

/* utils */
void show_mem(void *anything, size_t len);
void hash_sha256(char *input, char *output);

/* protocol and auth */
int match_command(struct t_client *client, const struct t_data *data);
int phase_1_hello(struct t_client *client);
int phase_2_challenge(struct t_client *client, const struct t_data *recv_data);
int phase_3_challenge(struct t_client *client);
int phase_4_challenge(struct t_client *client, const struct t_data *data);

#endif
