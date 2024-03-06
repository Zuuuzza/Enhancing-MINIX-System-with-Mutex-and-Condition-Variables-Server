#define _POSIX_SOURCE      1	/* tell headers to include POSIX stuff */
#define _MINIX             1	/* tell headers to include MINIX stuff */
#define _SYSTEM            1    /* get OK and negative error codes */

#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/mman.h>
#include <machine/vm.h>
#include <machine/vmparam.h>
#include <sys/vm.h>

#include <time.h>
#include <stdio.h>

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

#define MAX_MUTEXES 1028
#define MAX_PROCESSES 264
#define MAX_COND_VARS 1028
#define QUEUE_SIZE 264

// Statusy procesów 
#define CS_PROC_STATUS_EMPTY 0
#define CS_PROC_STATUS_NOT_WAITING 1
#define CS_PROC_STATUS_AWAITING_MUTEX 2
#define CS_PROC_STATUS_AWAITING_VARIABLE 3

#define CS_LOCK 1
#define CS_UNLOCK 2
#define CS_WAIT 3
#define CS_BROADCAST 4

#define OK 0
#define ERROR -1




// Struktura mutexa
typedef struct {
    int id;
    int is_locked;
    endpoint_t queue[MAX_PROCESSES];
    int status[MAX_PROCESSES];
    int rear;
} mutex_t;

//Struktura zmiennej warunkowej
typedef struct {
    endpoint_t proc;
    int id_cond;
    int taken;
} cond_var_t;

// Funkcja tworzenia kolejki
void enqueue(mutex_t* mutex, endpoint_t process, int status);
endpoint_t  dequeue(mutex_t* mutex);
endpoint_t front(mutex_t* mutex);
void delete_from_queue(mutex_t* mutex, endpoint_t process);






// Funkcje serwera
int do_cs_lock(message *m);
int do_cs_unlock(message *m);
int do_cs_wait(message *m);
int do_cs_broadcast(message *m);
mutex_t* find_mutex(int id);
mutex_t* add_mutex(int id);
cond_var_t* find_cond_var(int id);
cond_var_t* add_cond_var(int id);
// cond_var_t* find_cond_var(int id);


