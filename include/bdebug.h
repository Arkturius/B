#if !defined (_B_DEBUG_H)
# define _B_DEBUG_H

# define XLIB_NO_PREFIX
# include <xlib.h>

# if defined (B_DEBUG)

#  define B_DBG_FILE	"tree.txt"

extern int	b_tree_depth;
extern int	b_tree_parent;
extern FILE	*b_tree_file;

typedef struct b_tree_node
{
	StringC	name;
	StringC	file;
	int		line;

	int		depth;
	int		parent;
	int		child;
	int		next;
	int		seq;
}	BTreeNode;

typedef struct b_tree_edge
{
	StringC	from;
	StringC	to;
	int		label;
	int		weight;
}	BTreeEdge;

x_array(BTreeNode, BTree);
x_array(BTreeEdge, BTreeEdges);

extern BTree	b_tree;

_constructor(102) void
B_debug_tree_start(void);

_destructor(102) void
B_debug_tree_stop(void);

void 
B_tree_close_node(int *parent_idx);

void
B_tree_add_node(StringC func, int line, StringC file);

#  define	B_DBG_TREE																	\
	__attribute__((cleanup(B_tree_close_node), unused))	int _parent_idx = b_tree_parent;\
	B_tree_add_node(__func__, __LINE__, __FILE__);										\

# endif // B_DEBUG

void
B_dwarf_statement(void);

# if !defined (B_DBG_TREE)
#  define B_DBG_TREE
# endif // B_DBG_TREE

#endif // _B_DEBUG_H
