#include "graph.h"
#include <iostream>

extern void init_udp_socket(node_ *node);

graph_ * create_new_graph(std::string topology_name){
    graph_* g = new graph_();
    g->topology_name = topology_name;
    return g;
}

node_ * create_graph_node(graph_& graph, string node_name){
    node_ * N = new node_();
    N->node_name = node_name;
    graph.node_list.push_back(N);
    init_udp_socket(N);
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
    interface_assign_mac_address(&L->intf1);
    interface_assign_mac_address(&L->intf2);
    init_intf_nw_prop(L->intf1.intf_nw_props);
    init_intf_nw_prop(L->intf2.intf_nw_props);
}

interface_ * get_node_if_by_name(node_& node, string if_name){
    for (int i =0; i < MAX_INTF_PER_NODE; i++){
        if (node.intf[i]->if_name == if_name){
            return node.intf[i];
        }
    }
    interface_ * intf = new interface_();
    return intf;
}
node_ get_node_by_node_name(graph_& graph, string node_name){
 for (auto N : graph.node_list){
    if (N->node_name == node_name){
        return *N;
    }
 }   
 node_ a;
 return a;
}

void dump_graph(graph_ &graph){

    node_ *node;
    
    cout << "Topology Name = " << graph.topology_name << endl;

    for (std::list<node_*>::iterator it = graph.node_list.begin(); it != graph.node_list.end(); it++){
        dump_node(**it);    
    }
}

void dump_node(node_ &node){

    unsigned int i = 0;
    interface_ *intf;

    cout << "Node Name =  " << node.node_name << endl;
    for(int i = 0 ; i < MAX_INTF_PER_NODE; i++){
        
        intf = node.intf[i];
        if(!intf) break;
        dump_interface(*intf);
    }
}

void dump_interface(interface_ & interface){

   link_ *link = interface.link;
   node_ *nbr_node = get_nbr_node(&interface);

   cout << "Interface Name: " << interface.if_name << endl; 
   cout << "Local Node: " << interface.att_node->node_name << endl;
   cout << "Nbr Node: " << nbr_node->node_name << endl;
   cout << "cost: " << link->cost << endl;
}



