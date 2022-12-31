NAME							:=	webserv

CXX								:=	clang++
CXXFLAGS					:=	-Wall -Wextra -Werror -std=c++98 -MMD -MP

PYTHON_DIR				:=	python/
PYTHON						:=	$(PYTHON_DIR)python3.8

SRCS_DIR					=		srcs/
SRCS							=		webserv.cpp \
											ft_util.cpp \
											Config.cpp \

SRCS_PREFIXED			=		$(addprefix $(SRCS_DIR), $(SRCS))

INCLUDES_DIR			= 	includes/
INCLUDES					= 	webserv.hpp \
											ft_util.hpp \
											ft_signal.hpp \
											Config.hpp \
											parse.hpp \
											server.hpp \

INCLUDES_PREFIXED	= $(addprefix $(INCLUDES_DIR), $(INCLUDES))

OBJS_DIR					=		objs/
OBJ								=		$(SRCS:.cpp=.o)
OBJS							=		$(addprefix $(OBJS_DIR), $(OBJ))
DEPENDENCIES			=		$(OBJS:.o=.d)

DEBUG_MODE				=		0

## pythonの取り方を変えたので不要
# ifeq ($(shell uname),Linux)
# 	OS_LINUX = 1
# else
# 	OS_LINUX = 0
# endif

all: $(NAME)

$(NAME): $(PYTHON) $(OBJS_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	$(CXX) $(CXXFLAGS) -D DEBUG_MODE=$(DEBUG_MODE) -c $< -o $@ -I$(INCLUDES_DIR)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

-include $(DEPENDENCIES)

debug:
	make DEBUG_MODE=1

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

$(PYTHON): $(PYTHON_DIR)
	cp `which python3.8` $(PYTHON_DIR)

$(PYTHON_DIR):
	mkdir -p $(PYTHON_DIR)

.PHONY: all clean fclean re debug debug_mode
