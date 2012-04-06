#ifndef _MESSAGE
#define _MESSAGE 1
#include <string>
using namespace std;

#define ALL 2
#define NONE 0
#define ALLBUTONE -1
#define ONE 1

class Message
{
private:
	string data;
		
public:	
	Message(void);

	void setType(int);
	int getType(void);
	
	//Also sets the length (implicit), do error checking
	void setMessage(string);
	string getMessage(void);
	
	int length(void);
	
	//'Volgnummer'
	void setReferenceNumber(int);
	int getReferenceNumber(void);
	

	/* This needs a little bit of explanation
		We can send our messages to 1, all, or all -1 persons.
		Since all our connected clients/servers have a name 
		we can parse that name

		1 = send to that person
		-1 = send to everyone but person
		2 = send to all
		0 = send to none (default)
	*/
	void setRecipients(string, int);
	string getRecipients(int *);

	void getSender(unsigned long *, unsigned short *);
	void setSender(unsigned long, unsigned short);
};
#endif
