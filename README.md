
# IPv4/UDP headers + simple CHAP auth

## Features implemented

- Network communication with raw sockets
- IPV4 (de)serialization
- UDP (de)serialization
- Ring buffers (only at level 3)
- Simple CHAP authentication

## Run

```
nc -u -lp 14242
```

```
tcpdump udp -i lo -vvvv -s 0 -U -n
```

```
make && sudo ./chap
```

### Run server:

```
tar xvf mycharp_server.tgz
cd server/
make
./server 14242
```

## Missing

- validate received checksum
- ring buffer for received cmd interpreter (not only for raw packet reads)
- dynamic ip/ports
- auth timeouts
- support packet fragmentation

## Nice to have

- support ipv4 options
- support ipv6
- support tcp ;)

## Debug

https://hpd.gasmi.net/
