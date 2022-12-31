NAME							:=	webserv

CXX								:=	clang++
CXXFLAGS					:=	-Wall -Wextra -Werror -std=c++98 -MMD -MP

PYTHON_DIR				:=	python/
PYTHON						:=	$(PYTHON_DIR)python3.8

SRCS_DIR					=		srcs/
SRCS							=		$(shell cd $(SRCS_DIR); find *.cpp)

SRCS_PREFIXED			=		$(addprefix $(SRCS_DIR), $(SRCS))

INCLUDES_DIR			= 	includes/
INCLUDES					= 	$(shell cd $(INCLUDES_DIR); find *.hpp)

INCLUDES_PREFIXED	= $(addprefix $(INCLUDES_DIR), $(INCLUDES))

OBJS_DIR					=		objs/
OBJ								=		$(SRCS:.cpp=.o)
OBJS							=		$(addprefix $(OBJS_DIR), $(OBJ))
DEPENDENCIES			=		$(OBJS:.o=.d)

DEBUG_MODE				=		0

ifeq ($(shell uname),Linux)
	OS_LINUX = 1
else
	OS_LINUX = 0
endif

all: $(NAME)

$(NAME): $(PYTHON) $(OBJS_DIR) $(OBJS)
	$(CXX) $(CXXFLAGS) $(OBJS) -o $(NAME)

$(OBJS_DIR)%.o: $(SRCS_DIR)%.cpp
	$(CXX) $(CXXFLAGS) -D DEBUG_MODE=$(DEBUG_MODE) -c $< -o $@ -I$(INCLUDES_DIR)

$(OBJS_DIR):
	mkdir -p $(OBJS_DIR)

-include $(DEPENDENCIES)

debug:
	make re DEBUG_MODE=1

clean:
	rm -rf $(OBJS_DIR)

fclean: clean
	rm -rf $(NAME)

re: fclean all

$(PYTHON): $(PYTHON_DIR)
ifeq ($(OS_LINUX), 1)
	cp python/python3.8_Linux $@
else
	cp python/python3.8_Mac $@
endif


$(PYTHON_DIR):
	mkdir -p $(PYTHON_DIR)

.PHONY: all clean fclean re debug debug_mode
