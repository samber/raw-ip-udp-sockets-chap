
#include <arpa/inet.h>
#include <string.h>
#include "mychap.h"

inline void lvl4_udp_serialize(struct lvl4_udp_header *hdr, const uint16_t sport, const uint16_t dport, const struct t_data *data)
{
    bzero(hdr, sizeof(struct lvl4_udp_header));

    hdr->sport = sport;
    hdr->dport = dport;
    hdr->len = htons(sizeof(struct lvl4_udp_header) + data->len);
    hdr->csum = 0; // leave 0 for now
}
