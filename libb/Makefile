# 
# Makefile
#-------------------------------------------------------------------------------

NAME		:=	libb

SRCS_B		:=	$(wildcard *.b)

OBJ_DIR		:=	build

OBJS		:=	$(addprefix $(OBJ_DIR)/, $(SRCS_B:%.b=%.o))

RM			:=	rm -rf
MKDIR		:=	mkdir -p

CC			:=	gcc
CFLAGS		:=	-c -m32 -x assembler

all:	$(NAME)

$(NAME):	$(OBJS)
	@$(MAKE) --silent --no-print-directory -C ..
	@echo " ■  archiving	$@"
	@ar -rcs $@.a $^

$(OBJ_DIR)/%.s:	%.b
	@$(MKDIR) $(@D)
	@.././B $< > $@

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.s
	@$(CC) $(CFLAGS) $< -o $@

clean:
	@if [ -d $(OBJ_DIR) ]; then \
		echo " ■  deleted	$(NAME)/$(OBJ_DIR)"; \
		$(RM) $(OBJ_DIR); \
	fi

fclean:		clean
	@$(RM) $(NAME)
	@echo " ■  deleted	$(NAME)"

re:			fclean all
