# iocopy - an async io engine in 54 lines of C #

## Motivation ##

The program `tcpforward` is a quick and dirty utility for forwarding and tunneling tcp streams. Remote assistance to someone behind a NAT is a common use case. See the article [http://acg.github.com/2011/02/07/bouncing-hopping-tunneling-with-tcpforward.html](Bouncing, Hopping and Tunneling with tcpforward) for some other use cases.

Unfortunately, `tcpforward` has a long-standing bug: it's written in Perl. It really should be written in C. Even better, write *as little C code as possible*, and rely on existing programs and UNIX program factoring for the rest.

The `ucspi-tcp` suite of programs from Dan Bernstein provides most of the functionality of `tcpforward`. The only missing piece is the core async io engine. This engine just takes pairs of (rfd,wfd) file descriptors and uses non-blocking I/O to copy data from rfd to wfd. Think of it as a bundle of userspace pipes.

Let's envision a simple command line interface like this:

    iocopy 0 1

This just reads data from stdin and writes to stdout -- pretty boring. So what about this:

    iocopy v 0 7 6 1

This copies data from stdin to file descriptor 7, while also copying data from file descriptor 6 to stdout. The 'v' option tells iocopy to report all io operations.

Here are the examples from the `tcpforward` article, reformulated to use only `ucspi-tcp` programs in conjunction with `iocopy`.

## Example: Remote assistance, AKA "Bouncing Your Signal Off the Moon" ## 

Run the following on "moon":

    tcpserver -c 1 -v 127.0.0.1 9922 \
    sh -c 'exec 6>&0 7>&1 "$0" "$@"' \
    tcpserver -c 1 -v 127.0.0.1 9921 \
    ./iocopy v 0 7 6 1

On the destination machine:

    tcpclient -v 127.0.0.1 9922 \
    sh -c 'exec 0>&6 1>&7 "$0" "$@"' \
    tcpclient -v 127.0.0.1 22 \
    ./iocopy v 0 7 6 1

On the source machine:

    ssh -p 9921 moon

## Example: TCP port forwarding ##

    tcpserver -c 1 -v $srchost $srcport \
    tcpclient -v $dsthost $dstport \
    ./iocopy v 0 7 6 1

## Author ##

Alan Grow <alangrow+nospam@gmail.com>
Released under the BSD License

