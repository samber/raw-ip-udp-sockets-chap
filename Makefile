
NAME = chap

SRC = socket.c lvl4_udp.c lvl3_ipv4.c checksum.c main.c datagram.c utils.c lvl4.c protocol.c auth.c sha256.c
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
