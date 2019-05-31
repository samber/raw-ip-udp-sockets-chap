
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <arpa/inet.h>
#include "mychap.h"

void print_datagram_info(const struct t_data *datagram, const struct t_data *data)
{
    if (CHECK_LOG_LEVEL(ll_debug))
    {
      printf("Layers lengths\n\t=> lvl4 ipv4 header: %ld\n\t=> lvl4 udp header: %ld\n\t=> lvl7 data: %d", sizeof(struct lvl3_ipv4_header), sizeof(struct lvl4_udp_header), data->len);
      printf("\n\t=> total: %d\n\n", datagram->len);
        show_mem(datagram->data, datagram->len);
    }
}

int datagram_serialize(struct t_data *datagram, const struct t_addr *addr, const struct t_data *data)
{
    // alloc memory for diagram
    datagram->len = sizeof(struct lvl3_ipv4_header) + sizeof(struct lvl4_udp_header) + data->len; // excluding ipv4 options
    datagram->data = malloc(datagram->len);
    bzero(datagram->data, datagram->len);

    if (datagram->data == NULL)
        return 1;

    // copy data into datagram
    memcpy(datagram->data + sizeof(struct lvl3_ipv4_header) + sizeof(struct lvl4_udp_header), data->data, data->len); // excluding ipv4 options

    // Map header struct on datagram.
    // `lvl4_pseudo_hdr` overrides `lvl3_ipv4_hdr` for computing checksum (saving some memcpy).
    // In memory, `ipv4_pseudo_hdr` is aligned on the right of `ipv4_hdr`.
    // Excluding ipv4 options.
    struct lvl3_ipv4_header *lvl3_ipv4_hdr = datagram->data;
    struct lvl4_pseudo_header *lvl4_pseudo_hdr = datagram->data + (sizeof(struct lvl3_ipv4_header) - sizeof(struct lvl4_pseudo_header));
    struct lvl4_udp_header *lvl4_udp_hdr = datagram->data + sizeof(struct lvl3_ipv4_header);

    // build pseudo headers
    lvl4_pseudo_header_serialize(lvl4_pseudo_hdr, addr->encoded_saddr, addr->encoded_daddr, IPPROTO_UDP, sizeof(struct lvl4_udp_header) + data->len);
    // build udp headers
    lvl4_udp_serialize(lvl4_udp_hdr, addr->encoded_sport, addr->encoded_dport, data);

    // compute udp checksum
    lvl4_udp_hdr->csum = checksum((void *)(lvl4_pseudo_hdr), sizeof(struct lvl4_pseudo_header) + sizeof(struct lvl4_udp_header) + data->len);

    // build ipv4 headers
    // this overrides pseudo header
    lvl3_ipv4_serialize(lvl3_ipv4_hdr, addr->encoded_saddr, addr->encoded_daddr, IPPROTO_UDP, sizeof(struct lvl4_udp_header) + data->len);

    // compute ipv4 checksum
    lvl3_ipv4_hdr->csum = checksum(datagram->data, datagram->len);

    print_datagram_info(datagram, data);

    return 0;
}

// returns 0 if packet is incomplete (1 more read expected for a full packet)
// if packet headers are valid, it returns packet size (header ipv4 + header udp + data), not including trailing bytes (another packet ?)
// in case of error, returns size of data to drop invalid packet in ring buffer
int datagram_unserialize(const struct t_data *datagram, struct t_data *data, const struct t_addr *addr)
{
    // reset data size
    data->len = 0;
    data->data = NULL;

    // should call read one more time to fill ring buffer if total_length was not read
    // must be done before packet size
    if (datagram->len < sizeof(struct lvl3_ipv4_header)) // excluding options for now
        return 0;

    // map buffer on an ipv4 header
    struct lvl3_ipv4_header *lvl3_ipv4_hdr = datagram->data;

    size_t packet_size = ntohs(lvl3_ipv4_hdr->len);                                         // excluding options for now
    size_t headers_size = sizeof(struct lvl3_ipv4_header) + sizeof(struct lvl4_udp_header); // excluding options for now
    size_t data_size = packet_size - headers_size;

    // should call `read` one more time to fill ring buffer if total_length is greater than datagram->len
    if (datagram->len < packet_size)
        return 0;

    // STARTING HERE, WE HAVE A FULL PACKET

    // this packet is not to me (lvl3 checks)
    if (lvl3_ipv4_hdr->saddr != addr->encoded_daddr)
        return packet_size;
    if (lvl3_ipv4_hdr->daddr != addr->encoded_saddr)
        return packet_size;
    if (lvl3_ipv4_hdr->proto != IPPROTO_UDP)
        return packet_size;
    // other ipv4 checks
    if (lvl3_ipv4_hdr->version_and_ihl != (4 << 4) + 5) // excluding options for now
        return packet_size;

    // this packet is not to me (lvl4 checks)
    struct lvl4_udp_header *lvl4_udp_hdr = datagram->data + sizeof(struct lvl3_ipv4_header);
    if (lvl4_udp_hdr->sport != addr->encoded_dport)
        return packet_size;
    if (lvl4_udp_hdr->dport != addr->encoded_sport)
        return packet_size;
    if (ntohs(lvl4_udp_hdr->len) != packet_size - sizeof(struct lvl3_ipv4_header))
        return packet_size;

    // @todo: validate checksums

    data->len = data_size;
    if (data_size > 0)
        data->data = strndup(datagram->data + headers_size, data_size);

    print_datagram_info(datagram, data);

    return packet_size;
}
