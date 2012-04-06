#include <cstdlib>
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
	if (size == nrentries)
		entries = (entry_t *)realloc(entries, sizeof(entry_t) * (size *= 2));
	entries[nrentries++] = entry;
}

int Database::lookup(string name, entry_t * entry)
{
	for (int i = 0 ; i < nrentries ; i++) {
		if ((*entries[i].name).compare(name) == 0) {
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
