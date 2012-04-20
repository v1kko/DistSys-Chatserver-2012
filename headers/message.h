#ifndef _MESSAGE
#define _MESSAGE 1
#include <string>
using namespace std;

#define ALL 2
#define ONE 1
#define NONE 0
#define ALLBUTONECLIENT -1
#define ALLBUTONESERVER -2

class Message
{
private:
	string mdata, mrecipient;
	int mtype, msize, mrefnum, mrecvtype;
	unsigned long mip;
	unsigned short mport;
		
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

                2 = send to all
		1 = send to that person
                0 = send to none (default)
		-1 = send to everyone but person
		-2 = send to everyone but server
	*/
	void setRecipients(string, int);
	string getRecipients(int *);

	void getSender(unsigned long *, unsigned short *);
	void setSender(unsigned long, unsigned short);
};
#endif
