/**
* bcompiler.c
*/

#include <libgen.h>
#include <stdarg.h>

#include <b.h>

extern int		yylex();
extern int  	yylex_destroy(void);
extern int  	yyparse();
extern FILE 	*yyin;

static StringC	executable = NULL;
static StringC	input_file = NULL;
Compiler		B = {0};

#if defined(B_DEBUG)

int		b_tree_depth  = 0;
int		b_tree_parent = -1;
FILE	*b_tree_file  = NULL;
BTree	b_tree        = {0};

x_array(int,		ints);
x_array(long,		longs);
x_array(StringC,	StringCs);

ints		b_tree_ancestors  = {0};
ints		b_tree_weight     = {0};
BTreeEdges	b_tree_edges      = {0};
StringCs	b_tree_func_names = {0};

_constructor(102) void
B_debug_tree_start(void)
{
	b_tree_depth = 0;
	b_tree_file = fopen(B_DBG_FILE, "w");
	assert(b_tree_file && "failed to open debug file.");
}

_destructor(102) void
B_debug_tree_stop(void)
{
	fflush(stdout);

	int max_len = 0;

	arr_foreach(BTreeNode, node, b_tree)
	{
		int	curr_len = strlen(basename((String)node->file)) + 2;

		for (int i = node->line; i; ++curr_len, i /= 10);

		max_len = max(max_len, curr_len);
		node->file = basename((String)node->file);
	}

	arr_foreach(BTreeNode, node, b_tree)
	{
		int	pop = arr_count(b_tree_ancestors) - node->depth;
		bool found = false;

		arr_foreach(StringC, comp_name, b_tree_func_names)
		{
			if (strcmp(*comp_name, node->name))
				continue ;
			found = true;
		}
		if (!found)
			arr_append(b_tree_func_names, node->name);

		if (pop >= 0)
			arr_pop(b_tree_ancestors, (uint32_t)pop);
		fprintf(b_tree_file, "\033[90m%*s \033[0m", max_len, node->file);
		for (int i = 1; i < node->depth; ++i)
		{
			int ancestor = *arr_nth(b_tree_ancestors, i);
			if (arr_nth(b_tree, ancestor)->next == -1)
				fprintf(b_tree_file, "    ");
			else
				fprintf(b_tree_file, "│   ");
		}

		StringC	color = (
		{
			StringC	colors[5] = {"\033[31m", "\033[33m", "\033[32m", "\033[34m", "\033[35m"};
			int		idx = 0;

			switch (*node->name)
			{
				case 'B': idx = 0; break;
				case 'C': idx = 1; break;
				case 'A': idx = 2; break;
				case 'E': idx = 3; break;
				case 'R': idx = 4; break;
				default: unreachable("skill issued");
			}
			colors[idx];
		});

		if (node->depth)
		{
			if (node->next == -1)
				fprintf(b_tree_file, "└─> %s%s\n", color, node->name);
			else
				fprintf(b_tree_file, "├─> %s%s\n", color, node->name);
		}
		else
			fprintf(b_tree_file, "@ %s%s\n", color, node->name);
		fprintf(b_tree_file, "\033[0m");
		arr_append(b_tree_ancestors, arr_index(b_tree, node));
	}

	fclose(b_tree_file);

	b_tree_file = fopen("tree.dot", "w");
	assert(b_tree_file);

	fprintf(b_tree_file, "digraph CallGraph {\n");
	fprintf(b_tree_file, "  rankdir=LR;\n\n");
	arr_foreach(StringC, name, b_tree_func_names)
	{
		StringC	color = (
		{
			StringC	colors[5] = {"blue", "red", "green", "yellow", "purple"};
			int		idx = 0;

			switch (**name)
			{
				case 'B': idx = 0; break;
				case 'C': idx = 1; break;
				case 'A': idx = 2; break;
				case 'E': idx = 3; break;
				case 'R': idx = 4; break;
				default: unreachable("skill issued");
			}
			colors[idx];
		});
		fprintf(b_tree_file, "  \"%s\" [label=\"%s\", color=\"%s\", shape=box]\n", *name, *name, color);
	}
	fprintf(b_tree_file, "\n");
	arr_foreach(BTreeNode, node, b_tree)
	{
		if (node->parent != -1)
		{
			BTreeNode	*parent = arr_nth(b_tree, node->parent);
			BTreeEdge	edge = 
			{
				.from = parent->name,
				.to = node->name,
				.label = parent->line,
				.weight = 1,
			};
			BTreeEdge	*it;

			bool	present = false;
			arr_foreach(BTreeEdge, curr_edge, b_tree_edges)
			{
				if (strcmp(curr_edge->from, parent->name))
					continue ;
				if (strcmp(curr_edge->to, node->name))
					continue ;
				it = curr_edge;
				present = true;
			}
			if (present)
			{
				it->weight++;
				continue ;
			}
			arr_append(b_tree_edges, edge);
		}
	}

	arr_foreach(BTreeEdge, edge, b_tree_edges)
		fprintf(b_tree_file, "  \"%s\" -> \"%s\" [weight=%d]\n", edge->from, edge->to, edge->weight);

	fprintf(b_tree_file, "}\n");
	fclose(b_tree_file);

	arr_destroy(b_tree);
}

void 
B_tree_close_node(int *parent_idx)
{
	b_tree_depth--;
	b_tree_parent = *parent_idx;
}

void
B_tree_add_node(StringC func, int line, StringC file)
{
	BTreeNode	node = 
	{
		.name = func,
		.line = line,
		.file = file,
		.depth = b_tree_depth++,
		.parent = b_tree_parent,
		.child = -1,
		.next = -1,
	};
	
	int current_idx = arr_count(b_tree);

	if (b_tree_parent != -1)
	{
		BTreeNode	*parent = arr_nth(b_tree, b_tree_parent);

		if (parent->child == -1)
			parent->child = current_idx;
		else
		{
			int			child = parent->child;
			BTreeNode	*last_child;

			do 
			{
				last_child = arr_nth(b_tree, child);
				child = last_child->next;
			}
			while (child != -1);
			last_child->next = current_idx;
		}
	}
// 	if (arr_count(b_tree) == 256)
// 		breakpoint ;
	arr_append(b_tree, node);
	b_tree_parent = current_idx;
}

#endif // B_DEBUG

_constructor(101) bool
B_compiler_start()
{
	B_DBG_TREE;

	arr_reserve(B.symbols, 256);
	arr_reserve(B.scopes,  256);
}

_destructor(101) void
B_compiler_stop()
{
	B_DBG_TREE;

	static bool	dead = false;

	if (dead)
		return ;
	dead = true;

	arr_destroy(B.symbols);
	arr_destroy(B.internals);
	arr_destroy(B.scopes);
	arr_destroy(B.rostrings);
}

bool
B_compiler_run(void)
{
	B_DBG_TREE;

	B.input_file = (String) input_file;
	if (B.input_file)
	{
		B.directory = realpath(input_file, B.directory);
		B.directory[strlen(B.directory) - strlen(input_file) - 1] = 0;
	}

	bool ret = yyparse();

	fclose(yyin);
	yylex_destroy();

	B_symbol_table_dump();
	return (ret);
}

_noreturn void
B_compiler_error(StringC reason)
{
	B_DBG_TREE;

	exit(1);
	todo("%s: BETTER ERROR HANDLING", reason);

	StringC	line = b_row_start ? b_row_start : yytext;

	yytext[yyleng] = b_hold_char;

	StringC	nl = strchr(line, '\n');

	if (!nl)
		nl = line + strlen(line);

	log("\033[1m%s:%d:%d:\033[0m ", input_file, yylineno, b_col - (yyleng - 1));
	log(" %4d | %.*s", yylineno, (int)(nl - line), line);
	
	String	waves = malloc(b_col - yyleng + 1);
	if (!waves)
		exit(42069);
	
	memset(waves, '~', b_col - yyleng);
	waves[b_col - yyleng - 1] = 0;

	log("      | %*.s^\033[35;1m%s\033[0m", (int)b_col - 1, "", waves);

	todo("%s: %s", __func__, reason);
	B_compiler_stop();
	exit(1);
}

StringC
B_asprintf(StringC fmt, ...)
{
	char	buffer[2048] = {0};
	va_list	ap;

	va_start(ap, fmt);
	vsnprintf(buffer, sizeof(buffer) - 1, fmt, ap);
	va_end(ap);

	return (strdup(buffer));
}

int
yyerror(StringC reason)
{
	B_compiler_error(reason);
	return (1);
}

int
main(int argc, char **argv, char **envp)
{
	executable = shift_args(argc, argv);
	if (argc)
		input_file = shift_args(argc, argv);
	
	log("INPUT = %s", input_file);
	
	yyin = input_file ? fopen(input_file, "r") : stdin;

	if (!yyin)
		B_compiler_error("No such file or directory.");

	if (B_compiler_run())
		return (1);
	return (0);
}
