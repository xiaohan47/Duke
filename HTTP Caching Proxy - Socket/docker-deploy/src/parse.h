#include <string>
#include <vector>
using namespace std;

class clientRequest{
public:
    clientRequest & operator=(const clientRequest & rhs);
    string method;
    string host;
    string port;
    string request_line;
    string host_line;
    string header;
    string target;
    string version;
    long long maxAge;
    bool noStore;
    bool noCache;
    bool mustRevalidate;
    bool proxyRevalidate;
    int id;
    string ip;
    vector<char *> request;
    vector<int> request_len;       
};

class serverResponse{
public:
    serverResponse & operator=(const serverResponse & rhs);
    string response_line;
    string version;
    int statusCode;
    string header;
    long long recvTime;
    long long maxAge;
    string expireStr;
    bool isPrivate;
    bool noCache;
    bool noStore;
    bool mustRevalidate;
    bool proxyRevalidate;
    long long expires;
    string lastModified;
    string etag;
    vector<char *> response;
    vector<int> resp_len;
};

clientRequest parseClientReq(vector<char *> & httpreq, vector<int> request_len);
serverResponse parseServerResp(vector<char *> & httpres, vector<int> resp_len);
clientRequest & parseReq(clientRequest & newReq, string & httprequest);
serverResponse & parseResp(serverResponse & newRes, string & serverResp);
long long date2sec(string & httpDate);
void printReqInfo(clientRequest & req);
void printRespInfo(serverResponse & resp);
