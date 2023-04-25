CC	=	gcc

NAME	=	src/Cherokee

FLAGS	=	--coverage -lcriterion -lsqlite3

LDFLAGS	=	-lsqlite3 -Wall -Wpedantic

SRC	=	src/main.c \
			src/lib/http_header_parser.c \
			src/http_server.c \
			src/lib/content_type.c \
			src/lib/container_implementation.c \
			src/lib/return_code.c \
			src/db/db_man.c

TEST	=	tests/test_container_implementation.c \
					tests/test_return_code.c \
					tests/test_content_type.c \
					tests/test_http_parse_header.c \
					

OBJ	=	$(SRC:.c=.o)

$(NAME):	$(OBJ)
			$(CC) -o $(NAME) $(OBJ) $(LDFLAGS)

all: 		$(NAME)

test:	$(TEST)
	$(CC) -o $@ $(TEST) $(FLAGS)

test_clean:
	rm -rf test
	rm -f test *.gcda *.gcno

test_re : test_clean test

clean:
			rm -f $(OBJ)

fclean:		clean
			rm -f ${NAME}

re:			fclean all

.PHONY:		all clean fclean rell