#ifndef _MANAGER
#define _MANAGER 1

#include <string>
using namespace std;

class Manager {

	void setName(string);
	void setPassword(string);
	string getName(void);
	string getPassword(void);
		
	int isOnline(void);
	
	//Sets isOnline to true
	void setAdres(int);
	void setPort(int);
	
	int getAdress(void);
	int getPort(void);
	
	void setOffline(void);

};
#endif
