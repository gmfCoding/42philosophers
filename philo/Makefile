NAME        = philo
BUILD_DIR   = .build
SRC_DIR     = ./

SRCS        =	main.c \
				init.c \
				atomic.c \
				state.c \
				time.c \
				utils.c \

SRCS        := $(SRCS:%=$(SRC_DIR)/%)
OBJS        = $(SRCS:$(SRC_DIR)/%.c=$(BUILD_DIR)/%.o)
DEPS        = $(OBJS:.o=.d)

CC          = cc
CFLAGS      = -Wall -Wextra -Werror -g3 $(DFLAGS)
CPPFLAGS    = $(addprefix -I,$(INCS)) -MMD -MP
LDFLAGS     = $(addprefix -L,$(dir $(LIBS_TARGET)))
LDLIBS      = $(addprefix -l,$(LIBS))

RM          = rm -f
DIR_DUP     = mkdir -p $(@D)

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LDLIBS) -o $(NAME) $(DFLAGS)

$(OBJS): $(BUILD_DIR)/%.o: $(SRC_DIR)/%.c $(BUILD_DIR)
	$(CC) $(CFLAGS) $(CPPFLAGS) -c -o $@ $<

$(BUILD_DIR):
	$(DIR_DUP)

# cleans only the project.
clean:
	$(RM) $(OBJS) $(DEPS)
	$(call print_clean,$(addsuffix \n,$(OBJS)))

# full clean, clean all objects and libraries and binaries
fclean: clean
	# for f in $(dir $(LIBS_TARGET)); do $(MAKE) -C $$f fclean; done
	$(RM) $(NAME)
	$(call print_fclean,$(NAME))

re: fclean all

-include $(DEPS)

.PHONY: re fclean clean lclean all

export DFLAGS

# COLORS
export GREEN = \033[1;32m
export YELLOW = \033[0;33m
export BLUE = \033[1;34m
export CYAN = \033[1;36m
export NC = \033[0m
