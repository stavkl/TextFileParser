# TextFileParser
A text-file parser in c.

This project uses two independant programs that communicate over a thread-safe message queue.
The reader module detects a new file in the input folder and creates a child-process that parses the file line-by-line
into a pre-defined struct, and sends it over a thread-safe message queue (using the messenger module) to another 
multi-threaded program (message handler) that logs it into a designated data structure (hash table chaining doubly linked list).
