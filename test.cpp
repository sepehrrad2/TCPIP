#include "graph.h"
#include <iostream>
//using namespace std;

int main(){
    //cout << "hello" << endl;
    graph_ * a = create_new_graph("hello");
    cout << a->topology_name << " graph test" << endl;
    node_ * N1 = create_graph_node( *a, "first");
    node_ * N2 = create_graph_node( *a, "second");
    cout << N1->node_name << endl;
    insert_link_between_two_nodes(*N1, *N2,"eth0/1", "eth1/0",10);
}