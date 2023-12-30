## 简介
这个demo是关于信号量的使用，我们定义了一个信号量和两个线程，一个线程每1秒give一次信号量，另一个线程阻塞等待信号量，当信号量大于0时，take信号量。

## 关键内容
* 定义信号量
```
K_SEM_DEFINE(sem, 0, 1);
```
* give信号量
```
k_sem_give(&sem);
```
* take信号量
```
k_sem_take(&sem, K_FOREVER);
```