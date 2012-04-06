#ifndef _DATABASE
#define _DATABASE 1

#include <string>
using namespace std;

typedef struct {
	string * name;
	int ip;
	int port;	
} entry_t;


class Database {
private:
	int nrentries, size;
	entry_t * entries;
public:	
	Database();
	
	void insert(entry_t);
	
	//returns 1 if found, 0 otherwise
	int lookup(string, entry_t *);
	void delete_(string);
	
	
	
};
#endif
