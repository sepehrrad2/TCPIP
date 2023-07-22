#ifndef __NET__
#define __NET__

#include "utils.h"
#include <cstring>
#include <memory.h>

/* Device IDS */
#define L3_ROUTER   (1 << 0)
#define L2_SWITCH   (1 << 1)
#define HUB         (1 << 2)

class graph_;
class interface_;
class node_;


class ip_add_ {
public:
    char ip_addr[16];
};

class mac_add_ {
public:
    char mac[8];
};

class node_nw_prop_{

    /* Used to find various device types capabilities of
     * the node and other features*/
public:
    unsigned int flags;
     
    /*L3 properties*/ 
    bool is_lb_addr_config;
    ip_add_ lb_addr; /*loopback address of node*/
};

static void
init_node_nw_prop(node_nw_prop_& node_nw_prop) {

    node_nw_prop.flags = 0;
    node_nw_prop.is_lb_addr_config = FALSE;
    memset(node_nw_prop.lb_addr.ip_addr, 0, 16);
}

class intf_nw_props_ {
public:
    /*L2 properties*/
    mac_add_ mac_add;      /*Mac are hard burnt in interface NIC*/

    /*L3 properties*/
    bool is_ipadd_config; /*Set to true if ip add is configured, intf operates in L3 mode if ip address is configured on it*/
    ip_add_ ip_add;
    char mask;
};

static inline void
init_intf_nw_prop(intf_nw_props_ &intf_nw_props) {

    memset(intf_nw_props.mac_add.mac , 0 , 8);
    intf_nw_props.is_ipadd_config = FALSE;
    memset(intf_nw_props.ip_add.ip_addr, 0, 16);
    intf_nw_props.mask = 0;
}

void
interface_assign_mac_address(interface_* interface);

/*GET shorthand Macros*/
inline char* IF_MAC(interface_* intf_);
inline char* IF_IP(interface_* intf_) ;


inline char* NODE_LO_ADDR(node_* node);

/*APIs to set Network Node properties*/
bool node_set_device_type(node_& node, unsigned int F);
bool node_set_loopback_address(node_& node,const char *ip_addr);
bool node_set_intf_ip_address(node_& node,const char *local_if,const char *ip_addr, char mask);
bool node_unset_intf_ip_address(node_& node,const char *local_if);


/*Dumping Functions to dump network information
 * on nodes and interfaces*/
void dump_nw_graph(graph_ &graph);
void dump_node_nw_props(node_ &node);
void dump_intf_props(interface_ *interface);
  
#endif /* __NET__ */
