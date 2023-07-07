#include "graph.h"



graph_ * create_new_graph(std::string topology_name){
    graph_* g = new graph_();
    g->topology_name = topology_name;
    return g;
}

node_ * create_graph_node(graph_& graph, string node_name){
    node_ * N = new node_();
    N->node_name = node_name;
    graph.node_list.push_back(*N);
    return N;

}

void insert_link_between_two_nodes(node_& node1, 
                             node_& node2,
                             string from_if_name, 
                             string to_if_name, 
                             unsigned int cost){
    int nodes_empty_slot;
    link_* L = new link_();
    L->intf1.if_name = from_if_name;
    L->intf2.if_name = to_if_name;
    L->cost = cost;
    L->intf1.att_node = &node1;
    L->intf2.att_node = &node2;
    L->intf1.link = L;
    L->intf2.link = L;
    nodes_empty_slot = get_node_intf_available_slot(node1);
    node1.intf[nodes_empty_slot] = &L->intf1;
    nodes_empty_slot = get_node_intf_available_slot(node2);
    node2.intf[nodes_empty_slot] = &L->intf2;
}



