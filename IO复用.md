# IO复用

##SELECT函数
select函数原型如下：
int select(int maxfdp1, fdset *readset, fd_set *writeset, fd_set *exceptset, const struct timeval *timeout)
其中第一个参数maxfdp1指定待测试的描述字个数。它的值是最大的描述符的值+1
fd_set可以理解为一个集合，其中存储了我们需要的文件描述符，可以通过需要向集合中添加删除文件描述符
timeout结构体表示等待描述字中的任何一个就绪可以花费的最长时间，该参数有三种情况：
1. 当该指针为空时，将一直等待下去，直到某个io准备好时才返回。
2. 等待固定一段时间，在有一个io描述字准备好时返回，或者达到固定等待时间返回。
3. 不等待，检查描述字后立即返回，这称为轮询，此时结构体中的秒数与毫秒数都为0
返回值：
1. 负数，select函数出错
2. 0，超时，没有就绪的io
3. 正数，某个io准备就绪

## POLL函数
poll函数。poll的机制与select类似，与select在本质上没有多大差别，管理多个描述符也是轮询，但是poll没有最大文件描述符的限制。
poll函数的原型如下：
int poll(struct pollfd *fds, unsigned nfds, int temwout);
其中pollfd结构体定义为{
    int fd;
    short events;
    short revents;
}其中fd为文件描述符，events时监视该文件描述符的事件掩码，由用户设置。revents时文件描述符的操作结果事件掩码，
内核在调用返回时设置该值。
events中请求的任何事件都可能在revents中被返回。
poll与select不一样，不需要显示的请求异常情况报告。异常情况事件会在需要时从revents中返回。
第二个参数是一个非负数，用来标记数组fds中的元素数量。
timeout参数指定等待时间，默认单位是毫秒，如果等待时间超时，无论io是否准备好，都会返回。
poll本质上与select没有区别，它遍历文件描述符，然后查询每个对应的设备状态，如果准备就绪则在设备等待队列中加入一项并继续遍历，
如果遍历完之后没有发现就绪设备，则挂起当前进程，直到设备就绪或超时，被唤醒之后它需要再次遍历文件描述符。
poll是水触发的，如果报告了fd后，没有被处理，那么下一次poll还会再次报告该fd。


## EPOLL函数
epoll函数，
epoll函数的接口非常简单，一共就三个函数
int epoll_create(int size)创建一个epoll的句柄，size用来告诉内核这个监听的数目一共有多大。当创建好epoll句柄后，它会占用一个fd值。
使用epoll后，必须调用close()关闭，否则可能导致fd耗尽。
int epoll_ctl(int epfd, int op, int fd, struct epoll_event *event)
epoll的事件注册函数，它不在监听是表明要监听的事件，而是在这个函数中注册。
其中，第一个参数epfd是epoll_create的返回值，第二个参数表示动作，用三个宏表示：
1， EPOLL_CTL_ADD，注册新的fd到epfd中。2，EPOLL_CTL_MOD，修改已经注册的fd的监听事件。3， EPOLL_CTL_DEL，从epfd中删除一个fd。
第三个参数是需要监听的fd，第四个参数是告诉内核需要监听什么事件，epoll_event结构体的定义为epoll_data{
    void *ptr;
    int fd;
    uint32_t u32;
    uint64_t u64;
},
struct epoll_event{
    uint32_t events;
    epoll_data* data;
}events事件宏的集合。
int epoll_wait(int epfd, struct epoll_event* events, int maxevents, int timeout)
等待事件的产生，类似于select的调用。参数events用来从内核得到事件的集合，maxevents告诉内核这个events有多大，
这个maxevents的值不能大于创建epoll_create时的size，参数timeout是超时时间。
epoll有两种工作模式，ET与LT
ET模式下epoll_wait检测到描述符事件发生并将此事件通知应用程序，应用程序必须立即处理，否则下一次调用时，不会再次响应应用程序并通知此事件。
LT模式下检测到描述符时间发生并告知应用程序，应用程序可以不处理，下一次调用时仍会响应此事件。
所有添加到epoll中的事件都会与设备建立回调关系，也就是说，当相应的事件发生时会调用这个回调方法，这个回调方法叫做
ep_poll_callback，它会将发生的事件添加到rdlist双链表中。当epoll_wait返回时，我们只需要检查list链表，就能知道是哪个io准备就绪或者是超时了。
优点
1. epoll通过和内核和用户空间共享一块内存空间来避免拷贝
2. epoll连接数很大，1G内存能打开10W左右的连接
3. epoll只负责活跃的连接，因此效率比select和poll高很多。


## 总结
总结：
select和poll是最开始提出的io多路复用的模型，
io复用最开始提出，是为了避免每新建一个io（如tcp），就新建一个进程或线程维护它，而是将多个io的描述符传给函数select或epoll。
由内核轮询这些描述符，当有io就绪时再由内核通知进程。但对于poll和select来说，每次都需要将对应的文件描述符对应的数据结构从用户空间传递给内核空间。
而epoll则直接建立一块用户和内核共享的内存空间，在这块空间中创建并维护要监视的描述符队列。因此，避免了文件描述符过多时的拷贝操作。
另外，select和epoll都是轮询操作，每一次都遍历文件描述符列表，依次获取io的状态，在返回时，我们依然不清除是哪一个io就绪，因此需要再次遍历。
而epoll则通过注册回调函数实现，epoll监视io通过在驱动程序注册回调函数，在该io在对应状态准备就绪时，通过回调函数通知进程，进程将该io加入准备就绪队列中，
再返回时，我们只需要遍历就绪队列，此队列中都是准备好的io。例如：加入有一百万给tcp连接服务器，但是只有数千个连接处于活跃状态，此时对于select和poll
每次需要遍历一百万个数据描述符，在返回时需要再遍历这一百万个文件描述符，极大的浪费了事件。而epoll则不需要这些遍历。
