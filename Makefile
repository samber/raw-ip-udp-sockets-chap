
NAME = chap

SRC = src/socket.c src/lvl4_udp.c src/lvl3_ipv4.c src/checksum.c src/main.c src/datagram.c src/utils.c src/lvl4.c src/protocol.c src/auth.c src/sha256.c
OBJ = $(SRC:.c=.o)

CFLAGS = -W -Wall -Werror -Wextra -I ./include -lssl -lcrypto

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) -o $(NAME) $(OBJ) ${CFLAGS}

clean:
	rm -rf $(OBJ)

fclean: clean
	rm -rf $(NAME)

re: fclean all
