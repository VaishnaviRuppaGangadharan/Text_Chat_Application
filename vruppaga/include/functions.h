void printAuthor()
{
	cse4589_print_and_log("[AUTHOR:SUCCESS]\n");
	cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "oviyaaba");
	cse4589_print_and_log("[AUTHOR:END]\n");
}

void printPort()
{
	struct sockaddr_in portvalue;
	socklen_t len=sizeof(portvalue);
	if(getsockname(fdsocket,(struct sockaddr *)&portvalue, &len) == -1)
	{
		cse4589_print_and_log("[PORT:ERROR]\n");
		cse4589_print_and_log("[PORT:END]\n");
	}
	else
	{
		cse4589_print_and_log("[PORT:SUCCESS]\n");
		cse4589_print_and_log("PORT:%d\n", ntohs(portvalue.sin_port));
		cse4589_print_and_log("[PORT:END]\n");
	}
}

//https://www.binarytides.com/c-program-to-get-ip-address-from-interface-name-on-linux/ - reference
void printIP()
{
    const char* gServer = "8.8.8.8";
    int gPort = 53;
    struct sockaddr_in serv;
    int sock = socket ( AF_INET, SOCK_DGRAM, 0);
     
    if(sock < 0)
        perror("Socket error");
     
    memset( &serv, 0, sizeof(serv) );
    serv.sin_family = AF_INET;
    serv.sin_addr.s_addr = inet_addr( gServer );
    serv.sin_port = htons( gPort );
 
    int errRet = connect( sock , (const struct sockaddr*) &serv , sizeof(serv) );
     
    struct sockaddr_in name;
    socklen_t namelen = sizeof(name);
    errRet = getsockname(sock, (struct sockaddr*) &name, &namelen);  
    char buffer[100];
    const char* p = inet_ntop(AF_INET, &name.sin_addr, buffer, 100);
         
    if(p != NULL)
    {
    	cse4589_print_and_log("[IP:SUCCESS]\n");
        cse4589_print_and_log("IP:%s\n",buffer);
        cse4589_print_and_log("[IP:END]\n");
    }
    else
    {
    	cse4589_print_and_log("[IP:ERROR]\n");
	cse4589_print_and_log("[IP:END]\n");
    }
    close(sock);
}

//https://newbedev.com/how-do-you-validate-that-a-string-is-a-valid-ipv4-address-in-c -- reference.
int validateIP(char *ip)
{
	struct sockaddr_in temp;
	int result= inet_pton(AF_INET, ip, &temp.sin_addr);
	if(result==1)
		return 1;
	else
		return 0;
}

int getConnection(char *serverIP, int sPort)
{
    struct sockaddr_in localServerAddr;

    bzero(&localServerAddr, sizeof(localServerAddr));
    localServerAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP, &localServerAddr.sin_addr);
    localServerAddr.sin_port = htons(sPort);
    
    if(connect(fdsocket, (struct sockaddr*)&localServerAddr, sizeof(localServerAddr)) < 0)
        perror("Connect failed");
    else
    	printf("\nLogged in\n");
    
    return fdsocket;
}

