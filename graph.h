/*
 * =====================================================================================
 *
 *       Filename:  graph.h
 *
 *    Description:  This file contains the definition of all structures required to create a NetworkGraph
 *
 *        Version:  1.0
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Sepehr Radmannia         
 *        This program is distributed in the hope that it will be useful, but 
 *        WITHOUT ANY WARRANTY; without even the implied warranty of 
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *        General Public License for more details.
 *
 * =====================================================================================
 */

#ifndef __GRAPH__
#define __GRAPH__

#include <list>
#include <string>
#include <vector>
#include <assert.h>
#include "net.h"
//#include "gluethread/glthread.h"

using namespace std;
//#define NODE_NAME_SIZE   16
//#define IF_NAME_SIZE     16
#define MAX_INTF_PER_NODE   10

/*Forward Declarations*/
class node_;
class link_;


class interface_ {
public:
    string if_name;
    node_ *att_node;
    link_ *link;
    intf_nw_props_ intf_nw_props;
};

class link_ {
public:
    interface_ intf1;
    interface_ intf2;
    unsigned int cost;
}; 

class node_ {
public:
    string node_name;
    interface_ *intf[MAX_INTF_PER_NODE];
    unsigned int udp_port_number;
    int udp_sock_fd;
    node_nw_prop_ node_nw_prop;
};

class graph_{
public:
    string topology_name;
    std::list<node_ *> node_list; 
};

node_ * create_graph_node(graph_ &graph, string node_name);

graph_ * create_new_graph(string topology_name);

void insert_link_between_two_nodes(node_ &node1, 
                             node_ &node2,
                             string from_if_name, 
                             string to_if_name, 
                             unsigned int cost);

/*Helper functions*/
static node_ *
get_nbr_node(interface_ *interface){

    assert(interface->att_node);
    assert(interface->link);
    
    link_ *link = interface->link;
    if(&link->intf1 == interface)
        return link->intf2.att_node;
    else
        return link->intf1.att_node;
}
interface_ *
get_node_if_by_name(node_& node, string if_name);

node_ get_node_by_node_name(graph_& graph, string node_name);

static int
get_node_intf_available_slot(node_ &node){

    int i ;
    for( i = 0 ; i < MAX_INTF_PER_NODE; i++){
        if(node.intf[i])
            continue;
        return i;
    }
    return -1;
}

/*Display Routines*/
void dump_graph(graph_ &graph);
void dump_node(node_ &node);
void dump_interface(interface_ & interface);


#endif /* __NW_GRAPH_ */
