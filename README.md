# Project README: Enhancing MINIX System with Mutex and Condition Variables Server

## Introduction

This project adds a Mutex and Condition Variables Server (`cvs`) to MINIX to manage synchronization primitives for concurrent processes.

## Mutexes

### Functions:

- `cs_lock(int mutex_id)` - Acquires the mutex identified by `mutex_id`. Returns `0` on success, `-1` otherwise.
- `cs_unlock(int mutex_id)` - Releases the mutex identified by `mutex_id`. Returns `0` on success, `-1` otherwise.

## Condition Variables

### Functions:

- `cs_wait(int cond_var_id, int mutex_id)` - Suspends the calling process, waiting for the event identified by `cond_var_id`. Returns `0` on success, `-1` otherwise.
- `cs_broadcast(int cond_var_id)` - Signals the occurrence of the event identified by `cond_var_id`.

## Compilation and Execution

1. Navigate to `/usr/src/releasetools` directory.
   ```bash
     make install
     make includes
2. Navigate to `/usr/src/lib` directory.
   ```bash
     make
     make install
3. To start the server:
    ```bash
    service up /usr/sbin/cvs
4. To stop the server:
    ```bash
    service down cvs