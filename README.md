# Client-Server-Sim-in-C
Client Server Simulation with FIFOs in C (Named Pipes)
There are a number of ways to enable inter-process communication i.e Sockets, FIFOs, Shared Memory etc.
I have chosen FIFOs otherwise known as Named Pipes upload a file from a Client to Server. The client will provide a file to upload and this will be inserted as a data stream in
'packets' (fixed sized transfer) to the Server FIFO which the server will read and store. The server will then write to the Client FIFO with an "All done" message to indicate
the transfer has been completed. The server will run on an infinite loop waiting for a client to write something to the server FIFO.
