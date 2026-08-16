#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/stat.h>
#include <dirent.h>
#include <fcntl.h>

#define STORAGE_DIR "./cloud_storage/"
#define BUFFER_SIZE 1024

void error(const char *msg){
    perror(msg);
    exit(1);
}

int main (int argc, char*argv[]){
    if(argc < 2){
        fprintf(stderr, "Usage: <%s> <portno>\n", argv[0]);
        exit(1);
    }
    mkdir(STORAGE_DIR, 0777);
    int sockfd, newsockfd, portno;
    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Error opening socket");

    int opt = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    bzero((char *)&serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr))<0)
        error("Binding failed");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    printf("Server listening on port %d...\n", portno);
    
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) error("Error on accept");
    printf("Client connected.\n");

    char buffer[BUFFER_SIZE];
    while (1) {
        bzero(buffer, BUFFER_SIZE);
        int n = read(newsockfd, buffer, BUFFER_SIZE - 1);
        if (n < 0) {
            error("Error reading from socket");
            break;
        }

        if (strncmp("LIST", buffer, 4) == 0) {
            printf("Received request: LIST files\n");
            DIR *dir = opendir(STORAGE_DIR);
            struct dirent *entry;
            char file_list[BUFFER_SIZE] = "";

            if (dir == NULL) {
                strcpy(file_list, "Error opening storage directory.\n");
            } else { 
                while ((entry = readdir(dir)) != NULL) {
                    if (entry->d_type == DT_REG) {
                        strcat(file_list, entry->d_name);
                        strcat(file_list, "\n");
                    }
                }
                closedir(dir);
            }
            if (strlen(file_list) == 0) {
                strcpy(file_list, "No files found on server.\n");
            }
            write(newsockfd, file_list, strlen(file_list));
        }
        else if (strncmp("UPLOAD", buffer, 6) == 0) {
            char filename[256];
            long file_size;

            usleep(10000);
            read(newsockfd, filename, sizeof(filename));
            read(newsockfd, &file_size, sizeof(long));

            printf("Receiving file: %s (Size: %ld bytes)\n", filename, file_size);

            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s%s", STORAGE_DIR, filename);

            int file_fd = open(filepath, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            long total_received = 0;
            char data_buf[BUFFER_SIZE];

            while (total_received < file_size) {
                int bytes_to_read = (file_size - total_received < BUFFER_SIZE) ? (file_size - total_received) : BUFFER_SIZE;
                int r = read(newsockfd, data_buf, bytes_to_read);
                if (r <= 0) break;
                write(file_fd, data_buf, r);
                total_received += r;
            }
            close(file_fd);
            printf("File '%s' uploaded successfully.\n", filename);
            write(newsockfd, "Upload completed successfully", 30);
        }
        else if (strncmp("DOWNLOAD", buffer, 8) == 0) {
            char filename[256];
            usleep(10000);
            read(newsockfd, filename, sizeof(filename));

            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s%s", STORAGE_DIR, filename);

            int file_fd = open(filepath, O_RDONLY);
            if (file_fd < 0) {
                long err_code = -1;
                write(newsockfd, &err_code, sizeof(long));
                continue;
            }

            struct stat st;
            fstat(file_fd, &st);
            long file_size = st.st_size;
            write(newsockfd, &file_size, sizeof(long));

            char data_buf[BUFFER_SIZE];
            long total_sent = 0;

            while (total_sent < file_size) {
                int bytes_to_send = (file_size - total_sent < BUFFER_SIZE) ? (file_size - total_sent) : BUFFER_SIZE;
                int r = read(file_fd, data_buf, bytes_to_send);
                if (r <= 0) break;
                write(newsockfd, data_buf, r);
                total_sent += r;
            }
            close(file_fd);
            printf("File '%s' downloaded successfully.\n", filename);
        }
        else if (strncmp("EXIT", buffer, 4) == 0) {
            printf("Client requested to exit.\n");
            break;
        }
        else {
            printf("Unknown command received: %s\n", buffer);
        }
    } 

    close(newsockfd);
    close(sockfd);
    return 0;
}