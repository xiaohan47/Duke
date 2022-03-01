
#include <unordered_map>
#include <utility>
#include <list>
#include <pthread.h>
#include <unistd.h>
#include <cstdlib>
#include "parse.h"


using namespace std;

typedef string K;
typedef serverResponse unit;
typedef list<pair<K, unit> >::iterator V;
typedef unordered_map<K, V>::iterator mapIter;

class LRUCache{
public:
    LRUCache(size_t capacity);
    serverResponse * checkInCache(clientRequest & req);
    void updateCache(clientRequest & req, serverResponse & res);
    void addCache(clientRequest & req, serverResponse & res);
    bool isValid(clientRequest & req);

private:
    size_t cap;
    list<pair<K, unit> > kvList;
    unordered_map<K, V> m;
    pthread_mutex_t lock;
};

string req2key(clientRequest & req);
bool checkValid(long long recvTime, long long maxAge);
bool checkValid(long long expires);
