#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argc, char *argv[]) {
    if (argc < 2) { printf("Usage: %s <port>\n", argv[0]); exit(1); }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    struct sockaddr_in serv_addr;
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(atoi(argv[1]));

    bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr));
    listen(sockfd, 5);

    printf("Server listening... waiting for audio file.\n");
    int newsockfd = accept(sockfd, NULL, NULL);

    FILE *fp = fopen("received_audio.wav", "wb");
    char buffer[1024];
    int n;
    printf("Receiving audio...\n");
    while ((n = recv(newsockfd, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, n, fp);
    }
    fclose(fp);
    close(newsockfd);
    close(sockfd);
    
    printf("Audio received and saved as received_audio.wav. Playing now...\n");
    system("aplay received_audio.wav"); 
    return 0;
}