#include "parse.h"
#include <algorithm>
#include <cstring>
#include <ctime>
#include <iostream>

using namespace std;

clientRequest & clientRequest::operator=(const clientRequest & rhs) {
    if (this != &rhs) {
        method = rhs.method;
        host = rhs.host;
        port = rhs.port;
        request_line = rhs.request_line;
        host_line = rhs.host_line;
        header = rhs.header;
        target = rhs.target;
        version = rhs.version;
        maxAge = rhs.maxAge;
        noStore = rhs.noStore;
        noCache = rhs.noCache;
        mustRevalidate = rhs.mustRevalidate;
        proxyRevalidate = rhs.proxyRevalidate;
        id = rhs.id;
        ip = rhs.ip;
        request = rhs.request;
        request_len = rhs.request_len;
    }
    return *this;
}

serverResponse & serverResponse::operator=(const serverResponse & rhs) {
    if (this != &rhs) {
        response_line = rhs.response_line;
        version = rhs.version;
        statusCode = rhs.statusCode;
        header = rhs.header;
        recvTime = rhs.recvTime;
        maxAge = rhs.maxAge;
        expireStr = rhs.expireStr;
        isPrivate = rhs.isPrivate;
        noCache = rhs.noCache;
        noStore = rhs.noStore;
        mustRevalidate = rhs.mustRevalidate;
        proxyRevalidate = rhs.proxyRevalidate;
        expires = rhs.expires;
        lastModified = rhs.lastModified;
        etag = rhs.etag;
        response = rhs.response;
        resp_len = rhs.resp_len;
    }
    return *this;
}

//parse the request from client
clientRequest parseClientReq(vector<char *> & chunks, vector<int> request_lenth) {
    int reqLength = request_lenth[0];
    string httpreq(chunks[0], reqLength);
    clientRequest newReq;
    newReq.request = chunks;
    newReq.request_len = request_lenth;
    size_t delim = httpreq.find("\r\n\r\n");
    string headerPart = httpreq.substr(0, delim);
    parseReq(newReq, headerPart);
    return newReq;
}

//parse the response from server
serverResponse parseServerResp(vector<char *> & chunks, vector<int> resp_len) {
    int resLength = resp_len[0];
    
    string httpres(chunks[0], resLength);
    serverResponse newRes;
    newRes.response = chunks;
    newRes.resp_len = resp_len;
    size_t delim = httpres.find("\r\n\r\n");
    string headerPart = httpres.substr(0, delim);
    parseResp(newRes, headerPart);
    return newRes;
}

clientRequest & parseReq(clientRequest & newReq, string & httprequest) {
    size_t first_space = httprequest.find(' ');
    newReq.method = httprequest.substr(0,first_space);
    size_t second_space = httprequest.find(' ',first_space+1);
    newReq.target = httprequest.substr(first_space+1,second_space - (first_space+1));
    size_t first_end = httprequest.find("\r\n");
    newReq.version = httprequest.substr(second_space + 1, first_end - (second_space + 1));
    newReq.request_line = httprequest.substr(0,first_end);
    newReq.header = httprequest.substr(first_end + 2, string::npos);
    size_t host_begin = newReq.header.find("Host: ");
    size_t host_pos = host_begin + 6;
    newReq.host_line = newReq.header.substr(host_begin, newReq.header.find("\r\n", host_begin) - host_begin);
    string hostStr = newReq.header.substr(host_pos, newReq.header.find("\r\n",host_pos)-host_pos);
    size_t hostDelim = hostStr.find(':');
    if (hostDelim == string::npos) {
        newReq.host = hostStr;
        if (newReq.method == "CONNECT")
            newReq.port = "443";
        else
            newReq.port = "80";
    }
    else {
        newReq.host = hostStr.substr(0, hostDelim);
        if (newReq.method == "CONNECT")
            newReq.port = "443";
        else
            newReq.port = hostStr.substr(hostDelim + 1, string::npos);
    }
    newReq.mustRevalidate = (newReq.header.find("must-revalidate") != string::npos);
    newReq.proxyRevalidate = (newReq.header.find("proxy-revalidate") != string::npos);
    size_t agePos = newReq.header.find("max-age=");
    if (agePos == string::npos)
        newReq.maxAge = -1;
    else {
        size_t ageEnd = min(newReq.header.find(",", agePos), newReq.header.find("\r\n", agePos));
        newReq.maxAge = stoll(newReq.header.substr(agePos + 8, ageEnd - (agePos + 8)));
    }
    newReq.noCache = (newReq.header.find("no-cache") != string::npos);
    newReq.noStore = (newReq.header.find("no-store") != string::npos);
    return newReq;
}   

serverResponse & parseResp(serverResponse & newRes, string & serverResp) {
    size_t first_space = serverResp.find(' ');
    newRes.version = serverResp.substr(0,first_space);
    size_t second_space = serverResp.find(' ',first_space+1);
    newRes.statusCode = stoi(serverResp.substr(first_space+1,second_space - (first_space+1)));
    size_t first_end = serverResp.find("\r\n");
    newRes.response_line = serverResp.substr(0, first_end);
    newRes.header = serverResp.substr(first_end+2, string::npos);
    size_t dateBegin = newRes.header.find("Date: ");
    if (dateBegin == string::npos)
        newRes.recvTime = static_cast<long long>(time(NULL));
    else {
        size_t datePos = dateBegin + 6;
        string dateStr = newRes.header.substr(datePos, newRes.header.find("\r\n",datePos)-datePos);
        newRes.recvTime = date2sec(dateStr);
    }
    size_t agePos = newRes.header.find("s-maxage=");
    if (agePos == string::npos) {
        agePos = newRes.header.find("max-age=");
        if (agePos == string::npos)
            newRes.maxAge = -1;
        else {
            size_t ageEnd = min(newRes.header.find(",", agePos), newRes.header.find("\r\n", agePos));
            newRes.maxAge = stoll(newRes.header.substr(agePos + 8, ageEnd - (agePos + 8)));
        }
    }
    else {
        size_t sAgeEnd = min(newRes.header.find(",", agePos), newRes.header.find("\r\n", agePos));
        string maxAgeStr = newRes.header.substr(agePos + 9, sAgeEnd - (agePos + 9));
        if (maxAgeStr == "0")
            newRes.maxAge = -2;
        else
            newRes.maxAge = stoll(maxAgeStr);
    }
    time_t firstExpireTime = newRes.recvTime + newRes.maxAge;
    newRes.isPrivate = (newRes.header.find("private") != string::npos);
    newRes.noCache = (newRes.header.find("no-cache") != string::npos);
    newRes.noStore = (newRes.header.find("no-store") != string::npos);
    newRes.mustRevalidate = (newRes.header.find("must-revalidate") != string::npos);
    newRes.proxyRevalidate = (newRes.header.find("proxy-revalidate") != string::npos);
    size_t expireBegin = newRes.header.find("Expires: ");
    if (expireBegin == string::npos)
        newRes.expires = -1;
    else {
        size_t expirePos = expireBegin + 9;
        string expireStr = newRes.header.substr(expirePos, newRes.header.find("\r\n",expirePos)-expirePos);
        newRes.expires =  date2sec(expireStr);
    }
    time_t secondExpireTime = newRes.expires;
    time_t realExpireTime = min(firstExpireTime, secondExpireTime);
    newRes.expireStr = string(ctime(&realExpireTime));
    size_t lastModifiedBegin = newRes.header.find("Last-Modified: ");
    if (lastModifiedBegin == string::npos)
        newRes.lastModified = "NULL";
    else {
        size_t lastModifiedPos = lastModifiedBegin + 15;
        newRes.lastModified = newRes.header.substr(lastModifiedPos, newRes.header.find("\r\n",lastModifiedPos)-lastModifiedPos);
    }
    size_t etagBegin = newRes.header.find("ETag: ");
    if (etagBegin == string::npos)
        newRes.etag = "NULL";
    else {
        size_t etagPos = etagBegin + 6;
        newRes.etag =  newRes.header.substr(etagPos, newRes.header.find("\r\n",etagPos)-etagPos);
    }
    return newRes;
}

long long date2sec(string & httpDate) {
    struct tm timeInfo;
    memset(&timeInfo, 0, sizeof(timeInfo));
    strptime(httpDate.c_str(), "%a, %d %b %Y %T %Z", &timeInfo);
    return static_cast<long long>(mktime(&timeInfo));
}

void printReqInfo(clientRequest & req) {
    cout << "Client request:" << endl;
    cout << "method: " << req.method << endl;
    cout << "host: " << req.host << endl;
    cout << "port: " << req.port << endl;
    cout << "request_line: " << req.request_line << endl;
    cout << "host_line: " << req.host_line << endl;
    cout << "header: " << req.header << endl;
    cout << "target: " << req.target << endl;
    cout << "version: " << req.version << endl;
    cout << "maxAge: " << req.maxAge << endl;
    cout << "noStore: " << req.noStore << endl;
    cout << "noCache: " << req.noCache << endl;
    cout << "mustRevalidate: " << req.mustRevalidate << endl;
    cout << "proxyRevalidate: " << req.proxyRevalidate << endl;
    cout << "request_len: " << req.request_len[0] << endl;
    cout << "Client id: "<< req.id <<endl;
    cout << "Client ip: "<< req.ip <<endl;
}

void printRespInfo(serverResponse & resp) {
    cout << "Server Response:" << endl;
    cout << "version: " << resp.version << endl;
    cout << "statusCode: " << resp.statusCode << endl;
    cout << "header: " << resp.header << endl;
    cout << "recvTime: " << resp.recvTime << endl;
    cout << "maxAge: " << resp.maxAge << endl;
    cout << "isPrivate: " << resp.isPrivate << endl;
    cout << "noCache: " << resp.noCache << endl;
    cout << "noStore: " << resp.noStore << endl;
    cout << "mustRevalidate: " << resp.mustRevalidate << endl;
    cout << "proxyRevalidate: " << resp.proxyRevalidate << endl;
    cout << "expires: " << resp.expires << endl;
    cout << "lastModified: " << resp.lastModified << endl;
    cout << "etag: " << resp.etag << endl;
    cout << "resp_len: " << resp.resp_len[0] << endl;
}
