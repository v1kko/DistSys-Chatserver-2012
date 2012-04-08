#include <cstdlib>
#include <cstdio>
#include <database.h>

using namespace std;

Database::Database()
{
	nrentries = 0;
	entries = (entry_t *)malloc(sizeof(entry_t) * 10);
	size = 10;
}
	
void Database::insert(entry_t entry)
{
	if (this->lookup(*entry.name, NULL)) {
		delete entry.name;
		return;
	}
	
	entry.ref = (unsigned int *)malloc(sizeof(int));
	*entry.ref = 0;
	if (size == nrentries)
		entries = (entry_t *)realloc(entries, sizeof(entry_t) * (size *= 2));
	entries[nrentries++] = entry;
}

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

int Database::lookup(string name, entry_t * entry)
{
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
