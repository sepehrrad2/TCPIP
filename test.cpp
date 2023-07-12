#include "graph.h"
#include "net.h"
#include <iostream>
//using namespace std;

int main(){
    graph_ * topo = create_new_graph("Hello");
    node_ * N1 = create_graph_node( *topo, "first");
    node_ * N2 = create_graph_node( *topo, "second");
    node_ * N3 = create_graph_node( *topo, "third");

    insert_link_between_two_nodes(*N1, *N2,"eth1/2", "eth2/1",12);
    insert_link_between_two_nodes(*N1, *N3,"eth1/3", "eth3/1",13);
    insert_link_between_two_nodes(*N2, *N3,"eth2/3", "eth3/2",23);

    auto b = node_set_device_type(*N1, L3_ROUTER);
    b = node_set_loopback_address(*N1, "122.1.1.0");
    b = node_set_intf_ip_address(*N1, "eth1/2", "40.1.1.1", 24);
    b = node_set_intf_ip_address(*N1, "eth1/3", "20.1.1.1", 24);

    auto a = node_set_device_type(*N2, L3_ROUTER);
    a = node_set_loopback_address(*N2, "122.1.1.1");
    a = node_set_intf_ip_address(*N2, "eth2/1", "20.1.1.2", 24);
    a = node_set_intf_ip_address(*N2, "eth2/3", "30.1.1.1", 24);

    a = node_set_device_type(*N3, L3_ROUTER);
    a  =node_set_loopback_address(*N3, "122.1.1.2");
    a = node_set_intf_ip_address(*N3, "eth3/2", "30.1.1.2", 24);
    a = node_set_intf_ip_address(*N3, "eth3/1", "40.1.1.2", 24);

    dump_nw_graph(*topo);
    /*interface_ * intf =  get_node_if_by_name(*N1, "eth0/1");
    node_ b = get_node_by_node_name(*topo, "first"); 
    dump_graph(*topo);
    init_node_nw_prop(N1->node_nw_prop);
    dump_node_nw_props(*N1);   
    init_intf_nw_prop(N1->intf[0]->intf_nw_props);
    dump_intf_props(N1->intf[0]);*/
}