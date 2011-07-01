#include <unistd.h>       /* read(2), write(2) */
#include <stdio.h>        /* fprintf(3) */
#include <stdlib.h>       /* exit(3), atoi(3) */
#include <sys/select.h>   /* select(2) */
#include <sys/socket.h>   /* shutdown(2) */
#include <errno.h>        /* errno */
#include <fcntl.h>        /* fcntl(2) */
#include <string.h>       /* memmove(3), strerror(3) */

void diesys(char *s) {fprintf(stderr,"%s: %s\n",s,strerror(errno));exit(errno);}

int main(int argc, char** argv)
{
  static int i, j, *fd, bytes, nfds, maxfd, fds[16][2], len[16], active[2];
  static char buf[16][4096], first=1, verbose;
  static fd_set fdset[2];

  for (fd=fds[0], argc--, argv++; argc>1 && nfds<16; nfds++, argc-=2, argv+=2) {
    if (**argv == 'v') { verbose++; nfds--; argc++; argv--; continue; }
    for (j=0; j<2; j++, fd++) {
      if ((*fd = atoi(argv[j])) > maxfd) maxfd = *fd;
      if (fcntl(*fd,F_SETFL,fcntl(*fd,F_GETFL,0)|O_NONBLOCK)<0) diesys("fcntl");
      active[j] |= (1<<nfds);
    }
  }

  while (active[0] | active[1]) {
    if (!first)
      if (select(maxfd+1, &fdset[0], &fdset[1], 0, 0) < 0) diesys("select");
    for (first=0, i=0, fd=fds[0]; i<nfds; i++) {
      for (j=0; j<2; j++, fd++) {
        if (FD_ISSET(*fd, &fdset[j])) {
          if (j==1 && (bytes = write(*fd, buf[i], len[i])) > 0)
            memmove(buf[i], buf[i]+bytes, len[i]-bytes);
          else
            bytes = read(*fd, buf[i], sizeof(buf[i]));
          if (verbose) fprintf(stderr,"iocopy: %d %c %d\n",*fd,j?'<':'>',bytes);
          if (bytes < 0) diesys(j ? "write" : "read");
          if (j==0 && bytes == 0) active[0] &= ~(1<<i);
          len[i] += (j==1 ? -bytes : bytes);
        }
        if (j==1 && !len[i] && !(active[0] & (1<<i))) active[1] &= ~(1<<i);
      }
      for (j=0, fd=fds[i]; j<2; j++, fd++) {
        if ((j == len[i]>0) && (active[j] & (1<<i))) FD_SET(*fd, &fdset[j]);
        else FD_CLR(*fd, &fdset[j]);
        if (!(active[j] & (1<<i))) shutdown(*fd, j?SHUT_WR:SHUT_RD);
      }
    }
  }

  return 0;
}
