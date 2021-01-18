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

int main()
{
    key_t key;
    int shmid, semid, msqid;
    char *shm;
    struct msg_form msg;
    int flag = 1;

    /* acquire key value */
    if((key = ftok(".", 'z')) < 0)
    {
        perror("ftok error");
        exit(1);
    }

    /* create shared memory */
    if((shmid = shmget(key, 1024, 0)) == -1)
    {
        perror("shmget error");
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
    if ((msqid = msgget(key, 0)) == -1)
    {
        perror("msgget error");
        exit(1);
    }

    /* get semaphore */
    if((semid = semget(key, 0, 0)) == -1)
    {
        perror("semget error");
        exit(1);
    }

    printf("***************************************\n");
    printf("*                 IPC                 *\n");
    printf("*    Input r to send data to server.  *\n");
    printf("*    Input q to quit.                 *\n");
    printf("***************************************\n");

    while(flag)
    {
        char c;
        printf("Please input command: ");
        scanf("%c", &c);
        switch(c)
        {
            case 'r':
                printf("Data to send: ");
                sem_p(semid);
                scanf("%s", shm);
                sem_v(semid);
                /* clean up input buffer */
                while((c=getchar())!='\n' && c!=EOF);
                msg.mtype = 888;
                msg.mtext = 'r';
                msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
                break;
            case 'q':
                msg.mtype = 888;
                msg.mtext = 'q';
                msgsnd(msqid, &msg, sizeof(msg.mtext), 0);
                flag = 0;
                break;
            default:
                printf("Wrong input!\n");
                /* clean up input buffer */
                while((c=getchar())!='\n' && c!=EOF);
        }
    }

    shmdt(shm);

    return 0;
}
