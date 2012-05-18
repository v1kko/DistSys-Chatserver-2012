void Server::incomingMessage(Message  message) {

	int type, temp, temp1, size, ref;
	unsigned long ip;
	unsigned short port;
	string buffer, name, password;
	char cbuffer[200], cbuffer1[100], * t;
	entry_t entry, entry1, * entries, *pentry;
	entry.name = &name;
	type = message.getType();
	buffer = message.getMessage();
	ref = message.getReferenceNumber();
	message.getSender(&entry.ip, &entry.port);

	Message managermessage;

	if(manager->isOnline()) {
		sprintf(cbuffer, "%s %s (in) %d %s",manager->getName().c_str(),manager->getName().c_str(), type, buffer.c_str());
		managermessage.setMessage(cbuffer);
		managermessage.setType(310);
		managermessage.setReferenceNumber(manager->getRef());
		managermessage.setType(310);
		manager->getAdress(&ip, &port);
		if (!(ip == entry.ip && port == entry.port) && type != 150 && type != 140)
			connection->send(managermessage, ip, port);
	}

	switch (type) {
		case 100:
			//Check if name is available
			printf("100 - client -> server (Received)\n");
			strcpy(cbuffer, buffer.c_str());
			t = strtok(cbuffer, " ");
			name = (t != NULL) ? t : "";
			t = strtok(NULL, " ");
			password = (t != NULL) ? t : "";	

			//Check if this is a manager
			if (manager->getName() == name) { 
				if (!manager->isManager(name, password)) {
					printf("510 - server -> client (Sent) - Registratie mislukt\n");
					message.setType(510);
					message.setReferenceNumber(0);
					message.setMessage("Incorrect password");
					connection->send(message, entry.ip, entry.port);
					break;
				} else {
					printf("500 - server -> client (Sent) - Registratie gelukt\n");
					manager->setAdress(entry.ip, entry.port);
					manager->pingtimeout = PINGTIMEOUT;
					manager->setRef();
					message.setType(500);
					message.setMessage("");
					message.setReferenceNumber(manager->getRef());
					connection->send(message, entry.ip, entry.port);
					message.setType(310);
					sprintf(cbuffer, "%s %s Welcome Administrator welcome*-", manager->getName().c_str(),manager->getName().c_str());
					message.setMessage(cbuffer);
					connection->send(message, entry.ip, entry.port);
					sprintf(cbuffer, "%s %s type \"list\" to see all connected clients", manager->getName().c_str(),manager->getName().c_str());
					message.setMessage(cbuffer);
					connection->send(message, entry.ip, entry.port);
					break;
				}
			}	

			entry = database->createEntry(name, entry.ip, entry.port, DCLIENT);
			if(!database->insert(entry)) {
				printf("510 - server -> client (Sent) - Registratie mislukt\n");
				message.setType(510);
				message.setReferenceNumber(0);
				message.setMessage("Name already exists or the database is full");
				connection->send(message, entry.ip, entry.port);
				break;
			}

			//Register in network
			printf("500 - server -> client (Sent) - Registratie gelukt\n");
			message.setType(500);
			message.setMessage("");
			message.setRecipients(*entry.name, ONE);
			connection->send(message);
			
			printf("110 - server -> All but client (Sent) - user added\n");
			message.setType(110);
			message.setRecipients(*entry.name, ALLBUTONE);
			sprintf(cbuffer, "%6d%6d%s", 1, 1, (*entry.name).c_str());
			buffer = cbuffer;
			message.setMessage(buffer);
			connection->send(message);
			
			printf("110 - server -> client (Sending) - user list\n");

			//Send clientlist to new client
			entries = database->allEntries(DCLIENT,&size);
			for (int i = 0 ; i < size ; i++) {
					sprintf(cbuffer, "%6d%6d%s", i+1, size, 
						(*entries[i].name).c_str());
					buffer = cbuffer;
					message.setMessage(buffer);
					message.setRecipients(*entry.name, ONE);
					connection->send(message);
			}			
			entries = database->allEntries(ICLIENT,&size);
			for (int i = 0 ; i < size ; i++) {
					sprintf(cbuffer, "%6d%6d%s", i+1, size, 
						(*entries[i].name).c_str());
					buffer = cbuffer;
					message.setMessage(buffer);
					message.setRecipients(*entry.name, ONE);
					connection->send(message);
			}			
			break;			
		case 110:
			printf("110 - server -> server (Received)\n");
			if(!database->lookupServer(entry.ip, entry.port, &pentry))
				break;
			if (buffer.length() > 12) {
				name = buffer.substr(12);
				entry = database->createEntry(name, entry.ip, entry.port, ICLIENT);
				if (entry.name == 0)
					break;
				//Insert received entry
				if (!database->insertReplace(entry))
					break;
				
				//Send to other servers	
				printf("110 - server -> server (Sending to clients and servers)\n");
				message.setRecipients(*pentry->name , ALLBUTONE);
				connection->send(message);
			}
			//If it is the initial list from the parent server, we must reply
			if (parentip==entry.ip && parentport==entry.port && parentfirst) {

				//Reply after last 110 message
				if (!buffer.substr(0, 6).compare(buffer.substr(6, 6))) {
					printf("110 - server -> parent server (Sending)\n");
					parentfirst = 0 ;	
					//Check the number of entries we are going to send
					entries = database->allEntries(ICLIENT, &size);
					temp = 0, temp1 = 0;
					for (int i = 0 ; i < size ; i++) {
						if (!(entries[i].ip == parentip && 
								entries[i].port == parentport))
							temp ++;
					}
					entries = database->allEntries(DCLIENT, &size);
					temp += size;

					//If we have no client the parent doesn't know of,
					//send empty reply
					if (temp == 0) {
						sprintf(cbuffer, "%6d%6d", 0, 0);
						buffer = cbuffer;
						message.setMessage(buffer);
						message.setRecipients(parentname, ONE);
						connection->send(message);
					}

					//Create & send the client list
					entries = database->allEntries(ICLIENT, &size);
					for (int i = 0 ; i < size ; i++) {
						if (!(entries[i].ip == parentip && 
								entries[i].port == parentport)) {
							
							sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, 
								(*entries[i].name).c_str());
							buffer = cbuffer;
							message.setMessage(buffer);
							message.setRecipients(parentname, ONE);
							connection->send(message);
						}
					}
					entries = database->allEntries(DCLIENT, &size);
					for (int i = 0 ; i < size ; i++) {
						sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, 
							(*entries[i].name).c_str());
						buffer = cbuffer;
						message.setMessage(buffer);
						message.setRecipients(parentname, ONE);
						connection->send(message);
					}
				}
			}
			break;
		case 120:
			//Delete the client from our database and send to others
			printf("120 - client -> server (Received)\n");
			if (! database->lookupDclient(entry.ip, entry.port, &entry)) {
				break;	
			}
			message.setMessage(*entry.name + " " + buffer);
			message.setType(130);
			message.setRecipients("#all", ALL);
			database->delete_(*entry.name);	
			connection->send(message);
			printf("130 - server -> All (Sent)\n");
			break;
		case 130:
			//Delete the client from our database and send to others
			printf("130 - server -> server (Received)\n");
			manager->getAdress( &ip, &port);
			if (!database->lookupServer(entry.ip, entry.port, &pentry)&&(!manager->isOnline() || !entry.ip == ip || !entry.port == port))
				break;

			//Let the manager kick clients
			if (manager->isOnline() && entry.ip == ip && entry.port == port) {
				*entry.name = buffer.substr(0, buffer.find_first_of(' ')); 
				if (database->lookupDclient(entry.ip, entry.port, &entry)) {
					message.setRecipients("#all", ALL);
					connection->send(message);
					printf("130 - Manager -> All (Sent)\n");
					database->delete_(buffer.substr(0, buffer.find_first_of(' ')));
					message.setType(310);
					message.setReferenceNumber(manager->getRef());
					sprintf(cbuffer, "%s %s %s", manager->getName().c_str(),manager->getName().c_str(), "Client succesfully kicked");
					message.setMessage(cbuffer);
					connection->send(message, ip, port);
				} else {
					message.setType(310);
					message.setReferenceNumber(manager->getRef());
					sprintf(cbuffer, "%s %s %s", manager->getName().c_str(),manager->getName().c_str(), "Client is not a direct client, not kicked");
					message.setMessage(cbuffer);
					connection->send(message, ip, port);
				}
				connection->send(message);
			} else {
				*entry.name = buffer.substr(0, buffer.find_first_of(' ')); 
				if (database->lookupIclient(entry.ip, entry.port, &entry)) {
					message.setRecipients(*pentry->name, ALLBUTONE);
					connection->send(message);
					printf("130 - server -> All (Sent)\n");
					database->delete_(buffer.substr(0, buffer.find_first_of(' ')));
				}
			}
			break;
		case 140:
			//printf("140 - ping -> 150 - pong\n");// Clutters the logs
			message.setType(150);
			message.setReferenceNumber(0);
			//Check if it is the Control server
			if ( entry.ip == csip && entry.port == htons(2001)) {
				message.setReferenceNumber(csref++);
				cstimeout = CSTIMEOUT;
				connection->send(message, entry.ip, entry.port);
				break;
			}

			//Decide wether the client is a server, client or unknown
			if (database->lookupServer(entry.ip, entry.port, &pentry)) {
				message.setRecipients(*pentry->name, ONE);
				connection->send(message);
				break;
			}
			if (database->lookupDclient(entry.ip, entry.port, &entry)) {
				message.setRecipients(*entry.name, ONE);
				connection->send(message);
				break;
			}
			connection->send(message, entry.ip, entry.port);
			break;
		case 150:
			//printf("150 - pong (Received)");
			//Decide wether the client is a server or client
			manager->getAdress(&ip, &port);
			if (entry.ip == ip && entry.port == port) {
				manager->pingtimeout = PINGTIMEOUT;
			}
			if (database->lookupServer(entry.ip, entry.port, &pentry)) {
				*pentry->pingtimeout=PINGTIMEOUT;
				break;
			}
			if (database->lookupDclient(entry.ip, entry.port, &entry)) {

				*entry.pingtimeout=PINGTIMEOUT;
				break;
			}
			break;
		case 160:
			//printf("160 - client -> server (Received)\n");
			//Do we know this client?
			manager->getAdress( &ip, &port);
			if (! database->lookupDclient(entry.ip, entry.port, &entry) && (!manager->isOnline() || !entry.ip == ip || !entry.port == port)) 
				break;
			//Is the name available?
			
			
			if (database->lookup(buffer, NULL) || (*entry.name == manager->getName() && (!entry.ip == ip || !entry.port == port) )) {
				printf("530 - server -> client (Sent)\n");
				message.setType(530);
				message.setRecipients(*entry.name, ONE);
				message.setMessage("Name already taken");
				connection->send(message);
				break;
			}
			name = *entry.name;	
			//Replace the entry
			entry = database->createEntry(buffer, entry.ip, entry.port,DCLIENT);
			database->insertReplaceWithIp(entry);

			//special manager treatment
			if (manager->isOnline() && entry.ip == ip && entry.port == port ) {
				name = manager->getName();
				message.setType(110);
				message.setRecipients(buffer, ALLBUTONE);
				sprintf(cbuffer, "%6d%6d%s", 1, 1, name.c_str());
				message.setMessage(cbuffer);
				connection->send(message);

				//Check the number of entries we are going to send
				entries = database->allEntries(ICLIENT, &size);
				temp = size;
				temp1 = 0;
				entries = database->allEntries(DCLIENT, &size);
				temp += size;

				//Create & send the client list
				entries = database->allEntries(ICLIENT, &size);
				for (int i = 0 ; i < size ; i++) {
					if (!(entries[i].ip == parentip && 
							entries[i].port == parentport)) {
						
						sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, 
							(*entries[i].name).c_str());
						message.setMessage(cbuffer);
						message.setRecipients(buffer, ONE);
						connection->send(message);
					}
				}
				entries = database->allEntries(DCLIENT, &size);
				for (int i = 0 ; i < size ; i++) {
					sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, 
						(*entries[i].name).c_str());
					message.setMessage(cbuffer);
					message.setRecipients(buffer, ONE);
					connection->send(message);
				}
				manager->setOffline();
			}

			//Send a confirmation
			printf("520 - server -> client (Sent)\n");
			message.setType(520);
			message.setRecipients(*entry.name, ONE);
			message.setMessage("");
			connection->send(message);
			
			//Inform everyone of the namechange
			printf("170 - server -> All (Sent)\n");
			buffer.insert(0, " ").insert(0, name);
			message.setType(170);
			message.setRecipients(*entry.name, ALL);
			message.setMessage(buffer);
			connection->send(message);
			break;
		case 170:
			printf("170 - server -> server (Received)\n");
			//Do we know this server?
			if (!database->lookupServer(entry.ip, entry.port, &pentry))
				break;
			name = *pentry->name; 
			//Do we have an entry of this name?
			if (!database->lookup(buffer.substr(0, 
					temp = buffer.find_first_of(' ')), &entry))
				break;

			//Send to everyone else
			message.setRecipients(name, ALLBUTONE);
			connection->send(message);

			//Update entry (sneaky mode)
			*entry.name = buffer.substr(temp+1, string::npos);
			printf("170 - server -> All (Sent)\n");
			break;
		case 200:
		case 210:
			printf("200 - client -> server (Received)\n");
			//Do we know this client?
			manager->getAdress( &ip, &port);
			if (! database->lookupDclient(entry.ip, entry.port, &entry) && (!manager->isOnline() || !entry.ip == ip || !entry.port == port)) {
				break;	
			}
			
			//Special Treatment for Managers
			if ((manager->isOnline() && entry.ip == ip && entry.port == port)) {	
				if (buffer == "#all list") {
					//Send client list
					message.setType(310);
					message.setReferenceNumber(manager->getRef());
					sprintf(cbuffer, "%s %s List of Directly Connected Clients", manager->getName().c_str(),manager->getName().c_str());
					message.setMessage(cbuffer);
					connection->send(message, ip, port);
					entries = database->allEntries(DCLIENT, &size); 
					for (int i = 0 ; i < size ; i++) {
						message.setReferenceNumber(manager->getRef());
						sprintf(cbuffer, "%s %s %s", manager->getName().c_str(),manager->getName().c_str(), (*entries[i].name).c_str());
						message.setMessage(cbuffer);
						connection->send(message, ip, port);
					}
					message.setReferenceNumber(manager->getRef());
					sprintf(cbuffer, "%s %s List of Indirectly Connected Clients", manager->getName().c_str(),manager->getName().c_str());
					message.setMessage(cbuffer);
					connection->send(message, ip, port);
					entries = database->allEntries(ICLIENT, &size); 
					for (int i = 0 ; i < size ; i++) {
						message.setReferenceNumber(manager->getRef());
						sprintf(cbuffer, "%s %s %s", manager->getName().c_str(),manager->getName().c_str(), (*entries[i].name).c_str());
						message.setMessage(cbuffer);
						connection->send(message, ip, port);
					}
					break;
				} else {
					//Say it with an appropriate name
					sprintf(cbuffer, "%s%s%c%d", "mod@", "0.0.0.0", ':', sport);
					name = cbuffer;
					printf("%s\n", name.c_str());
					struct ifaddrs * iflist = NULL, * ifa = NULL;
					getifaddrs(&iflist);
					for (ifa = iflist ; ifa != NULL ; ifa = ifa->ifa_next) {
						if (ifa->ifa_addr == NULL || ifa->ifa_addr->sa_family != AF_INET)
							continue;
						t = (char *)inet_ntop(ifa->ifa_addr->sa_family, &((struct sockaddr_in *)ifa->ifa_addr)->sin_addr, cbuffer1, 200);
						if ( t == NULL )
							continue;
						if (!strcmp(t, "0.0.0.0") || !strcmp(t, "127.0.0.1"))
							continue;
						sprintf(cbuffer, "%s%s%c%d", "mod@", cbuffer1, ':', sport);
						name = cbuffer;
					}
					entry.name = &name;
					freeifaddrs(iflist);
				}
			}

			type == 200 ? message.setType(300): message.setType(310);
			buffer.insert(0, " ").insert(0, *entry.name);
			message.setMessage(buffer);
			temp = buffer.find_first_of(' ')+1;

			//Check for All
			if (buffer.substr(temp, 
					buffer.find_first_of (' ', temp+1)-temp) == "#all") {
				printf("300 -  server -> ALL (Sending)\n");
				message.setRecipients("#all", ALL);
				connection->send(message);
				break;
			}

			//Send to the recipient
			printf("300 -  server -> server/client (Sending)\n");
			temp = buffer.find_first_of(' ')+1;
			message.setRecipients(buffer.substr(temp, 
					buffer.find_first_of(' ', temp)-temp), ONE);
			connection->send(message);
			//Send to the sender
			if ((!manager->isOnline() || !entry.ip == ip || !entry.port == port)) {	
				message.setRecipients(*entry.name, ONE);
				connection->send(message);
			}
			break;				
		case 300:
		case 310:
			//Send to correct recipient
			printf("300 - server -> server (Received)\n");
			if (!database->lookupServer(entry.ip, entry.port, &pentry))
				break;
			temp = buffer.find_first_of(' ')+1;
			name = buffer.substr(temp, buffer.find_first_of(' ', temp)-temp);

			type == 300 ? message.setType(300): message.setType(310);
			//Check for all
			if (name == "#all") {
				printf("300 -  server -> ALL (Sending)\n");
				message.setRecipients(*pentry->name, ALLBUTONE);
				connection->send(message);
				break;
			}


			if (database->lookup(name, NULL)) {
				message.setRecipients(name, ONE);
				connection->send(message);
				printf("300 - server -> client/server (Sent)\n");
			}
			break;
		case 600:
			printf("600 - server -> server (Received)\n");
			
			//Insert Server child into database
			entry = database->createEntry(name= buffer, 
					entry.ip, entry.port, SERVER);

			//break if we dont have enough space (maybe send action message?)
			if (!database->insertReplaceWithIp(entry))
				break;
			
			printf("110 - server -> server (Sending to new child)\n");
			//Check the number of entries we are going to send
			entries = database->allEntries(ICLIENT, &size);
			temp = 0, temp1 = 0;
			temp = size;
			entries = database->allEntries(DCLIENT, &size);
			temp += size;

			//If we have no client the parent doesn't know of,
			//send empty reply
			message.setType(110);
			if (temp == 0) {
				sprintf(cbuffer, "%6d%6d", 0, 0);
				buffer = cbuffer;
				message.setMessage(buffer);
				message.setRecipients(name, ONE);
				connection->send(message);
			}
			//Create & send the client list
			entries = database->allEntries(ICLIENT, &size);
			for (int i = 0 ; i < size ; i++) {
				sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, 
					(*entries[i].name).c_str());
				buffer = cbuffer;
				message.setMessage(buffer);
				message.setRecipients(name, ONE);
				connection->send(message);
			}
			entries = database->allEntries(DCLIENT, &size);
			for (int i = 0 ; i < size ; i++) {
				sprintf(cbuffer, "%6d%6d%s", ++temp1, temp, 
					(*entries[i].name).c_str());
				buffer = cbuffer;
				message.setMessage(buffer);
				message.setRecipients(name, ONE);
				connection->send(message);
			}
			break;
		case 602:
			printf("602 - Control server -> server (Received)\n");
			if (entry.ip != csip || entry.port != csport)
				break;

			if (buffer.compare("none") == 0) {
				printf("602 - No parent received (Now root of network)\n");
				parentname="none";
				parentfirst = 0;
				break;
			}
			sprintf(cbuffer, "%s", buffer.c_str());
			t = strtok(cbuffer, ":");
			//Insert future parent into database
			struct sockaddr_in sa;
			inet_pton(AF_INET, t, &sa.sin_addr);
			parentip = entry.ip = sa.sin_addr.s_addr;
			if ((t = strtok(NULL, ":")) == NULL) break;
			parentport = entry.port = htons(atoi(t));
			if ((t = strtok(NULL, ":")) == NULL) break;
			parentname = name = t;
			entry = database->createEntry(name, entry.ip, entry.port, SERVER);
			parentfirst = 1;
			if (!database->insertReplaceWithIp(entry))
				break;
							
			printf("600 - Server -> Server: Trying to register (Sending)\n");
			message.setType(600);
			message.setMessage(ident);
			message.setRecipients(name, ONE);
			connection->send(message);
			break;
		case 604:
			printf("604 - Control server -> server (Received)\n");
			if (entry.ip != csip || entry.port != csport)
				break;
			sprintf(cbuffer, "%s", buffer.c_str());
			if (!(t = strtok(cbuffer, " "))) break;
			//Do we know this server?
			if (database->lookup(t, &entry)) {
				database->delete_(*entry.name);
				//Remove children of dead server
				while(database->lookupIclient(entry.ip, entry.port, &entry1)) {
					message.setType(130);
					message.setRecipients(*entry1.name, ALL);
					message.setMessage(*entry1.name + " Received 604");
					database->delete_(*entry1.name);
					connection->send(message);
				}
			}
			//Is this our parent? (if not we dont have to connect to the new parent)
			if ((strcmp(t, parentname.c_str())))
				break;

			//connect to the new parent	
			parentname = "Undefined";
			if (!(t = strtok(NULL, ": "))) break;
			if (!strcmp(t, "none")) {	
				printf("604 - No parent received (Now root of network)\n");
				parentname="none";
				parentfirst = 0;
				break;
			}
			if (!strcmp(t, "self")) {	
				printf("604 - self received (Some bogus server 604'd us)\n");
				parentname="Undefined";
				csref = 0;		
				message.setType(601);
				message.setReferenceNumber(csref++);
				message.setMessage(ident);
				connection->send(message, csip, csport);
				printf("601 - Server -> Control server: Request for parent Server\n");
				break;
			}

			inet_pton(AF_INET, t, &sa.sin_addr);
			parentip = entry.ip = sa.sin_addr.s_addr;
			if ((t = strtok(NULL, ": ")) == NULL) break;
			parentport = entry.port = htons(atoi(t));
			if ((t = strtok(NULL, ": ")) == NULL) break;
			parentname = name = t;
			parentfirst = 1;
			entry = database->createEntry(name, entry.ip, entry.port, SERVER);
			if (!database->insertReplaceWithIp(entry))
				break;
		
			printf("600 - Server -> Server: Trying to register (Sending)\n");
			message.setType(600);
			message.setMessage(ident);
			message.setRecipients(name, ONE);
			connection->send(message);
			break;
		case 700:
			//only manager can stop the server
			if (!manager->isOnline())
				break;
			manager->getAdress(&ip, &port);
			if (ip != entry.ip || port != entry.port)
				break;
			message.setType(701);
			message.setMessage("Beheerder stopt de server");
			message.setRecipients("all", ALL);
			connection->send(message);
			connection->send(message, ip, port);
			printf("700 - Mod -> Server, Shutting down gracefully");
			fflush(NULL);
			exit(0);
		default:
			break;		
	}
}	
