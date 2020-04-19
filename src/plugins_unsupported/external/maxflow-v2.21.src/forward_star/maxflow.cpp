/* maxflow.cpp */

#include <stdio.h>
#include "graph.h"

/*
	special constants for node->parent
*/
#define TERMINAL ( (arc_forward *) 1 )		/* to terminal */
#define ORPHAN   ( (arc_forward *) 2 )		/* orphan */

#define INFINITE_D 1000000000		/* infinite distance to the terminal */

/***********************************************************************/

/*
	Functions for processing active list.
	i->next points to the next node in the list
	(or to i, if i is the last node in the list).
	If i->next is NULL iff i is not in the list.

	There are two queues. Active nodes are added
	to the end of the second queue and read from
	the front of the first queue. If the first queue
	is empty, it is replaced by the second queue
	(and the second queue becomes empty).
*/

inline void Graph::set_active(node *i)
{
	if (!i->next)
	{
		/* it's not in the list yet */
		if (queue_last[1]) queue_last[1] -> next = i;
		else               queue_first[1]        = i;
		queue_last[1] = i;
		i -> next = i;
	}
}

/*
	Returns the next active node.
	If it is connected to the sink, it stays in the list,
	otherwise it is removed from the list
*/
inline Graph::node * Graph::next_active()
{
	node *i;

	while ( 1 )
	{
		if (!(i=queue_first[0]))
		{
			queue_first[0] = i = queue_first[1];
			queue_last[0]  = queue_last[1];
			queue_first[1] = NULL;
			queue_last[1]  = NULL;
			if (!i) return NULL;
		}

		/* remove it from the active list */
		if (i->next == i) queue_first[0] = queue_last[0] = NULL;
		else              queue_first[0] = i -> next;
		i -> next = NULL;

		/* a node in the list is active iff it has a parent */
		if (i->parent) return i;
	}
}

/***********************************************************************/

void Graph::maxflow_init()
{
	node *i;
	node_block *nb;

	queue_first[0] = queue_last[0] = NULL;
	queue_first[1] = queue_last[1] = NULL;
	orphan_first = NULL;

	for (nb=node_block_first; nb; nb=nb->next)
	for (i=&nb->nodes[0]; i<nb->current; i++)
	{
		i -> next = NULL;
		i -> TS = 0;
		if (i->tr_cap > 0)
		{
			/* i is connected to the source */
			i -> is_sink = 0;
			i -> parent = TERMINAL;
			set_active(i);
			i -> TS = 0;
			i -> DIST = 1;
		}
		else if (i->tr_cap < 0)
		{
			/* i is connected to the sink */
			i -> is_sink = 1;
			i -> parent = TERMINAL;
			set_active(i);
			i -> TS = 0;
			i -> DIST = 1;
		}
		else
		{
			i -> parent = NULL;
		}
	}
	TIME = 0;
}

/***********************************************************************/

void Graph::augment(node *s_start, node *t_start, captype *cap_middle, captype *rev_cap_middle)
{
	node *i;
	arc_forward *a;
	captype bottleneck;
	nodeptr *np;


	/* 1. Finding bottleneck capacity */
	/* 1a - the source tree */
	bottleneck = *cap_middle;
	for (i=s_start; ; )
	{
		a = i -> parent;
		if (a == TERMINAL) break;
		if (IS_ODD(a))
		{
			a = MAKE_EVEN(a);
			if (bottleneck > a->r_cap) bottleneck = a -> r_cap;
			i = NEIGHBOR_NODE_REV(i, a -> shift);
		}
		else
		{
			if (bottleneck > a->r_rev_cap) bottleneck = a -> r_rev_cap;
			i = NEIGHBOR_NODE(i, a -> shift);
		}
	}
	if (bottleneck > i->tr_cap) bottleneck = i -> tr_cap;
	/* 1b - the sink tree */
	for (i=t_start; ; )
	{
		a = i -> parent;
		if (a == TERMINAL) break;
		if (IS_ODD(a))
		{
			a = MAKE_EVEN(a);
			if (bottleneck > a->r_rev_cap) bottleneck = a -> r_rev_cap;
			i = NEIGHBOR_NODE_REV(i, a -> shift);
		}
		else
		{
			if (bottleneck > a->r_cap) bottleneck = a -> r_cap;
			i = NEIGHBOR_NODE(i, a -> shift);
		}
	}
	if (bottleneck > - i->tr_cap) bottleneck = - i -> tr_cap;


	/* 2. Augmenting */
	/* 2a - the source tree */
	*rev_cap_middle += bottleneck;
	*cap_middle -= bottleneck;
	for (i=s_start; ; )
	{
		a = i -> parent;
		if (a == TERMINAL) break;
		if (IS_ODD(a))
		{
			a = MAKE_EVEN(a);
			a -> r_rev_cap += bottleneck;
			a -> r_cap -= bottleneck;
			if (!a->r_cap)
			{
				/* add i to the adoption list */
				i -> parent = ORPHAN;
				np = nodeptr_block -> New();
				np -> ptr = i;
				np -> next = orphan_first;
				orphan_first = np;
			}
			i = NEIGHBOR_NODE_REV(i, a -> shift);
		}
		else
		{
			a -> r_cap += bottleneck;
			a -> r_rev_cap -= bottleneck;
			if (!a->r_rev_cap)
			{
				/* add i to the adoption list */
				i -> parent = ORPHAN;
				np = nodeptr_block -> New();
				np -> ptr = i;
				np -> next = orphan_first;
				orphan_first = np;
			}
			i = NEIGHBOR_NODE(i, a -> shift);
		}
	}
	i -> tr_cap -= bottleneck;
	if (!i->tr_cap)
	{
		/* add i to the adoption list */
		i -> parent = ORPHAN;
		np = nodeptr_block -> New();
		np -> ptr = i;
		np -> next = orphan_first;
		orphan_first = np;
	}
	/* 2b - the sink tree */
	for (i=t_start; ; )
	{
		a = i -> parent;
		if (a == TERMINAL) break;
		if (IS_ODD(a))
		{
			a = MAKE_EVEN(a);
			a -> r_cap += bottleneck;
			a -> r_rev_cap -= bottleneck;
			if (!a->r_rev_cap)
			{
				/* add i to the adoption list */
				i -> parent = ORPHAN;
				np = nodeptr_block -> New();
				np -> ptr = i;
				np -> next = orphan_first;
				orphan_first = np;
			}
			i = NEIGHBOR_NODE_REV(i, a -> shift);
		}
		else
		{
			a -> r_rev_cap += bottleneck;
			a -> r_cap -= bottleneck;
			if (!a->r_cap)
			{
				/* add i to the adoption list */
				i -> parent = ORPHAN;
				np = nodeptr_block -> New();
				np -> ptr = i;
				np -> next = orphan_first;
				orphan_first = np;
			}
			i = NEIGHBOR_NODE(i, a -> shift);
		}
	}
	i -> tr_cap += bottleneck;
	if (!i->tr_cap)
	{
		/* add i to the adoption list */
		i -> parent = ORPHAN;
		np = nodeptr_block -> New();
		np -> ptr = i;
		np -> next = orphan_first;
		orphan_first = np;
	}


	flow += bottleneck;
}

/***********************************************************************/

void Graph::process_source_orphan(node *i)
{
	node *j;
	arc_forward *a0_for, *a0_for_first, *a0_for_last;
	arc_reverse *a0_rev, *a0_rev_first, *a0_rev_last;
	arc_forward *a0_min = NULL, *a;
	nodeptr *np;
	int d, d_min = INFINITE_D;

	/* trying to find a new parent */
	a0_for_first = i -> first_out;
	if (IS_ODD(a0_for_first))
	{
		a0_for_first = (arc_forward *) (((char *)a0_for_first) + 1);
		a0_for_last = (arc_forward *) ((a0_for_first ++) -> shift);
	}
	else a0_for_last = (i + 1) -> first_out;
	a0_rev_first = i -> first_in;
	if (IS_ODD(a0_rev_first))
	{
		a0_rev_first = (arc_reverse *) (((char *)a0_rev_first) + 1);
		a0_rev_last  = (arc_reverse *) ((a0_rev_first ++) -> sister);
	}
	else a0_rev_last = (i + 1) -> first_in;


	for (a0_for=a0_for_first; a0_for<a0_for_last; a0_for++)
	if (a0_for->r_rev_cap)
	{
		j = NEIGHBOR_NODE(i, a0_for -> shift);
		if (!j->is_sink && (a=j->parent))
		{
			/* checking the origin of j */
			d = 0;
			while ( 1 )
			{
				if (j->TS == TIME)
				{
					d += j -> DIST;
					break;
				}
				a = j -> parent;
				d ++;
				if (a==TERMINAL)
				{
					j -> TS = TIME;
					j -> DIST = 1;
					break;
				}
				if (a==ORPHAN) { d = INFINITE_D; break; }
				if (IS_ODD(a))
					j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
				else
					j = NEIGHBOR_NODE(j, a -> shift);
			}
			if (d<INFINITE_D) /* j originates from the source - done */
			{
				if (d<d_min)
				{
					a0_min = a0_for;
					d_min = d;
				}
				/* set marks along the path */
				for (j=NEIGHBOR_NODE(i, a0_for->shift); j->TS!=TIME; )
				{
					j -> TS = TIME;
					j -> DIST = d --;
					a = j->parent;
					if (IS_ODD(a))
						j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
					else
						j = NEIGHBOR_NODE(j, a -> shift);
				}
			}
		}
	}
	for (a0_rev=a0_rev_first; a0_rev<a0_rev_last; a0_rev++)
	{
		a0_for = a0_rev -> sister;
		if (a0_for->r_cap)
		{
			j = NEIGHBOR_NODE_REV(i, a0_for -> shift);
			if (!j->is_sink && (a=j->parent))
			{
				/* checking the origin of j */
				d = 0;
				while ( 1 )
				{
					if (j->TS == TIME)
					{
						d += j -> DIST;
						break;
					}
					a = j -> parent;
					d ++;
					if (a==TERMINAL)
					{
						j -> TS = TIME;
						j -> DIST = 1;
						break;
					}
					if (a==ORPHAN) { d = INFINITE_D; break; }
					if (IS_ODD(a))
						j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
					else
						j = NEIGHBOR_NODE(j, a -> shift);
				}
				if (d<INFINITE_D) /* j originates from the source - done */
				{
					if (d<d_min)
					{
						a0_min = MAKE_ODD(a0_for);
						d_min = d;
					}
					/* set marks along the path */
					for (j=NEIGHBOR_NODE_REV(i,a0_for->shift); j->TS!=TIME; )
					{
						j -> TS = TIME;
						j -> DIST = d --;
						a = j->parent;
						if (IS_ODD(a))
							j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
						else
							j = NEIGHBOR_NODE(j, a -> shift);
					}
				}
			}
		}
	}

	if (i->parent = a0_min)
	{
		i -> TS = TIME;
		i -> DIST = d_min + 1;
	}
	else
	{
		/* no parent is found */
		i -> TS = 0;

		/* process neighbors */
		for (a0_for=a0_for_first; a0_for<a0_for_last; a0_for++)
		{
			j = NEIGHBOR_NODE(i, a0_for -> shift);
			if (!j->is_sink && (a=j->parent))
			{
				if (a0_for->r_rev_cap) set_active(j);
				if (a!=TERMINAL && a!=ORPHAN && IS_ODD(a) && NEIGHBOR_NODE_REV(j, MAKE_EVEN(a)->shift)==i)
				{
					/* add j to the adoption list */
					j -> parent = ORPHAN;
					np = nodeptr_block -> New();
					np -> ptr = j;
					if (orphan_last) orphan_last -> next = np;
					else             orphan_first        = np;
					orphan_last = np;
					np -> next = NULL;
				}
			}
		}
		for (a0_rev=a0_rev_first; a0_rev<a0_rev_last; a0_rev++)
		{
			a0_for = a0_rev -> sister;
			j = NEIGHBOR_NODE_REV(i, a0_for -> shift);
			if (!j->is_sink && (a=j->parent))
			{
				if (a0_for->r_cap) set_active(j);
				if (a!=TERMINAL && a!=ORPHAN && !IS_ODD(a) && NEIGHBOR_NODE(j, a->shift)==i)
				{
					/* add j to the adoption list */
					j -> parent = ORPHAN;
					np = nodeptr_block -> New();
					np -> ptr = j;
					if (orphan_last) orphan_last -> next = np;
					else             orphan_first        = np;
					orphan_last = np;
					np -> next = NULL;
				}
			}
		}
	}
}

void Graph::process_sink_orphan(node *i)
{
	node *j;
	arc_forward *a0_for, *a0_for_first, *a0_for_last;
	arc_reverse *a0_rev, *a0_rev_first, *a0_rev_last;
	arc_forward *a0_min = NULL, *a;
	nodeptr *np;
	int d, d_min = INFINITE_D;

	/* trying to find a new parent */
	a0_for_first = i -> first_out;
	if (IS_ODD(a0_for_first))
	{
		a0_for_first = (arc_forward *) (((char *)a0_for_first) + 1);
		a0_for_last = (arc_forward *) ((a0_for_first ++) -> shift);
	}
	else a0_for_last = (i + 1) -> first_out;
	a0_rev_first = i -> first_in;
	if (IS_ODD(a0_rev_first))
	{
		a0_rev_first = (arc_reverse *) (((char *)a0_rev_first) + 1);
		a0_rev_last  = (arc_reverse *) ((a0_rev_first ++) -> sister);
	}
	else a0_rev_last = (i + 1) -> first_in;


	for (a0_for=a0_for_first; a0_for<a0_for_last; a0_for++)
	if (a0_for->r_cap)
	{
		j = NEIGHBOR_NODE(i, a0_for -> shift);
		if (j->is_sink && (a=j->parent))
		{
			/* checking the origin of j */
			d = 0;
			while ( 1 )
			{
				if (j->TS == TIME)
				{
					d += j -> DIST;
					break;
				}
				a = j -> parent;
				d ++;
				if (a==TERMINAL)
				{
					j -> TS = TIME;
					j -> DIST = 1;
					break;
				}
				if (a==ORPHAN) { d = INFINITE_D; break; }
				if (IS_ODD(a))
					j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
				else
					j = NEIGHBOR_NODE(j, a -> shift);
			}
			if (d<INFINITE_D) /* j originates from the sink - done */
			{
				if (d<d_min)
				{
					a0_min = a0_for;
					d_min = d;
				}
				/* set marks along the path */
				for (j=NEIGHBOR_NODE(i, a0_for->shift); j->TS!=TIME; )
				{
					j -> TS = TIME;
					j -> DIST = d --;
					a = j->parent;
					if (IS_ODD(a))
						j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
					else
						j = NEIGHBOR_NODE(j, a -> shift);
				}
			}
		}
	}
	for (a0_rev=a0_rev_first; a0_rev<a0_rev_last; a0_rev++)
	{
		a0_for = a0_rev -> sister;
		if (a0_for->r_rev_cap)
		{
			j = NEIGHBOR_NODE_REV(i, a0_for -> shift);
			if (j->is_sink && (a=j->parent))
			{
				/* checking the origin of j */
				d = 0;
				while ( 1 )
				{
					if (j->TS == TIME)
					{
						d += j -> DIST;
						break;
					}
					a = j -> parent;
					d ++;
					if (a==TERMINAL)
					{
						j -> TS = TIME;
						j -> DIST = 1;
						break;
					}
					if (a==ORPHAN) { d = INFINITE_D; break; }
					if (IS_ODD(a))
						j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
					else
						j = NEIGHBOR_NODE(j, a -> shift);
				}
				if (d<INFINITE_D) /* j originates from the sink - done */
				{
					if (d<d_min)
					{
						a0_min = MAKE_ODD(a0_for);
						d_min = d;
					}
					/* set marks along the path */
					for (j=NEIGHBOR_NODE_REV(i,a0_for->shift); j->TS!=TIME; )
					{
						j -> TS = TIME;
						j -> DIST = d --;
						a = j->parent;
						if (IS_ODD(a))
							j = NEIGHBOR_NODE_REV(j, MAKE_EVEN(a) -> shift);
						else
							j = NEIGHBOR_NODE(j, a -> shift);
					}
				}
			}
		}
	}

	if (i->parent = a0_min)
	{
		i -> TS = TIME;
		i -> DIST = d_min + 1;
	}
	else
	{
		/* no parent is found */
		i -> TS = 0;

		/* process neighbors */
		for (a0_for=a0_for_first; a0_for<a0_for_last; a0_for++)
		{
			j = NEIGHBOR_NODE(i, a0_for -> shift);
			if (j->is_sink && (a=j->parent))
			{
				if (a0_for->r_cap) set_active(j);
				if (a!=TERMINAL && a!=ORPHAN && IS_ODD(a) && NEIGHBOR_NODE_REV(j, MAKE_EVEN(a)->shift)==i)
				{
					/* add j to the adoption list */
					j -> parent = ORPHAN;
					np = nodeptr_block -> New();
					np -> ptr = j;
					if (orphan_last) orphan_last -> next = np;
					else             orphan_first        = np;
					orphan_last = np;
					np -> next = NULL;
				}
			}
		}
		for (a0_rev=a0_rev_first; a0_rev<a0_rev_last; a0_rev++)
		{
			a0_for = a0_rev -> sister;
			j = NEIGHBOR_NODE_REV(i, a0_for -> shift);
			if (j->is_sink && (a=j->parent))
			{
				if (a0_for->r_rev_cap) set_active(j);
				if (a!=TERMINAL && a!=ORPHAN && !IS_ODD(a) && NEIGHBOR_NODE(j, a->shift)==i)
				{
					/* add j to the adoption list */
					j -> parent = ORPHAN;
					np = nodeptr_block -> New();
					np -> ptr = j;
					if (orphan_last) orphan_last -> next = np;
					else             orphan_first        = np;
					orphan_last = np;
					np -> next = NULL;
				}
			}
		}
	}
}

/***********************************************************************/

Graph::flowtype Graph::maxflow()
{
	node *i, *j, *current_node = NULL, *s_start, *t_start;
	captype *cap_middle, *rev_cap_middle;
	arc_forward *a_for, *a_for_first, *a_for_last;
	arc_reverse *a_rev, *a_rev_first, *a_rev_last;
	nodeptr *np, *np_next;

	prepare_graph();
	maxflow_init();
	nodeptr_block = new DBlock<nodeptr>(NODEPTR_BLOCK_SIZE, error_function);

	while ( 1 )
	{
		if (i=current_node)
		{
			i -> next = NULL; /* remove active flag */
			if (!i->parent) i = NULL;
		}
		if (!i)
		{
			if (!(i = next_active())) break;
		}

		/* growth */
		s_start = NULL;

		a_for_first = i -> first_out;
		if (IS_ODD(a_for_first))
		{
			a_for_first = (arc_forward *) (((char *)a_for_first) + 1);
			a_for_last = (arc_forward *) ((a_for_first ++) -> shift);
		}
		else a_for_last = (i + 1) -> first_out;
		a_rev_first = i -> first_in;
		if (IS_ODD(a_rev_first))
		{
			a_rev_first = (arc_reverse *) (((char *)a_rev_first) + 1);
			a_rev_last = (arc_reverse *) ((a_rev_first ++) -> sister);
		}
		else a_rev_last = (i + 1) -> first_in;

		if (!i->is_sink)
		{
			/* grow source tree */
			for (a_for=a_for_first; a_for<a_for_last; a_for++)
			if (a_for->r_cap)
			{
				j = NEIGHBOR_NODE(i, a_for -> shift);
				if (!j->parent)
				{
					j -> is_sink = 0;
					j -> parent = MAKE_ODD(a_for);
					j -> TS = i -> TS;
					j -> DIST = i -> DIST + 1;
					set_active(j);
				}
				else if (j->is_sink)
				{
					s_start = i;
					t_start = j;
					cap_middle     = & ( a_for -> r_cap );
					rev_cap_middle = & ( a_for -> r_rev_cap );
					break;
				}
				else if (j->TS <= i->TS &&
				         j->DIST > i->DIST)
				{
					/* heuristic - trying to make the distance from j to the source shorter */
					j -> parent = MAKE_ODD(a_for);
					j -> TS = i -> TS;
					j -> DIST = i -> DIST + 1;
				}
			}
			if (!s_start)
			for (a_rev=a_rev_first; a_rev<a_rev_last; a_rev++)
			{
				a_for = a_rev -> sister;
				if (a_for->r_rev_cap)
				{
					j = NEIGHBOR_NODE_REV(i, a_for -> shift);
					if (!j->parent)
					{
						j -> is_sink = 0;
						j -> parent = a_for;
						j -> TS = i -> TS;
						j -> DIST = i -> DIST + 1;
						set_active(j);
					}
					else if (j->is_sink)
					{
						s_start = i;
						t_start = j;
						cap_middle     = & ( a_for -> r_rev_cap );
						rev_cap_middle = & ( a_for -> r_cap );
						break;
					}
					else if (j->TS <= i->TS &&
							 j->DIST > i->DIST)
					{
						/* heuristic - trying to make the distance from j to the source shorter */
						j -> parent = a_for;
						j -> TS = i -> TS;
						j -> DIST = i -> DIST + 1;
					}
				}
			}
		}
		else
		{
			/* grow sink tree */
			for (a_for=a_for_first; a_for<a_for_last; a_for++)
			if (a_for->r_rev_cap)
			{
				j = NEIGHBOR_NODE(i, a_for -> shift);
				if (!j->parent)
				{
					j -> is_sink = 1;
					j -> parent = MAKE_ODD(a_for);
					j -> TS = i -> TS;
					j -> DIST = i -> DIST + 1;
					set_active(j);
				}
				else if (!j->is_sink)
				{
					s_start = j;
					t_start = i;
					cap_middle     = & ( a_for -> r_rev_cap );
					rev_cap_middle = & ( a_for -> r_cap );
					break;
				}
				else if (j->TS <= i->TS &&
				         j->DIST > i->DIST)
				{
					/* heuristic - trying to make the distance from j to the sink shorter */
					j -> parent = MAKE_ODD(a_for);
					j -> TS = i -> TS;
					j -> DIST = i -> DIST + 1;
				}
			}
			for (a_rev=a_rev_first; a_rev<a_rev_last; a_rev++)
			{
				a_for = a_rev -> sister;
				if (a_for->r_cap)
				{
					j = NEIGHBOR_NODE_REV(i, a_for -> shift);
					if (!j->parent)
					{
						j -> is_sink = 1;
						j -> parent = a_for;
						j -> TS = i -> TS;
						j -> DIST = i -> DIST + 1;
						set_active(j);
					}
					else if (!j->is_sink)
					{
						s_start = j;
						t_start = i;
						cap_middle     = & ( a_for -> r_cap );
						rev_cap_middle = & ( a_for -> r_rev_cap );
						break;
					}
					else if (j->TS <= i->TS &&
							 j->DIST > i->DIST)
					{
						/* heuristic - trying to make the distance from j to the sink shorter */
						j -> parent = a_for;
						j -> TS = i -> TS;
						j -> DIST = i -> DIST + 1;
					}
				}
			}
		}

		TIME ++;

		if (s_start)
		{
			i -> next = i; /* set active flag */
			current_node = i;

			/* augmentation */
			augment(s_start, t_start, cap_middle, rev_cap_middle);
			/* augmentation end */

			/* adoption */
			while (np=orphan_first)
			{
				np_next = np -> next;
				np -> next = NULL;

				while (np=orphan_first)
				{
					orphan_first = np -> next;
					i = np -> ptr;
					nodeptr_block -> Delete(np);
					if (!orphan_first) orphan_last = NULL;
					if (i->is_sink) process_sink_orphan(i);
					else            process_source_orphan(i);
				}

				orphan_first = np_next;
			}
			/* adoption end */
		}
		else current_node = NULL;
	}

	delete nodeptr_block;

	return flow;
}

/***********************************************************************/

Graph::termtype Graph::what_segment(node_id i)
{
	if (((node*)i)->parent && !((node*)i)->is_sink) return SOURCE;
	return SINK;
}

