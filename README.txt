This directory contains the libargs library, which allows an executable to use
variable input parameters. Parameter names can be identified using simply a
keyword, or a keyword preceded by one or more '-' characters. Parameter and
their arguments can be separated by a space character as defined by the isspace function, or by either ':' or '='. The '#' character is used for comments. The library also allows to load configuration files to read parameters.

Dependencies:
-GNU sed (to generated dependency files)
-GCC (tested with GCC 4.4.5)
-GNU Make
