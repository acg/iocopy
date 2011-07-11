# Implementation #

The async io engine must do typical producer-consumer stuff:

  * buffer data from rfd until wfd becomes writable
  * buffer is empty? mask writable event
  * buffer is non-empty? mask readable event
  * shutdown the read side on eof
  * shutdown the write side after eof and drain
  * exit after *all* channels have drained

