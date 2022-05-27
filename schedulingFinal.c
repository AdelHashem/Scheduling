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
void QueueIn(Queue* q ,int ndata,int cpu,int io,int pri,int time);
void QueueInf(Queue* q ,int ndata,int cpu,int io,int pri,int time);
void QueueEnd(Queue* q ,int ndata,int cpu,int io,int pri,int time);
int priorityOut(Queue *q);
int QueueOut(Queue*q);
///////////////////////////////////////



void LoadFile(char *path);
void Getdata();
void FCFS();
void RR(int quantum);
void output(int cycle,Queue *running,Queue *ready,Queue *blocked,int type);
void trtime();

FILE *f;
Queue data;
int *tr;
int datasize,cputime,finish;
void Getcputime();
int main()
{
    char path[512] = "";
    
    int detector;
    int quantum;
    scanf("%d",&detector);
    // see if it fcfs or RR
    if (detector == 0)//fcfs
    {
        LoadFile(path);
    }else{ //RR
        
        scanf("%d",&quantum);
        LoadFile(path);
    }

    if (f = fopen(path,"r")){
        Getdata();
        tr =(int*) malloc(data.size * sizeof(int));
        datasize = data.size;
        Getcputime();
        Queue temp = data;
        if (detector == 0)
        {
            FCFS();
            trtime(0);
        }
            
        else
        {
            RR(quantum);
            trtime(1);
        }
            

        

    }else
        printf("File Not Exist");

    return 0;
}

void Getcputime()
{
    cputime = 0;
    Qnode *temp = data.top;
    while(temp)
    {
        cputime += temp->cpu;
        temp = temp->next;
    }
    cputime *=2;
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
    QueueIn(&data,p,cpu,io,arrive,arrive);
    }
}

void FCFS()
{
    Queue ready,blocked,running; // create 3 Qs
    CreateQueue(&ready);CreateQueue(&blocked);CreateQueue(&running);
    int cycle = 0; //start with cycle 0
    int P_time = -1,n_P = data.size;
    int flag = 0;
    int temp;
    int i =0;
    while (flag == 0 )
    { 
       
        P_time ++;
        Queue temoq = data;
        // push p when its time comes
        while(!IsEmpty(&data) && priorityOut(&data) == cycle)
        {
            if(data.top->cpu !=0){
            QueueIn(&ready,data.top->data,data.top->cpu,data.top->io,i,data.top->time);
            temp = QueueOut(&data);
            temoq = data;
            }
            else
            {
                QueueIn(&blocked,data.top->data,data.top->cpu,data.top->io,data.top->io,data.top->time);
                temp = QueueOut(&data);
                temoq = data;
            }
        }
        // Process the blocking Q
        Queue blockedBefore = blocked; //to output blocked Q correctly
        
        
        ////////

        // Deal with running P
        if(!IsEmpty(&ready) && IsEmpty(&running)){
            QueueIn(&running,ready.top->data,ready.top->cpu,ready.top->io,0,ready.top->time);
            temp = QueueOut(&ready);
            P_time = 0;}
        else if(!IsEmpty(&running))
        {
            //see if the p ended
            if(running.top->cpu == P_time)
            {
                if(running.top->io == -1) //no i/o
                {
                    tr[running.top->data] = cycle - running.top->time;
                    temp = QueueOut(&running);
                }else //i/o blocking
                {
                    QueueIn(&blocked,running.top->data,running.top->cpu,running.top->io,running.top->io,running.top->time);
                    temp = QueueOut(&running);
                }
                if(!IsEmpty(&ready))
                    {QueueIn(&running,ready.top->data,ready.top->cpu,ready.top->io,0,ready.top->time);
                     temp = QueueOut(&ready);}
                P_time = 0;               
            }
            

        }
        if(data.size == 0 && IsEmpty(&ready) && IsEmpty(&running) && IsEmpty(&blocked)){
            flag = 1;
            finish = cycle;
            break;
        }
        Qnode *tempn = blocked.top;
        for(int i = 0;i< blocked.size;i++)
        {
            tempn->io --;
            tempn->priority --;
            tempn = tempn->next;
        }

        output(cycle,&running,&ready,&blocked,0);
        while(!IsEmpty(&blocked) && priorityOut(&blocked) == 0)
        {
            int cputemp = blocked.top->cpu;
            if(cputemp !=0){
            QueueIn(&ready,blocked.top->data,blocked.top->cpu,-1,i,blocked.top->time);
            }
            else
            {
                tr[blocked.top->data] = cycle - blocked.top->time + 1;
            }
            temp = QueueOut(&blocked);
        }




        if(!IsEmpty(&running))
            printf("%d cycle :%d ",running.top->data,cycle);

        cycle ++;
        i++;
        
            
    }
    
}

void RR(int quantum)
{
    int p_time[data.size];
    memset(p_time, 0, sizeof(p_time));
    Queue ready,blocked,running; // create 3 Qs
    CreateQueue(&ready);CreateQueue(&blocked);CreateQueue(&running);
    int cycle = 0,counter = 0; //start with cycle 0
    int n_P = data.size;
    int flag = 0,priflag = 0;
    int temp;
    int i = 0;
    while (flag == 0 )
    {
        counter = 0;
        while (counter < quantum)
        {
            int pri = 0;
            
            // push p when its time comes
        while(!IsEmpty(&data) && priorityOut(&data) == cycle)
        {
            
            if(data.top->cpu !=0){
                
                if(!IsEmpty(&ready))
                    pri = prioritymax(&ready);
                if(priflag == 0)
                    pri ++;
            QueueIn(&ready,data.top->data,data.top->cpu,data.top->io,pri,data.top->time);
            temp = QueueOut(&data);
            }
            else
            {
                QueueIn(&blocked,data.top->data,data.top->cpu,data.top->io,data.top->io,data.top->time);
                temp = QueueOut(&data);
            }
            priflag = 1;
        }
        //see block q
        /*while(!IsEmpty(&blocked) && priorityOut(&blocked) == 0)
        {
            int cputemp = blocked.top->cpu;
            if(cputemp !=0){
                pri = 0;
                if(!IsEmpty(&ready))
                    pri = prioritymax(&ready);
                if (priflag == 0)
                    pri ++;
            QueueIn(&ready,blocked.top->data,blocked.top->cpu,-1,pri,blocked.top->time);
            }
            else
            {
                tr[blocked.top->data] = cycle - blocked.top->time;
            }
            temp = QueueOut(&blocked);
        }*/

        // for running

            if(!IsEmpty(&ready) && IsEmpty(&running)){
            QueueIn(&running,ready.top->data,ready.top->cpu,ready.top->io,0,ready.top->time);
            temp = QueueOut(&ready);
            }
        else if(!IsEmpty(&running))
        {
            //see if the p ended
            if(running.top->cpu == p_time[running.top->data])
            {
                if(running.top->io == -1) //no i/o
                {
                    tr[running.top->data] = cycle - running.top->time;
                    temp = QueueOut(&running);
                }else //i/o blocking
                {
                    QueueIn(&blocked,running.top->data,running.top->cpu,running.top->io,running.top->io,running.top->time);
                    p_time[running.top->data] = 0;
                    temp = QueueOut(&running);
                }
                               
            }

        }
        if(data.size == 0 && IsEmpty(&ready) && IsEmpty(&running) && IsEmpty(&blocked)){
            flag = 1;
            finish = cycle;
            break;
        }
            if(!IsEmpty(&running)){
                p_time[running.top->data] ++;
                printf("cycle : %d Runing : %d\n",cycle,running.top->data);
                if(running.top->cpu == p_time[running.top->data])
                    counter = quantum;
            }
            

        // Process the blocking Q
        Qnode *tempn = blocked.top;
        for(int i = 0;i< blocked.size;i++)
        {
            tempn->io --;
            tempn->priority --;
            tempn = tempn->next;
        }
        
        ////////
        
            
            output(cycle,&running,&ready,&blocked,1);    

            while(!IsEmpty(&blocked) && priorityOut(&blocked) == 0)
        {
            int cputemp = blocked.top->cpu;
            if(cputemp !=0){
                pri = 0;
                if(!IsEmpty(&ready))
                    pri = prioritymax(&ready);
                if (priflag == 0)
                    pri ++;
            QueueIn(&ready,blocked.top->data,blocked.top->cpu,-1,pri,blocked.top->time);
            }
            else
            {
                tr[blocked.top->data] = cycle - blocked.top->time + 1;
            }
            temp = QueueOut(&blocked);
        }



            counter ++;
            priflag = 0;
            if(IsEmpty(&running))
                counter=0;
            cycle ++;
            
        }



        if(!IsEmpty(&running)){
            //if The P ended at the end of quan
            
            if(running.top->cpu == p_time[running.top->data])
            {
                if(running.top->io == -1) //no i/o
                {
                    tr[running.top->data] = cycle - running.top->time;
                    temp = QueueOut(&running);
                }else //i/o blocking
                {
                    QueueIn(&blocked,running.top->data,running.top->cpu,running.top->io,running.top->io,running.top->time);
                    p_time[running.top->data] = 0;
                    temp = QueueOut(&running);
                }
            }
            else{
                if (!IsEmpty(&ready))
                {
                    if(!IsEmpty(&ready))
                    i = prioritymax(&ready);
                    i++;
                    QueueIn(&running,ready.top->data,ready.top->cpu,ready.top->io,i,ready.top->time);
                    temp = QueueOut(&ready);
                }else
                     i = prioritymax(&running) + 1;             
                QueueIn(&ready,running.top->data,running.top->cpu,running.top->io,i,running.top->time);
                temp = QueueOut(&running);
                priflag = 1;
                i++;
            }

                
            
        }

    }
}





void output(int cycle,Queue *running,Queue *ready,Queue *blocked,int type)
{
    FILE *fc;
    if (type == 0)
         fc = fopen("ouputFCFS.txt","a");
    else
         fc= fopen("ouputRR.txt","a");
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
void trtime(int type)
{
    FILE *fp;
    if (type == 0)
         fp = fopen("ouputFCFS.txt","a");
    else
         fp= fopen("ouputRR.txt","a");
    fprintf(fp,"\n");
    fprintf(fp,"Finishing time: %d\n",--finish);
    fprintf(fp,"CPU Utilization: %.2f\n",(float)cputime/(float)++finish);
    fprintf(fp,"\n");
    for(int i = 0; i < datasize;i++)
        fprintf(fp,"Turnaround time of Process %d: %d\n",i,tr[i]);
    fprintf(fp,"\n");
    fclose(fp);
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


void QueueIn(Queue* q ,int ndata,int cpu,int io,int pri,int time)
{
Qnode *newn = (Qnode*)malloc(sizeof(Qnode));
newn->data = ndata;
newn->priority = pri;
newn->cpu = cpu;
newn->io = io;
newn->time = time;
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
while (temp->next != NULL && pri == temp->next->priority && ndata> temp->next->data)
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

int prioritymax(Queue *q)
{
    Qnode * temp = q->top;
    while(temp->next != NULL)
        temp = temp->next;
    return temp->priority;
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