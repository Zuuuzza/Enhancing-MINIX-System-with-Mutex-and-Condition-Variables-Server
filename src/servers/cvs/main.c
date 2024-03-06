#include "cvs.h"

// Tablica mutexów
mutex_t mutexes[MAX_MUTEXES];

cond_var_t cond_vars[MAX_COND_VARS];


// Funkcja wyszukiwania mutexu
mutex_t* find_mutex(int id) {
    // printf("find_mutex\n");
    // printf("id: %d\n", id);
    for (int i = 0; i < MAX_MUTEXES; i++) {
        if (mutexes[i].id == id && mutexes[i].is_locked) {
            //printf("mutex found\n");
            return &mutexes[i];
        }
    }
    // printf("mutex not found\n");
    return NULL;
}

// Funkcja dodawania procesu do kolejki
mutex_t * add_mutex(int id) {
    for (int i = 0; i < MAX_MUTEXES; i++) {
        if (mutexes[i].is_locked == 0 ) {
            mutexes[i].id = id;
            mutexes[i].is_locked = 1;
            mutexes[i].rear = 0;
            return &mutexes[i];
        }
    }
    return NULL;
}



// Obsługa blokady mutexa
int do_cs_lock(message *m) {
    mutex_t* mutex = find_mutex(m->m1_i1);
    int id = m->m1_i1;
    if (mutex == NULL) {
        mutex = add_mutex(id);
        enqueue(mutex, m->m_source, CS_PROC_STATUS_AWAITING_MUTEX);
        m->m_type = OK;
        sendnb(m->m_source, m);
        return OK;  
    }
    // Dodanie procesu do kolejki oczekujących na mutex
    enqueue(mutex, m->m_source, CS_PROC_STATUS_AWAITING_MUTEX);
    m->m_type = OK;
    return OK;
}

// Obsługa zwalniania mutexa
int do_cs_unlock(message *m) {
    mutex_t* mutex = find_mutex(m->m1_i1);    
    if (mutex == NULL) {
        m->m_type = ERROR;
        errno = EPERM;
        sendnb(m->m_source, m);
        return ERROR;
    }
    // Zwolnienie mutexu
    endpoint_t owner;
    owner = dequeue(mutex);
    m->m_type = OK;
    sendnb(owner, m);
    //jest kolejny
    if(mutex->rear > 0){
        owner = front(mutex);
        m->m_type = OK;
        sendnb(owner, m);
    }
    else{
        mutex->is_locked = 0;
        mutex->id = -1;
    }

    return OK;
}

// Funkcja oczekiwania na zmienną warunkową
int do_cs_wait(message *m) {
    //printf("do_cs_wait\n");
    int cond_var_id = m->m1_i1;
    for(int i = 0; i < MAX_COND_VARS; i++){
        if(cond_vars[i].taken == 0){
            cond_vars[i].id_cond = cond_var_id;
            cond_vars[i].proc = m->m_source;
            cond_vars[i].taken = 1;
            m->m_type = OK;
            return OK;
        }
    }
    return OK;
}

// Funkcja ogłaszania zdarzenia na zmiennej warunkowej
int do_cs_broadcast(message *m) {
    int cond_var_id = m->m1_i1;
    //printf("do_cs_broadcast\n");
    for(int i = 0; i < MAX_COND_VARS; i++){
        if(cond_vars[i].id_cond == cond_var_id){
            cond_vars[i].taken = 0;
            cond_vars[i].id_cond = -1;
            m->m_type = OK;
            sendnb(cond_vars[i].proc, m);
            cond_vars[i].proc = 0;
        }
    }
    m->m_type = OK;
    sendnb(m->m_source, m);
    return OK;
}

// Funkcja obsługi sygnału
void handle_sig(endpoint_t proc_nr) {
    for(int i = 0; i < MAX_MUTEXES; i++){
        if(mutexes[i].is_locked){
            for(int j = 0; j < MAX_PROCESSES; j++){
                if(mutexes[i].queue[j] == proc_nr){
                    if(front(&mutexes[i]) == proc_nr){
                        message m;
                        m.m1_i1 = mutexes[i].id;
                        do_cs_unlock(&m);
                    }
                    else{
                        delete_from_queue(&mutexes[i], proc_nr);
                    }
                }
            }
        }
    }
}

// Inicjalizacja serwera
void server_init() {
    // Inicjalizacja mutexów
    for (int i = 0; i < MAX_MUTEXES; i++) {
        mutexes[i].id = -1;
        mutexes[i].is_locked = 0;
        mutexes[i].rear = 0;
        for(int j = 0; j < MAX_PROCESSES; j++){
            mutexes[i].queue[j] = 0;
            mutexes[i].status[j] = 0;
        }
    }

    // Inicjalizacja zmiennych warunkowych
    for(int i = 0; i < MAX_COND_VARS; i++){
        cond_vars[i].id_cond = -1;
        cond_vars[i].taken = 0;
        cond_vars[i].proc = 0;
    }
}

// Główna pętla serwera
void server_loop() {

    message m;
    int status;

    sef_startup();


    while (TRUE) {
        // Oczekiwanie na wiadomość
        status = sef_receive(ANY, &m);

        if(m.m_source == 0){
            handle_sig(m.PM_PROC);
            continue;
        }

        if (status != OK) {
            printf("ERROR.\n");
            continue;
        }

        switch (m.m_type) {
            case CS_LOCK:
                do_cs_lock(&m);
                break;
            case CS_UNLOCK:
                do_cs_unlock(&m);
                break;
            case CS_WAIT:
                do_cs_wait(&m);
                break;
            case CS_BROADCAST:
                do_cs_broadcast(&m);
                break;
            default:
                printf("Unknow type: %d\n", m.m_type);
        }
    }
}

// Główna funkcja programu
int main() {
    server_init();
    server_loop();
    return 0;
}






