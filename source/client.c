/*************************************************************************
  > File Name: client.c
  > Author: 小学生
  > Mail: wyf157703@163.com 
  > Created Time: 2021年05月09日 星期日 09时38分30秒
 ************************************************************************/

#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define port 8000

int sockfd;
struct sockaddr_in sockaddr;
char ipaddr[16];


void my_link(void)
{
	printf("socket start\n");
	//创建soket
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1){
		perror("socket");
		exit(-1);
	}


	bzero(&sockaddr,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port); //转换
	//	inet_pton(AF_INET,ipaddr,&sockaddr.sin_addr);
	sockaddr.sin_addr.s_addr = inet_addr(ipaddr);


	if(connect(sockfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr)) == -1){
		perror("connect");
		exit(-1);
	}
	/*
	   write(sockfd,&buf,sizeof(buf));
	   printf("socket sssssart\n");
	   */	
}

void upload_file(char *filename)
{
	int FileNameSize = strlen(filename);
	char cmd = 'U';
	struct stat fstat;
	char buf[1024];
	int count = 0;
	int fd;

	//打开文件
	fd = open(filename,O_RDONLY);
	printf("%s\n",filename);
	//发送命令
	write(sockfd,&cmd,1);
	printf("123\n");

	//发送文件名
	write(sockfd,(void *)&FileNameSize,4);
	write(sockfd,filename,FileNameSize);

	//发送文件长度
	if((stat(filename,&fstat)) == -1)
		return;
	write(sockfd,(void *)&fstat.st_size,4);
	//发送文件模式
	write(sockfd,(void *)&fstat.st_mode,4);

	//发送文件数据
	while((count = read(fd,(void* )buf,1024)) > 0)
	{
		write(sockfd,buf,count);
	}

	close(fd);
}

void download_file(char *filename)
{
	char cmd = 'D';
	int  fd;
	int  FileNameSize = strlen(filename);
	int  buf[1024];
	int  filesize = 0,count = 0,totalrecv = 0;
	struct stat fstat;

	//发送命令
	write(sockfd,&cmd,1);

	//发送文件名
	write(sockfd,(void*)&FileNameSize,4);
	write(sockfd,filename,FileNameSize);

	//打开文件并创建
	if((fd = open(filename,O_RDWR|O_CREAT)) == -1)
	{
		perror("open");
		exit(-1);
	}

	//接受文件长度和模式
	read(sockfd,&filesize,4);
	read(sockfd,&fstat.st_mode,4);

	while((count = read(sockfd,(void *)buf,1024)) > 0)
	{
		write(fd,buf,count);
		totalrecv += count;

		if(totalrecv == filesize){
			chmod(filename,(fstat.st_mode | S_IRWXU));
			totalrecv = 0;
			printf("quit\n");
			break;
		}
	}

	close(fd);
}

void quit(void)
{
	char cmd = 'Q';
	write(sockfd,&cmd,1);
	system("clear");

	exit(0);
}

void my_menu(void)
{
	int num;
	char c;
	char file_up[30];
	char file_dw[30];

	while(1)
	{
		printf("\n-------------1.upload file----------\n");
		printf("-------------2.download file--------\n");
		printf("-------------3.quit-----------------\n");

		printf("please enter number: ");
		scanf("%d",&num);

		switch(num){
			case 1 :
				{
					printf("upload file:");
					while((c = getchar()) != '\n' && c != EOF);
					fgets(file_up,30,stdin);
					file_up[strlen(file_up)-1] = '\0';
					upload_file(file_up);
				}
				break;
			case 2:
				{
					printf("download file:");
					while((c = getchar()) != '\n' && c != EOF);
					fgets(file_dw,30,stdin);
					file_dw[strlen(file_dw)-1] = '\0';
					download_file(file_dw);
				}
				break;
			case 3:
				{
					quit();
					break;
				}
				break;
			default:
				{
					printf("please enter right number\n");
				}
				break;
		}
	}
}


int main(int argc,char *argv[])
{

	if(argc != 2){
		printf("error: you must enter ipaddr like this: 193.0.0.6\n");
		exit(1);
	}

	strcpy(ipaddr,argv[1]);
	printf("123\n");

	//建立socket连接
	my_link();

	//打印菜单
	my_menu();
	return 0;
}
