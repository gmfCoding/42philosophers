NAME        = philo
BUILD_DIR   = .build
SRC_DIR     = ./
INCS		= ./


SRCS        =	main.c \
				init.c \
				atomic.c \
				state.c \
				time.c \
				utils.c \

SRCS        := $(SRCS:%=$(SRC_DIR)/%)
OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS        = $(OBJS:.o=.d)

OPFLAGS = -Ofast
#-flto -march=native

DFLAGS = $(OPFLAGS) #-fsanitize=addresse -g3

CC          = cc
CFLAGS      = -Wall -Wextra -Werror  $(DFLAGS)
CPPFLAGS    = $(addprefix -I,$(INCS)) -MMD -MP

RM          = rm -f

all: $(NAME)

$(NAME): $(OBJS)
	@printf "${BLUE}"
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) $(DFLAGS) -o $(NAME) 

$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	@printf "${GREEN}"
	$(CC) $(CFLAGS) $(CPPFLAGS) -c $< -o $@

$(BUILD_DIR):
	@printf "${CYAN}"
	mkdir -p $(BUILD_DIR)

# cleans only the project.
clean:
	@printf "${RED}"
	$(RM) $(OBJS) $(DEPS)

# full clean, clean all objects and binaries
fclean: clean
	@printf "${RED}"
	$(RM) $(NAME)

re: fclean all

-include $(DEPS)

.PHONY: re fclean clean all

export DFLAGS

# COLORS
export RED = \033[0;31m
export GREEN = \033[0;32m
export YELLOW = \033[0;33m
export BLUE = \033[0;34m
export CYAN = \033[0;36m
export NC = \033[0m
