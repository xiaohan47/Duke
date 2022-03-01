#include "cache.h"
#include <iostream>
#include <algorithm>
#include <ctime>
using namespace std;

//set the cap of the cache
LRUCache::LRUCache(size_t capacity) : cap(capacity) {
	lock = PTHREAD_MUTEX_INITIALIZER;
}

//check if a request is in the cache
unit * LRUCache::checkInCache(clientRequest & req) {
	pthread_mutex_lock(&lock);
	mapIter it = m.find(req2key(req));
	if (it == m.end()) {
		pthread_mutex_unlock(&lock);
		return NULL;
	}
	kvList.splice(kvList.begin(), kvList, it->second);
	pthread_mutex_unlock(&lock);
	return &(it->second->second);
}

//update a cache if the server send updated response
void LRUCache::updateCache(clientRequest & req, serverResponse & res) {
	pthread_mutex_lock(&lock);
	mapIter it = m.find(req2key(req));
	if (it == m.end()) {
		pthread_mutex_unlock(&lock);
		addCache(req, res);
	}
	it->second->second = res;
	kvList.splice(kvList.begin(), kvList, it->second);
	pthread_mutex_unlock(&lock);
}

//add new response to cache
void LRUCache::addCache(clientRequest & req, serverResponse & res) {
	if (req.noStore || res.noStore || res.isPrivate || res.statusCode != 200) {
		cout << "This response is not allowed to be stored in cache" << endl;
		return;
	}
	if (kvList.size() >= cap) {
		pthread_mutex_lock(&lock);
		if (kvList.size() >= cap) {
			m.erase(kvList.back().first);
			kvList.pop_back();
		}
		pthread_mutex_unlock(&lock);
	}
	pthread_mutex_lock(&lock);
	kvList.emplace_front(req2key(req), res);
	//m[newKey] = kvList.begin();
	m[req2key(req)] = kvList.begin();
	pthread_mutex_unlock(&lock);
	
}

//check if a given request already exist in the cache
bool LRUCache::isValid(clientRequest & req) {
	pthread_mutex_lock(&lock);
	mapIter it = m.find(req2key(req));
	if (it == m.end()) {
		cerr << "Error: No cached response for this request" << endl;
		exit(EXIT_FAILURE);
	}
	unit & res = it->second->second;
	pthread_mutex_unlock(&lock);
	if (req.noCache || res.noCache || req.mustRevalidate || res.mustRevalidate || req.proxyRevalidate
		|| res.proxyRevalidate || res.maxAge == -2)
		return false;
	if (req.maxAge == -1 && res.maxAge == -1) {
		if (res.expires == -1)
			return true; // Regard it as valid ???
		return checkValid(res.expires);
	}
	else if (req.maxAge == -1)
		return checkValid(res.recvTime, res.maxAge);
	else if (res.maxAge == -1)
		return checkValid(res.recvTime, req.maxAge);
	return checkValid(res.recvTime, min(res.maxAge, req.maxAge));
}

string req2key(clientRequest & req) {
	return req.target;
}

//checkc time valid
bool checkValid(long long recvTime, long long maxAge) {
	if (recvTime + maxAge >= static_cast<long long>(time(NULL)))
		return true;
	return false;
}

//checkc expires time valid
bool checkValid(long long expires) {
	if (expires >= static_cast<long long>(time(NULL)))
		return true;
	return false;
}