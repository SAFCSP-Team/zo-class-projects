#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define YELLOW  "\033[33m"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void display_server_boxes(char boxes[4][20]) {
    printf("\n--- Server View: Boxes Status ---\n");
    for (int i = 0; i < 4; i++) {
        printf("Box %d: ", i + 1);
        if (strcmp(boxes[i], "red") == 0) {
            printf("%s[██]%s (Red)\n", RED, RESET);
        } else if (strcmp(boxes[i], "blue") == 0) {
            printf("%s[██]%s (Blue)\n", BLUE, RESET);
        } else if (strcmp(boxes[i], "yellow") == 0) {
            printf("%s[██]%s (Yellow)\n", YELLOW, RESET);
        } else if (strcmp(boxes[i], "green") == 0) {
            printf("%s[██]%s (Green)\n", GREEN, RESET);
        } else {
            printf("[  ] (Empty)\n");
        }
    }
    printf("--------------------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: <%s> <portno>\n", argv[0]);
        exit(1);
    }

    int sockfd, newsockfd, portno, n;
    portno = atoi(argv[1]);

    struct sockaddr_in serv_addr, cli_addr;
    socklen_t clilen;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("Error opening socket\n");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("Binding failed.\n");

    char buffer[255];
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
    if (newsockfd < 0) error("Error on accepting.\n");

    char server_boxes[4][20] = {"empty", "empty", "empty", "empty"};

    while (1) {
        bzero(buffer, 255);
        n = read(newsockfd, buffer, 255);
        if (n < 0) error("Error on reading.\n");

        if (strncmp("bye", buffer, 3) == 0) {
            printf("Client disconnected.\n");
            break;
        }

        int box_num;
        char color[20];
        
        if (sscanf(buffer, "Box %d color is: %s", &box_num, color) == 2) {
            if (box_num >= 1 && box_num <= 4) {
                strcpy(server_boxes[box_num - 1], color);
            }
        } else if (sscanf(buffer, "Box %d is now Empty", &box_num) == 1) {
            if (box_num >= 1 && box_num <= 4) {
                strcpy(server_boxes[box_num - 1], "empty");
            }
        }

        display_server_boxes(server_boxes);

        bzero(buffer, 255);
        strcpy(buffer, "Color updated successfully on Server");
        
        n = write(newsockfd, buffer, strlen(buffer));
        if (n < 0) error("Error on writing\n");
    }

    close(newsockfd);
    close(sockfd);
    return 0;
}