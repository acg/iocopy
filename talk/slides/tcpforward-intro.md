# Motivation #

## tcpforward ##

* [tcpforward](https://github.com/acg/tcpforward)
* a small netcat-like utility

## Common uses ##

* remote assistance to someone behind a NAT
* port reassignment, proxying, tunneling
* see the [tcpforward article](http://acg.github.com/2011/02/07/bouncing-hopping-tunneling-with-tcpforward.html) for more

## Example ##

    tcpforward -l 0.0.0.0:443 -c localhost:22

