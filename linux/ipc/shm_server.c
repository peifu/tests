#include <stdio.h>
#include <stdlib.h>
#include <sys/shm.h>  // shared memory
#include <sys/sem.h>  // semaphore
#include <sys/msg.h>  // message queue
#include <string.h>   // memcpy

/*
 * message queue struct
 */
struct msg_form {
    long mtype;
    char mtext;
};

/*
 * union used to init semctl
 */
union sem_un
{
    int              val; /*for SETVAL*/
    struct semid_ds *buf;
    unsigned short  *array;
};

int init_sem(int sem_id, int value)
{
    union sem_un tmp;
    tmp.val = value;
    if(semctl(sem_id, 0, SETVAL, tmp) == -1)
    {
        perror("Init Semaphore Error");
        return -1;
    }
    return 0;
}


/*
 * semaphore P operation:
 *   if sem = 1, get resource and sem -= 1
 *   if sem = 0, wait resource
 */
int sem_p(int sem_id)
{
    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = -1; /* P operation */
    sbuf.sem_flg = SEM_UNDO;

    if(semop(sem_id, &sbuf, 1) == -1)
    {
        perror("P operation Error");
        return -1;
    }
    return 0;
}

/*
 * semaphore V operation:
 *   release resource and sem += 1
 *   if process is waiting, wakeup it
 */
int sem_v(int sem_id)
{
    struct sembuf sbuf;
    sbuf.sem_num = 0;
    sbuf.sem_op = 1;  /* V operation */
    sbuf.sem_flg = SEM_UNDO;

    if(semop(sem_id, &sbuf, 1) == -1)
    {
        perror("V operation Error");
        return -1;
    }
    return 0;
}

int del_sem(int sem_id)
{
    union sem_un tmp;
    if(semctl(sem_id, 0, IPC_RMID, tmp) == -1)
    {
        perror("Delete Semaphore Error");
        return -1;
    }
    return 0;
}

int creat_sem(key_t key)
{
    int sem_id;
    if((sem_id = semget(key, 1, IPC_CREAT|0666)) == -1)
    {
        perror("semget error");
        exit(-1);
    }
    init_sem(sem_id, 1);  /* inited as 1, not used */
    return sem_id;
}


int main()
{
    key_t key;
    int shmid, semid, msqid;
    char *shm;
    char data[] = "this is server";
    struct shmid_ds buf1;  /* buffer for shared memory */
    struct msqid_ds buf2;  /* buffer for message queue */
    struct msg_form msg;   /* message queue */

    /* acquire key value */
    if((key = ftok(".", 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

    /* create shared memory */
    if((shmid = shmget(key, 1024, IPC_CREAT|0666)) == -1)
    {
        perror("Create Shared Memory Error");
        exit(1);
    }

    /* attach shared memory */
    shm = (char*)shmat(shmid, 0, 0);
    if((int)shm == -1)
    {
        perror("Attach Shared Memory Error");
        exit(1);
    }


    /* create message queue */
    if ((msqid = msgget(key, IPC_CREAT|0777)) == -1)
    {
        perror("msgget error");
        exit(1);
    }

    /* create semaphore */
    semid = creat_sem(key);

    while(1)
    {
        msgrcv(msqid, &msg, 1, 888, 0);
        if(msg.mtext == 'q')
            break;
        if(msg.mtext == 'r')
        {
            sem_p(semid);
            printf("%s\n",shm);
            sem_v(semid);
        }
    }

    shmdt(shm);

    shmctl(shmid, IPC_RMID, &buf1);
    msgctl(msqid, IPC_RMID, &buf2);
    del_sem(semid);
    return 0;
}
