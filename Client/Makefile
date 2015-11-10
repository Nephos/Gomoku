SRC	=	Player.cpp \
		Network.cpp \
		HttpRequest.cpp \
		Display.cpp \
		Obj.cpp \
		main.cpp

OBJ	=	$(SRC:.cpp=.o)

NAME=	client

CXXFLAGS += -Werror -Wextra -Wall -std=c++11 -I./include

LDXXFLAGS =	-lboost_system -lGL -lGLU -lX11 -lpthread

all: $(NAME)

$(NAME): $(OBJ)
	g++ -o $(NAME) $(LDXXFLAGS) $(OBJ)

debug:
	g++ $(SRC) $(LDXXFLAGS) $(CXXFLAGS) -g -o $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
