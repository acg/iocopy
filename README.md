# iocopy - an async io engine in 54 lines of C #

To be presented at the [54lines meetup on July 12, 2011 in NYC](http://www.meetup.com/54lines/events/20962661/).

## Motivation ##

The program [tcpforward](https://github.com/acg/tcpforward) is a small netcat-like utility for forwarding and tunneling tcp streams. Remote assistance to someone behind a NAT is a common use case. See the article [Bouncing, Hopping and Tunneling with tcpforward](http://acg.github.com/2011/02/07/bouncing-hopping-tunneling-with-tcpforward.html) for some other use cases (including ways to stick it to the man).

Unfortunately, tcpforward has a long-standing bug: it's written in Perl. It should be written in C. Even better, it could be written in the UNIX factoring tradition: write as few lines of code as possible, reuse other small generic programs wherever possible. Often, the result of this approach is another small generic program.

54 lines of code should be enough for anyone, right?

## The Specification ##

Programs in djb's [http://cr.yp.to/ucspi-tcp.html](ucspi-tcp) suite already provide most of the functionality of tcpforward. The primary missing piece is the core async io engine in tcpforward. This engine takes pairs of (rfd,wfd) file descriptors and uses non-blocking i/o to copy data from rfd to wfd. Think of it as a bundle of pipes with flow managed by a userspace process.

So, the basic operation of this program is to copy data between pairs of file descriptors. What would a command line interface to this look like? How about:

    iocopy [options] rfd1 wfd1 rfd2 wfd2 ...

Q: What's an equivalent UNIX program for this invocation?

    iocopy 0 1

A: cat(1), dd(1), others

Here's another invocation:

    iocopy v 0 7 6 1

This copies data from stdin to file descriptor 7, while also copying data from file descriptor 6 to stdout. What's the significance of descriptors 6 and 7? Those are "network in" and "network out" for clients in the [ucspi standard](http://cr.yp.to/proto/ucspi.txt). The 'v' option tells the program to report all io operations on stderr.

## The Implementation ##

The implementation uses only the standard C library. No fancy libraries like libevent or libev, just a straight up select(2) loop with a fixed limit on the number of fd pairs.

The code isn't obfuscated C, but it would also be a stretch to say it fits comfortably in a 54x80 terminal window.

  * 8 lines of #includes (boo...)
  * 8 lines of argument processing and setup
  * 24 lines of async io engine code
  * 13 lines of declarations, whitespace etc
  * ...checks for errors and dies with appropriate messages!

The async io engine must do typical producer-consumer stuff:

  * buffer read data until wfd becomes writable
  * mask readable event unless the buffer is empty
  * mask writable event unless the buffer is non-empty
  * shutdown the read side on eof
  * shutdown the write side after eof on the read side *and* the buffer is drained
  * exit only after both sides of *all* channels have been shutdown

## Examples ##

Here are examples from the tcpforward article, reformulated to use only ucspi-tcp programs and iocopy.

### Example: Remote assistance, AKA "Bouncing Your Signal Off the Moon" ###

Run the following on "moon":

    tcpserver -c 1 -v moon 9922 \
    sh -c 'exec 6>&0 7>&1 "$0" "$@"' \
    tcpserver -c 1 -v moon 9921 \
    ./iocopy v 0 7 6 1

On the destination machine:

    tcpclient -v moon 9922 \
    sh -c 'exec 0>&6 1>&7 "$0" "$@"' \
    tcpclient -v 127.0.0.1 22 \
    ./iocopy v 0 7 6 1

On the source machine:

    ssh -p 9921 moon

### Example: Hopping Over the Middleman ###

On host "gateway":

    tcpserver -c 1 -v 0 9922 \
    tcpclient -v production 22 \
    ./iocopy v 0 7 6 1

On your local machine:

    scp -o Port=9922 somefile gateway:somefile

Or:

    rsync -e "ssh -p 9922" -avzp somedir/ gateway:somedir/

### Example: Tunneling Through Corporate Firewalls ###

On host "freedom":

    tcpserver -c 1 -v 0 443 \
    tcpclient -v 127.0.0.1 22 \
    ./iocopy v 0 7 6 1

On your local machine:

    ssh -p 443 freedom

## Author ##

Alan Grow <alangrow+nospam@gmail.com>

Released under the BSD License

