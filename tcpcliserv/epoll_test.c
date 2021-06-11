#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/epoll.h>
#include <string.h>
 
#define MAXEVENTS 100000
 
//函数:
//功能:创建和绑定一个TCP socket
//参数:端口
//返回值:创建的socket
static int
create_and_bind (char *port)
{
  struct addrinfo hints;
  struct addrinfo *result, *rp;
  int s, s1, sfd;
 
  memset (&hints, 0, sizeof (struct addrinfo));
  hints.ai_family = AF_UNSPEC;     /* Return IPv4 and IPv6 choices */
  hints.ai_socktype = SOCK_STREAM; /* We want a TCP socket */
  hints.ai_flags = AI_PASSIVE;     /* All interfaces */
 
  s = getaddrinfo (NULL, port, &hints, &result);
  if (s != 0)
    {
      fprintf (stderr, "getaddrinfo: %s\n", gai_strerror (s));
      return -1;
    }
 
  for (rp = result; rp != NULL; rp = rp->ai_next)
    {
      sfd = socket (rp->ai_family, rp->ai_socktype, rp->ai_protocol);
      if (sfd == -1)
        continue;
        
      s = bind (sfd, rp->ai_addr, rp->ai_addrlen);
      if (s == 0)
        {
          /* We managed to bind successfully! */
          break;
        }
 
      close (sfd);
    }
 
  if (rp == NULL)
    {
      fprintf (stderr, "Could not bind\n");
      return -1;
    }
 
  freeaddrinfo (result);
 
  return sfd;
}
 
 
//函数
//功能:设置socket为非阻塞的
static int
make_socket_non_blocking (int sfd)
{
  int flags, s;
 
  //得到文件状态标志
  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }
 
  //设置文件状态标志
  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }
 
  return 0;
}
 
//端口由参数argv[1]指定
int
main (int argc, char *argv[])
{
  int sfd, s;
  int efd;
  struct epoll_event event;
  struct epoll_event *events;//指针即数组嘛
 
  if (argc != 2)
    {
      fprintf (stderr, "Usage: %s [port]\n", argv[0]);
      exit (EXIT_FAILURE);
    }
 
  sfd = create_and_bind (argv[1]);
  if (sfd == -1)
    abort ();
 
  s = make_socket_non_blocking (sfd);
  if (s == -1)
    abort ();
 
  s = listen (sfd, SOMAXCONN);//sfd是一个监听套接字
  if (s == -1)
    {
      perror ("listen");
      abort ();
    }
 
  //除了参数size被忽略外,此函数和epoll_create完全相同
  efd = epoll_create1 (0);
  if (efd == -1)
    {
      perror ("epoll_create");
      abort ();
    }
 
  event.data.fd = sfd;
  event.events = EPOLLIN | EPOLLET;//读入,边缘触发方式
  s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
  if (s == -1)
    {
      perror ("epoll_ctl");
      abort ();
    }
 
  /* Buffer where events are returned */
  events = calloc (MAXEVENTS, sizeof event);//分配并初始化一个大小为MAXEVENTS的数组，数组类型是epoll_event，这一步是必须的！
 
  /* The event loop: 常用的架构 */
  while (1)
    {
      int n, i;
 
      n = epoll_wait (efd, events, MAXEVENTS, -1);//-1表示永远阻塞的等待下去。
      for (i = 0; i < n; i++)
        {
          if ((events[i].events & EPOLLERR) ||
              (events[i].events & EPOLLHUP) ||
              (!(events[i].events & EPOLLIN)))
            {
              /* An error has occured on this fd, or the socket is not
                 ready for reading (why were we notified then?) */
              fprintf (stderr, "epoll error\n");
              close (events[i].data.fd);
              continue;
            }
 
          else if (sfd == events[i].data.fd)
            {
              /* We have a notification on the listening socket, which
                 means one or more incoming connections. 
                 因为可能有不止一个连接接入，所以用了while(1)*/
              while (1)
                {
                  struct sockaddr in_addr;
                  socklen_t in_len;
                  int infd;
                  char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];
 
                  in_len = sizeof in_addr;
                  infd = accept (sfd, &in_addr, &in_len);//返回一个已连接套接字，但是这里不用考虑accept被中断后重启的事情，是因为sfd是非阻塞型的套接字！
                  if (infd == -1)
                    {
                        //这个error表示的是：
                      if ((errno == EAGAIN) ||
                          (errno == EWOULDBLOCK))
                        {
                          /* We have processed all incoming
                             connections. */
                          break;
                        }
                      else
                        {
                          perror ("accept");
                          break;
                        }
                    }
 
                                  //将地址转化为主机名或者服务名
                  s = getnameinfo (&in_addr, in_len,
                                   hbuf, sizeof hbuf,
                                   sbuf, sizeof sbuf,
                                   NI_NUMERICHOST | NI_NUMERICSERV);//flag参数:以数字名返回
                                  //主机地址和服务地址
                    //s == 0 表示成功
                  if (s == 0)
                    {
                      printf("Accepted connection on descriptor %d "
                             "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                    }
 
                  /* Make the incoming socket non-blocking and add it to the
                     list of fds to monitor. */
                  s = make_socket_non_blocking (infd);//依然要把这个accept的套接字设置成非阻塞的！
                  if (s == -1)
                    abort ();
 
                  event.data.fd = infd;
                  event.events = EPOLLIN | EPOLLET;
                  s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);//将该fd添加到监听队列中去。
                  if (s == -1)
                    {
                      perror ("epoll_ctl");
                      abort ();
                    }
                }
              continue;
            }
          else
            {
              /* We have data on the fd waiting to be read. Read and
                 display it. We must read whatever data is available
                 completely, as we are running in edge-triggered mode
                 and won't get a notification again for the same
                 data. 
                 工作在边沿触发模式，必须要一次性读完缓冲区的所有数据，否则epoll_wait将不会由于第一次进程没有读完而返回第二次！*/
              int done = 0;
              //必须有着while(1)，当read完成时才跳出这个死循环！
              while (1)
                {
                  ssize_t count;
                  char buf[512];
 
                  count = read (events[i].data.fd, buf, sizeof(buf));
                  if (count == -1)
                    {
                      /* If errno == EAGAIN, that means we have read all
                         data. So go back to the main loop. */
                      if (errno != EAGAIN)
                        {
                          perror ("read");
                          done = 1;
                        }
                      break;
                    }
                  else if (count == 0)
                    {
                      /* End of file. The remote has closed the
                         connection. */
                      done = 1;
                      break;
                    }

                  /* Write the buffer to standard output */
                  s = write (1, buf, count);
                  if (s == -1){
                      perror ("write");
                      abort ();
                    }
                  //写回该客户，如同echo回射服务一样！  
                  s = write(events[i].data.fd, buf, count);
                  if (s == -1){
                      perror ("write");
                      abort ();
                    }
                }

              if (done)
                {
                  printf ("Closed connection on descriptor %d\n",
                          events[i].data.fd);
 
                  /* Closing the descriptor will make epoll remove it
                     from the set of descriptors which are monitored. */
                  close (events[i].data.fd);//是直接调用close吗？？？
                }
            }
        }
    }
 
  free (events);
 
  close (sfd);
 
  return EXIT_SUCCESS;
}