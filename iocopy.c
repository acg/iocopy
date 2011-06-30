#include <unistd.h>   /* read, write */
#include <stdio.h>    /* fprintf */
#include <stdlib.h>   /* exit, atoi */
#include <sys/select.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>   /* memmove, strerror */

void diesys(char *s) {fprintf(stderr,"%s: %s\n",s,strerror(errno));exit(errno);}

int main(int argc, char** argv)
{
  int i, j, fd, bytes;
  static int nfds, maxfd, fds[16][2], len[16];
  static char buf[16][1024];
  static fd_set fdset[2];

  for (;;)
  {
    argv++; argc--; if (argc < 2) break;
    for (j=0; j<2; j++) {
      fd = fds[nfds][j] = atoi(*argv[j]);
      if (fcntl(fd,F_SETFL,fcntl(fd,F_GETFL,0)|O_NONBLOCK) < 0) diesys("fcntl");
      if (fd > maxfd) maxfd = fd;
    }
    argv++; argc--; nfds++;
  }

  for (;;)
  {
    for (i=0; i<nfds; i++)
    {
      for (j=1; j>=0; j--)
      {
        if (FD_ISSET(fd=fds[i][j], &fdset[j])) {
          switch (j) {
            case 1: if ((bytes = write( fd, buf[i], len[i] )) < 0) break;
              memmove( buf[i], buf[i]+bytes, len[i]-bytes );
              break;
            case 0: bytes = read( fd, buf[i], sizeof(buf[i]) );
              break;
          }
          if (bytes < 0) diesys(j ? "write error" : "read error");
          len[i] += (j ? -bytes : bytes);
        }
        (j == len[i]>0) ? FD_CLR(fd, &fdset[j]) : FD_SET(fd, &fdset[j]);
      }
    }
    if (select(maxfd, &fdset[0], &fdset[1], 0, 0) < 0)
      diesys("select error");
  }

  return 0;
}
