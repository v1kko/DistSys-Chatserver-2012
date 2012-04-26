#ifndef _DATABASE
#define _DATABASE 1

#include <string>
using namespace std;

#define SERVER 0
#define DCLIENT 1
#define ICLIENT 2


typedef struct entry_t {
	string * name;
	unsigned long ip;
	unsigned short port;
	unsigned int * ref;
	int type;
	entry_t * server;
} entry_t;


typedef struct entry_list_t {
	int nrentries;
	int size;
	entry_t * entries;
} entry_list_t;

class Database {
private:
	entry_list_t * list;
public:
	Database();
	
	entry_t createEntry(string, long, short, int);

	void insert(entry_t);
	void insertReplace(entry_t);
	void insertReplaceWithIp(entry_t);
		
	//returns 1 if found, 0 otherwise
	int lookup(string, entry_t *);

	int lookupServer(unsigned long, unsigned short, entry_t **);
	int lookupDclient(unsigned long, unsigned short, entry_t *);

	void delete_(string);
	void freeEntry(entry_t);
	entry_t * allEntries(int, int *);
	
};
#endif
