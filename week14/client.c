#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define RESET   "\033[0m"
#define GREEN   "\033[32m"
#define RED     "\033[31m"
#define BLUE    "\033[34m"
#define YELLOW  "\033[33m"

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void display_boxes(char boxes[4][20]) {
    printf("\n--- Boxes Status ---\n");
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
    printf("--------------------\n");
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: <%s> <Server_IP> <portno>\n", argv[0]);
        exit(1);
    }

    int sockfd, portno, n;
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
        error("Connection failed\n");

    char boxes[4][20] = {"empty", "empty", "empty", "empty"};
    char buffer[255];

    while (1) {
        display_boxes(boxes);
        
        printf("Enter command (e.g., '1 red', '2 blue', '3 yellow', '3 delete', or 'bye'):\n> ");
        bzero(buffer, 255);
        if (fgets(buffer, 255, stdin) == NULL) break;
        buffer[strcspn(buffer, "\r\n")] = 0;

        if (strlen(buffer) == 0) continue;

        if (strncmp("bye", buffer, 3) == 0) {
            write(sockfd, buffer, strlen(buffer));
            break;
        }

        int box_num;
        char action[20];
        
        if (sscanf(buffer, "%d %s", &box_num, action) == 2) {
            if (box_num >= 1 && box_num <= 4) {
                if (strcasecmp(action, "red") == 0 || strcasecmp(action, "blue") == 0 || 
                    strcasecmp(action, "yellow") == 0 || strcasecmp(action, "green") == 0) {
                    strcpy(boxes[box_num - 1], action);
                } else if (strcasecmp(action, "delete") == 0 || strcasecmp(action, "remove") == 0 || strcasecmp(action, "none") == 0) {
                    strcpy(boxes[box_num - 1], "empty");
                } else {
                    printf("[!] Invalid color! Use: red, blue, yellow, green, or delete.\n");
                    continue;
                }
            } else {
                printf("[!] Invalid box number! Choose between 1 and 4.\n");
                continue;
            }
        } else {
            printf("[!] Invalid format! (e.g., 1 red  OR  3 delete)\n");
            continue;
        }

        char send_msg[255];
        if (strcmp(boxes[box_num - 1], "empty") == 0) {
            snprintf(send_msg, sizeof(send_msg), "Box %d is now Empty", box_num);
        } else {
            snprintf(send_msg, sizeof(send_msg), "Box %d color is: %s", box_num, boxes[box_num - 1]);
        }

        n = write(sockfd, send_msg, strlen(send_msg));
        if (n < 0) error("Error on writing\n");

        bzero(buffer, 255);
        n = read(sockfd, buffer, 255);
        if (n < 0) error("Error on reading\n");

        printf("Server Response: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}