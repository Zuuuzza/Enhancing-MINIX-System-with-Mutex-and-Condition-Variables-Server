

#define CS_LOCK 1
#define CS_UNLOCK 2
#define CS_WAIT 3
#define CS_BROADCAST 4
//deklaracje do cv.c

int cs_lock(int mutex_id);
int cs_unlock(int mutex_id);
int cs_wait(int cond_id, int mutex_id);
int cs_broadcast(int cond_id);
