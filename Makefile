#
# Makefile
#-------------------------------------------------------------------------------

NAME		:=	B

COMP_SCRIPT	:=	bcomp.sh

SRCS		:=	bcompiler.c			\
				symbols.c			\
				scopes.c			\
				eval/declaration.c	\
				eval/control.c		\
				eval/expression.c	\
				eval/computing.c	\
				codegen/regalloc.c	\
				codegen/codegen.c	\
				codegen/emission.c	\

SRC_DIR		:=	src
INC_DIR		:=	include
OBJ_DIR		:=	build

SRCS		:=	$(addprefix $(SRC_DIR)/, $(SRCS))

OBJS 		:=	$(addprefix $(OBJ_DIR)/, $(SRCS:%.s=%.o))

RM			:=	rm -rf
MKDIR		:=	mkdir -p

ifndef LEXER_NAME
	LEXER_NAME	:= blexer
endif

LEXER_SRC	:=	$(SRC_DIR)/$(LEXER_NAME).l
LEXER		:=	$(OBJ_DIR)/$(LEXER_NAME).c

ifndef PARSER_NAME
	PARSER_NAME	:= bparser
endif

PARSER_SRC	:=	$(SRC_DIR)/$(PARSER_NAME).y
PARSER_OUT	:=	$(OBJ_DIR)/$(PARSER_NAME)
PARSER		:=	$(PARSER_OUT).c

LIBBDIR		:=	libb
LIBB		:=	libb.a

CFLAGS		:=	-Wall -Wextra -Wno-return-type -Wno-unused-parameter -Wno-override-init -Wno-array-bounds

ifeq ($(VERBOSE), 1)
	CFLAGS	+=	-DB_VERBOSE
endif

ifeq ($(DEBUG), 1)
	CFLAGS	+=	-DB_DEBUG -DB_DUMP_ALL
endif

all:		$(NAME)

nix:
	@nix develop

$(NAME):	$(PARSER) $(LEXER) $(SRCS)
	@echo " ■  building	$@"
	@$(CC) $(CFLAGS) $^ -o $@ -I$(INC_DIR) -I$(OBJ_DIR) -g

parser:		$(PARSER)

$(PARSER):	$(PARSER_SRC)
	@mkdir -p $(@D)
	@echo " ■  building	parser"
	@bison $< -Wcounterexamples -v --output=$(PARSER) --header=$(PARSER_OUT).h

lexer:		$(LEXER)

$(LEXER):	$(LEXER_SRC)
	@mkdir -p $(@D)
	@echo " ■  building	lexer"
	@flex --outfile=$(LEXER) $< 

libb:		$(LIBB)

$(LIBB):	$(NAME)
	@echo " ■  compiling	$@"
	@$(MAKE) --no-print-directory -C $(LIBBDIR)

clean:
	@$(MAKE) --no-print-directory -C $(LIBBDIR) clean
	@if [ -d $(OBJ_DIR) ]; then \
		echo " ■  deleted	$(OBJ_DIR)"; \
		$(RM) $(OBJ_DIR); \
	fi

fclean:			clean
	@$(RM) $(NAME)
	@echo " ■  deleted	$(NAME)"
	@$(MAKE) --no-print-directory -C $(LIBBDIR) fclean

re:					fclean all

BOLD			=	\033[1m
ITALIC			=	\033[3m

RED				=	\033[31m
GREEN			=	\033[32m
YELLOW			=	\033[33m
CYAN			=	\033[36m
GRAY			=	\033[90m

RESET			=	\033[0m
