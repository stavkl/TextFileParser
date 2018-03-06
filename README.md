# TextFileParser
A text-file parser in c.
This project uses two independant programs that communicate over a thread-safe message queue.
It analyzes a text-file that contains entries of phone calls and text messages and creates reports for subscribers and operators.

The reader module detects a new file in the input folder and creates a child-process that parses the file line-by-line
into a pre-defined struct, and sends it over a thread-safe message queue (using the messenger module) to another 
multi-threaded program (message handler) that analyzes the struct logs it into a designated data structure (hash table chaining doubly linked list).

