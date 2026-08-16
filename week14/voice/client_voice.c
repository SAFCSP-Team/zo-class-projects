#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

int main(int argc, char *argv[]) {
    if (argc < 3) { printf("Usage: %s <ip> <port>\n", argv[0]); exit(1); }

    printf("Recording for 5 seconds automatically...\n");
    system("arecord -d 5 -f S16_LE -r 16000 -c 1 temp.wav");
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(atoi(argv[2]));
    inet_pton(AF_INET, argv[1], &serv_addr.sin_addr);

    connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));

    FILE *fp = fopen("temp.wav", "rb");
    char buffer[1024];
    int n;
    printf("Sending audio to server...\n");
    while ((n = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
        send(sockfd, buffer, n, 0);
    }
    
    fclose(fp);
    close(sockfd);
    printf("File sent!\n");
    return 0;
}