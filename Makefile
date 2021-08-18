NAME			= 21NG1NY

SRCS_LIST	= \
				main.cpp \
				GNLHandler.cpp \
				CNFPath.cpp \
				RQTHandler.cpp \
				CNFHandler.cpp \
				CGIHandler.cpp \
				CNTHandler.cpp \
				Request.cpp \
				Response.cpp \
				Server.cpp \
				Base64.cpp \
				FHandler.cpp \
				SRVstd.cpp \
				MIMEs.cpp \
				SRVcds.cpp \
				Utils.cpp

SRCS_FOLDER		= srcs
CGI_FOLDER		= cgi-bin

SRCS			= $(addprefix ${SRCS_FOLDER}/, ${SRCS_LIST})

OBJS			= ${SRCS:.cpp=.o}

INCLUDES		= -I includes

CC				= clang++
CFLAGS 			= -Wall -Wextra -Werror -std=c++98 -pthread
RM				= rm -f

LINK			= ln -sf
PY				= $(shell which python3)
SH				= $(shell which sh)
# PHP			= $(shell which php)

all:			$(NAME)

$(NAME):		$(OBJS)
				$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)
				$(LINK) $(PY) $(CGI_FOLDER)/py_cgi
				$(LINK) $(SH) $(CGI_FOLDER)/sh_cgi
				clear && curl -L git\.io/unix

%.o: %.cpp
				${CC} ${CFLAGS} $(INCLUDES) -o $@ -c $<

clean:
				${RM} ${OBJS}

fclean:			clean
				${RM} ${NAME}
				${RM} $(CGI_FOLDER)/py_cgi
				${RM} $(CGI_FOLDER)/sh_cgi

re:				fclean all

.PHONY: 		all fclean clean re