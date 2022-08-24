#include "csapp.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <pthread.h>
#include <time.h>


int score;
int level;
int numTomatoes;
char Grid[108];
pthread_mutex_t lock;
pthread_mutex_t lock2, lock3;
//int c = 0;


typedef struct
{
    int x;
    int y;
} Position;

typedef struct Client Client;
struct Client {

  char playernumber;
  int fd;
  //bool inGame;
  Position playerPosition;
  Client* next;

};

Client* globalClient = NULL;

Position indexToCord(int i){
    Position newPos;
    int count=0;
    while(i>=10){
        i-=10;
        count++;
    }
    newPos.x=i;
    newPos.y=count;
    return newPos;
}
int CordToindex(int x,int y){
    int index =(y*10)+x;
    return index;
}


Client* insert(Client* head,int connfd, char playernum, Position p) { 

    if (head == NULL) {

        head = malloc(sizeof(Client));
        head->next = NULL;
        head->fd = connfd;
        //head->inGame = ingame;
        head->playernumber = playernum;
        head->playerPosition = p;

    }
    else {

        Client* newJob = malloc(sizeof(Client));
        newJob->next = head;
        newJob->fd = connfd;
        //newJob->inGame = ingame;
        newJob->playernumber = playernum;
        newJob->playerPosition = p;
        head = newJob;

    }
    return head;

}
Client* deleteClient(Client* head, char number){
    
    // Store head node
    Client* temp = head;
    Client* prev = NULL;

    // If head node itself holds
    // the key to be deleted
    pthread_mutex_lock(&lock);

    if (temp != NULL )
    {
            if(temp->playernumber== number){
                // printf("In delete 1\n");
                head = temp->next; // Changed head
                // printf("In delete 2\n");

                int p = CordToindex(temp->playerPosition.x, temp->playerPosition.y);
                Grid[p] = 'G';
                free(temp);  
                pthread_mutex_unlock(&lock);                             // free old head
                return head;
            }
    }
        
    while (temp != NULL)
    {
        if(temp->playernumber != number){
            prev = temp;
            temp = temp->next;
        }else{
            break;
        }
    }

 
    // If key was not present in linked list
    if (temp == NULL){
        pthread_mutex_unlock(&lock); 
        return head;
    }
 
    // Unlink the node from linked list
    int q= CordToindex(temp->playerPosition.x, temp->playerPosition.y);
    Grid[q] = 'G';

    prev->next = temp->next;
    //updatejobID();
 
    // Free memory
    free(temp);
    pthread_mutex_unlock(&lock); 
    return head;
    
}
Client* pop(Client* head) {

    if (head == NULL) {

    }
    else {

        if (head->next != NULL) {
            Client* temp = head;
            head = head->next;
            free(temp);
        }
        else {
            Client* temp = head;
            head = NULL;
            free(temp);
        }
    }

    return head;

}
void terminate_all(Client* head) {
    Client* del;

    while (head != NULL) {
        del = head;
        head = head->next;
        free(del);

    }
}


double rand01()
{
    return (double) rand() / (double) RAND_MAX;
}


/*----------------Score and Level Helper-------------------*/
int charToInt(char c){
    return c - '0';
}
void initScore(){

    int num1,num2,num3,num4,num5,ans;
    ans = score;
    num1 = ans/10000;
    ans-=(num1*10000);
    num2 = ans/1000;
    ans-=(num2*1000);
    num3 = ans/100;
    ans-=(num3*100);
    num4 = ans/10;
    ans-=(num4*10);
    num5 = ans/1;
    ans-=(num5*1);

    Grid[100]= num1+'0';
    Grid[101]= num2+'0';
    Grid[102]= num3+'0';
    Grid[103]= num4+'0';
    Grid[104]= num5+'0';

}
void initlevel(){

    int num1,num2,num3,ans;
    ans = level;
    num1 = ans/100;
    ans-=(num1*100);
    num2 = ans/10;
    ans-=(num2*10);
    num3 = ans/1;
   
    Grid[105]= num1+'0';
    Grid[106]= num2+'0';
    Grid[107]= num3+'0';
  

}
int getScore(){
    int sum =0;
  


    sum+=charToInt(Grid[100])*10000;
    sum+=charToInt(Grid[101])*1000;
    sum+=charToInt(Grid[102])*100;
    sum+=charToInt(Grid[103])*10;
    sum+=charToInt(Grid[104])*1;
    return sum;

}
int getlevel(){
    int sum =0; 
  
    sum+=charToInt(Grid[105])*100;
    sum+=charToInt(Grid[106])*10;
    sum+=charToInt(Grid[107])*1;
    return sum;

}
/*---------------------------------------------------------*/
/*----------------Grid helper------------------------------*/
void generategrid(){
    for (int i = 0; i < 100; i++) {
        int p = rand()%10;

        if(p==1){
            Grid[i]='T';
            numTomatoes++;
        }else{
            Grid[i]='G';
        }

    }
    for (int j = 0; j < 8; j++){
        Grid[100+j]='0';

    }
    initScore();
    initlevel();
}
void updategrid(){

    Client* temp =  globalClient;

    //pthread_mutex_lock(&lock); 
    while(temp!=NULL){
       int index = CordToindex(temp->playerPosition.x,temp->playerPosition.y);
       char item = Grid[index];
       if(item =='T'){

           //pthread_mutex_lock(&lock);
           numTomatoes--;
           //pthread_mutex_unlock(&lock);
           if (numTomatoes == 0) {
                //pthread_mutex_lock(&lock);
                level++;
                //pthread_mutex_unlock(&lock);
                initlevel();
                generategrid();
                updategrid();

           }
           Grid[index]= temp->playernumber;
       }else if(item =='G'){
           Grid[index]= temp->playernumber;
       }
       temp = temp->next;  
    }
    int tomatoCount=0;
    for (int i = 0; i < 100; i++) {

        if(Grid[i]=='T'){
            tomatoCount++;
        }

    }

    if(tomatoCount == 0){
        //pthread_mutex_lock(&lock);
        numTomatoes =0;
        level++;
        //pthread_mutex_unlock(&lock);
        initlevel();
        generategrid();
        updategrid();

    }

    //pthread_mutex_unlock(&lock); 



}
char getPlayernumber(int connfd){
    Client * temp = globalClient;
    while(temp!= NULL){

        if(temp->fd == connfd){
            return temp->playernumber;
        }

        temp= temp->next;

    }
    return 'N';
}
/*---------------------------------------------------------*/

void moveTo(char move, char playernumbe )
{
    Client *temp = globalClient;
    while(temp !=NULL){
        if(temp->playernumber == playernumbe){
            break;
        }

        temp=temp->next;
    }

    if(temp == NULL){
        return;
    }
    int oldIndex = CordToindex(temp->playerPosition.x, temp->playerPosition.y);

    Position CurrentplayerPosition = temp->playerPosition;
    int x,y;

    switch (move)
    {
        case'N':
        y = CurrentplayerPosition.y-1;
        x = CurrentplayerPosition.x;
        break;

        case'E': 
        y = CurrentplayerPosition.y;
        x = CurrentplayerPosition.x-1;

        break;

        case'W':
        y = CurrentplayerPosition.y;
        x = CurrentplayerPosition.x+1;

        break;

        case'S':
        y = CurrentplayerPosition.y+1;
        x = CurrentplayerPosition.x;

        break;

        case'Q':
        //globalClient = deleteClient(temp);
        y = CurrentplayerPosition.y;
        x = CurrentplayerPosition.x; 

        break;

        default:
        break;
    }


    
    if (x < 0 || x >= 10|| y < 0 || y >= 10)
        return;

    // Sanity check: player can only move to 4 adjacent squares
    if (!(abs(CurrentplayerPosition.x - x) == 1 && abs(CurrentplayerPosition.y - y) == 0) &&!(abs(CurrentplayerPosition.x - x) == 0 && abs(CurrentplayerPosition.y - y) == 1)) 
    {
        fprintf(stderr, "Invalid move attempted from (%d, %d) to (%d, %d)\n", CurrentplayerPosition.x, CurrentplayerPosition.y, x, y);
        return;
    }

    temp->playerPosition.x = x;
    temp->playerPosition.y = y;

    int newIndex= CordToindex(x,y);

    if (Grid[newIndex] == 'T') {
        Grid[newIndex] = temp->playernumber;
        pthread_mutex_lock(&lock);
        score++;
        numTomatoes--;
        pthread_mutex_unlock(&lock);
        initScore();
        if (numTomatoes == 0) {
            pthread_mutex_lock(&lock);
            level++;
            pthread_mutex_unlock(&lock);
            initlevel();
            generategrid();
            updategrid();
        }
    }
    Grid[oldIndex] = 'G';
}

void* thread(void* confd);

int main(int argc, char* argv[]){

    
    srand(time(NULL));
    score= 0;
    level =1;
    generategrid();
    int playerCount = 0;

    pthread_t tid;
    int listenfd, *connfd;
    socklen_t clientlen;
    struct sockaddr_storage clientaddr; /* Enough room for any addr */ 
    char client_hostname[MAXLINE], client_port[MAXLINE];
    listenfd = Open_listenfd(argv[1]);
    while (1) {
        clientlen = sizeof(struct sockaddr_storage); // Important! 
        connfd =Malloc(sizeof(int)); 
        *connfd = Accept(listenfd, (SA *)&clientaddr, &clientlen);
        if(*connfd!=0){
            playerCount += 1;
            char playerNum = playerCount + '0';
            Getnameinfo((SA *) &clientaddr, clientlen, client_hostname, MAXLINE, client_port, MAXLINE, 0);
            printf("Connected to (%s, %s)\n", client_hostname, client_port);
            Position initialPosition;
            initialPosition.x = rand()%10;
            initialPosition.y = rand()%10;
            globalClient = insert(globalClient,*((int *)connfd), playerNum, initialPosition);
            updategrid();
            Pthread_create(&tid, NULL, thread, connfd);
        }else{
            Free(connfd);
        }
  
  
    }
    Close(*((int*)connfd));

}



void* thread(void* confd){

    int connfd = *((int *)confd);
    Pthread_detach(pthread_self());
    Free(confd);

    bool updateclient = false;
    char currPNUM = getPlayernumber(connfd);
    if(currPNUM=='N'){
        perror("Error while getting Player Number in thread \n");
    }   

    Rio_writen(connfd, Grid, 108);
    rio_t rio;
    while(1){
       // sleep(1);
        
        char buf[MAXLINE];
        Rio_readinitb(&rio, connfd);
        Rio_readlineb(&rio, buf, 2);
        //playermove = buf[0];
        if(buf[0] == 'E'){
            moveTo(buf[0], currPNUM);
            updateclient =true;
        }
         if(buf[0] == 'S'){
            moveTo(buf[0], currPNUM);
            updateclient =true;
        }
         if(buf[0] == 'W'){
            moveTo(buf[0], currPNUM);
            updateclient =true;
        }
         if(buf[0] == 'N'){
            moveTo(buf[0], currPNUM);
            updateclient =true;
        }

        if(buf[0] == 'Q'){

            globalClient =deleteClient(globalClient,currPNUM);
            updategrid();
            updateclient =true;
            break;

        }

        if(updateclient){
            updategrid();
            updateclient=false;
        }

        Rio_writen(connfd, Grid, 108);
        buf[0] = 'K';          
        
    }
    return NULL;
}