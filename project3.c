//Zabeeh Ullah Babar
#define _REENTRANT
#include <pthread.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <semaphore.h>
#define BUFFER_SIZE 15

char buffer[BUFFER_SIZE];

/* the semaphores */
sem_t full, empty, mutex;


/*Producer Thread*/
void* thread1 ();
/*Consumer Thread*/
void* thread2 ();


int main (void)
{
    pthread_t	tid1[1];     /* process id for thread 1 */
    pthread_t	tid2[1];     /* process id for thread 2 */
    pthread_attr_t	attr[1]; /* attribute pointer array */

    /* Create the mutex lock and initialize to 1*/
    sem_init(&mutex, 0, 1);

    /* Create the full semaphore and initialize to 0 */
    sem_init(&full, 0, 0);

    /* Create the empty semaphore and initialize to BUFFER_SIZE */
    sem_init(&empty, 0, BUFFER_SIZE);

    /* Required to schedule thread independently.*/
    pthread_attr_init(&attr[0]);
    pthread_attr_setscope(&attr[0], PTHREAD_SCOPE_SYSTEM);
    /* end to schedule thread independently */

    /* Create the threads */
    pthread_create(&tid1[0], &attr[0], thread1, NULL);
    pthread_create(&tid2[0], &attr[0], thread2, NULL);

    /* Wait for the threads to finish */
    pthread_join(tid1[0], NULL);
    pthread_join(tid2[0], NULL);

    /* Destroying semaphores*/
    sem_destroy(&mutex);
    sem_destroy(&empty);
    sem_destroy(&full);

    /*Terminate threads */
    pthread_exit(NULL);
}

/*Producer Thread*/
void* thread1 ()
{
    FILE* fp;
    fp= fopen("mytest.dat", "r");
    int read = 1;
    int i = 0;
    char c;

    while(read)
    {
        // acquire the empty lock
        sem_wait(&empty);
        // acquire the mutex lock
        sem_wait(&mutex);

        //Start Critical Section
        if (i == 15)
            i = 0;

        if (fscanf(fp,"%c", &c) != EOF)
            buffer[i] = c;

        else {
            buffer[i] = '*';
            read = 0;
        }
        i++;
        //End Critical Section

        // release the mutex lock
        sem_post(&mutex);
        // signal full
        sem_post(&full);
    }
    close(fp);
}

/*Consumer Thread*/
void* thread2 ()
{
    int i = 0;
    int read = 1;
    while (read){
        // aquire the full lock
        sem_wait(&full);
        // aquire the mutex lock
        sem_wait(&mutex);

        //Start Critical Section
        if (i == 15) i = 0;
        sleep(1);
        if (buffer[i] != '*'){
            printf("%c", buffer[i]);
            fflush(stdout);
        }
        else {
            read = 0;
        }
        i++;
        //End Critical Section

        // release the mutex lock
        sem_post(&mutex);
        // signal empty
        sem_post(&empty);
    }
}
