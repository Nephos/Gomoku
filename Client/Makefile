SRC	=	Player.cpp \
		Network.cpp \
		HttpRequest.cpp \
		Display.cpp \
		Obj.cpp \
		Human.cpp \
		Computer.cpp \
		ComputerMinMax.cpp \
		main.cpp

# CXX	=	clang++

OBJ	=	$(SRC:.cpp=.o)

NAME	=	client

CXXFLAGS +=	-Wextra -Wall -std=c++11 -I./include

LDXXFLAGS =	-lboost_system -lGL -lGLU -lX11 -lpthread `imlib2-config --libs`

all: $(NAME)

$(NAME): $(OBJ)
	$(CXX) -o $(NAME) $(LDXXFLAGS) $(OBJ)

opti: CXXFLAGS += -O3
opti: $(NAME)

debug: CXXFLAGS += -D DEBUG -g
debug: $(NAME)

clean:
	$(RM) $(OBJ)

fclean: clean
	$(RM) $(NAME)

re: fclean all

.PHONY: all clean fclean re debug
