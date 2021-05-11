/*************************************************************************
  > File Name: server.c
  > Author: 小学生
  > Mail: wyf157703@163.com 
  > Created Time: 2021年05月09日 星期日 11时32分34秒
 ************************************************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <arpa/inet.h>

#define port 8000

int sockfd,newfd;
int sin_size;
struct sockaddr_in sockaddr;
struct sockaddr_in clientaddr;

void handle(char cmd)
{
	char filename[30] = {0};
	int FileNameSize = 0;
	int fd,len;
	int filesize;
	int count = 0, totalrecv = 0;
	char buf[1024];
	struct stat fstat;


	switch(cmd)
	{
		case 'U':
			{
				//接受文件名
				read(newfd,&FileNameSize,4);
				read(newfd,(void*)filename,FileNameSize);
				filename[FileNameSize] = '\0';
				printf("%s\n",filename);

				//创建文件
				if((fd = open(filename,O_RDWR|O_CREAT)) == -1)
				{
					perror("open");
					exit(-1);
				}

				//接受文件长度
				read(newfd,&filesize,4);
				//接受文件模式
				read(newfd,&fstat.st_mode,4);
				//接收文件
				while((count = read(newfd,(void *)buf,1024)) > 0)
				{
					write(fd,&buf,count);
					totalrecv += count;

					if(totalrecv == filesize){
						chmod(filename,(fstat.st_mode | S_IRWXU));
						totalrecv = 0;
						break;

					}
				}
				printf("upload success\n");

				close(fd);
			}
			break;

		case 'D':
			{
				//接收文件名
				read(newfd,&FileNameSize,4);
				read(newfd,filename,FileNameSize);
				filename[FileNameSize] = '\0';
				//打开文件
				if((fd = open(filename,O_RDONLY)) == -1)
				{
					perror("creat");
					exit(-1);
				}
				//发送文件长度和文件模式
				if((stat(filename,&fstat)) == -1)
					return;
				write(newfd,(void *)&fstat.st_size,4);
				write(newfd,(void *)&fstat.st_mode,4);

				while((count = read(fd,(void *)buf,1024)) > 0)
				{
					write(newfd,&buf,count);
				}
				printf("download success\n");

				close(fd);
			}
			break;
	}
}

int main(int argc,char *argv[])
{
	int i;
	char cmd;
	char buf[1024];
	char bufff[100];
	ssize_t	 n;
	//创建套接字
	if((sockfd = socket(AF_INET,SOCK_STREAM,0)) == -1)
	{
		perror("socket");
		exit(-1);
	}

	memset(&sockaddr,0,sizeof(sockaddr));
	sockaddr.sin_family = AF_INET;
	sockaddr.sin_port = htons(port);
	sockaddr.sin_addr.s_addr = htonl(INADDR_ANY);

	//绑定
	if(bind(sockfd,(struct sockaddr*)&sockaddr,sizeof(sockaddr)) == -1){
		perror("bind");
		exit(-1);
	}

	//监听
	if(listen(sockfd,10) == -1){
		perror("listen");
		exit(-1);
	}

	printf("listen ....\n");

	//连接
	while(1)
	{

		sin_size = sizeof(clientaddr);

		newfd = accept(sockfd, (struct sockaddr *)&clientaddr, &sin_size);
		printf("%s,%d\n",inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,buf,sizeof(buf)),ntohs(clientaddr.sin_port));
		while(1)
		{
			read(newfd,&cmd,1);
			if(cmd == 'Q')
				break;
			else
				handle(cmd);
		}
		if(close(newfd) == 0)
			break;

	}
	close(sockfd);
	return 0;
}

//	n = read(newfd, buf, sizeof(buf));

/*	if((newfd = accept(sockfd,(struct sockaddr *)(&clientaddr),&sin_size) == -1)){
	perror("accept");
	exit(-1);
	}
	printf("accept ....\n");


	printf("%s,%d\n",inet_ntop(AF_INET,&clientaddr.sin_addr.s_addr,buf,sizeof(buf)),ntohs(clientaddr.sin_port));

	n =read(newfd,buf,sizeof(bufff));

	for(i = 0; i < n; i++)
	{
	printf("%c\n",buf[i]);
	}



	while(1)
	{
	read(newfd,&cmd,1);
	printf("%c\n",cmd);
	if(cmd == 'Q')
	break;
	else
	handle(cmd);
	}
	close(newfd);
	}







//	read(newfd,&cmd,1);
//	printf("%c\n",cmd);
while(1)
{
read(newfd,&cmd,1);
if(cmd == 'U'){
printf("qwe\n");
break;
}
else {
handle(cmd);
}
}

close(newfd);


close(sockfd);
return 0;
}
*/
