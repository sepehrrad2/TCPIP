#include "comm.h"
#include "graph.h"
#include <sys/socket.h>
#include <pthread.h>
#include <netinet/in.h>
#include <memory.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <errno.h>
#include <netdb.h> /*for struct hostent*/
#include <unistd.h>
static int
_send_pkt_out(int sock_fd, char *pkt_data, unsigned int pkt_size, 
                unsigned int dst_udp_port_no){

    int rc;
    struct sockaddr_in dest_addr;
   
    struct hostent *host = (struct hostent *) gethostbyname("127.0.0.1"); 
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = dst_udp_port_no;
    dest_addr.sin_addr = *((struct in_addr *)host->h_addr);

    rc = sendto(sock_fd, pkt_data, pkt_size, 0, 
            (struct sockaddr *)&dest_addr, sizeof(struct sockaddr));
    
    return rc;
}


static unsigned int udp_port_number = 40000;

static unsigned int 
get_next_udp_port_number(){
    
    return udp_port_number++;
}

void
init_udp_socket(node_ *node){

    if(node->udp_port_number)
        return;
    
    node->udp_port_number = get_next_udp_port_number();
     
    int udp_sock_fd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );
    
    if(udp_sock_fd == -1){
        cout << "Socket Creation Failed for node " <<  node->node_name << endl;
        return;   
    }

    struct sockaddr_in node_addr;
    node_addr.sin_family      = AF_INET;
    node_addr.sin_port        = node->udp_port_number;
    node_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udp_sock_fd, (struct sockaddr *)&node_addr, sizeof(struct sockaddr)) == -1) {
        cout << "Error : socket bind failed for Node " << node->node_name << endl;
        return;
    }

    node->udp_sock_fd = udp_sock_fd;
}

static char recv_buffer[MAX_PACKET_BUFFER_SIZE];
static char send_buffer[MAX_PACKET_BUFFER_SIZE];

static void
_pkt_receive(node_ *receving_node, 
            char *pkt_with_aux_data, 
            unsigned int pkt_size){

    char *recv_intf_name = pkt_with_aux_data;
    std::string intf_name(recv_intf_name);
    interface_ *recv_intf = get_node_if_by_name(*receving_node, intf_name);

    if(!recv_intf){
        cout << "Error : Pkt recvd on unknown interface" << recv_intf->if_name << "on node:  "  << receving_node->node_name << endl;
        return;
    }

    pkt_receive(receving_node, recv_intf, pkt_with_aux_data + 16, 
                pkt_size - 16);
}

static void *
_network_start_pkt_receiver_thread(void *arg){

    node_ *node;
    
    fd_set active_sock_fd_set,
           backup_sock_fd_set;
    
    int sock_max_fd = 0;
    int bytes_recvd = 0;
    
    graph_ *topo = static_cast<graph_*>(arg);

    unsigned int addr_len = sizeof(struct sockaddr);

    FD_ZERO(&active_sock_fd_set);
    FD_ZERO(&backup_sock_fd_set);
    
    struct sockaddr_in sender_addr;

    for (auto node : topo->node_list){

        
        if(!node->udp_sock_fd) 
            continue;

        if(node->udp_sock_fd > sock_max_fd)
            sock_max_fd = node->udp_sock_fd;

        FD_SET(node->udp_sock_fd, &backup_sock_fd_set);
            
    }

    while(1){

        memcpy(&active_sock_fd_set, &backup_sock_fd_set, sizeof(fd_set));

        select(sock_max_fd + 1, &active_sock_fd_set, NULL, NULL, NULL);

        for(auto node : topo->node_list){


            if(FD_ISSET(node->udp_sock_fd, &active_sock_fd_set)){
    
                memset(recv_buffer, 0, MAX_PACKET_BUFFER_SIZE);
                bytes_recvd = recvfrom(node->udp_sock_fd, (char *)recv_buffer, 
                        MAX_PACKET_BUFFER_SIZE, 0, (struct sockaddr *)&sender_addr, &addr_len);
                
                _pkt_receive(node, recv_buffer, bytes_recvd);
            }
            
        }
    }
}


void
network_start_pkt_receiver_thread(graph_ *topo){

    pthread_attr_t attr;
    pthread_t recv_pkt_thread;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

    pthread_create(&recv_pkt_thread, &attr, 
                    _network_start_pkt_receiver_thread, 
                    (void *)topo);
}

/*Public APIs to be used by the other modules*/
int
send_pkt_out(char *pkt, unsigned int pkt_size,interface_ *interface){

    int rc = 0;

    node_ *sending_node = interface->att_node;
    node_ *nbr_node = get_nbr_node(interface);
    
    if(!nbr_node)
        return -1;

    unsigned int dst_udp_port_no = nbr_node->udp_port_number;
    
    int sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP );

    if(sock < 0){
        cout << "Error : Sending socket Creation failed , errno = " << errno << endl;
        return -1;
    }
    
    interface_ *other_interface = &interface->link->intf1 == interface ? \
                                    &interface->link->intf2 : &interface->link->intf1;

    memset(send_buffer, 0, MAX_PACKET_BUFFER_SIZE);

    char *pkt_with_aux_data = send_buffer;
    char destination[16];
    strcpy(destination, other_interface->if_name.c_str());

    strncpy(pkt_with_aux_data, destination, 16);

    pkt_with_aux_data[16] = '\0';

    memcpy(pkt_with_aux_data + 16, pkt, pkt_size);

    rc = _send_pkt_out(sock, pkt_with_aux_data, pkt_size + 16, 
                        dst_udp_port_no);

    close(sock);
    return rc; 
}

/*extern void
layer2_frame_recv(node_ *node, interface_ *interface,
                     char *pkt, unsigned int pkt_size);
*/

int
pkt_receive(node_ *node, interface_ *interface, char *pkt, unsigned int pkt_size){

    /*Do further processing of the pkt here*/

    cout << "packet received on: " << node->node_name << endl;
    cout << interface->if_name << endl;  
    //layer2_frame_recv(node, interface, pkt, pkt_size );
    return 0;
}
/*
int
send_pkt_flood(node_ *node, char *pkt, unsigned int pkt_size){

    unsigned int i = 0;
    interface_ *intf; 

    for( ; i < MAX_INTF_PER_NODE; i++){

        intf = node->intf[i];
        cout << "testing ..." << endl;
        if(!intf) return 0;
        cout << "test pass " << endl;
        send_pkt_out(pkt, pkt_size, intf);
    }
}
*/

