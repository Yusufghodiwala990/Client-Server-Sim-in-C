/*######################################################################################

# Name: lab6_client.c, IPC with FIFOS, Client Code
# Written by: Yusuf Ghodiwala - April 2021
# CLient Purpose: To connect with a server to upload a file
# 

# Description of parameters to be passed:
#  
#  FILE NAME  : The user will need to send a file to upload
# 
#  Usage:  ./lab6_client <filename
#  
#  Description of the Program logic;
#   This client code will first check the presence of the two FIFOs needed to communicate
#   back and forth with the server. It will exit if they don't exist. It will then
#   open the client FIFO to write data to. It will also open the file the user passed.
#   Using a structure, it will write the contents of the file to the FIFO in Fixed Size
#   'Packets' or bytes to keep things consistent. The code will now wait for a message
#   from the server, print it and print it's own exit message to signify the transfer 
#   has been done.
#    
#   
#
#
# # DATA DICTIONARY
# VARIABLES                     DESCRIPTION
# fd                            file descriptor for the client FIFO
# filename                      pointer of type FILE * to open and read the file passed
# data_stream                   struct to maintain fixed size write()'s to client FIFO
# transfer                      instance of data_stream. It has a bytes int and a buffer
# dm                            file descriptor to open the Server Fifo for "ALL DONE" msg
# done_Message                  char array to store the "ALL DONE" from the Server FIFO

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


// directives for FIFOs to be created

#define SF "serv_FIFO"     // for server->client info
#define CF "client_FIFO"   // for client->server info
#define MAX_FILE 120      //  MAX length of file_path passed by user





int main(int argc, char * argv[])
{

    // checking for the presence of the client->server FIFO using access()

    if(access(CF,F_OK)== -1){
        printf("\nNo Client-Server FIFO to initiate data Transfer!\n");
        exit(2);
    }
    
    // checking for the presence of the sever->client FIFO using access()

    if(access(SF,F_OK) == -1){
        printf("\nNo Server-Client FIFO to receive any Confirmation!\n");
        exit(2);
    }
     
    
    // Error checking if a file was not passed

    if(argc < 2){
        printf("\n No filename was passed!\n");
        exit(3);    

    }


    // Error checking if the file path was too long

    if(strlen(argv[1]) > MAX_FILE){
        printf("\n Filename is too long! Only 120 character allowed.\n");
        exit(2);
    }



    int fd;         // file descriptor for client->server FIFO


    FILE * filename;   // FILE * pointer for the file passed by the user

    
    // opening the file for reading

    filename = fopen(argv[1],"r");
    

    // error checking if file could not be opened 

    if(filename == NULL){
        printf("\nError : Could not open file\n");
        exit(2);
    }

  printf("Filename is %s",argv[1]);




    if(fd = open(argv[1], O_RDONLY) == -1){
        fprintf(stderr, "Error %d\n", errno);
            perror("Open file");
            exit;
    }


    // structure to manage fixed size data transfer
    struct data_stream{
        int bytes;            // to store the current bytes being transfered
        char BUF[1024];       // to store the data in the buffer
    };


     // instance of data_stream which will be referenced

    struct data_stream transfer;   

    
    // opening the client->server FIFO
     
     fd = open(CF,O_WRONLY);

     // error checking with the opening 
    
     if(fd == -1){
      printf("Something went wrong...\n");
      exit(2);
    }



    // sending the filename to the server

   write(fd,argv[1],120);

   if(fd == -1){
       printf("Something went wrong when sending filename");
       exit(2);
   }
 
  

     
    

    
       // intializing the byte counter for transfer

      transfer.bytes = 0;

      // char to hold the current char while looping through the file
      char c;

      
      // loop to run through the bytes of the file

      while((c = fgetc(filename)) != EOF)
      {
           
          
          // if the current packet reached 1023 (0-1023 = 1024)
          // we send it over to the client->FIFO

          if(transfer.bytes == 1023){

                // sending the structure itself

                 // error checking

                write(fd, &transfer, sizeof(transfer));
                    printf("-----BREAK-----");
                   
                   // resetting the bytes counter for the current 'packet'

                    transfer.bytes = 0;
                    
                
          }
            
        

                
          // assigning the character into the structs buffer

          transfer.BUF[transfer.bytes] = c;
          printf("%c",transfer.BUF[transfer.bytes]);

          // incrementing the bytes counter for the current 'packet'
          transfer.bytes++;
       

      }
    
     // for bytes < 1024
     // remainder of the bytes being written


     if(write(fd,&transfer,sizeof(transfer)) == -1){
         printf("Error... could not write");
         return 2;
         
     }


        printf("\n\n---WRITING DONE---\n\n");

        

        // char array to store the ALL DONE message which is sent by the server

        char done_Message[10];

        // opening the server->client FIFO

        int dm = open(SF,O_RDONLY);
          


        // error checking
        if(dm == -1){
            printf("Could not open the FIFO for the ALL DONE message\n");
            exit;
        }
        


        // reading the ALL DONE message from the server->client FIFO

        read(dm,done_Message,sizeof(done_Message));

        printf("\n\n\n RECEIVED MESSAGE FROM SERVER : %s\n",done_Message);





        printf("\nCLIENT : ----OK----\n\n\n");

     // closing all descriptors and pointer
         
     close(fd);
     fclose(filename);
     return 0;
}