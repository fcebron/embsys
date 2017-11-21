#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>

#include <util.h>

//-----------------------------------------------------------------------------
int main(int argc, char *argv [])
{
    char * port = NULL,
         * port2 = NULL;

    // parse comand line
    if ((argc != 3) && (argc != 5))
    {
        fprintf(stderr, "Invalid usage: gps_reader -p port_name\n or : gps_reader -d port_name1 port_name2\n");
        exit(EXIT_FAILURE);
    }

    char * options = "p:d:";
    int option;
    while((option = getopt(argc, argv, options)) != -1)
    {
        switch(option)
        { 
            case 'd':
                port2 = optarg;
                break;

            case 'p':
                port = optarg;
                break;

            case '?':
                fprintf(stderr, "Invalid option %c\n", optopt);
                exit(EXIT_FAILURE);
        }
    }

    // Open serial port
    //  Port 1:
    int fd = open(port, O_RDWR | O_NOCTTY);
    if (fd == -1)
    {
        perror("open - port1");
        exit(EXIT_FAILURE);
    }
    tcflush(fd, TCIOFLUSH);

    //  Port 2:
    int fd2 = open(port2, O_RDWR | O_NOCTTY);
    if (fd2 == -1)
    {
        perror("open - port2");
        exit(EXIT_FAILURE);
    }
    tcflush(fd2, TCIOFLUSH);

    // read port
    char buff[50],
         buff2[50];
    fd_set fdset;

    while(1)
    {
        bzero(buff, sizeof(buff));

        FD_ZERO(&fdset);
        FD_SET(fd, &fdset);
        FD_SET(fd2, &fdset);

        select(fd+2, &fdset, NULL, NULL, NULL);
        
        //  Port 1:
        if (FD_ISSET(fd, &fdset))
        {
            int bytes = read (fd, buff, sizeof(buff));

            if (bytes > 0)
            {
                printf("Port1: %s\n", buff);
                fflush(stdout);
            }
        }

        //  Port 2:
        if (FD_ISSET(fd2, &fdset))
        {
            int bytes = read (fd2, buff2, sizeof(buff2));

            if (bytes > 0)
            {
                printf("Port2: %s\n", buff2);
                fflush(stdout);
            }
        }
    }

    // close serial port
    close(fd);

    exit(EXIT_SUCCESS);
}
