# Specification #

What can we reuse?

* [ucspi-tcp](http://cr.yp.to/ucspi-tcp.html) suite
* sockets: addressing, connecting, accepting, listening

What's still missing?

* the core async io engine in tcpforward
* takes pairs of (rfd,wfd) file descriptors
* uses non-blocking i/o to copy data from rfd to wfd

