# 
# Makefile
#-------------------------------------------------------------------------------

NAME		:=	libb

SRCS_B		:=	$(wildcard *.b)

OBJ_DIR		:=	build

OBJS		:=	$(addprefix $(OBJ_DIR)/, $(SRCS_B:%.b=%.o))

all:	$(NAME)

$(NAME):	$(OBJS)
	@make -C .. --no-print-directory
	@echo " ■  archiving	$@"
	@ar -rcs $@.a $^

$(OBJ_DIR)/%.s:	%.b
	@mkdir -p $(@D)
	@.././B $< > $@

$(OBJ_DIR)/%.o: $(OBJ_DIR)/%.s
	@gcc -c -m32 -x assembler $< -o $@

clean:
	@rm -rf $(NAME).a

fclean:		clean
	@rm -rf $(OBJ_DIR)

re:			fclean all
