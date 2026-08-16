#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFFER_SIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: <%s> <Server_IP> <portno>\n", argv[0]);
        exit(1);
    }

    int sockfd, portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Error opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) {
        fprintf(stderr, "Error, no such host!");
        exit(1);
    }

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Connection failed");

    char buffer[BUFFER_SIZE];

    while (1) {
        printf("\n--- Cloud Storage Menu ---\n");
        printf("1. list\n");
        printf("2. upload <filename>\n");
        printf("3. download <filename>\n");
        printf("4. bye\n");
        printf("> ");

        bzero(buffer, BUFFER_SIZE);
        if (fgets(buffer, BUFFER_SIZE, stdin) == NULL) break;
        buffer[strcspn(buffer, "\r\n")] = 0;

        if (strlen(buffer) == 0) continue;

        if (strncmp("bye", buffer, 3) == 0) {
            write(sockfd, "EXIT", 4);
            break;
        }

        if (strncmp("list", buffer, 4) == 0) {
            write(sockfd, "LIST", 4);
            bzero(buffer, BUFFER_SIZE);
            read(sockfd, buffer, BUFFER_SIZE);
            printf("\n[Server Files]:\n%s", buffer);
        }
        else if (strncmp("upload ", buffer, 7) == 0) {
            char *filename = buffer + 7;
            int file_fd = open(filename, O_RDONLY);
            if (file_fd < 0) {
                printf("[!] Local file not found.\n");
                continue;
            }

            struct stat st;
            stat(filename, &st);
            long file_size = st.st_size;

            write(sockfd, "UPLOAD", 6);
            usleep(10000);
            
            write(sockfd, filename, 256);
            write(sockfd, &file_size, sizeof(long));

            char data_buf[BUFFER_SIZE];
            int r;
            while ((r = read(file_fd, data_buf, sizeof(data_buf))) > 0) {
                write(sockfd, data_buf, r);
            }
            close(file_fd);

            bzero(buffer, BUFFER_SIZE);
            read(sockfd, buffer, BUFFER_SIZE);
            printf("[Server Response]: %s\n", buffer);
        }
        else if (strncmp("download ", buffer, 9) == 0) {
            char *filename = buffer + 9;
            write(sockfd, "DOWNLOAD", 8);
            usleep(10000);
            
            write(sockfd, filename, 256);

            long file_size;
            int bytes_read = read(sockfd, &file_size, sizeof(long));
            
            if (bytes_read <= 0 || file_size == -1) {
                printf("File not found on server.\n");
                continue;
            }

            int file_fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            long total_received = 0;
            char data_buf[BUFFER_SIZE];

            while (total_received < file_size) {
                int bytes_to_read = (file_size - total_received < BUFFER_SIZE) ? (file_size - total_received) : BUFFER_SIZE;
                int r = read(sockfd, data_buf, bytes_to_read);
                if (r <= 0) break;
                write(file_fd, data_buf, r);
                total_received += r;
            }
            close(file_fd);
            printf("File '%s' downloaded successfully.\n", filename);
        } else {
            printf(" Invalid command.\n");
        }
    }

    close(sockfd);
    return 0;
}