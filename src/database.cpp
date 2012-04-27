#include <cstdlib>
#include <cstdio>
#include <database.h>

using namespace std;

Database::Database()
{
	list = (entry_list_t *)calloc(3, sizeof(entry_list_t));
	for (int i = 0 ; i < 3 ; i++) {
		list[i].size = 10;
		list[i].entries =  (entry_t *)malloc(sizeof(entry_t) * 10);
	}
}
	
entry_t Database::createEntry(string name, long ip, short port, int type) {
	entry_t entry, * entry1;
	entry.name = 0;
	entry.ref = 0;
	if (type == ICLIENT) {
		if (!lookupServer(ip, port, &entry1)) {
			return entry;
		}
	}
	entry.name = new string(name);
	entry.ref = (unsigned int *)calloc(1, sizeof(unsigned int));
	entry.pingtimeout = (char *)malloc(sizeof(char));
	*entry.pingtimeout = PINGTIMEOUT;
	entry.ip = ip;
	entry.port = port;
	entry.type = type;
	return entry;	
}
void Database::insert(entry_t entry)
{
	int type = entry.type;
	entry_list_t * db = &list[type];
	if (this->lookup(*entry.name, NULL)) {
		this->freeEntry(entry);
		return;
	}
	
	if (db->size == db->nrentries)
		db->entries = (entry_t *)realloc(db->entries, sizeof(entry_t) * (db->size *= 2));
	db->entries[db->nrentries++] = entry;
}

void Database::insertReplace(entry_t entry)
{
	int type = entry.type;
	entry_list_t * db = &list[type];
	entry_t entry1;
	if (this->lookup(*entry.name, &entry1)) {
		this->delete_(*entry.name);
	}
	
	if (db->size == db->nrentries)
		db->entries = (entry_t *)realloc(db->entries, sizeof(entry_t) * (db->size *= 2));
	db->entries[db->nrentries++] = entry;
}

void Database::insertReplaceWithIp(entry_t entry)
{
	int type = entry.type;
	entry_list_t * db = &list[type];
	entry_t * entry1, entry2;
	if (type == SERVER) {
		if (this->lookupServer(entry.ip, entry.port, &entry1)) {
			this->delete_(*entry1->name);
		}
	}
	if (type == DCLIENT) {
		if (this->lookupDclient(entry.ip, entry.port, &entry2)) {
			this->delete_(*entry2.name);
		}
	}
	if (db->size == db->nrentries)
		db->entries = (entry_t *)realloc(db->entries, sizeof(entry_t) * (db->size *= 2));
	db->entries[db->nrentries++] = entry;
}

int Database::lookup(string name, entry_t * entry)
{
	for (int j = 0; j < 3 ; j++) {
		for (int i = 0 ; i < list[j].nrentries ; i++) {
			if ((*list[j].entries[i].name).compare(name) == 0) {
				if (entry != NULL)
					*entry = list[j].entries[i];
				return 1;	
			}
		}
	}
	return 0;
}

int Database::lookupServer(unsigned long ip, unsigned short port, entry_t ** entry)
{
	for (int i = 0 ; i < list[SERVER].nrentries ; i++) {
		if (list[SERVER].entries[i].ip == ip && list[SERVER].entries[i].port == port) {
			if (entry != NULL)
				*entry = &list[SERVER].entries[i];
			return 1;	
		}
	}
	return 0;
}

int Database::lookupDclient(unsigned long ip, unsigned short port, entry_t * entry)
{
	for (int i = 0 ; i < list[DCLIENT].nrentries ; i++) {
		if (list[DCLIENT].entries[i].ip == ip && list[DCLIENT].entries[i].port == port) {
			if (entry != NULL)
				*entry = list[DCLIENT].entries[i];
			return 1;	
		}
	}
	return 0;

}

int Database::lookupIclient(unsigned long ip, unsigned short port, entry_t * entry)
{
	for (int i = 0 ; i < list[ICLIENT].nrentries ; i++) {
		if (list[ICLIENT].entries[i].ip == ip && list[ICLIENT].entries[i].port == port) {
			if (entry != NULL)
				*entry = list[ICLIENT].entries[i];
			return 1;	
		}
	}
	return 0;

}

void Database::delete_(string name)
{
	for (int j = 0 ; j < 3 ; j++) {
		entry_list_t * db = &list[j];
		for (int i = 0 ; i < db->nrentries ; i++) {
			if ((*db->entries[i].name).compare(name) == 0) {
				this->freeEntry(db->entries[i]);
				db->entries[i]=db->entries[db->nrentries-1];
				db->nrentries--;
				if (db->size == db->nrentries*2)
					db->entries = (entry_t *)realloc(db->entries, sizeof(entry_t) * (db->size /= 2));
				return;	
			}
		}
	}
	return;
}

void Database::freeEntry(entry_t entry) {
	if (entry.name)
		delete entry.name;
	if (entry.ref)
		free(entry.ref);
}

entry_t * Database::allEntries(int type, int * s)
{
	* s = list[type].nrentries;
	return list[type].entries;
}
