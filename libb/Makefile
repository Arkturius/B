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

$(OBJ_DIR)/%.o:	%.b
	@mkdir -p $(@D)
	@.././B $< > $<.s
	@gcc -c -m32 -x assembler $<.s -o $@
