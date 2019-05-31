
#include <arpa/inet.h>
#include <string.h>
#include "mychap.h"

inline void lvl4_pseudo_header_serialize(struct lvl4_pseudo_header *hdr, const uint32_t saddr, const uint32_t daddr, const uint8_t proto, const uint16_t data_len)
{
    bzero(hdr, sizeof(struct lvl4_pseudo_header));

    hdr->saddr = saddr;
    hdr->daddr = daddr;
    hdr->placeholder = 0;
    hdr->proto = proto;
    hdr->data_len = htons(data_len);
}
