#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <signal.h>   
int s;
void kill_socket()
 {
  printf("������ �������� ����������\n");
  close(s);
 }

int main(int argc, char *argv)
{
 struct sockaddr_in serv_addr;
 struct in_addr addr;

 char ch;
 int tmp, tmp_1, i;
 signal(SIGPIPE, kill_socket);
  addr.s_addr = INADDR_ANY;
  serv_addr.sin_family=AF_INET;			//��������� ���������� �������
  serv_addr.sin_addr.s_addr=inet_addr("217.71.130.131");
  serv_addr.sin_port=htons(1500);

  if((s=socket(AF_INET,SOCK_STREAM,0))==-1)	//��������� �����
  {
   printf("������: �� ���� ������� �����!\n");
   return 1;
  }

  if(connect(s,(struct sockaddr *)&serv_addr, sizeof(struct sockaddr_in))==-1)
  {
   printf("������: �� ���� ������������ � �������!\n");
   return 1;
  }
   printf("���������� � �������� �����������!\n");

   if(read(s,&tmp,4)>0)
   {
    printf("\t����� ���������� �� ��� ������!\n");
    printf("������ ���������� �������������: %d\n", tmp+1);
   }
    for (i=0;i<=tmp;i++)
    {
	read(s,&tmp_1,4);
	printf("%d. %s\n",i+1, inet_ntoa(tmp_1));
    }
    while (read(s,&tmp,4))
    {
	printf("������ � %s ����������� � �������! ��������������!\n",inet_ntoa(tmp));
    }
    
    close(s);
  return 0;
}

 
