## Introduction
This demo is about the use of semaphores. We have defined a semaphore and two threads. One thread gives the semaphore every 1 second, while the other thread is blocked waiting for the semaphore. When the semaphore count is greater than 0, it takes the semaphore.

## Key Content
* Defining a semaphore:
  ```
  K_SEM_DEFINE(sem, 0, 1);
  ```
* Giving a semaphore:
  ```
  k_sem_give(&sem);
  ```
* Taking a semaphore:
  ```
  k_sem_take(&sem, K_FOREVER);
  ```