
#include "mychap.h"

// unsigned short csum(unsigned short *ptr, int nbytes)
// {
//     register uint32_t sum = 0;
//     unsigned short oddbyte;
//     register short answer;

//     sum = 0;
//     while (nbytes > 1)
//     {
//         sum += *ptr++;
//         nbytes -= 2;
//     }
//     if (nbytes == 1)
//     {
//         oddbyte = 0;
//         *((u_char *)&oddbyte) = *(u_char *)ptr;
//         sum += oddbyte;
//     }

//     sum = (sum >> 16) + (sum & 0xffff);
//     sum = sum + (sum >> 16);
//     answer = (short)~sum;

//     return (answer);
// }

uint16_t checksum(void *anything, int nbytes)
{
    register uint32_t sum = 0;
    uint16_t *ptr = anything;

    while (nbytes > 1)
    {
        sum += *ptr++;
        nbytes -= 2;
    }

    if (nbytes > 0)
        sum += *(uint8_t *)ptr;

    while (sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}
