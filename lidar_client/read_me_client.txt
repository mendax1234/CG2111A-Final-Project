These are the files used for lidar scanning and sending the raw data over TLS.
Note that all libraries other than networking has been deleted to prevent bloat, they are provided by the EPP2 course.
Remaining files here serve as a guide on how to setup TLS communication between RPi and PC, they do not work out of the box and require setting up the virtual environment and installing the libraries which are not provided here.
Client key, cert, csr and signing perm goes into SlamLab -> nodes, TLS_node_data.

