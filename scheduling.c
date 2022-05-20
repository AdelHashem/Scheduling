#include <stdio.h>
#include <string.h>
#include<unistd.h>
#include <stdlib.h>

//Building Queue
typedef struct qnode{
int data;
int cpu;
int io;
int priority;
int time;
struct qnode* next ;
}Qnode;

typedef struct {
Qnode* top;
Qnode *rear;
int size;
}Queue;

int IsEmpty(Queue*q);
void CreateQueue(Queue *q);
Qnode* NewNode(int ndata,int prio);
void QueueIn(Queue* q ,int ndata,int cpu,int io,int pri);
int priorityOut(Queue *q);
int QueueOut(Queue*q);
///////////////////////////////////////



void LoadFile(char *path);
void Getdata();
void FCFS();
void output(int cycle,Queue *running,Queue *ready,Queue *blocked);

FILE *f;
Queue data;
int tr[3];

int main()
{
    char path[512] = "";
    
    int detector;
    scanf("%d",&detector);
    // see if it fcfs or RR
    if (detector == 0)//fcfs
    {
        LoadFile(path);
    }else{ //RR
        int quantum;
        scanf("%d",&quantum);
        LoadFile(path);
    }

    if (f = fopen(path,"r")){
        Getdata();
        FCFS();

    }else
        printf("File Not Exist");

    return 0;
}

void LoadFile(char *path)
{
getcwd(path,sizeof(path)); //go to the current path
printf("Enter The File Name ==> ");
char filename[50];
scanf("%s",filename);
strcat(path,filename);
}

void Getdata()
{
    CreateQueue(&data); //Create Pri Q with Arrival Time As priority
    int p,cpu,io,arrive; //
    while(!feof(f)){
    fscanf(f, "%d %d %d %d",&p,&cpu,&io,&arrive);
    QueueIn(&data,p,cpu,io,arrive);
    }
}

void FCFS()
{
    Queue ready,blocked,running; // create 3 Qs
    CreateQueue(&ready);CreateQueue(&blocked);CreateQueue(&running);
    int cycle = 0; //start with cycle 0
    int P_time = -1;
    int flag = 0;
    int temp;
    while (flag == 0 )
    { 
       
        P_time ++;
        // push p when its time comes
        while(!IsEmpty(&data) && priorityOut(&data) == cycle)
        {
            QueueIn(&ready,data.top->data,data.top->cpu,data.top->io,0);
            temp = QueueOut(&data);
        }
        // Process the blocking Q
        Qnode *tempn = blocked.top;
        for(int i = 0;i< blocked.size;i++)
        {
            tempn->io --;
            tempn->priority --;
            tempn = tempn->next;
        }
        while(!IsEmpty(&blocked) && priorityOut(&blocked) == 0)
        {
            QueueIn(&ready,blocked.top->data,blocked.top->cpu,-1,0);
            temp = QueueOut(&blocked);
        }
        ////////

        // Deal with running P
        if(!IsEmpty(&ready) && IsEmpty(&running)){
            QueueIn(&running,ready.top->data,ready.top->cpu,ready.top->io,0);
            temp = QueueOut(&ready);
            P_time = 0;}
        else if(!IsEmpty(&running))
        {
            //see if the p ended
            if(running.top->cpu == P_time)
            {
                if(running.top->io == -1) //no i/o
                {
                    temp = QueueOut(&running);
                }else //i/o blocking
                {
                    QueueIn(&blocked,running.top->data,running.top->cpu,running.top->io,running.top->io);
                    temp = QueueOut(&running);
                }
                if(!IsEmpty(&ready))
                    {QueueIn(&running,ready.top->data,ready.top->cpu,ready.top->io,0);
                     temp = QueueOut(&ready);}
                P_time = 0;               
            }
        }
        output(cycle,&running,&ready,&blocked);
        if(!IsEmpty(&running))
            printf("%d cycle :%d ",running.top->data,cycle);

        cycle ++;
        if(data.size == 0 && IsEmpty(&ready) && IsEmpty(&running) && IsEmpty(&blocked)){
            flag = 1;
        }
            
    }
    
}

void output(int cycle,Queue *running,Queue *ready,Queue *blocked)
{
    FILE *fc = fopen("ouputFCFS.txt","a");
    fprintf(fc,"%d ",cycle);
    if(running->size == 1)
        {fprintf(fc,"%d:Running ",running->top->data);}
    Qnode *temp = ready->top;
    while (temp != NULL)
    {
        fprintf(fc,"%d: Ready ",temp->data);
        temp = temp->next;
    }
    temp = blocked->top;
    while (temp)
    {
        fprintf(fc,"%d: blocked ",temp->data);
        temp = temp->next;
    }
    fprintf(fc,"\n");
    fclose(fc);

}



// Queue Funs
int IsEmpty(Queue*q)
{
return !q->size;
}
void CreateQueue(Queue *q)
{
    q->top = NULL;
    q->rear = NULL;
    q->size = 0;
}

Qnode* NewNode(int ndata,int prio)
{
    Qnode *temp = (Qnode*) malloc(sizeof(Qnode));
    temp->data = ndata;
    temp->priority = prio;
    temp->next = NULL;
    return temp;
}


void QueueIn(Queue* q ,int ndata,int cpu,int io,int pri)
{
Qnode *newn = (Qnode*)malloc(sizeof(Qnode));
newn->data = ndata;
newn->priority = pri;
newn->cpu = cpu;
newn->io = io;
newn->next=NULL;

if (IsEmpty(q))
{
q->top = newn;
q->rear = newn;
q->size ++;
}
else if (q->top->priority > pri || (q->top->priority == pri && ndata < q->top->data ))
{
newn->next=q->top;
q->top = newn;
q->size ++;
}
else
{
Qnode *temp = q->top;
while(temp->next != NULL && pri > temp->next->priority)
{
temp=temp->next;

}
if (temp->next !=NULL  &&temp->priority == pri && temp->next->data < ndata)
    temp=temp->next;

newn->next = temp->next;
temp->next =newn;
q->size++;
}
}

int priorityOut(Queue *q)
{
    return q->top->priority;
}

int QueueOut(Queue*q)
{
    Qnode *qtemp = q->top;
    int temp;
    temp= qtemp->data;
    q->top = qtemp->next;
    q->size --;
    free(qtemp);
    return temp;
}

