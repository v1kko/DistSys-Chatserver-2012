#ifndef _DATABASE
#define _DATABASE 1

#include <string>
using namespace std;

typedef struct {
	string * name;
	unsigned long ip;
	unsigned short port;
	unsigned int * ref;
	char directlyconnected;
	char isClient;	
} entry_t;


class Database {
private:
	int nrentries, size;
	entry_t * entries;
public:
	int conClients;	
	Database();
	
	void insert(entry_t);
	void insertReplace(entry_t);
	void insertReplaceWithIp(entry_t);
	//returns 1 if found, 0 otherwise
	int lookup(string, entry_t *);
	int lookup(unsigned long, unsigned short, entry_t *);
	void delete_(string);
	entry_t * allEntries(int *);
	
};
#endif
