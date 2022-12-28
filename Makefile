NAME							:=	webserv

CXX								:=	clang++
CXXFLAGS					:=	-Wall -Wextra -Werror -std=c++98 -MMD -MP

PYTHON_DIR				:=	python/
PYTHON						:=	$(PYTHON_DIR)python3.8

SRCS_DIR					=		srcs/
SRCS							=		webserv.cpp \

SRCS_PREFIXED			=		$(addprefix $(SOURCES_FOLDER), $(SOURCES))

INCLUDES_DIR			= 	includes/
INCLUDES					= 	WebServ.hpp \

INCLUDES_PREFIXED	= $(addprefix $(INCLUDES_FOLDER), $(INCLUDES))


OBJS_DIR					=		objs/
OBJ								=		$(SRCS:.cpp=.o)
OBJS							=		$(addprefix $(OBJECTS_FOLDER), $(OBJECT))
DEPENDENCIES			=		$(OBJECTS:.o=.d)

DEBUG							=		0

ifeq ($(shell uname),Linux)
	OS_LINUX = 1
else
	OS_LINUX = 0
endif

all: $(NAME)

$(NAME): $(PYTHON) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: $(OBJS_DIR) $(SRCS_DIR)%.cpp
	$(CXX) $(CXXFLAGS) -D OS_LINUX=$(OS_LINUX) -D DEBUG=$(DEBUG) -c $< -o $@ -I$(INCLUDES_DIR)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

-include $(DEPENDENCIES)

debug: fclean
	DEBUG=1 make

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

$(PYTHON): $(PYTHON_DIR)
	cp `which python3.8` $(PYTHON_DIR)

$(PYTHON_DIR):
	mkdir -p $(PYTHON_DIR)

.PHONY: all clean fclean re 
