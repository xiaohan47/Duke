#include "proxy.h"
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <mutex>
#include <cstring>
#include <iostream>
#include <thread>
#include <arpa/inet.h>
#include <fstream>
#include <ctime>

using namespace std;
ofstream logFile("proxy.log");
mutex my_mutex;

//Start the proxy
void runProxy() {
    int log_id=0;
    LRUCache proxy_cache(1000);
    // set up the proxy socket.   
    int socket_fd = set_server();

    // concurrency implementation
    while (true) {
        struct sockaddr_storage socket_addr;
        socklen_t socket_addr_len = sizeof(socket_addr);
        int client_connection_fd;
        client_connection_fd = accept(
            socket_fd, (struct sockaddr *)&socket_addr, &socket_addr_len);
        if (client_connection_fd == -1) {
            my_mutex.lock();
            logFile <<"(no-id): ERROR in RUN PROXY, cannot accept connection on socket"<<endl;
            my_mutex.unlock();
            continue;
        }

        struct sockaddr_in * addr = (struct sockaddr_in *)&socket_addr;
        string client_ip = inet_ntoa(addr->sin_addr);

        //create new thread for each accept connection, then send to reqProcess to handle single thread
        thread new_request(reqProcess, client_connection_fd, client_ip, log_id,& proxy_cache);
        new_request.detach();
        log_id++;
    } 
}

//set proxy server 
int set_server(){
    int status;
    int socket_fd;
    struct addrinfo host_info;
    struct addrinfo *host_info_list;
    const char *hostname = NULL;
    const char *port = "12345";

    memset(&host_info, 0, sizeof(host_info));
    host_info.ai_family = AF_UNSPEC;
    host_info.ai_socktype = SOCK_STREAM;
    host_info.ai_flags = AI_PASSIVE;

    status = getaddrinfo(hostname, port, &host_info, &host_info_list);
    if (status != 0) {
        my_mutex.lock();
        logFile <<"(no-id): ERROR in proxy SER SERVER, cannot get address info for proxy"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    socket_fd = socket(host_info_list->ai_family, host_info_list->ai_socktype,
                       host_info_list->ai_protocol);
    if (socket_fd == -1) {
        my_mutex.lock();
        logFile <<"(no-id): ERROR in proxy SER SERVER, cannot create socket"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    int yes = 1;
    status = setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int));
    status =
        bind(socket_fd, host_info_list->ai_addr, host_info_list->ai_addrlen);
    if (status == -1) {
        my_mutex.lock();
        logFile <<"(no-id): ERROR in proxy SER SERVER, cannot bind socket"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot bind socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }

    status = listen(socket_fd, 100);
    if (status == -1) {
        my_mutex.lock();
        logFile <<"(no-id): ERROR in proxy SER SERVER, cannot listen on socket"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot listen on socket" << endl;
        cerr << "  (" << hostname << "," << port << ")" << endl;
        exit(EXIT_FAILURE);
    }
    freeaddrinfo(host_info_list);
    return socket_fd;
}

//Handle single request from client
void reqProcess(int client_fd, string client_ip, int log_id, LRUCache * proxy_cache) {
    //Receive Client Request    
    vector<char *> client_req;
    vector<int> request_len;
    while(true){
        char receive_buffer[65535];
        memset(receive_buffer, 0, sizeof(receive_buffer));
        int lenth = recv(client_fd, receive_buffer, sizeof(receive_buffer), 0);
        if(lenth<=0){
            my_mutex.lock();
            logFile <<log_id<<": ERROR in proxy RECEIVE, multiple chunks, Client Connection Closed"<<endl;
            my_mutex.unlock();
            return;
        }
        client_req.push_back(receive_buffer);
        request_len.push_back(lenth);
        char * pch;
        pch = strstr (receive_buffer,"\r\n\r\n");
        if(pch!=NULL){
            break;
        }
    }
    clientRequest req = parseClientReq(client_req,request_len);
    
    //adding id and ip, print to log
    req.id = log_id;
    req.ip = client_ip;
    time_t now = time(0);
    char * dt = ctime(&now);
    my_mutex.lock();
    logFile <<req.id<<": \""<<req.request_line<<"\" from "<<req.ip<<" @ "<< dt;
    my_mutex.unlock();
    
    //connect to server socket
    int server_socket = proxyServer(req);
    if(server_socket == -1){
        return;
    }

    //Based on differnet method, choose the correct next step
    string post = "POST";
    string connect = "CONNECT";
    string get = "GET";
    if (req.method.compare(post) == 0) {
        reqPOST(client_fd, server_socket, req);
    }
    else if (req.method.compare(connect)==0) {
        reqCONNECT(client_fd, server_socket, req);
    }
    else if (req.method.compare(get)==0) {
        reqGET(client_fd, server_socket, req, proxy_cache);
    } 
    else {
        return;
    }
}

//Connect to server 
int proxyServer(clientRequest &req) {
    int status;
    int socket_server_fd;
    struct addrinfo server_info;
    struct addrinfo *server_info_list;

    const char *serverport = req.port.c_str();
    const char *servername = req.host.c_str();

    memset(&server_info, 0, sizeof(server_info));
    server_info.ai_family = AF_UNSPEC;
    server_info.ai_socktype = SOCK_STREAM;

    status =
        getaddrinfo(servername, serverport, &server_info, &server_info_list);
    if (status != 0) {
        my_mutex.lock();
        logFile <<req.id<<": ERROR in CONNECT SERVER, cannot get address info for host"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot get address info for host" << endl;
        cerr << "  (" << servername << "," << serverport << ")" << endl;
        return -1;
    }

    socket_server_fd =
        socket(server_info_list->ai_family, server_info_list->ai_socktype,
               server_info_list->ai_protocol);
    if (socket_server_fd == -1) {
        my_mutex.lock();
        logFile <<req.id<<": ERROR in CONNECT SERVER, cannot create socket"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot create socket" << endl;
        cerr << "  (" << servername << "," << serverport << ")" << endl;
        return -1;
    }

    status = connect(socket_server_fd, server_info_list->ai_addr,
                     server_info_list->ai_addrlen);
    if (status == -1) {
        my_mutex.lock();
        logFile <<req.id<<": ERROR in CONNECT SERVER, cannot connect to socket"<<endl;
        my_mutex.unlock();
        cerr << "Error: cannot connect to socket" << endl;
        cerr << "  (" << servername << "," << serverport << ")" << endl;
        return -1;
    }

    freeaddrinfo(server_info_list);
    return socket_server_fd;
}

//receive the response from server
serverResponse receive_server(int server_socket){   
    vector<char *> server_resp;
    vector<int> response_len;
    while(true){
        char resp_buffer[65535];
        memset(resp_buffer, 0, sizeof(resp_buffer));
        int lenth = recv(server_socket, resp_buffer, sizeof(resp_buffer), 0);
        server_resp.push_back(resp_buffer);
        response_len.push_back(lenth);
        //only ending with \r\n\r\n indicates the end of receive
        char * pch;
        pch = strstr (resp_buffer,"\r\n\r\n");
        if(pch!=NULL){
            break;
        }
    }
    serverResponse resp = parseServerResp(server_resp, response_len);   
    return resp;
}

//Function to handle the POST and simple GET
serverResponse forwardReceive(int client_fd, int server_socket, clientRequest &req) {
    my_mutex.lock();
    logFile << req.id << ": Requesting "<<"\""<< req.request_line << "\"" <<" from "<<req.host<<endl;
    my_mutex.unlock();
    
    send_loop(server_socket,req.request,req.request_len);
    
    serverResponse resp = receive_server(server_socket);
    my_mutex.lock();
    logFile << req.id << ": Received "<<"\""<< resp.response_line << "\"" <<" from "<<req.host<<endl;
    my_mutex.unlock();
    
    my_mutex.lock();
    logFile << req.id << ": Responding "<<"\""<< resp.response_line << "\"" <<endl;
    my_mutex.unlock();
    send_loop(client_fd,resp.response,resp.resp_len);
    
    close(client_fd);
    close(server_socket);
    return resp;
}

//Send chunks to server or client;
void send_loop(int fd, vector<char *> send_vector,vector<int> request_len) {
    for (size_t i = 0; i < send_vector.size(); i++) {
        int s;
        int len = request_len[i];
        while(len>0){
            s = send(fd, send_vector[i], len, 0);
            len -= s;
            send_vector[i] += s;
        }
    }  
}

// Method: POST, send client request to server, forward server response to
void reqPOST(int client_fd, int server_socket, clientRequest &req) {
    serverResponse resp = forwardReceive(client_fd,server_socket,req);
}

// Method: CONNECT
void reqCONNECT(int client_fd, int server_socket, clientRequest &req) {
    // send back 200OK to client
    string send_client = "HTTP/1.1 200 OK\r\n\r\n";
    send(client_fd, send_client.c_str(), send_client.size(), 0);
    my_mutex.lock();
    logFile << req.id << ": Responding "<<"\" HTTP/1.1 200 OK\"" <<endl;
    my_mutex.unlock();
    
    fd_set readfds;
    int numfds;
    if (client_fd > server_socket) {
        numfds = client_fd + 1;
    } else {
        numfds = server_socket + 1;
    }

    while (true) {
        FD_ZERO(&readfds);
        FD_SET(server_socket, &readfds);
        FD_SET(client_fd, &readfds);

        int sel = select(numfds, &readfds, NULL, NULL, NULL);
        if (sel == -1) {
            cerr << "CONNECT ERROR." << endl;
            return;
        }
        else if(sel == 0){
            continue;
        }
        else {
            int fds[2] = {client_fd, server_socket};
            for (int i = 0; i < 2; i++) {
                if (FD_ISSET(fds[i], &readfds)) {
                    // loop receive all
                    while (true) {
                        char receive_buffer[65535];
                        memset(receive_buffer, 0, sizeof(receive_buffer));
                        int rec_fd= recv(fds[i], receive_buffer,
                                        sizeof(receive_buffer), 0);
                        
                        if (rec_fd < 0) {
                            cout << "CONNECT ENDS" << endl;
                            return;
                        }
                        // recv == 0, connection closed
                        else if (rec_fd == 0) {
                            close(client_fd);
                            close(server_socket);
                            my_mutex.lock();
                            logFile << req.id << ": Tunnel closed"<<endl;
                            my_mutex.unlock();
                            return;
                        } 
                        else if(rec_fd<65535){
                          send(fds[1 - i], receive_buffer, rec_fd, 0);
                          break;
                        }
                        else{
                            send(fds[1 - i], receive_buffer, rec_fd, 0);
                        }
                    }
                    
                }
            }
        }
    }
}

// Method: GET
void reqGET(int client_fd, int server_socket, clientRequest &req,
            LRUCache * proxy_cache) {        
    unit *valid_response = proxy_cache->checkInCache(req);
    // if not in cache
    if (valid_response == NULL) {
        my_mutex.lock();
        logFile << req.id << ": not in cache"<<endl;
        my_mutex.unlock();
        serverResponse resp = forwardReceive(client_fd,server_socket,req);
        //If the response does not need cache
        if (req.noStore || resp.noStore || resp.isPrivate){
            string s;
            if(req.noStore){
                s = "Client request no store in header";
            }
            else if(resp.noStore){
                s = "Server response no store";
            }
            else{
                s = "Server response is private";
            }
            my_mutex.lock();
            logFile << req.id << ": not cacheable because "<<s<<endl;
            my_mutex.unlock();
        }
        //If the response need cache
        else{
            proxy_cache->addCache(req, resp);
            if(req.mustRevalidate){
                my_mutex.lock();
                logFile << req.id << ": cached, but requires re-validation "<<endl;
                my_mutex.unlock();
            }
            else{
                my_mutex.lock();
                logFile << req.id << ": cached, expires at "<<resp.expireStr;
                my_mutex.unlock();
            }
        }
    }
    // if in cache
    else {
        // the response stored in cache is not expired.
        if (proxy_cache->isValid(req)) {
            my_mutex.lock();
            logFile << req.id << ": in cache, valid"<<endl;
            logFile << req.id << ": Responding "<<"\""<< valid_response->response_line << "\"" <<endl;
            my_mutex.unlock();
            send_loop(client_fd,valid_response->response,valid_response->resp_len);
            close(client_fd);
            close(server_socket);
        }
        // the response stored in cache is expired.
        else {
            my_mutex.lock();
            logFile << req.id << ": in cache, requires validation"<<endl;
            my_mutex.unlock();
            revalidationProcess(req, *valid_response, client_fd, server_socket, proxy_cache);
        }
    }
}

//Revalidation Process 
void revalidationProcess(clientRequest &request, serverResponse &response, int client_fd,
                         int server_socket, LRUCache * proxy_cache) {
    //create revalidation message
    string revalid_request = request.request_line;
    revalid_request.append("\r\n");
    revalid_request.append(request.host_line);
    revalid_request.append("\r\n");
    if (response.etag != "NULL") {
        revalid_request.append("If-None-Match: ");
        revalid_request.append(response.etag);
        revalid_request.append("\r\n");
    }
    if (response.lastModified != "NULL") {
        revalid_request.append("If-Modified-Since: ");
        revalid_request.append(response.lastModified);
    }
    
    char* request_buffer = new char[revalid_request.length()+1];
    strcpy(request_buffer, revalid_request.c_str());
    
    //send the revalidation message to server
    my_mutex.lock();
    logFile << request.id << ": Requesting "<<"\""<< request.request_line << "\"" <<endl;
    my_mutex.unlock();

    send(server_socket, request_buffer, sizeof(request_buffer), 0);

    //receive the server response
    serverResponse response_server = receive_server(server_socket);

    my_mutex.lock();
    logFile << request.id << ": Received "<<"\""<< response_server.response_line << "\"" <<endl;
    my_mutex.unlock();
    
    if (response_server.statusCode == 304) {
        // send to client,close sockets
        my_mutex.lock();
        logFile << request.id << ": Responding "<<"\""<< response_server.response_line << "\"" <<endl;
        my_mutex.unlock();
        send_loop(client_fd,response_server.response,response_server.resp_len);
    } 
    else if (response_server.statusCode == 200) {
        // update and send
        proxy_cache->updateCache(request, response_server);
        my_mutex.lock();
        logFile << request.id << ": Responding "<<"\""<< response_server.response_line << "\"" <<endl;
        my_mutex.unlock();
        send_loop(client_fd,response_server.response,response_server.resp_len);
    } 
    else {
        cout << "Response from Server Error." << endl;
        return;
    }
    close(client_fd);
    close(server_socket);
}