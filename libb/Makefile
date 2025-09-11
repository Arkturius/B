# 
# Makefile
#-------------------------------------------------------------------------------

NAME		:=	libb

SRCS_B		:=	$(wildcard *.b)
SRCS_ASM	:=	$(wildcard *.s)

OBJ_DIR		:=	build

OBJS		:=	$(addprefix $(OBJ_DIR)/, $(SRCS_ASM:%.s=%.o))
OBJS		+=	$(addprefix $(OBJ_DIR)/, $(SRCS_B:%.b=%.o))

all:	$(NAME)

$(NAME):	$(OBJS)
	@echo " ■  archiving	$@"
	@ar -rcs $^ -o $@.a

$(OBJ_DIR)/%.o:	%.s
	@mkdir -p $(@D)
	@gcc -m32 -x assembler $< -o $@

$(OBJ_DIR)/%.o:	%.b
	@mkdir -p $(@D)
	@./B $< > $<.s
	@gcc -m32 -x assembler $<.s -o $@
