#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <util.h>
#include <syslog.h>


int max(int nbA, int nbB) {
    /**
     * The maximum function between 2 int
     */
    if (nbA >= nbB){
        return nbA;
    } 
    else {
        return nbB;
    }
}


//-----------------------------------------------------------------------------
int main(int argc, char *argv []) {
    // ###########################################
    // ######     Creation of the PIPE     #######
    // ###########################################
    int pipe_fds[2];
    if(pipe(pipe_fds) != 0) {
        perror("Creation pipe");
        return EXIT_FAILURE;
    }

    // ###########################################
    // ######             FORK             #######
    // ###########################################
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }


    // ===========================================
    // ======         Child Process        =======
    // ===========================================
    else if (pid == 0) {
        // For debug:
        // printf("Child: Fork succeded!\n");

        unsigned char buffChild[7];
        size_t bytesReceived;

        // Closing the Output of the pipe:
        close(pipe_fds[1]);

        // Setup for syslog:
        openlog("GPS_Reader", 0, LOG_USER);

        pid_t pidFather = getppid();
        //printf("My father is: %d \n", pidFather);
        syslog(LOG_ERR, "Father PID: %d", pidFather);
        

        while (1) {
            // Initialization of the buffer:
            bzero(buffChild, sizeof(buffChild));

            // Reading message from Father:
            bytesReceived = read(pipe_fds[0], buffChild, sizeof(buffChild));

            if (bytesReceived > 0) {
                // Printing message received:
                // printf("Time: %s\n", buffChild);
                // fflush(stdout);
                syslog(LOG_INFO, "GPS Time: %s", buffChild);
            }
        }

        // Closing Input of the pipe:
        close(pipe_fds[1]);

        // Killing Child process:
        _exit(EXIT_SUCCESS);
    }


    // ===========================================
    // ======         Father Process       =======
    // ===========================================
    else {
        // For debug:
        // printf("Father: Fork succeded!\n");
        
        // Closing the Input of the pipe:        
        close(pipe_fds[0]);

        // for the fork
        int status;

        // Testing the pipe:
        size_t bytesWritten;
        // unsigned char msg[7];
        // sprintf(msg, "PipeW!");

        // bytesWritten = write(pipe_fds[1], msg, strlen(msg));
        
        // // Writting error:
        // if (bytesWritten == 0) {
        //     perror("Writing pipe Initialization failed!");
        //     return EXIT_FAILURE;
        // }
        

        // -------------------------------------------
        // ------          Port Reader         -------
        // -------------------------------------------
        char * port1 = NULL,
             * port2 = NULL;

        // Parsing comand line:
        if ((argc != 3) && (argc != 5)) {
            fprintf(stderr, "Invalid usage: gps_reader -p port_name\n or dual port : gps_reader -p port_name1 -d port_name2\n");
            exit(EXIT_FAILURE);
        }

        int option,
            portNumber = 1;
        char * options = "p:d:";

        while((option = getopt(argc, argv, options)) != -1) {
            switch(option) { 
                case 'p':
                    port1 = optarg;
                    break;

                case 'd':  // Dual mode
                    port2 = optarg;
                    portNumber = 2;
                    break;

                case '?':
                    fprintf(stderr, "Invalid option %c\n", optopt);
                    exit(EXIT_FAILURE);
            }
        }

        // Opening serial ports:
        // --------------------
        //  Port 1:
        int fd1 = open(port1, O_RDWR | O_NOCTTY);
        // Failed to open serial port:
        if (fd1 == -1) {
            perror("open - port1");
            exit(EXIT_FAILURE);
        }



        //  Port 2:
        int fd2;

        if (portNumber == 2) {
            fd2 = open(port2, O_RDWR | O_NOCTTY);
            // Failed to open serial port:
            if (fd2 == -1) {
                perror("open - port2");
                exit(EXIT_FAILURE);
            }
            
        }
        

        // reading ports:
        // --------------
        size_t bytesRead1,
               bytesRead2;

        int bufferSize = 50;

        char buff1[bufferSize],
             buff2[bufferSize];


        // Parsing NMEA frames:
        char time_str[7];
        int time_value;

        // Initialization fdset:
        // ---------------------
        int max_fd;
        fd_set fdset;

        while(1) {

            // Updating fdset:
            // ---------------------
            FD_ZERO(&fdset);
            FD_SET(fd1, &fdset);
            tcflush(fd1, TCIOFLUSH);

            if (portNumber == 2) {
                FD_SET(fd2, &fdset);
                tcflush(fd2, TCIOFLUSH);

                max_fd = max(fd1, fd2);
                select(max_fd+1, &fdset, NULL, NULL, NULL);
            }
            else {
                select(fd1+1, &fdset, NULL, NULL, NULL);
            }

            //  Port 1:
            if (FD_ISSET(fd1, &fdset)) {
                bzero(buff1, sizeof(buff1));
                bytesRead1 = read (fd1, buff1, sizeof(buff1));

                if (bytesRead1 > 0) {
                    printf("Port1: %s\n", buff1);
                    fflush(stdout);

                    // Sending time to the Child:
                    if (is_gll(buff1) == 0) {
                        // It is a GLL NMEA frame so the Time is inside it
                        time_value = get_time(buff1, time_str);

                        bytesWritten = write(pipe_fds[1], time_str, strlen(time_str));
                        
                        // Writting failed:
                        if (bytesWritten == 0) {
                            perror("Writing time from father to son failed!");
                            return EXIT_FAILURE;
                        }
                    }
                }
            }

            //  Port 2:
            if (portNumber == 2) {
                if (FD_ISSET(fd2, &fdset)) {
                    bzero(buff2, sizeof(buff2));
                    bytesRead2 = read (fd2, buff2, sizeof(buff2));

                    if (bytesRead2 > 0) {
                        printf("Port2: %s\n", buff2);
                        fflush(stdout);
                    }
                }
            }
        }

        // close serial ports:
        close(fd1);

        if (portNumber == 2) {
            close(fd2);
        }

        (void)waitpid(pid, &status, 0);
    }
    return EXIT_SUCCESS;
}




    