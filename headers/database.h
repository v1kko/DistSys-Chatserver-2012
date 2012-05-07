#ifndef _DATABASE
#define _DATABASE 1

#include <string>
using namespace std;

#define SERVER 0
#define DCLIENT 1
#define ICLIENT 2

#define PINGTIMEOUT 3

typedef struct entry_t {
	string * name;
	unsigned long ip;
	unsigned short port;
	unsigned int * ref;
	char * pingtimeout;
	int type;
} entry_t;


typedef struct entry_list_t {
	int nrentries;
	int size;
	entry_t * entries;
} entry_list_t;

class Database {
private:
	entry_list_t * list;
	unsigned int maxclients;
#define nrclients list[0].nrentries + list[1].nrentries + list[2].nrentries 

public:
	Database(unsigned int);
	
	entry_t createEntry(string, unsigned long, unsigned short, int);

	int insert(entry_t);
	int insertReplace(entry_t);
	int insertReplaceWithIp(entry_t);
		
	//returns 1 if found, 0 otherwise
	int lookup(string, entry_t *);

	int lookupServer(unsigned long, unsigned short, entry_t **);
	int lookupDclient(unsigned long, unsigned short, entry_t *);
	int lookupIclient(unsigned long, unsigned short, entry_t *);

	void delete_(string);
	void freeEntry(entry_t);
	entry_t * allEntries(int, int *);
	
};
#endif
