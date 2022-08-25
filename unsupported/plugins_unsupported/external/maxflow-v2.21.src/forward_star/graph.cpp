/* graph.cpp */


#include <stdio.h>
#include "graph.h"

Graph::Graph(void (*err_function)(char *))
{
	error_function = err_function;
	node_block_first = NULL;
	arc_for_block_first = NULL;
	arc_rev_block_first = NULL;
	flow = 0;
}

Graph::~Graph()
{
	while (node_block_first)
	{
		node_block *next = node_block_first -> next;
		delete node_block_first;
		node_block_first = next;
	}

	while (arc_for_block_first)
	{
		arc_for_block *next = arc_for_block_first -> next;
		delete arc_for_block_first -> start;
		arc_for_block_first = next;
	}

	while (arc_rev_block_first)
	{
		arc_rev_block *next = arc_rev_block_first -> next;
		delete arc_rev_block_first -> start;
		arc_rev_block_first = next;
	}
}

Graph::node_id Graph::add_node()
{
	node *i;

	if (!node_block_first || node_block_first->current+1 > &node_block_first->nodes[NODE_BLOCK_SIZE-1])
	{
		node_block *next = node_block_first;
		node_block_first = (node_block *) new node_block;
		if (!node_block_first) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
		node_block_first -> current = & ( node_block_first -> nodes[0] );
		node_block_first -> next = next;
	}

	i = node_block_first -> current ++;
	i -> first_out = (arc_forward *) 0;
	i -> first_in = (arc_reverse *) 0;

	i -> tr_cap = 0;

	return (node_id) i;
}

void Graph::add_edge(node_id from, node_id to, captype cap, captype rev_cap)
{
	arc_forward *a_for;
	arc_reverse *a_rev;

	if (!arc_for_block_first || arc_for_block_first->current+1 > &arc_for_block_first->arcs_for[ARC_BLOCK_SIZE])
	{
		arc_for_block *next = arc_for_block_first;
		char *ptr = new char[sizeof(arc_for_block)+1];
		if (!ptr) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
		if ((int)ptr & 1) arc_for_block_first = (arc_for_block *) (ptr + 1);
		else              arc_for_block_first = (arc_for_block *) ptr;
		arc_for_block_first -> start = ptr;
		arc_for_block_first -> current = & ( arc_for_block_first -> arcs_for[0] );
		arc_for_block_first -> next = next;
	}

	if (!arc_rev_block_first || arc_rev_block_first->current+1 > &arc_rev_block_first->arcs_rev[ARC_BLOCK_SIZE])
	{
		arc_rev_block *next = arc_rev_block_first;
		char *ptr = new char[sizeof(arc_rev_block)+1];
		if (!ptr) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
		if ((int)ptr & 1) arc_rev_block_first = (arc_rev_block *) (ptr + 1);
		else              arc_rev_block_first = (arc_rev_block *) ptr;
		arc_rev_block_first -> start = ptr;
		arc_rev_block_first -> current = & ( arc_rev_block_first -> arcs_rev[0] );
		arc_rev_block_first -> next = next;
	}

	a_for = arc_for_block_first -> current ++;
	a_rev = arc_rev_block_first -> current ++;

	a_rev -> sister = (arc_forward *) from;
	a_for -> shift  = (int) to;
	a_for -> r_cap = cap;
	a_for -> r_rev_cap = rev_cap;

	((node *)from) -> first_out =
		(arc_forward *) ((int)(((node *)from) -> first_out) + 1);
	((node *)to) -> first_in =
		(arc_reverse *) ((int)(((node *)to) -> first_in) + 1);
}

void Graph::set_tweights(node_id i, captype cap_source, captype cap_sink)
{
	flow += (cap_source < cap_sink) ? cap_source : cap_sink;
	((node*)i) -> tr_cap = cap_source - cap_sink;
}

void Graph::add_tweights(node_id i, captype cap_source, captype cap_sink)
{
	register captype delta = ((node*)i) -> tr_cap;
	if (delta > 0) cap_source += delta;
	else           cap_sink   -= delta;
	flow += (cap_source < cap_sink) ? cap_source : cap_sink;
	((node*)i) -> tr_cap = cap_source - cap_sink;
}

/*
	Converts arcs added by 'add_edge()' calls
	to a forward star graph representation.

	Linear time algorithm.
	No or little additional memory is allocated
	during this process
	(it may be necessary to allocate additional
	arc blocks, since arcs corresponding to the
	same node must be contiguous, i.e. be in one
	arc block.)
*/
void Graph::prepare_graph()
{
	node *i;
	arc_for_block *ab_for, *ab_for_first;
	arc_rev_block *ab_rev, *ab_rev_first, *ab_rev_scan;
	arc_forward *a_for;
	arc_reverse *a_rev, *a_rev_scan, a_rev_tmp;
	node_block *nb;
	bool for_flag = false, rev_flag = false;
	int k;

	if (!arc_rev_block_first)
	{
		node_id from = add_node(), to = add_node();
		add_edge(from, to, 1, 0);
	}

	/* FIRST STAGE */
	a_rev_tmp.sister = NULL;
	for (a_rev=arc_rev_block_first->current; a_rev<&arc_rev_block_first->arcs_rev[ARC_BLOCK_SIZE]; a_rev++)
	{
		a_rev -> sister = NULL;
	}

	ab_for = ab_for_first = arc_for_block_first;
	ab_rev = ab_rev_first = ab_rev_scan = arc_rev_block_first;
	a_for = &ab_for->arcs_for[0];
	a_rev = a_rev_scan = &ab_rev->arcs_rev[0];

	for (nb=node_block_first; nb; nb=nb->next)
	{
		for (i=&nb->nodes[0]; i<nb->current; i++)
		{
			/* outgoing arcs */
			k = (int) i -> first_out;
			if (a_for + k > &ab_for->arcs_for[ARC_BLOCK_SIZE])
			{
				if (k > ARC_BLOCK_SIZE) { if (error_function) (*error_function)("# of arcs per node exceeds block size!"); exit(1); }
				if (for_flag) ab_for = NULL;
				else          { ab_for = ab_for -> next; ab_rev_scan = ab_rev_scan -> next; }
				if (ab_for == NULL)
				{
					arc_for_block *next = arc_for_block_first;
					char *ptr = new char[sizeof(arc_for_block)+1];
					if (!ptr) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
					if ((int)ptr & 1) arc_for_block_first = (arc_for_block *) (ptr + 1);
					else              arc_for_block_first = (arc_for_block *) ptr;
					arc_for_block_first -> start = ptr;
					arc_for_block_first -> current = & ( arc_for_block_first -> arcs_for[0] );
					arc_for_block_first -> next = next;
					ab_for = arc_for_block_first;
					for_flag = true;
				}
				else a_rev_scan = &ab_rev_scan->arcs_rev[0];
				a_for = &ab_for->arcs_for[0];
			}
			if (ab_rev_scan)
			{
				a_rev_scan += k;
				i -> parent = (arc_forward *) a_rev_scan;
			}
			else i -> parent = (arc_forward *) &a_rev_tmp;
			a_for += k;
			i -> first_out = a_for;
			ab_for -> last_node = i;

			/* incoming arcs */
			k = (int) i -> first_in;
			if (a_rev + k > &ab_rev->arcs_rev[ARC_BLOCK_SIZE])
			{
				if (k > ARC_BLOCK_SIZE) { if (error_function) (*error_function)("# of arcs per node exceeds block size!"); exit(1); }
				if (rev_flag) ab_rev = NULL;
				else          ab_rev = ab_rev -> next;
				if (ab_rev == NULL)
				{
					arc_rev_block *next = arc_rev_block_first;
					char *ptr = new char[sizeof(arc_rev_block)+1];
					if (!ptr) { if (error_function) (*error_function)("Not enough memory!"); exit(1); }
					if ((int)ptr & 1) arc_rev_block_first = (arc_rev_block *) (ptr + 1);
					else              arc_rev_block_first = (arc_rev_block *) ptr;
					arc_rev_block_first -> start = ptr;
					arc_rev_block_first -> current = & ( arc_rev_block_first -> arcs_rev[0] );
					arc_rev_block_first -> next = next;
					ab_rev = arc_rev_block_first;
					rev_flag = true;
				}
				a_rev = &ab_rev->arcs_rev[0];
			}
			a_rev += k;
			i -> first_in = a_rev;
			ab_rev -> last_node = i;
		}
		/* i is the last node in block */
		i -> first_out = a_for;
		i -> first_in  = a_rev;
	}

	/* SECOND STAGE */
	for (ab_for=arc_for_block_first; ab_for; ab_for=ab_for->next)
	{
		ab_for -> current = ab_for -> last_node -> first_out;
	}

	for ( ab_for=ab_for_first, ab_rev=ab_rev_first;
		  ab_for;
		  ab_for=ab_for->next, ab_rev=ab_rev->next )
	for ( a_for=&ab_for->arcs_for[0], a_rev=&ab_rev->arcs_rev[0];
		  a_for<&ab_for->arcs_for[ARC_BLOCK_SIZE];
		  a_for++, a_rev++ )
	{
		arc_forward *af;
		arc_reverse *ar;
		node *from;
		int shift = 0, shift_new;
		captype r_cap, r_rev_cap, r_cap_new, r_rev_cap_new;

		if (!(from=(node *)(a_rev->sister))) continue;
		af = a_for;
		ar = a_rev;

		do
		{
			ar -> sister = NULL;

			shift_new = ((char *)(af->shift)) - (char *)from;
			r_cap_new = af -> r_cap;
			r_rev_cap_new = af -> r_rev_cap;
			if (shift)
			{
				af -> shift = shift;
				af -> r_cap = r_cap;
				af -> r_rev_cap = r_rev_cap;
			}
			shift = shift_new;
			r_cap = r_cap_new;
			r_rev_cap = r_rev_cap_new;

			af = -- from -> first_out;
			if ((arc_reverse *)(from->parent) != &a_rev_tmp)
			{
				from -> parent = (arc_forward *)(((arc_reverse *)(from -> parent)) - 1);
				ar = (arc_reverse *)(from -> parent);
			}
		} while (from=(node *)(ar->sister));

		af -> shift = shift;
		af -> r_cap = r_cap;
		af -> r_rev_cap = r_rev_cap;
	}

	for (ab_for=arc_for_block_first; ab_for; ab_for=ab_for->next)
	{
		i = ab_for -> last_node;
		a_for = i -> first_out;
		ab_for -> current -> shift     = a_for -> shift;
		ab_for -> current -> r_cap     = a_for -> r_cap;
		ab_for -> current -> r_rev_cap = a_for -> r_rev_cap;
		a_for -> shift = (int) (ab_for -> current + 1);
		i -> first_out = (arc_forward *) (((char *)a_for) - 1);
	}

	/* THIRD STAGE */
	for (ab_rev=arc_rev_block_first; ab_rev; ab_rev=ab_rev->next)
	{
		ab_rev -> current = ab_rev -> last_node -> first_in;
	}

	for (nb=node_block_first; nb; nb=nb->next)
	for (i=&nb->nodes[0]; i<nb->current; i++)
	{
		arc_forward *a_for_first, *a_for_last;

		a_for_first = i -> first_out;
		if (IS_ODD(a_for_first))
		{
			a_for_first = (arc_forward *) (((char *)a_for_first) + 1);
			a_for_last = (arc_forward *) ((a_for_first ++) -> shift);
		}
		else a_for_last = (i + 1) -> first_out;

		for (a_for=a_for_first; a_for<a_for_last; a_for++)
		{
			node *to = NEIGHBOR_NODE(i, a_for -> shift);
			a_rev = -- to -> first_in;
			a_rev -> sister = a_for;
		}
	}

	for (ab_rev=arc_rev_block_first; ab_rev; ab_rev=ab_rev->next)
	{
		i = ab_rev -> last_node;
		a_rev = i -> first_in;
		ab_rev -> current -> sister = a_rev -> sister;
		a_rev -> sister = (arc_forward *) (ab_rev -> current + 1);
		i -> first_in = (arc_reverse *) (((char *)a_rev) - 1);
	}
}
