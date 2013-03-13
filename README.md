gnugo_controller
================

A C library to allow easy interfacing with the gnugo engine.

To use in your application, include go_controller.h. The library manages the
creation of gnugo instances and communication with them, via the Go Text
Protocol (GTP).

More information about GNUGo: http://www.gnu.org/software/gnugo/

More information about GTP: http://www.lysator.liu.se/~gunnar/gtp/

## Notes
This library currently requires gnugo (or a symlink to it) to be in the working
directory. Eventually, it will just require gnugo to be on the path.
