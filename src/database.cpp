#include <cstdlib>
#include <cstdio>
#include <database.h>
#include <cstring>

using namespace std;


/** TO DO
 *  replace
 *  lookup
 */


Database::Database(){
	begin_clients_indirect=NULL;
	end_clients_indirect=NULL;
	begin_clients_direct=NULL;
	end_clients_direct=NULL;
	begin_servers_direct=NULL;
	end_servers_direct=NULL;
}

void* create_new_entry(int type){
  switch(type){
    case 1:{
      client_i* output = (client_i*)calloc(1,sizeof(client_i));
   	  return output;
    }break;
    
    case 2:{
      client_d* output = (client_d*)calloc(1,sizeof(client_d));
      return output;
    }break;
    
    case 3:{
      server* output = (server*)calloc(1,sizeof(server));
      return output;
    }break;
 
    default:{
      return NULL;
    }break;
  }
}

/*
 * checkt nu alleen op socket/ip miss ook wel handig om ook met ref te controlleren.
 * 
 */


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
      client_i * temp;
      if(end_clients_indirect==NULL){
	      begin_clients_indirect = end_clients_indirect = (client_i*)entry;
	      return 0;
      }
      else{
	      temp=begin_clients_indirect;
	      while(temp->next != NULL){
//TODO    if(strcmp(temp->name,((client_i*)entry)->name)==0) return 1; 
	        temp=temp->next;
    	  }
	      end_clients_indirect->next=(client_i*)entry;
	      return 0;
      }
    }break;
    
    case 2:{
      client_d * temp;
      if(end_clients_direct==NULL){
	begin_clients_direct = end_clients_direct = (client_d*)entry;
	return 0;
      }
      else{
	temp=begin_clients_direct;
	while(temp->next != NULL){
//TODO	  if(strcmp(temp.name,(client_d*)entry.name==0)return 1; 
	  temp=temp->next;
	}
	end_clients_direct->next=(client_d*)entry;
	return 0;
      }
    }break;

    case 3:{
      server * temp;
      if(end_servers_direct==NULL){
	begin_servers_direct = end_servers_direct = (server*)entry;
	return 0;
      }
      else{
	temp=begin_servers_direct;
	while(temp->next != NULL){
//TODO	  if(temp.socket == (server*)entry.socket) return 1;
	  temp=temp->next;
	}
	end_servers_direct->next=(server*)entry;
	return 0;
      }
    }break;
    
    default: {
      return 1;
    }break;
  }
}

int Database::delete_(void* entry, int type){
  switch(type){
    case 1:{
            client_i * temp;
      temp=begin_clients_indirect;
      while(temp->next != NULL){
//TODO        if(strcmp(temp.name,(client_i*)entry.name==0){
	  temp->prev->next=temp->next;
	  temp->next->prev=temp->prev;
	  delete temp;
	  return 0;
//	} 
        temp=temp->next;
      }  
      return 1;
    }break;
    
    case 2:{
      client_d * temp;
      temp=begin_clients_direct;
      while(temp->next != NULL){
//TODO        if(strcmp(temp.name,(client_d*)entry.name==0){
	  temp->prev->next=temp->next;
	  temp->next->prev=temp->prev;
	  delete temp;
	  return 0;
//	} 
        temp=temp->next;
      }  
      return 1;
    
    }break;
        
    case 3:{
            server * temp;
      temp=begin_servers_direct;
      while(temp->next != NULL){
//TODO        if(temp.socket==(server_d*)entry.socket){
	  temp->prev->next=temp->next;
	  temp->next->prev=temp->prev;
	  delete temp;
	  return 0;
//	} 
        temp=temp->next;
      }  
      return 1;
    
    }break;
    
    default:{
      return 1;
    }break;
  }

}

int Database::replace(void* entry, int type) {
return 0;
}
int Database::return_list(void* entry, int type) {
return 0;
}
int Database::look_up_name(string* koekje) {
return 0;
}
int Database::look_up_ip(unsigned long ip) {
return 0;
}


//hieronder nog vervangen


/*
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
}*/
