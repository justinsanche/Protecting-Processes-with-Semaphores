
//Project 3

#define TRUE 1
#define SLEEP_TIME 1
#define BUFFER_SIZE 15

#define SHMKEY ((key_t) 7660)

#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>

typedef struct
{
    int value;
} shared_mem;

//full, empty, mutex Sems
sem_t full, empty, mutex;

char data;
FILE* fp;

//Buffer
shared_mem *counter;
char buf[BUFFER_SIZE];

//Producing Function
void* producer (void *ptr)
{
    fp = fopen("mytest.dat", "r"); 
    
    while (fscanf(fp, "%c", &data) != EOF)
	{
        sem_wait(&empty);

        if (counter->value == BUFFER_SIZE)
	 	{
            counter->value = 0;
        }
        sem_wait(&mutex);
        buf[counter->value % BUFFER_SIZE] = data;    
        counter->value++;
        sem_post(&mutex);
        printf("Producing Character: %c \n", data);
        fflush(stdout);

        sem_post(&full);
	}
    close(fp); 
 
    buf[counter->value] = '*';
    counter->value++;
}

//Consuming Function
void* consumer (void *ptr)
{
    sem_post(&empty);   

    char item;
    
    while (item != '*')
    {
    	//the consumer should run slower
        sleep(SLEEP_TIME);
        sem_wait(&full);

        if (counter->value > 0)
		{
            sem_wait(&mutex);
            item = buf[(counter->value - 1)];
            counter->value--;
            sem_post(&mutex);
            printf("Consuming Character: %c \n", item);
            fflush(stdout);
        }

        sem_post(&empty);
    }
}


int main()
{

	sem_init(&mutex,0,1);
	sem_init(&full,0,0);

	int r = 0;
    int i;
	int shmid;	  			/* shared memory ID */
    pthread_t tid1[1];     	/* process id for thread 1 */
    pthread_t tid2[1];     	/* process id for thread 2 */
    pthread_attr_t attr[1]; /* attribute pointer array */
    
    char *shmadd;
    shmadd = (char *) 0;
   
    /* Create and connect to a shared memory segment */
    if ((shmid = shmget (SHMKEY, sizeof(int), IPC_CREAT | 0666)) < 0)
    {
        perror ("shmget");
        exit (1);
    }
    
    if ((counter = (shared_mem *) shmat (shmid, shmadd, 0)) == (shared_mem *) -1)
    {
        perror ("shmat");
        exit (0);
    }
    
	fflush(stdout);
    /* Required to schedule thread independently.*/
    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);  
	/* end to schedule thread independently */
    
    /* Create the threads */
    pthread_create(&tid1[0], &attr[0], producer, NULL);
    pthread_create(&tid2[0], &attr[0], consumer, NULL);

    /* Wait for the threads to finish */
    pthread_join(tid1[0], NULL);
    pthread_join(tid2[0], NULL);
    
    //Destroying Semaphores
    sem_destroy(&empty);
	sem_destroy(&full);
	sem_destroy(&mutex);

	//Memory Clean-Up
	if ((shmctl (shmid, IPC_RMID, (struct shmid_ds *) 0)) == -1)
	{
		perror ("shmctl");
		exit (-1);
	}

	printf("from parent counter  =  %d\n", counter->value);
    printf("------------------------------------------------\n");
	printf("\t\t	End of simulation\n");

	exit(0);
}
