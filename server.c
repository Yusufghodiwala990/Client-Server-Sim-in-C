/*######################################################################################

# Name: IPC with FIFOS, Server Code
# Written by: Yusuf Ghodiwala - April 2021
# CLient Purpose: To connect with a client, receive a filename and the contents and
#                  write a localcopy the print a confirmation message
# 

# Description of parameters to be passed:
#   NO PARAMETERS
#     
#  
#  Description of the Program logic;
#   The server code which runs first (before the client) will check for both,
#   client->server & server->client FIFOs, if they don't exist, it creates them.
#   The server code will then receive a filename which it will rename and then receive
#   fixed size bytes and write them to a localfile which it creates. After copying,
#   it will then write a message to the server->client FIFO to indicate the transfer 
#   was complete.
#    
#   
#
#
# # DATA DICTIONARY
# VARIABLES                     DESCRIPTION
# fd                            file descriptor for the server FIFO
# data_stream                   struct to maintain fixed size read()'s from client FIFO
# transfer                      instance of data_stream. It has a bytes int and a buffer
# dm                            file descriptor to open the Server Fifo for "ALL DONE" msg
# done_Message                  char array to store the "ALL DONE" from the Server FIFO
# lc                            file descriptor for the local copy created by the server code
######################################################################################
*/





#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

// directives for FIFO filenames to be created
#define SF "serv_FIFO"
#define CF "client_FIFO"




int main(int argc, char * argv[])
{
    // file descriptor to open and read the client->server FIFO

    int fd;


     // exact same structure from the client code to store the current 'packet' info

     struct data_stream{
        int bytes;           // to store the bytes of the current packet
        char BUF[1024];      // to store the bytes in the buffer of the current packet
    };


    // instance of data_stream which will be referenced to control the structure
 
    struct data_stream transfer;

   // checking if the client fifo exists
   if(access(CF,F_OK) == -1)
   {
     printf("\nDid not find Client-Server FIFO, creating it...\n");
        
     // creating it if it doesn't exist
     if(mkfifo(CF,0666) == -1) 
         if (errno != EEXIST)
            { perror("server : mkfifo");
            exit(2);}

   }
  

      // checking if the server fifo exists

   if(access(SF,F_OK) == -1)
   {
       printf("\nDid not find Server-Client FIFO, creating it...\n");


       // creating if it doesn't exist

       if(mkfifo(SF,0666) == -1) 
         if (errno != EEXIST)
            { perror("client : mkfifo");
            exit(2);}

   }
       printf("\nBOTH FIFOS EXIST\n");

       // file descriptor to open/create a local copy 

        int lc; 

      // opening the client->server FIFO

      fd = open(CF,O_RDONLY);

      // error checking the open()
      if(fd == -1)
        {perror(CF); exit(2);}
        
       printf("\n Opened FIFO\n");



      // char array to store the filename the client sends

    char fname[120];

    // reading the client->server FIFO to read the filename passed

    if(read(fd, fname, sizeof(fname)) == -1){
        printf("Could not read the FIFO for filename\n");
    }
     

     // concatenating the filename with -localcopy

    strcat(fname, "-localcopy");
     


    printf("file_name is %s\n", fname);


    // a char * to store the filename instead of an array

    char *fileame;  
    fileame = fname;

  
         
// using the lc file descriptor to create a local copy if it does not exist
// and write to it


        if((lc = open(fileame, O_CREAT | O_WRONLY,0666)) == -1){
        fprintf(stderr, "Error %d\n", errno);
            perror("Open file");
            exit;
    }
    


        // bool flag to indicate the packet's bytes are < 1024 and the read()
        // should stop because it's the last packet

       int more = 1;

       

// loop to read from the client->server FIFO, condition being there > 1024 bytes left

       while(more){

           // reading from the FIFO and populating the transfer struct

           read(fd, &transfer, sizeof(transfer));
       
            
           

            // if the bytes are < the fixed size
            // we break out of the loop by turning the more flag off

           if(transfer.bytes < 1023)
           {
               more = 0;

               // writing the last block
                  write(lc,transfer.BUF,transfer.bytes);
                   close(lc);
              

               

           }

          else{
           // otherwise keep writing

          write(lc,transfer.BUF,transfer.bytes);
          }

       }


      
        // array to store the confirmation message 

        char done_Message[10] = "ALL DONE";
        

           // opening the server->client FIFO

            int dm = open(SF,O_WRONLY);

            if(dm == -1){
                printf("\nCould not open the Server->Client FIFO to send msg\n");
            }
               

            // writing the message to the FIFO

            write(dm,&done_Message,sizeof(done_Message));

close(fd);

            
            

    
}

    
