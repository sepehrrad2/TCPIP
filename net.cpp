/*
 * =====================================================================================
 *
 *       Filename:  net.c
 *
 *    Description:  This file contains general pupose Networking routines
 *
 *        Version:  1.0
 *        Created:  Wednesday 18 September 2019 08:36:50  IST
 *       Revision:  1.0
 *       Compiler:  gcc
 *
 *         Author:  Er. Abhishek Sagar, Networking Developer (AS), sachinites@gmail.com
 *        Company:  Brocade Communications(Jul 2012- Mar 2016), Current : Juniper Networks(Apr 2017 - Present)
 *        
 *        This file is part of the NetworkGraph distribution (https://github.com/sachinites).
 *        Copyright (c) 2017 Abhishek Sagar.
 *        This program is free software: you can redistribute it and/or modify
 *        it under the terms of the GNU General Public License as published by  
 *        the Free Software Foundation, version 3.
 *
 *        This program is distributed in the hope that it will be useful, but 
 *        WITHOUT ANY WARRANTY; without even the implied warranty of 
 *        MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU 
 *        General Public License for more details.
 *
 *        You should have received a copy of the GNU General Public License 
 *        along with this program. If not, see <http://www.gnu.org/licenses/>.
 *
 * =====================================================================================
 */


#include <memory.h>
#include "utils.h"
#include <stdio.h>
#include <string>
#include "net.h"
#include "graph.h"
#include <iostream>
/*Heuristics, Assign a unique mac address to interface*/
void
interface_assign_mac_address(interface_* interface){

    memset(IF_MAC(interface), 0, 8);
    strcpy(IF_MAC(interface), interface->att_node->node_name.c_str());
    strcat(IF_MAC(interface), interface->if_name.c_str());
}

inline char* IF_MAC(interface_* intf_) { return (intf_->intf_nw_props.mac_add.mac); }
inline char* IF_IP(interface_* intf_)   { return (intf_->intf_nw_props.ip_add.ip_addr);}


inline char* NODE_LO_ADDR(node_* node) {return (node->node_nw_prop.lb_addr.ip_addr);}

bool node_set_device_type(node_& node, unsigned int F){

    SET_BIT(node.node_nw_prop.flags, F);
    return true;
}

bool node_set_loopback_address(node_ &node,const char *ip_addr){

    assert(ip_addr);

    if(IS_BIT_SET(node.node_nw_prop.flags, HUB))
        assert(0); /*Wrong Config : A HUB do not have any IP addresses*/

    if(!IS_BIT_SET(node.node_nw_prop.flags, L3_ROUTER))
        assert(0); /*You must enable L3 routing on device first*/

    node.node_nw_prop.is_lb_addr_config = true;
    strncpy(NODE_LO_ADDR(&node), ip_addr, 16);
    NODE_LO_ADDR(&node)[16] = '\0';
    
    return true;
}

bool node_set_intf_ip_address(node_& node,const char *local_if, 
                                const char *ip_addr, char mask) {

    interface_ *interface = get_node_if_by_name(node, local_if);
    if(!interface) assert(0);

    strncpy(IF_IP(interface), ip_addr, 16);
    IF_IP(interface)[16] = '\0';
    interface->intf_nw_props.mask = mask; 
    interface->intf_nw_props.is_ipadd_config = true;
    return true;
}

bool node_unset_intf_ip_address(node_ & node,const char *local_if){

    return true;
}

void dump_node_nw_props(node_ &node){

    cout << "Node Name = " <<  node.node_name << endl;
    cout << "node flags : " << node.node_nw_prop.flags << endl;
    if(node.node_nw_prop.is_lb_addr_config){
        cout << "lo addr : " << NODE_LO_ADDR(&node) << endl;
    }
}

void dump_intf_props(interface_ *interface){

    dump_interface(*interface);

    if(interface->intf_nw_props.is_ipadd_config){
        printf("\t IP Addr = %s/%u", IF_IP(interface), interface->intf_nw_props.mask);
    }
    else{
         printf("\t IP Addr = %s/%u", "Nil", 0);
    }

    printf("MAC : %u:%u:%u:%u:%u:%u\n", 
        IF_MAC(interface)[0], IF_MAC(interface)[1],
        IF_MAC(interface)[2], IF_MAC(interface)[3],
        IF_MAC(interface)[4], IF_MAC(interface)[5]);
}

void dump_nw_graph(graph_ & graph){

    node_ *node;
    interface_ *interface;
    unsigned int i;
    
    cout << "Topology Name = " << graph.topology_name << endl;

    for(auto node : graph.node_list){

        //node = graph_glue_to_node(curr);
        dump_node_nw_props(*node);
        for( i = 0; i < MAX_INTF_PER_NODE; i++){
            interface = node->intf[i];
            if(!interface) break;
            dump_intf_props(interface);
        }
    }

}
