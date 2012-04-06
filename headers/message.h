#include <string>
using namespace std;
class Message
{
	Message(void);

	void setType(int);
	int getType(void);
	void setMessage(string);
	string getMessage(void);
	
	//'Volgnummer'
	void setReferenceNumber(int);
	int getReferenceNumber(void);
	

	/* This needs a little bit of explanation
		We can send our messages to 1, all, or all -1 persons.
		Since all our connected clients/servers have a name 
		we can parse that name

		name = send to that person
		~name = send to everyone but person
		all = send to all
		none = send to none (default)
	*/
	void setRecipients(string);
	string getSender(void);

	void send(void);
};
