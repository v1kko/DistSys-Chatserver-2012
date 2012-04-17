#include <cstdlib>
#include <cstdio>
#include <database.h>

using namespace std;


/** TO DO
 *  delete
 *  -efficiencie
 *  -gaten
 *   - dualy linked list miss handiger dan een array??
 *  replace
 */


Database::Database(){
	nclients_indirect = 0;
	nclients_direct = 0;
	nservers_indirect = 0;
	nservers_direct = 0;
	size_clients_indirect = 10;
	size_clients_direct = 10;
	size_servers_indirect = 10;
	size_servers_direct = 10;
	clients_indirect = (client_i *)malloc(sizeof(client_i) * size_clients_indirect);
	clients_direct = (client_d *)malloc(sizeof(client_d) * size_clients_direct);
	servers_indirect = (server_i *)malloc(sizeof(server_i) * size_servers_indirect);
	servers_direct = (server_d *)malloc(sizeof(server_d) * size_servers_direct);
}

int Database::insert(void* entry, int type){
  int i,error=0;
  /*
   * 1 = indirectly connected clients
   * 2 = directly connected clients
   * 3 = indirectly connected servers
   * 4 = directly connected servers
   */
  switch(type){
    case 1:{
      for(i=0; i<nclients_indirect; i++){
	if ((*clients_indirect[i].name).compare((client_i*)entry.name) == 0)return -1;
      }
      if(nclients_indirect==size_clients_indirect)
        clients_indirect = (client_i *)realloc(clients_indirect,sizeof(client_i) * (size_clients_indirect *= 2));
      clients_indirect[nclients_indirect++]=(client_i*)entry; 
      return 0;
    }break;
    
    case 2:{
      for(i=0; i<nclients_direct; i++){
	if ((*clients_direct[i].name).compare((client_d*)entry.name) == 0) return -1;
      }
      if(nclients_direct==size_clients_direct)
        clients_direct = (client_i *)realloc(clients_direct,sizeof(client_d) * (size_clients_indirect *= 2));
      clients_indirect[nclients_direct++]=(client_d*)entry; 
      return 0;
    }break;
    
    case 3:{
      for(i=0; i<nservers_indirect; i++){
	if(servers_indirect[i].socket==(server_i*)entry.socket)return -1;
      }
      if(nservers_indirect==size_servers_indirect)
	servers_indirect = (server_i*)realloc(servers_indirect,sizeof(servers_indirect) * (size_servers_indirect *=2));
      servers_indirect[nservers_indirect++]=(server_i*)entry;
    }break;
    
    case 4:{
      for(i=0; i<nservers_direct; i++){
	if(servers_direct[i].socket==(server_d*)entry.socket)return -1;
      }
      if(nservers_direct==size_servers_direct)
	servers_direct = (server_i*)realloc(servers_direct,sizeof(servers_direct) * (size_servers_direct *=2));
      servers_direct[nservers_direct++]=(server_d*)entry;
    
    }break;
  }
}

//hieronder nog vervangen

void Database::insertReplace(entry_t entry)
{
	entry_t entry1;
	if (this->lookup(*entry.name, &entry1)){
		this->delete_(*entry1.name);
	}	
	entry.ref = (unsigned int *)malloc(sizeof(int));
	*entry.ref = 0;
	if (size == nrentries)
		entries = (entry_t *)realloc(entries, sizeof(entry_t) * (size *= 2));
	entries[nrentries++] = entry;
}

void Database::insertReplaceWithIp(entry_t entry)
{
	entry_t entry1;
	if (this->lookup(*entry.name, &entry1) || this->lookup(entry.ip, entry.port, &entry1)){
		this->delete_(*entry1.name);
	}	
	entry.ref = (unsigned int *)malloc(sizeof(int));
	*entry.ref = 0;
	if (size == nrentries)
		entries = (entry_t *)realloc(entries, sizeof(entry_t) * (size *= 2));
	entries[nrentries++] = entry;
}

int Database::lookup(string name, entry_t * entry){
	for (int i = 0 ; i < nrentries ; i++) {
		if ((*entries[i].name).compare(name) == 0) {
			if (entry != NULL)
				*entry = entries[i];
			return 1;	
		}
	}
	return 0;
}

int Database::lookup(unsigned long ip, unsigned short port, entry_t * entry)
{
	for (int i = 0 ; i < nrentries ; i++) {
		if (entries[i].ip == ip && entries[i].port == port) {
			if (entry != NULL)
				*entry = entries[i];
			return 1;	
		}
	}
	return 0;
}

void Database::delete_(string name)
{
	for (int i = 0 ; i < nrentries ; i++) {
		if ((*entries[i].name).compare(name) == 0) {
			free(entries[i].ref);
			delete entries[i].name;
			entries[i]=entries[nrentries-1];
			nrentries--;
			return;	
		}
	}
	if (size == nrentries*2)
		entries = (entry_t *)realloc(entries, sizeof(entry_t) * (size /= 2));
	return;
}

entry_t * Database::allEntries(int * s)
{
	* s = nrentries;
	return entries;
}
