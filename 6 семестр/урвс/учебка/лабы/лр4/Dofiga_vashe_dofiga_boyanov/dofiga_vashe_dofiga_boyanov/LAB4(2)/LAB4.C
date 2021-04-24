#include<math.h>
#include<fcntl.h>
#include<unistd.h>
#include<signal.h>

union BinaryData
{
  float FloatValue;
  int IntegerValue;
};

void int2bin (int ,char[]);
int bin2int (char[]);

int main(void)
{ 
  union BinaryData f,f1;
  int status,k,i,pid,fd,fd1,BytesRead;
  unsigned int mask=1,N;
  float XStep,x,PoweredX;
  char str1[33],str2[33];
  printf ("Enter interations quantity->");
  scanf ("%d",&N);
  printf ("Enter X value->");
  scanf ("%f",&x);
  if ((x<0)||(x>M_PI))
  {
    printf ("Value of X must be from 0 to PI\n");
    return 1;
  }

  pid=fork();
  if (pid==-1)
  {
    printf ("Can not start an offspring process\n");
    return 1;
  }
  if (pid==0)
  {
    fd1=open("temporary.data",O_RDONLY);
    while (strcmp(str2,"stop"))
    {
      BytesRead=read(fd1,str2,32);
      str2[32]='\0';
      if ((BytesRead!=0)&&(strcmp(str2,"stop"))) 
      {
        f1.IntegerValue=bin2int(str2);
        printf ("-=%f=-\n",f1.FloatValue);
      }
    }
  }
  else
  {
    fd=open("temporary.data",O_RDWR|O_CREAT);
    f.FloatValue=PoweredX=x;
    int2bin(f.IntegerValue,str1);
    write(fd,str1,32);
    for (i=1;i<N;i++)
    {
      PoweredX*=x*x;
      f.FloatValue=PoweredX/(2*i+1);
//      printf ("\n---%f---\n",f.FloatValue);
      int2bin(f.IntegerValue,str1);
      write(fd,str1,32);
    }
    strcpy(str1,"stop");
    write(fd,str1,32);
//    wait(&status);
  }
  return 0;
};

void int2bin (int IntegerValue,char BinaryValue[33])
{
  int i;
  unsigned int mask=1;
  for (i=31;i>=0;i--)
  {
    if (IntegerValue & (int)1)
      BinaryValue[i]='1';
    else
      BinaryValue[i]='0';
    IntegerValue=IntegerValue>>1;
  }
  BinaryValue[32]='\0';
};

int bin2int (char BinaryValue[33])
{
  int i,PowerOf2=1,IntegerValue;
  IntegerValue=0;
  for (i=0;i<32;i++)
  {
    if (BinaryValue[31-i]=='1')
      IntegerValue+=PowerOf2;
    PowerOf2*=2;
  }
  return IntegerValue;
};