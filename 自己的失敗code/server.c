#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
// #include <time.h>
#include <sys/time.h>
#include  <errno.h>
#include <arpa/inet.h> 
#include <sys/wait.h>
#include <pthread.h>
#define TIMEOUT 100
/*****************notice**********************
 * 
 * You can follow the comment inside the code.
 * This kind of comment is for basic part.
 * =============== 
 * Some hints...
 * ===============
 * 
 * This kind of comment is for bonus part.
 * ---------------
 * Some hints...
 * ---------------
 * 
 * 
 * 
 *********************************************/


//==============
// Packet header 
//==============
typedef struct header{
	unsigned int seq_num;
	unsigned int ack_num;
	unsigned char is_last;
}Header;

//==================
// Udp packet & data 
//==================
typedef struct udp_pkt{
	Header header;
	char data[1024];
}Udp_pkt;


//=============
// Declaration
//=============
int sockfd;
struct sockaddr_in info, client_info;
Udp_pkt snd_pkt,rcv_pkt;
socklen_t len;
pthread_t th1,th2;
int first_time_create_thread = 0;
struct timeval tv={0,100000};
printf("%lld\n", (long long)tv.tv_sec);
tv.tv_sec=0;
tv.tv_usec=100000;
// struct timeval timeout={0,100000};
// timeout.tv_sec=0;
// timeout.tv_usec=100000;


//==================================
// You should complete this function
//==================================

int sendFile(FILE *fd)

{	
	int filesize=ftell(fd);
	//----------------------------------------------------------------
	// Bonus part for declare timeout threads if you need bonus point,
	// umcomment it and manage the thread by youself
	//----------------------------------------------------------------
	// At the first time, we need to create thread.
	if(!first_time_create_thread)
	{
		first_time_create_thread=1;
		//pthread_create(&th1, NULL, receive_thread, NULL);
		//pthread_create(&th2, NULL, timeout_process, NULL);
	}
	/*******************notice************************
	 * 
	 * In basic part, you should finish this function.
	 * You can try test_clock.c for clock() usage.
	 * checking timeout and receive client ack.
	 * 
	 ************************************************/
	int numbytes;
	int now_num=0;
	int isTimeout=0; 
	 fseek(fd, 0, SEEK_SET); //文件指標移動到頭部
	while (1)
	{
		
		//==========================
		// Send video data to client
		//==========================

		// clock_t TIMEOUT= (100* 1000) / CLOCKS_PER_SEC;
		// clock_t expiredTime = (clock() *1000)/CLOCKS_PER_SEC+TIMEOUT;
		// printf(" expiredTime: %ld \n", expiredTime);
		// printf(" expiredTime: %ld \n", expiredTime);
		// t = (clock()*1000) /CLOCKS_PER_SEC;
		
		// sendto(fd, &num, sizeof(num), 0,(struct sockaddr *)&servaddr, sizeof(struct sockaddr));
		
		if (!isTimeout)
		{
			// printf(" filesize: %d\n", filesize);
			// printf(" sizeof(snd_pkt.data): %zu\n", sizeof(snd_pkt));

			memset(snd_pkt.data, '\0', sizeof(snd_pkt.data));
			printf(" -before subtract--filesize: %d\n", filesize);
			// if (( filesize - 1036 ) >= 0 ){
			fread(snd_pkt.data, sizeof(char), 1023 , fd  );
			// else{
			// fread(snd_pkt.data, filesize, 1 , fd  );
			// }
			
			
			// printf(" -after subtract--filesize: %d\n", filesize);
			snd_pkt.header.seq_num = now_num;
			printf(" -snd_pkt.header.seq_num: %d\n", snd_pkt.header.seq_num);
			// printf(" snd_pkt.header.seq_num: %d\n", snd_pkt.header.seq_num);
			if (( filesize - 1023 ) > 0 ){

			numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len);
			filesize -= 1023;
			}
			else{
			snd_pkt.header.is_last=1;
				numbytes = sendto(sockfd, &snd_pkt, 180, 0,(struct sockaddr *)&client_info, len);
				// numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len);
				filesize -= filesize;
			}
			printf(" after subtract filesize: %d\n", filesize);
			
		}else{
		// retransmission:
			numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len);
			printf(" current subtract filesize: %d\n", filesize);
			isTimeout=0;
		}


		// if (filesize<=0)
		printf("\tSent %d bytes\n", numbytes);
		
		//======================================
		// Checking timeout & Receive client ack
		//======================================
		// t = clock()*1000/CLOCKS_PER_SEC -t;

		// printf(" new time: %ld \n", (clock()*1000)/CLOCKS_PER_SEC );
		int ret=setsockopt(sockfd,SOL_SOCKET,SO_RCVTIMEO,&tv,sizeof(struct timeval));
		if (ret<0){
			printf("setsockopt failed\n");
			perror("Error: ");
		}
		
		
		// assert(ret != -1);
		
		int rcv_size = recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), MSG_WAITALL , (struct sockaddr *)&client_info, (socklen_t *)&len);

		// if (rcv_size==-1 && errno==EAGAIN)
		if (rcv_size<0 && errno == EINPROGRESS)
		{
			printf("timeout in server!!!");
			isTimeout=1;
			continue;
		}
		else{
			len = sizeof(client_info);
			recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0 , (struct sockaddr *)&client_info, (socklen_t *)&len);
			now_num++;
			printf("      Receive a packet ack_num = %d\n", rcv_pkt.header.ack_num);
			isTimeout=0;
		}
		
		/*
		if (  clock() * 1000 / CLOCKS_PER_SEC >= expiredTime )
		{
			//timeout發生，RETRANSMISSION
			printf("\tTimeout!Resent Packet!\n");
			continue;

		}*/
			
		//=============================================
		// Set is_last flag for the last part of packet
		//=============================================
		if (snd_pkt.header.is_last) break;
		
	}

	printf("send file successfully\n");
	fclose(fd);
	return 0;
}

int main(int argc, char *argv[])
{
	//===========================
	// argv[1] is for server port
	//===========================
	sockfd = socket(AF_INET , SOCK_DGRAM , 0);

	if (sockfd == -1)
	{
		printf("Fail to create a socket.");
	}
	//=======================
	// input server info
	// IP address = 127.0.0.1
	//=======================
	bzero(&info,sizeof(info));//struct sockaddr_in，zeroes out the whole struct
	info.sin_family = AF_INET;
	int port=atoi(argv[1]);
	info.sin_addr.s_addr = INADDR_ANY ;//INADDR_ANY就是指定地址为0.0.0.0的地址,这个地址事实上表示不确定地址,或“所有地址”、“任意地址”。
	info.sin_port = htons(port);//將主機的無符號短整形數轉換成網絡字節順序
	//基本上，你需要在送出封包以前將數值轉換為 Network Byte Order，並在收到封包之後將數值轉回 Host Byte Order。
	//printf("server %s : %d\n", inet_ntoa(info.sin_addr), htons(info.sin_port));

	//================
	// Bind the socket
	//================	
	if(bind(sockfd, (struct sockaddr *)&info, sizeof(info)) == -1)
	{
		perror("server_sockfd bind failed: ");
		return 0;
	}

	//====================================
	// Create send packet & receive packet
	//====================================
	memset(rcv_pkt.data, '\0', sizeof(rcv_pkt.data));

	//====================
	// Use for client info
	//====================
	bzero(&client_info,sizeof(client_info));
	client_info.sin_family = AF_INET;
	len = sizeof(client_info);

	printf("====Parameter====\n");
	printf("Server's IP is 127.0.0.1\n");
	printf("Server is listening on port %d\n",port);
	printf("==============\n");

	while(1)
	{
		//=========================
		// Initialization parameter
		//=========================
		snd_pkt.header.seq_num = 0;
		snd_pkt.header.ack_num = 0;
		snd_pkt.header.is_last = 0;
		FILE *fd;
		
		printf("server waiting.... \n");
		char *str;
		while ((recvfrom(sockfd, &rcv_pkt, sizeof(rcv_pkt), 0, (struct sockaddr *)&client_info, (socklen_t *)&len)) != -1)
		{
			//! In client, we set is_last 1 to comfirm server get client's first message.
			if(rcv_pkt.header.is_last == 1)
				break;
		}  
		printf("process command.... \n");
		str = strtok(rcv_pkt.data, " ");
		
		//===============================================================
		// command "download filename": download the file from the server
		// and then check if filename is exist
		//===============================================================
		if(strcmp(str, "download") == 0) 
		{
			str = strtok(NULL, " \n");
			printf("filename is %s\n",str);
			//===================
			// if file not exists 
			//===================
			if((fd = fopen(str, "rb")) == NULL) 
			{    
				//=======================================
				// Send FILE_NOT_EXISTS msg to the client
				//=======================================
				printf("FILE_NOT_EXISTS\n");
                		strcpy(snd_pkt.data, "FILE_NOT_EXISTS");
				int numbytes;
				if ((numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len)) == -1) 
				{
					printf("sendto error\n");
					return 0;
				}
				printf("server: sent %d bytes to %s\n", numbytes,inet_ntoa(client_info.sin_addr));//用于网络字节序IP转化点分十进制IP;
			} 
			//==================
			// else, file exists 
			//==================
			else 
			{
				//文件指针指向文件的檔案結尾，為了使用ftell回傳從檔案頭到當前位置的 byte 數
				fseek(fd,0,SEEK_END);
                		printf("FILE_EXISTS\n");
				strcpy(snd_pkt.data, "FILE_EXISTS");
				
				
				//==================================
				// Send FILE_EXIST msg to the client
				//==================================
				int numbytes;
				if ((numbytes = sendto(sockfd, &snd_pkt, sizeof(snd_pkt), 0,(struct sockaddr *)&client_info, len)) == -1) 
				{
					printf("sendto error\n");
					return 0;
				}
				printf("server: sent %d bytes to %s\n", numbytes,inet_ntoa(client_info.sin_addr));

				//==========================================================================
				// Sleep 1 seconds before transmitting data to make sure the client is ready
				//==========================================================================
				sleep(1);
				printf("trasmitting...\n");

				//=====================================
				//! Start to send the file to the client
				//=====================================

				sendFile(fd);
				
			}
		}
		else 
		{
			printf("Illegal request!\n");   
		}
	}
}
