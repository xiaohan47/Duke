#include <iostream>
#include <cstring>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include "cache.h"
#include <string.h>

void runProxy();
int set_server();
void reqProcess(int client_fd,string client_ip,int log_id, LRUCache * proxy_cache);
int proxyServer(clientRequest & req);
serverResponse receive_server(int server_socket);
serverResponse forwardReceive(int client_fd, int server_socket, clientRequest &req);
void send_loop(int fd, vector<char *> send_vector,vector<int> request_len);
void reqPOST(int client_fd,int server_socket,clientRequest & req);
void reqCONNECT(int client_fd,int server_socket,clientRequest & req);
void reqGET(int client_fd,int server_socket,clientRequest & req,LRUCache * proxy_cache);
void revalidationProcess(clientRequest &request, serverResponse &response, int client_fd,
                         int server_socket, LRUCache * proxy_cache);