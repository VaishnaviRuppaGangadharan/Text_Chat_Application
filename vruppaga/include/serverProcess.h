void initlist()
{
	for(int i=0;i<5;i++)
	{
		listPointer[i]=(struct listInfo *)malloc(sizeof(struct listInfo));
		listPointer[i]->list_id=0; 
		clientPointer[i]=(struct listInfo *)malloc(sizeof(struct blockList));
		clientPointer[i]->bid=0;
		strcpy(clientPointer[i]->ip1,"null");
		strcpy(clientPointer[i]->ip2,"null");
		strcpy(clientPointer[i]->ip3,"null");
		strcpy(clientPointer[i]->ip4,"null");
	}
}

void serverProcess(int sPort)
{
	printf("\n Inside server side with port %d", sPort);
	int serverSockfd, serverSocketMain, selret, currIndex, fdaccept=0, sendSocket=0;
	struct serverMessageStruct serverInfo;
	struct listInfo send_list;
	struct clientMessageStruct receiveData;
	struct sockaddr_in serverAddress, clientAddress;
	fd_set serverMasterList, serverWatchList;

	serverSockfd = socket(AF_INET, SOCK_STREAM, 0);
	fdsocket=serverSockfd;
	if(serverSockfd < 0)
		perror("Cannot create socket");
	else
		printf("created socket");
		
	bzero(&serverAddress, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(sPort);
	int bindRet = bind(serverSockfd, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
	if( bindRet< 0 )
		perror("Bind failed");
	else
		printf("Binded to socket\n");
	//Listen 
	if(listen(serverSockfd, 5) < 0)
		perror("Unable to listen on port");
	else
		printf("listening to socket\n");
	
	initlist();
	
	FD_ZERO(&serverMasterList); 
	FD_ZERO(&serverWatchList);
	FD_SET(serverSockfd, &serverMasterList);
	FD_SET(STDIN, &serverMasterList);
	serverSocketMain = serverSockfd;

    while(1)
    { 
	fflush(stdout);	
        memcpy(&serverWatchList, &serverMasterList, sizeof(serverMasterList));
        selret = select(serverSocketMain + 1, &serverWatchList, NULL, NULL, NULL);
        if(selret < 0)
        {
            perror("select failed.");
            exit(1);
        }

        if(selret > 0)
        {
        	 for(currIndex=0; currIndex<=serverSocketMain; currIndex+=1)
            {
                fflush(stdout);
                memset(&serverInfo, '\0', sizeof(serverInfo));
                if(FD_ISSET(currIndex, &serverWatchList))
                {
	                if (currIndex == STDIN)
	                {
	                	char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
	                	memset(cmd, '\0', CMD_SIZE);
				if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) 
					exit(-1);

				cmd[strlen(cmd)-1]='\0';
				
				//Process PA1 commands here ...
				if((strcmp(cmd, "AUTHOR"))==0)
				{
					printAuthor();
				}
				else if((strcmp(cmd, "IP"))==0)
				{
					printIP();
				}
				else if((strcmp(cmd, "PORT"))==0)
				{
					printPort();
				}
				else if((strcmp(cmd, "LIST"))==0)
				{
				cse4589_print_and_log("[LIST:SUCCESS]\n");
				for(int i=0;i<5;i++)
				{
					if(listPointer[i]->list_id!=0)
					{	
						cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", listPointer[i]->list_id, listPointer[i]->hostname, listPointer[i]->listIP, listPointer[i]->portNum);
					}
				}
				cse4589_print_and_log("[LIST:END]\n");
				}
				else if((strcmp(cmd, "STATISTICS"))==0)
				{
				cse4589_print_and_log("[STATISTICS:SUCCESS]\n");
				for(int i=0;i<5;i++)
				{
					if(listPointer[i]->list_id!=0)
					{	
						cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", listPointer[i]->list_id, listPointer[i]->hostname, listPointer[i]->numMsgSent, listPointer[i]->numMsgRecv,listPointer[i]->status);
					}
				}
				cse4589_print_and_log("[STATISTICS:END]\n");
				}
				else if((strncmp(cmd, "BLOCKED",7))==0)
				{
					char ip[32];
					int k=8,i=0;
					for(;cmd[k]!='\0';i=i+1)
					{	
						ip[i]=cmd[k];
						k++;
					}
					ip[i]='\0';
					if((validateIP(ip))==1)
					{
						cse4589_print_and_log("[BLOCKED:SUCCESS]\n");
						int itr=0;
						for(; clientPointer[itr]->bid!=0; itr=itr+1);	
						int j=1;
						for(;j<4;j=j+1)
						{
						if(strcmp(clientPointer[itr]->ip1,"null")!=0)
						{
							for(int i=0;i<5;i++)
							{
								if((strcmp(listPointer[i]->listIP,clientPointer[itr]->ip1)) == 0)
								{	
									cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", 1, listPointer[i]->hostname, listPointer[i]->listIP, listPointer[i]->portNum);
									break;
								}									                   	
							}	
						}
						}
						
						
						cse4589_print_and_log("[BLOCKED:END]\n");
					}
					else
					{
						cse4589_print_and_log("[BLOCKED:ERROR]\n");
						cse4589_print_and_log("[BLOCKED:END]\n");
					}				
				}
				free(cmd);
	                }
	                
	                else if(currIndex == serverSockfd)
	                {
				int clinetAddrLen = sizeof(clientAddress);
				fdaccept = accept(serverSockfd, (struct sockaddr *)&clientAddress, &clinetAddrLen);
				if(fdaccept < 0)
					perror("Accept failed.");
				char ip[INET_ADDRSTRLEN];
				inet_ntop(AF_INET,&clientAddress.sin_addr.s_addr,ip, INET_ADDRSTRLEN);                       
				ntohs(clientAddress.sin_port);
				FD_SET(fdaccept, &serverMasterList);
				if(fdaccept > serverSocketMain) 
					serverSocketMain = fdaccept;
				char host[1024];
				getnameinfo((struct sockaddr *)&clientAddress, clinetAddrLen,host, sizeof(host), 0,0,0);
				
				int n=0;
				for(;clientPointer[n]->bid != 0; n=n+1);
				clientPointer[n]->bid=n+1;
				strcpy(clientPointer[n]->blockIP,ip);
				
				int m=0;
				for(;listPointer[m]->list_id != 0 ;m=m+1);
				listPointer[m]->list_id=m+1;
				listPointer[m]->portNum=ntohs(clientAddress.sin_port);
				listPointer[m]->fd_socket=fdaccept;
				listPointer[m]->numMsgSent=0;
				listPointer[m]->numMsgRecv=0;
				strcpy(listPointer[m]->status,"logged-in");
				strcpy(listPointer[m]->listIP,ip);
				strcpy(listPointer[m]->hostname,host);
				strcpy(serverInfo.cmd,"LOGINOVER");
				if(send(fdaccept, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
					printf("Done!\n");
				fflush(stdout);
	                }
	                else
	                {
				memset(&receiveData, '\0', sizeof(receiveData));
				int retRecv = recv(currIndex, &receiveData, sizeof(receiveData), 0);
				if( retRecv <= 0)
				{
					//int delete=0;
					int last=0,i=0;
					for(;i<5;i++)
					{
						if(listPointer[i]->fd_socket==currIndex)
							break;
					}
					int j=i;
					i=0;
					for(;j<5;j++)
					{
						i=i+1;
						if(listPointer[j+1]->list_id!=0)
						{
							strcpy(listPointer[j]->hostname,listPointer[j+1]->hostname);
							strcpy(listPointer[j]->listIP,listPointer[j+1]->listIP);
							listPointer[j]->portNum=listPointer[j+1]->portNum;
							listPointer[j]->fd_socket=listPointer[j+1]->fd_socket;
							listPointer[j]->numMsgRecv=listPointer[j+1]->numMsgRecv;
							listPointer[j]->numMsgSent=listPointer[j+1]->numMsgSent;
							strcpy(listPointer[j]->status,listPointer[j+1]->status);
						}	

					}
					listPointer[i]->list_id=0;
					printf("Remote Host terminated connection!\n");
					FD_CLR(currIndex,&serverMasterList);
				}
	                    else 
	                    {
	                    	if((strcmp(receiveData.cmd,"SEND"))==0)
	                    	{
					char sender_ip[32];
					char receivers_ip[32];

					for(int i=0;i<5;i++)
					{
						if(listPointer[i]->fd_socket==currIndex)
						{	
							strcpy(sender_ip,listPointer[i]->listIP);
							listPointer[i]->numMsgSent+=1;
							break;
						}
						
					}
					for(int i=0;i<5;i++)
					{
						if((strcmp(listPointer[i]->listIP,receiveData.ip)) == 0)
						{	
							sendSocket=listPointer[i]->fd_socket;
							listPointer[i]->numMsgRecv+=1;
							break;
						}									                   	

					}

					strcpy(receivers_ip,receiveData.ip);
					
					int flag=1, itr=0;
					while(clientPointer[itr]->bid!=0)
					{
						if(strcmp(sender_ip,clientPointer[itr]->blockIP)==0)
							break;
						itr+=1;
					}

					if(strcmp(clientPointer[itr]->ip1,receivers_ip)==0)
					{
						flag=0;
					}
					if(strcmp(clientPointer[itr]->ip2,receivers_ip)==0)
					{
						flag=0;
					}
					if(strcmp(clientPointer[itr]->ip3,receivers_ip)==0)
					{
						flag=0;	
					}
					if(strcmp(clientPointer[itr]->ip4,receivers_ip)==0)
					{
						flag=0;
					}
					if(flag==1)
					{		
						strcpy(serverInfo.cmd,"MESSAGE");
						strcpy(serverInfo.sender_ip,sender_ip);
						strcpy(serverInfo.info,receiveData.info);

						if(send(sendSocket, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
						{
						cse4589_print_and_log("[RELAYED:SUCCESS]\n");
						cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",sender_ip, receivers_ip, receiveData.info);
						strcpy(serverInfo.cmd,"MESSAGEDONE");
						if(send(currIndex, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
							printf("Done!\n");
						}
						else
						{
							cse4589_print_and_log("[RELAYED:ERROR]\n");
							strcpy(serverInfo.cmd,"MESSAGEFAIL");
							if(send(currIndex, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
								printf("Done!\n");
						}
						cse4589_print_and_log("[RELAYED:END]\n");
					}
					else
					{
						cse4589_print_and_log("[RELAYED:SUCCESS]\n");
						cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n",sender_ip, receivers_ip, receiveData.info);
						strcpy(serverInfo.cmd,"MESSAGEDONE");
						if(send(currIndex, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
						{
							printf("Done\n");	
						}
						cse4589_print_and_log("[RELAYED:END]\n");
					}
	                    	}
	                    	else if((strcmp(receiveData.cmd,"BROADCAST"))==0)
	                    	{
		                    	char sender_ip[32];
					for(int i=0;i<5;i++)
					{
						if(listPointer[i]->fd_socket==currIndex)
						{	
							strcpy(sender_ip,listPointer[i]->listIP);
							listPointer[i]->numMsgSent+=1;
							break;
						}
						
					}
					for(int i=0;i<5;i++)
					{
						if((listPointer[i]->list_id!=0) && ((strcmp(listPointer[i]->listIP,sender_ip))!=0))
						{
							sendSocket=listPointer[i]->fd_socket;
							strcpy(serverInfo.cmd,"MESSAGE");
							strcpy(serverInfo.sender_ip,sender_ip);
							strcpy(serverInfo.info,receiveData.info);
							if(send(sendSocket, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
							{
								cse4589_print_and_log("[RELAYED:SUCCESS]\n");
								cse4589_print_and_log("MESSAGE from:%s, to:255.255.255.255\n[MSG:]:%s\n",sender_ip, receiveData.info);	
								cse4589_print_and_log("[RELAYED:END]\n");
							}
							else
							{
							cse4589_print_and_log("[RELAYED:ERROR]\n");	
							cse4589_print_and_log("[RELAYED:END]\n");
							}	
						}									
					}	
													
	                    	}
	                    	else if((strcmp(receiveData.cmd,"BLOCK"))==0)
	                    	{
	                    		char sender_ip[32];
					for(int i=0;i<5;i++)
					{
						if(listPointer[i]->fd_socket==currIndex)
						{	
							strcpy(sender_ip,listPointer[i]->listIP);
							break;
						}	
					}
					int z=0;
					printf("\nBLOCK SENDER IP:-%s",sender_ip);

					while((clientPointer[z]->bid)!=0)
					{
					if(strcmp(receiveData.ip,clientPointer[z]->blockIP)==0)
						break;
						z++;
					}

					if(strcmp(clientPointer[z]->ip1,"null")==0)
					{
						strcpy(clientPointer[z]->ip1,sender_ip);
					}
					else if(strcmp(clientPointer[z]->ip2,"null")==0)
					{
						strcpy(clientPointer[z]->ip2,sender_ip);
					}
					else if(strcmp(clientPointer[z]->ip3,"null")==0)
					{
						strcpy(clientPointer[z]->ip3,sender_ip);
					}
					else if(strcmp(clientPointer[z]->ip4,"null")==0)
					{
						strcpy(clientPointer[z]->ip4,sender_ip);
					}
    		
	                    	}
	                    	else if((strcmp(receiveData.cmd,"UNBLOCK"))==0)
	                    	{
	                    		char sender_ip[32];
					for(int i=0;i<5;i++)
					{
						if(listPointer[i]->fd_socket==currIndex)
						{	
							strcpy(sender_ip,listPointer[i]->listIP);
							break;
						}	
					}
					int z=0;
					while((clientPointer[z]->bid)!=0)
					{
						printf("\ninside server  block");
						if(strcmp(receiveData.ip,clientPointer[z]->blockIP)==0)
							break;
						z++;
					}
					         	if(strcmp(clientPointer[z]->ip1,sender_ip)==0)
							{
								strcpy(clientPointer[z]->ip1,"null");
							}
							else if(strcmp(clientPointer[z]->ip2,sender_ip)==0)
							{
								strcpy(clientPointer[z]->ip2,"null");
							}
							else if(strcmp(clientPointer[z]->ip3,sender_ip)==0)
							{
								strcpy(clientPointer[z]->ip3,"null");
							}
							else if(strcmp(clientPointer[z]->ip4,sender_ip)==0)
							{
								strcpy(clientPointer[z]->ip4,"null");
							}	
	                    	}
	                    	else if((strcmp(receiveData.cmd,"LIST"))==0)
	                    	{
	                    		for(int i=0;i<5;i++)
					{
						if(listPointer[i]->list_id!=0)
						{	
						printf("%-5d%-35s%-20s%-8d\n", listPointer[i]->list_id, listPointer[i]->hostname, listPointer[i]->listIP, listPointer[i]->portNum);
						
						send_list.list_id=listPointer[i]->list_id;
						strcpy(send_list.hostname,listPointer[i]->hostname);
						strcpy(send_list.listIP,listPointer[i]->listIP);
						send_list.portNum=listPointer[i]->portNum;

						strcpy(serverInfo.cmd,"LIST");
						serverInfo.listData=send_list;
						if(send(currIndex, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
								printf("Done!\n");	
						}
					}
					strcpy(serverInfo.cmd,"LISTDONE");
					if(send(currIndex, &serverInfo, sizeof(serverInfo), 0) == sizeof(serverInfo))
						printf("Done!\n");
	                            fflush(stdout);					
	                    	}
	                    	else if((strcmp(receiveData.cmd,"LOGOUT"))==0)
	                    	{
					for(int i=0;i<5;i++)
					{
						if(listPointer[i]->fd_socket==currIndex)
						{	
							strcpy(listPointer[i]->status,"logged-out");
							close(currIndex);
							FD_CLR(currIndex, &serverMasterList);
						}	
					}
	                    	}
				fflush(stdout);
	                    }
	             	}   
	         }
            }
        }
    }
}
