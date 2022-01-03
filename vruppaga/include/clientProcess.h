#include "structures.h"

void clientProcess(int cPort)
{
	int clientLoginFlag=0, flag = 1;//using as bool
	struct sockaddr_in currentAddr;
	struct clientMessageStruct cmnd;
	fdsocket = socket(AF_INET, SOCK_STREAM, 0);
	if(fdsocket < 0)
		perror("Failed to create socket");
	currentAddr.sin_family=AF_INET;
	currentAddr.sin_addr.s_addr=INADDR_ANY;
	currentAddr.sin_port=htons(cPort);
	setsockopt(fdsocket, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
	int bindRet = bind(fdsocket, (struct  sockaddr*) &currentAddr, sizeof(struct sockaddr_in));
	if( bindRet == 0)
		printf("\nBIND success\n");
	else
	{
		printf("\nError BIND\n");
		exit(-1);
	}
	int server=0, j;
	FD_ZERO(&clientMasterList);
	FD_ZERO(&clientUserList);
	FD_SET(STDIN, &clientMasterList);
	socketMain=0;
	while(1)
	{
		fflush(stdout);	
		FD_ZERO(&clientMasterList); 
		FD_ZERO(&clientUserList);
		FD_SET(STDIN, &clientMasterList);
		FD_SET(server, &clientMasterList);
		socketMain=server;
		memcpy(&clientUserList, &clientMasterList, sizeof(clientMasterList));
        
		int selret = select(socketMain + 1, &clientUserList, NULL, NULL, NULL);
		if(selret < 0)
		{
		    perror("select failed.");
		    exit(-1);
		}

        else
        {
            for(currentSockAddr=0; currentSockAddr<=socketMain; currentSockAddr+=1)
            {
                if(FD_ISSET(currentSockAddr, &clientUserList))
                {
                    if (currentSockAddr == STDIN)
                    {
				        char *cInput = (char*) malloc(sizeof(char)*256);//MSG_SIZE=256
				    	memset(cInput, '\0', 256);
						if(fgets(cInput, 255, stdin) == NULL) 
							exit(-1);
						cInput[strlen(cInput)-1]='\0';
						//implementing PA1 commands here.
						//AUTHOR
						if((strcmp(cInput,"AUTHOR"))==0)
							printAuthor();
							
						//IP
						else if((strcmp(cInput,"IP"))==0)
							printIP();
							
						//PORT 
						else if((strcmp(cInput,"PORT"))==0)
							printPort();
						
						//LIST	
						else if((strcmp(cInput,"LIST"))==0 && clientLoginFlag==1)
						{
							strcpy(cmnd.cmd,"LIST");
							if(send(server, &cmnd, sizeof(cmnd), 0) == sizeof(cmnd))
								cse4589_print_and_log("[LIST:SUCCESS]\n");
							else
								cse4589_print_and_log("[LIST:ERROR]\n");
							
							fflush(stdout);
						}
						//LOGIN
						else if((strncmp(cInput,"LOGIN",5))==0)
						{
							char ip[32], port[32];
							int k=6, j=0, errorFlag=0, i;
							for(;cInput[k]!=' ';j=j+1)
							{
								ip[j]=cInput[k];
								k=k+1;
							}
							ip[j]='\0';
							k=k+1;
							for(j=0 ;cInput[k]!='\0'; j=j+1)
							{
								port[j]=cInput[k];
								k=k+1;
							}
							port[j]='\0';
							for (i=0;i<strlen(port); i=i+1)
							{
								if (!isdigit(port[i]))
								{
								    printf ("PORT not valid\n");
								    errorFlag=1;
								}
							}
							if(1 > atoi(port) && atoi(port) > 65535)
								errorFlag =1;
							if((validateIP(ip)==1) &&(errorFlag !=1))
							{
								server= getConnection(ip, atoi(port));
								FD_SET(server, &clientMasterList);
								socketMain=server;
								clientLoginFlag=1;
								cse4589_print_and_log("[LOGIN:SUCCESS]\n");
							}
							else
							{
								cse4589_print_and_log("[LOGIN:ERROR]\n");
								cse4589_print_and_log("[LOGIN:END]\n");
							}
							fflush(stdout);
						}
						//REFRESH
						else if((strcmp(cInput,"REFRESH"))==0&& clientLoginFlag==0)
						{  
							cse4589_print_and_log("[REFRESH:SUCCESS]\n"); 
							cse4589_print_and_log("[REFRESH:END]\n");                 
						}
						//SEND
				        	else if((strncmp(cInput,"SEND",4))==0&& clientLoginFlag==1)
						{
							char cip[32];
							char message[256];
							int k=5, j=0;
							for(;cInput[k]!=' ';j=j+1)
							{
								cip[j]=cInput[k];
								//j=j+1;
								k=k+1;
							}
							cip[j]='\0';
							j=0;
							k=k+1;
							while(cInput[k]!='\0')
							{
								message[j]=cInput[k];
								k=k+1;
								j=j+1;
							}
							message[j]='\0';
							if((validateIP(cip))==1)
							{
								strcpy(cmnd.cmd,"SEND");
								strcpy(cmnd.ip,cip);
								strcpy(cmnd.info,message);
								if(send(server, &cmnd, sizeof(cmnd), 0) == sizeof(cmnd))
								{
									cse4589_print_and_log("[SEND:SUCCESS]\n");
								}
							}
							else
							{
								cse4589_print_and_log("[SEND:ERROR]\n");
								cse4589_print_and_log("[SEND:END]\n");
							}
							
							fflush(stdout);
						}
						//BROADCAST
						else if((strncmp(cInput,"BROADCAST",9))==0&& clientLoginFlag==1)
						{
							char message[256];
							int k=10,j=0;
							while(cInput[k]!='\0')
							{
								message[j]=cInput[k];
								k=k+1;
								j=j+1;
							}
							strcpy(cmnd.cmd,"BROADCAST");
							strcpy(cmnd.info,message);
							if(send(server, &cmnd, sizeof(cmnd), 0) == sizeof(cmnd))
								cse4589_print_and_log("[BROADCAST:SUCCESS]\n");
							else
								cse4589_print_and_log("BROADCAST:ERROR\n");
							cse4589_print_and_log("[BROADCAST:END]\n");
							fflush(stdout);
							
						}
						//BLOCK
						else if((strncmp(cInput,"BLOCK",5))==0 && clientLoginFlag==1)
						{
							char ip[32];
							int k=6,i=0;
							while(cInput[k]!='\0')
							{	
								ip[i]=cInput[k];
								i++;
								k++;
							}
							ip[i]='\0';
							if((validateIP(ip))==1)
							{	
								strcpy(cmnd.cmd,"BLOCK");
								strcpy(cmnd.ip,ip);
								printf("Entered ip is:- %s\n",ip);
								if(send(server, &cmnd, sizeof(cmnd), 0) == sizeof(cmnd))
								{
									cse4589_print_and_log("[BLOCK:SUCCESS]\n");
									cse4589_print_and_log("[BLOCK:END]\n");
								}
								else
								{
									cse4589_print_and_log("[BLOCK:ERROR]\n");
									cse4589_print_and_log("[BLOCK:END]\n");
								}
							}
							else
							{
								cse4589_print_and_log("[BLOCK:ERROR]\n");
								cse4589_print_and_log("[BLOCK:END]\n");
							}
							fflush(stdout);
						}
						//UNBLOCK
						else if((strncmp(cInput,"UNBLOCK",7))==0&& clientLoginFlag==1)
						{
							char ip[32];
							int k=8,i=0;
							strcpy(cmnd.cmd,"UNBLOCK");
							while(cInput[k]!='\0')
							{	
								ip[i]=cInput[k];
								i++;
								k++;
							}
							ip[i]='\0';
							if((validateIP(ip))==1)
							{
								strcpy(cmnd.ip,ip);
								if(send(server, &cmnd, sizeof(cmnd), 0) == sizeof(cmnd))
								{
									cse4589_print_and_log("[UNBLOCK:SUCCESS]\n");
									cse4589_print_and_log("[UNBLOCK:END]\n");
								}
								else
								{
									cse4589_print_and_log("[UNBLOCK:ERROR]\n");
									cse4589_print_and_log("[UNBLOCK:END]\n");
								}
							}
							else
							{
								cse4589_print_and_log("[UNBLOCK:ERROR]\n");
								cse4589_print_and_log("[UNBLOCK:END]\n");
							}
							
							fflush(stdout);
						}
						//LOGOUT
						else if((strcmp(cInput,"LOGOUT"))==0&& clientLoginFlag==1)
						{
							strcpy(cmnd.cmd,"LOGOUT");
							if(send(server, &cmnd, sizeof(cmnd), 0) == sizeof(cmnd))
							{
								cse4589_print_and_log("[LOGOUT:SUCCESS]\n");
								clientLoginFlag=0;
								struct sockaddr_in currentAddr;
								fdsocket = socket(AF_INET, SOCK_STREAM, 0);// return socket file descriptor
								if(fdsocket < 0)
									perror("Failed to create socket");
								currentAddr.sin_family=AF_INET;
								currentAddr.sin_addr.s_addr=INADDR_ANY;
								currentAddr.sin_port=htons(cPort);
								setsockopt(fdsocket, SOL_SOCKET, SO_REUSEPORT, &flag, sizeof(flag));
								int bindRet = bind(fdsocket, (struct  sockaddr*) &currentAddr, sizeof(struct sockaddr_in));
								if(bindRet == 0)
								{
									printf("\nLOGOUT BIND SUCCESS\n");
								}
								else
								{
									printf("\nERROR LOGOUT BIND\n");
									cse4589_print_and_log("[LOGOUT:ERROR]\n");
									exit(-1);
								}

								server=close(server);
							}
							cse4589_print_and_log("[LOGOUT:END]\n");	
						}
						
						//EXIT
						else if((strcmp(cInput,"EXIT"))==0)
						{
							close(server);
							cse4589_print_and_log("[EXIT:SUCCESS]\n");
							cse4589_print_and_log("[EXIT:END]\n");
							exit(0);
						}
                    }
                    else
                    {
        				struct serverMessageStruct srcv;
        				memset(&srcv, '\0', sizeof(srcv));
	                   		int retRecv = recv(server, &srcv, sizeof(srcv), 0);
						if( retRecv>= 0)
						{
							if(strcmp(srcv.cmd,"MESSAGE")==0)
							{	
								cse4589_print_and_log("[RECEIVED:SUCCESS]\n");
								cse4589_print_and_log("MESSAGE from:%s\n[message]:%s\n",srcv.sender_ip,srcv.info);
								cse4589_print_and_log("[RECEIVED:END]\n");
							}
							else if(strcmp(srcv.cmd,"MESSAGEDONE")==0)
							{	
								//cse4589_print_and_log("[SEND:SUCCESS]\n");
								cse4589_print_and_log("[SEND:END]\n");
							}
							else if(strcmp(srcv.cmd,"MESSAGEFAIL")==0)
							{	cse4589_print_and_log("[SEND:ERROR]\n");
								cse4589_print_and_log("[SEND:END]\n");
							}
							else if(strcmp(srcv.cmd,"LIST")==0)
							{	
								cse4589_print_and_log("%-5d%-35s%-20s%-8d\n",srcv.listData.list_id,srcv.listData.hostname,srcv.listData.listIP,srcv.listData.portNum);
							}
							else if(strcmp(srcv.cmd,"LISTDONE")==0)
							{	
								cse4589_print_and_log("[LIST:END]\n");
							}
							else if(strcmp(srcv.cmd,"LOGINOVER")==0)
							{	
								cse4589_print_and_log("[LOGIN:END]\n");
							}
							
							fflush(stdout);
						}
                    } 
                    fflush(stdout);	
                }
            }
        }	
	}
}

