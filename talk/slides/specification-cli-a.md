# Specification #

Basic operation: copy data between pairs of file descriptors.

<div class="diagram">
  <img class="pipe-bundle" src="images/pipe-bundle.svg" />
</div>

What would a command line interface to this look like?

    iocopy [options] rfd1 wfd1 rfd2 wfd2 ...

