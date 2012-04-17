#ifndef _DATABASE
#define _DATABASE 1

#include <string>
using namespace std;

typedef struct{
  string* name;
  client_i *next,*prev;
  int authorisatie;
  unsigned short socket;
}client_d;

typedef struct{
  string* name;
  client_i *next,*prev;
  server_i *to;
}client_i;

//systeem voor herkennen unieke servers. ID waarschijnlijk het makkelijkste.

typedef struct{
  server_d *next,*prev;
  unsigned short socket;
  int is_parent;
  unsigned int ref;
}server_d;

//miss is het handig om hier later ook nog een lijst met clients aan toe te voegen.

typedef struct{
  server_i *next,*prev;
  unsigned long ip;
  unsigned short port;
  unsigned int ref;
}server_i;


/*
 * 1 = indirectly connected clients
 * 2 = directly connected clients
 * 3 = indirectly connected servers
 * 4 = directly connected servers
 */

class Database {
private:
	int nrentries, size;
	entry_t * entries;
public:
	int conClients;	
	Database();
	void* create_new_entry(int type)
	void insert(void* entry, int type);
	void insertReplace(void* entry, int type);
	void insertReplaceWithIp(void* entry, int type);
	//returns 1 if found, 0 otherwise
	int lookup(void* entry, int type);
	void delete_(void* entry, int type);
	
};
#endif
