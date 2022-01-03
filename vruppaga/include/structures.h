struct clientMessageStruct
{
	char cmd[20];
	char ip[32];
	char info[256];
};

struct blockList
{
	int bid;
	char blockIP[32];
	char ip1[32];
	char ip2[32];
	char ip3[32];
	char ip4[32];
	char buffer[1024];
}*clientPointer[5];


struct listInfo
{
	int list_id;
	char hostname[40];
	char listIP[32];
	int portNum;
	int fd_socket;
	int numMsgRecv;
	int numMsgSent;
	char status[20];

}*listPointer[5];

struct serverMessageStruct
{
	char cmd[20];
	char sender_ip[32];
	char info[256];
	struct listInfo listData;
};

