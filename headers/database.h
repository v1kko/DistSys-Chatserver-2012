#ifndef _DATABASE
#define _DATABASE 1

#include <string>
using namespace std;

typedef struct{
  string* name;
  int authorisatie;
  unsigned short socket;
}client_d;

typedef struct{
  string* name;
  unsigned long ip;
  unsigned short port;
}client_i;

typedef struct{
  unsigned short socket;
  int is_parent;
  unsigned int* ref;
}server_d;

//miss is het handig om hier later ook nog een lijst met clients aan toe te voegen.

typedef struct{
  unsigned long ip;
  unsigned short port;
  unsigned int* ref;
}server_i;


class Database {
private:
	int nrentries, size;
	entry_t * entries;
public:
	int conClients;	
	Database();
	
	void insert(void* entry, int type);
	void insertReplace(void* entry, int type);
	void insertReplaceWithIp(void* entry, int type);
	//returns 1 if found, 0 otherwise
	int lookup(void* entry, int type);
	void delete_(void* entry, int type);
	
};
#endif
