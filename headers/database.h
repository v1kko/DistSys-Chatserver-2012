#ifndef _DATABASE
#define _DATABASE 1

#include <string>

#define INDIRECT_CLIENT 1
#define DIRECT_CLIENT 2
#define SERVER 3

using namespace std;

typedef struct{
  string* name;
  client_i *next,*prev;
  char authorisatie;
  unsigned short port;
  unsigned long ip;
  int ref;
  char pingtimeout;
}client_d;

typedef struct{
  string* name;
  client_i *next,*prev;
  server *to;
}client_i;

//miss is het handig om hier later ook nog een lijst met clients aan toe te voegen.

typedef struct{
  server_i *next,*prev;
  string * name;
  unsigned long ip;
  unsigned short port;
  unsigned int ref;
  char pingtimeout;
}server;

class Database {
private:
	int nrentries, size;
	entry_t * entries;
public:
	int conClients;	
	Database();
	void* create_new_entry(int type)
	int insert(void* entry, int type);
	int replace(void* entry, int type);
	int return_list(void* entry, int type);
	int look_up_name(strin* koekje);
	int look_up_ip(unsigned long ip);
	void delete_(void* entry, int type);
	
};
#endif
