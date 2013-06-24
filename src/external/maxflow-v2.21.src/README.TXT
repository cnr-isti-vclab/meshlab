###################################################################
#                                                                 #
#    MAXFLOW - software for computing mincut/maxflow in a graph   #
#                        Version 2.21                             #
#    http://www.cs.ucl.ac.uk/staff/V.Kolmogorov/software.html     #
#                                                                 #
#    Yuri Boykov (yuri@csd.uwo.ca)                                #
#    Vladimir Kolmogorov (v.kolmogorov@cs.ucl.ac.uk)              #
#    2001                                                         #
#                                                                 #
###################################################################

1. Introduction.

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

Tested under windows, Visual C++ 6.0 compiler and unix (SunOS 5.8
and RedHat Linux 7.0, GNU c++ compiler).

##################################################################

2. Licence.

Copyright UCL Business PLC

This program is available under dual licence:
    
1) Under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.
Note that any program that incorporates the code under this licence must, under the terms of the GNU GPL, be released under a licence compatible with the GPL. GNU GPL does not permit incorporating this program into proprietary programs. If you wish to do this, please see the alternative licence available below.
GNU General Public License can be found at http://www.gnu.org/licenses/old-licenses/gpl-2.0.html

2) Proprietary Licence from UCL Business PLC.
To enable programers to include the MaxFlow software in a proprietary system (which is not allowed by the GNU GPL), this licence gives you the right to incorporate the software in your program and distribute under any licence of your choosing. The full terms of the licence and applicable fee, are available from the Licensors at: http://www.uclb-elicensing.com/optimisation_software/maxflow_computervision.html

##################################################################

3. Graph representation.

There are two versions of the algorithm using different
graph representations (adjacency list and forward star).
The former one uses more than twice as much memory as the
latter one but is 10-20% faster.

Memory allocation (assuming that all capacities are 'short' - 2 bytes):

                 |   Nodes    |   Arcs
------------------------------------------
Adjacency list   | *24 bytes  | *14 bytes
Forward star     | *28 bytes  |  6 bytes

(* means that often it should be rounded up to be a multiple of 4
- some compilers (e.g. Visual C++) seem to round up elements
of arrays unless the are structures containing only char[].)

Note that arcs are always added in pairs - in forward and reverse directions.
Arcs between nodes and terminals (the source and the sink) are
not stored as arcs, but rather as a part of nodes.

The assumption for the forward star representation is that
the maximum number of arcs per node (except the source
and the sink) is much less than ARC_BLOCK_SIZE (1024 by default).

Both versions have the same interface.

##################################################################

4. Example usage.

This section shows how to use the library to compute
a minimum cut on the following graph:

		        SOURCE
		       /       \
		     1/         \2
		     /      3    \
		   node0 -----> node1
		     |   <-----   |
		     |      4     |
		     \            /
		     5\          /6
		       \        /
		          SINK

///////////////////////////////////////////////////

#include <stdio.h>
#include "graph.h"

void main()
{
	Graph::node_id nodes[2];
	Graph *g = new Graph();

	nodes[0] = g -> add_node();
	nodes[1] = g -> add_node();
	g -> set_tweights(nodes[0], 1, 5);
	g -> set_tweights(nodes[1], 2, 6);
	g -> add_edge(nodes[0], nodes[1], 3, 4);

	Graph::flowtype flow = g -> maxflow();

	printf("Flow = %d\n", flow);
	printf("Minimum cut:\n");
	if (g->what_segment(nodes[0]) == Graph::SOURCE)
		printf("node0 is in the SOURCE set\n");
	else
		printf("node0 is in the SINK set\n");
	if (g->what_segment(nodes[1]) == Graph::SOURCE)
		printf("node1 is in the SOURCE set\n");
	else
		printf("node1 is in the SINK set\n");

	delete g;
}

///////////////////////////////////////////////////
