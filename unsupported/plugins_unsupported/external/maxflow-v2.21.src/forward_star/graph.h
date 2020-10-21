/* graph.h */
/*
	This software library implements the maxflow algorithm
	described in

		An Experimental Comparison of Min-Cut/Max-Flow Algorithms
		for Energy Minimization in Vision.
		Yuri Boykov and Vladimir Kolmogorov.
		In IEEE Transactions on Pattern Analysis and Machine Intelligence (PAMI), 
		September 2004

	This algorithm was developed by Yuri Boykov and Vladimir Kolmogorov
	at Siemens Corporate Research. To make it available for public use,
	it was later reimplemented by Vladimir Kolmogorov based on open publications.

	If you use this software for research purposes, you should cite
	the aforementioned paper in any resulting publication.

	----------------------------------------------------------------

	For description, license, example usage, discussion of graph representation	and memory usage see README.TXT.
*/

#ifndef __GRAPH_H__
#define __GRAPH_H__

#include "block.h"

/*
	Nodes, arcs and pointers to nodes are
	added in blocks for memory and time efficiency.
	Below are numbers of items in blocks
*/
#define NODE_BLOCK_SIZE 512
#define ARC_BLOCK_SIZE 1024
#define NODEPTR_BLOCK_SIZE 128

class Graph
{
public:
	typedef enum
	{
		SOURCE	= 0,
		SINK	= 1
	} termtype; /* terminals */

	/* Type of edge weights.
	   Can be changed to char, int, float, double, ... */
	typedef short captype;
	/* Type of total flow */
	typedef int flowtype;

	typedef void * node_id;

	/* interface functions */

	/* Constructor. Optional argument is the pointer to the
	   function which will be called if an error occurs;
	   an error message is passed to this function. If this
	   argument is omitted, exit(1) will be called. */
	Graph(void (*err_function)(char *) = NULL);

	/* Destructor */
	~Graph();

	/* Adds a node to the graph */
	node_id add_node();

	/* Adds a bidirectional edge between 'from' and 'to'
	   with the weights 'cap' and 'rev_cap' */
	void add_edge(node_id from, node_id to, captype cap, captype rev_cap);

	/* Sets the weights of the edges 'SOURCE->i' and 'i->SINK'
	   Can be called at most once for each node before any call to 'add_tweights'.
	   Weights can be negative */
	void set_tweights(node_id i, captype cap_source, captype cap_sink);

	/* Adds new edges 'SOURCE->i' and 'i->SINK' with corresponding weights
	   Can be called multiple times for each node.
	   Weights can be negative */
	void add_tweights(node_id i, captype cap_source, captype cap_sink);

	/* After the maxflow is computed, this function returns to which
	   segment the node 'i' belongs (Graph::SOURCE or Graph::SINK) */
	termtype what_segment(node_id i);

	/* Computes the maxflow. Can be called only once. */
	flowtype maxflow();

/***********************************************************************/
/***********************************************************************/
/***********************************************************************/
	
private:
	/* internal variables and functions */

	struct arc_forward_st;
	struct arc_reverse_st;

#define IS_ODD(a) ((int)(a) & 1)
#define MAKE_ODD(a)  ((arc_forward *) ((int)(a) | 1))
#define MAKE_EVEN(a) ((arc_forward *) ((int)(a) & (~1)))
#define MAKE_ODD_REV(a)  ((arc_reverse *) ((int)(a) | 1))
#define MAKE_EVEN_REV(a) ((arc_reverse *) ((int)(a) & (~1)))

	/* node structure */
	typedef struct node_st
	{
		/*
			Usually i->first_out is the first outgoing
			arc, and (i+1)->first_out-1 is the last outgoing arc.
			However, it is not always possible, since
			arcs are allocated in blocks, so arcs corresponding
			to two consecutive nodes may be in different blocks.

			If outgoing arcs for i are last in the arc block,
			then a different mechanism is used. i->first_out
			is odd in this case; the first outgoing arc
			is (a+1), and the last outgoing arc is
			((arc_forward *)(a->shift))-1, where
			a = (arc_forward *) (((char *)(i->first_out)) + 1);

			Similar mechanism is used for incoming arcs.
		*/
		arc_forward_st	*first_out;	/* first outcoming arc */
		arc_reverse_st	*first_in;	/* first incoming arc */

		arc_forward_st	*parent;	/* describes node's parent
									   if IS_ODD(parent) then MAKE_EVEN(parent) points to 'arc_reverse',
									   otherwise parent points to 'arc_forward' */

		node_st			*next;		/* pointer to the next active node
									   (or to itself if it is the last node in the list) */

		int				TS;			/* timestamp showing when DIST was computed */
		int				DIST;		/* distance to the terminal */
		short			is_sink;	/* flag showing whether the node is in the source or in the sink tree */

		captype			tr_cap;		/* if tr_cap > 0 then tr_cap is residual capacity of the arc SOURCE->node
									   otherwise         -tr_cap is residual capacity of the arc node->SINK */
	} node;

	/* arc structures */
#define NEIGHBOR_NODE(i, shift) ((node *) ((char *)(i) + (shift)))
#define NEIGHBOR_NODE_REV(i, shift) ((node *) ((char *)(i) - (shift)))
	typedef struct arc_forward_st
	{
		int				shift;		/* node_to = NEIGHBOR_NODE(node_from, shift) */
		captype			r_cap;		/* residual capacity */
		captype			r_rev_cap;	/* residual capacity of the reverse arc*/
	} arc_forward;

	typedef struct arc_reverse_st
	{
		arc_forward		*sister;	/* reverse arc */
	} arc_reverse;

	/* 'pointer to node' structure */
	typedef struct nodeptr_st
	{
		node_st			*ptr;
		nodeptr_st		*next;
	} nodeptr;

	typedef struct node_block_st
	{
		node					*current;
		struct node_block_st	*next;
		node					nodes[NODE_BLOCK_SIZE];
	} node_block;

#define last_node LAST_NODE.LAST_NODE

	typedef struct arc_for_block_st
	{
		char					*start;		/* the actual start address of this block.
											   May be different from 'this' since 'this'
											   must be at an even address. */
		arc_forward				*current;
		struct arc_for_block_st	*next;
		arc_forward				arcs_for[ARC_BLOCK_SIZE]; /* all arcs must be at even addresses */
		union
		{
			arc_forward			dummy;
			node				*LAST_NODE;	/* used in graph consruction */
		}						LAST_NODE;
	} arc_for_block;

	typedef struct arc_rev_block_st
	{
		char					*start;		/* the actual start address of this block.
											   May be different from 'this' since 'this'
											   must be at an even address. */
		arc_reverse				*current;
		struct arc_rev_block_st	*next;
		arc_reverse				arcs_rev[ARC_BLOCK_SIZE]; /* all arcs must be at even addresses */
		union
		{
			arc_reverse			dummy;
			node				*LAST_NODE;	/* used in graph consruction */
		}						LAST_NODE;
	} arc_rev_block;

	node_block			*node_block_first;
	arc_for_block		*arc_for_block_first;
	arc_rev_block		*arc_rev_block_first;
	DBlock<nodeptr>		*nodeptr_block;

	void	(*error_function)(char *);	/* this function is called if a error occurs,
										   with a corresponding error message
										   (or exit(1) is called if it's NULL) */

	flowtype			flow;		/* total flow */

/***********************************************************************/

	node				*queue_first[2], *queue_last[2];	/* list of active nodes */
	nodeptr				*orphan_first, *orphan_last;		/* list of pointers to orphans */
	int					TIME;								/* monotonically increasing global counter */

/***********************************************************************/

	/* functions for processing active list */
	void set_active(node *i);
	node *next_active();

	void prepare_graph();
	void maxflow_init();
	void augment(node *s_start, node *t_start, captype *cap_middle, captype *rev_cap_middle);
	void process_source_orphan(node *i);
	void process_sink_orphan(node *i);
};

#endif
