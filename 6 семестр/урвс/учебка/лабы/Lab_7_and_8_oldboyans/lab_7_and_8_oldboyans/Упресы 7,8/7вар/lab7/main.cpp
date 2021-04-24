/**
 * Program for lab07
 *
 * @author      Malev Kirill
 * @author      Pavlov Alexey
 * @version     1.0
 */

#include <iostream>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
//For message query we must implement these:
#include <sys/ipc.h>
#include <sys/msg.h>
//For sems
#include <sys/sem.h>
//For proper query and sem  initializing
#include <errno.h>
//For sprintf only
#include <stdio.h>

using namespace std;

/*         следующий тип должен быть определен взависимости от конфигурации ОС */
typedef union semun {
            int val;
            struct semid_ds *buf;
            short *array;
            } semun;

//Structure for query message handling
struct QueryMessage {
  long mtype;
  char mtext[64];
};

/**
 * Query initializing function
 * @param       key_t msgkey - key to initialize query with
 * @return      int msgid - if successfull, -1 if not
 */
int initmsg (key_t msgkey) {    // функция инициализации очереди
  int msgid;
  if (( msgid = msgget (msgkey, 0600 |IPC_CREAT|IPC_EXCL)) == -1) {
            if (errno == EEXIST) //  если очередь существовала ранее   
                msgid = msgget (msgkey, 0);	//то открыть ее 
               cout << "Already exist"<<endl;
  } 
  if (msgid == -1 )
    return (-1);
  return (msgid);             
}

/**
 * Semaphore initializing function
 * @param       key_t semkey - key to initialize semaphore with
 * @param       int num - number of semaphores to initialize
 * @return      int semid - if successfull, -1 if not
 */
int initsem (key_t semkey, int num) {
  int status =0, semid, i;
  if (( semid = semget (semkey, num, 0666 | IPC_CREAT |IPC_EXCL)) == -1) {
    if (errno == EEXIST) /*  если набор семафоров существовал ранее   */
      semid = semget (semkey, num, 0);    /*   то открыть его            */
  } 
  if (semid == -1)
      return (-1);
  return (semid);             
}



/**
 * Semaphore value changing function
 * @param       int semid - id of semaphore to change
 * @param       int num - number of semaphore in set to mode
 * @param       int value - value to set
 * @return      0 - if successfull, 1 if not
 */
int setSem (int semid, int num, int value) { 
 struct sembuf p_buf;
 p_buf.sem_num = num;
 p_buf.sem_op = value;
 p_buf.sem_flg = 0;
 if (semop (semid, &p_buf, 1) == -1) {
   cout<<"error in p " << semop (semid, &p_buf, 1)<<endl; 
   exit (1);  
 }
 return (0);
}


int main () {
  pid_t pid_p1,pid_p2,pid_p3,pid_p4; //process ids
  key_t semkey = 0X200, msgkey = 0X201;	//Keys for IPC calls
  int number, status,msgid, semid; //initial values
  struct msqid_ds msq_stat;
  union semun arg;
  QueryMessage qm_p1,qm_p2,qm_p3,qm_p4,rcvd;	//For message sending/delivering
  char* poem[] = {
    "Midway upon the journey of our life",
    "I found myself within a forest dark,",
    "For the straightforward pathway had been lost.",
    "Ah me! how hard a thing it is to say",
    "What was this forest savage, rough, and stern,",
    "Which in the very thought renews the fear.",
    "So bitter is it, death is little more;",
    "But of the good to treat, which there I found,",
    "Speak will I of the other things I saw there.",
    "I cannot well repeat how there I entered,",
    "So full was I of slumber at the moment",
    "In which I had abandoned the true way.",
    "But after I had reached a mountain's foot,",
    "At that point where the valley terminated,",
    "Which had with consternation pierced my heart,",
    "Upward I looked, and I beheld its shoulders,",
    "Vested already with that planet's rays",
    "Which leadeth others right by every road.",
    "Then was the fear a little quieted",
    "That in my heart's lake had endured throughout",
    "The night, which I had passed so piteously.",
    "And even as he, who, with distressful breath,",
    "Forth issued from the sea upon the shore,",
    "Turns to the water perilous and gazes;"
  };

  const int qm_len=sizeof(QueryMessage); //This is size of message. Need it to use msg*

  cout << "Pwnz_teh_win_team presents:"<<endl;

  //Semaphore initializing
  if ((semid = initsem (semkey,4)) < 0) { 
    cout<< "semid = "<<semid<<" errno = "<< errno<<endl;   
    exit (1);
  }
  cout<< "Semophore is ok semid = " <<semid<<endl;

  //Query initializing
  if ((msgid = initmsg (msgkey)) < 0) { 
    cout << "msgid= " <<msgid<<" errno="<<errno<<endl;          
    return (-1);
  }
  cout << "Query is ok msgid = "<<msgid <<endl;

  //Setting base semaphore numbers
  for (int i = 0;  i < 4; i++) {
    arg.val = 0;	//Set 0 as a base
    status = semctl (semid, i, SETVAL, arg); 
    cout<<"sem="<<i<<"\tstatus="<<status<<"\terrno="<< errno<<endl;
  }

  if ( !(pid_p1 = fork ())) {	// Make fork fot p1
    
    cout << endl<<"Fork for pid_p1 created"<<endl<<endl;
    qm_p1.mtype=1;
    for (int i=0;i<23;i+=4) {
      setSem(semid,0,-1);	//Blocking until semaphore is set
      sprintf(qm_p1.mtext,"%s",poem[i]);	// Setting line from a poem as a message
      cout << endl<<i<<"\t"<<qm_p1.mtext<<endl;
      if (msgsnd(msgid, &qm_p1, qm_len, 0) == -1) {	//Message sending
        cout <<"error of send message"<<endl;
        return(-1);
      }  
    }
    exit (0);
  }
  /*else {
    cout << "unable to create fork for p1" << endl;
  }*/
  if (!(pid_p2=fork () )) {
    cout << endl<<"Fork for pid_p2 created"<<endl<<endl;
    qm_p2.mtype=1;
    for (int i=1;i<23;i+=4) {
      setSem(semid,1,-1);	//Block this fork until we have permession to send message
      sprintf(qm_p2.mtext,"%s",poem[i]);	// Setting line from a poem as a message
      cout << endl<<i<<"\t"<< qm_p2.mtext<<endl;
      if (msgsnd(msgid, &qm_p2, qm_len, 0) == -1) {	//Message sending
        cout <<"error of send message"<<endl;
        return(-1);
      }  
    }
    exit (0); 
  }
  if (!(pid_p3=fork () )) {
    cout << endl<<"Fork for pid_p3 created"<<endl<<endl;
    qm_p3.mtype=1;
    for (int i=2;i<23;i+=4) {
      setSem(semid,2,-1);
      sprintf(qm_p3.mtext,"%s",poem[i]);	// Setting line from a poem as a message
      cout<<endl<<i<<"\t" << qm_p3.mtext<<endl;
      if (msgsnd(msgid, &qm_p3, qm_len, 0) == -1) {	//Message sending
        cout <<"error of send message"<<endl;
        return(-1);
      }  
    }
    exit(0);
  }
  if (!(pid_p4=fork () )) {
    cout << endl<<"Fork for pid_p4 created"<<endl<<endl;
    qm_p4.mtype=1;
    for (int i=3;i<24;i+=4) {
      setSem(semid,3,-1);
      sprintf(qm_p4.mtext,"%s",poem[i]);	// Setting line from a poem as a message
      cout<< endl<<i<<"\t"<< qm_p4.mtext<<endl;
      if (msgsnd(msgid, &qm_p4, qm_len, 0) == -1) {	//Message sending
        cout <<"error of send message"<<endl;
        return(-1);
      }  
    }
    exit(0);
  }
 
  cout << endl<< "Starting poem building"<<endl<<endl;

  for (int j=0;j<6;j++){
    //Cicle for 4 semaphores we have
    for (int i=0;i<4;i++){
      setSem(semid,i,1);	//Give fork permission to send message
      if (msgrcv (msgid, &rcvd, qm_len, 1, 0)) {  //Message recieving
        cout << i<<"\t>"<<rcvd.mtext << endl;	//And printing to stdout
      }
    }
  }
  
  //Waiting for forks to close
  number=waitpid (pid_p1,&status,0);
  cout << "Process with pid = "<<pid_p1<<" exited with status "<<status<<endl;
  number=waitpid (pid_p2,&status,0);
  cout << "Process with pid = "<<pid_p2<<" exited with status "<<status<<endl;
  number=waitpid (pid_p3,&status,0);
  cout << "Process with pid = "<<pid_p3<<" exited with status "<<status<<endl;
  number=waitpid (pid_p4,&status,0);
  cout << "Process with pid = "<<pid_p4<<" exited with status "<<status<<endl;

  //Closing IPC structtures like message
  status = msgctl (msgid, IPC_RMID, &msq_stat); 
  if (status == -1)
     cout<<"error of RMD mes"<<endl;           
  else 
     cout <<"Message query remove is ok"<<endl;
  //and semaphore
  status = semctl (semid, 0,IPC_RMID, arg); 
  if (status == -1)
     cout<<"error of RMD sem"<<endl;           
  else 
     cout <<"Sem object remove is ok"<<endl;
  return 0;
}
