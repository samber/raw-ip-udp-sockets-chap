
#include <arpa/inet.h>
#include <string.h>
#include "mychap.h"

inline void lvl3_ipv4_serialize(struct lvl3_ipv4_header *hdr, const uint32_t saddr, const uint32_t daddr, const uint8_t proto, const size_t data_length)
{
    bzero(hdr, sizeof(struct lvl3_ipv4_header));

    // hdr->version = 4;
    // hdr->ihl = 5; // excluding options
    hdr->version_and_ihl = (4 << 4) + 5; // excluding options
    hdr->tos = 0x0;
    hdr->len = htons(sizeof(struct lvl3_ipv4_header) + data_length); // excluding options
    hdr->id = htons(4242);
    // hdr->flags = (0 << 2) + (0 << 1) + 0; // Evil Bit 😂
    // hdr->frag_offset = 0x0;
    hdr->flags_and_frag_offset = 0b01000000; // do not fragment
    hdr->ttl = 0xff;
    hdr->proto = proto;
    hdr->csum = 0; // leave 0 for now
    hdr->saddr = saddr;
    hdr->daddr = daddr;
}
