#include <minix/callnr.h>
#include <minix/com.h>
#include <minix/config.h>
#include <minix/ipc.h>
#include <minix/endpoint.h>
#include <minix/sysutil.h>
#include <minix/const.h>
#include <minix/type.h>
#include <minix/syslib.h>
#include <minix/rs.h>

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
#include <cv.h>

int cs_lock(int mutex_id){
    message m;
    m.m1_i1 = mutex_id;
    endpoint_t tmp;
    //printf("mutex_id: %d\n", mutex_id);
    int r = -1, rs;
    do{
        rs = minix_rs_lookup("cvs", &tmp);
        r = syscall(tmp, CS_LOCK, &m);        
    }while(errno == EINTR && r == -1);
    return r;
 }

int cs_unlock(int mutex_id){
    message m;
    m.m1_i1 = mutex_id;
    endpoint_t  tmp ;
    int rs = minix_rs_lookup("cvs", &tmp);
    int r = syscall(tmp, CS_UNLOCK, &m); 
    if(r == -1 && (errno == EINTR)){}
    return r;
}


int cs_wait(int cond_var, int mutex){
    message m;
    endpoint_t tmp;
    int rs = minix_rs_lookup("cvs", &tmp);
    m.m1_i1 = mutex;
    int r = syscall(tmp, CS_UNLOCK, &m);
    if(r == -1){
        return -1;
    }
    m.m1_i1 = cond_var;
    r = syscall(tmp, CS_WAIT, &m);
    if(r == -1 && errno != EINTR){
        return -1;
    }
    m.m1_i1 = mutex;
    do{
        r = syscall(tmp, CS_LOCK, &m);
    }while(r == -1 && errno == EINTR);
    return r;
}

int cs_broadcast(int cond_var){
    message m;
    m.m1_i1 = cond_var;
    endpoint_t tmp;
    int r = -1, rs;
    rs = minix_rs_lookup("cvs", &tmp);
    r = syscall(tmp, CS_BROADCAST, &m);
    return r;
}