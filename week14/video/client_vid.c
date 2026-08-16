
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

static int sockfd = -1;

void error(const char *msg) {
    perror(msg);
    exit(1);
}

void cleanup_and_exit(int sig) {
    (void)sig;
    printf("\n[!] Disconnecting...\n");
    system("pkill feh >/dev/null 2>&1");
    system("pkill aplay >/dev/null 2>&1");
    if (sockfd >= 0) close(sockfd);
    exit(0);
}

ssize_t readn(int fd, void *buf, size_t n) {
    size_t total = 0;
    char *p = (char *)buf;
    while (total < n) {
        ssize_t r = read(fd, p + total, n - total);
        if (r < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (r == 0) return 0; 
        total += (size_t)r;
    }
    return (ssize_t)total;
}


int recv_file(int sockfd, const char *tmp_path, const char *final_path) {
    uint32_t net_size = 0;
    ssize_t n = readn(sockfd, &net_size, sizeof(net_size));
    if (n <= 0) return -1;

    uint32_t size = ntohl(net_size);
    if (size == 0) return 0;

    FILE *fp = fopen(tmp_path, "wb");
    if (!fp) return -1;

    char buffer[4096];
    uint32_t total_received = 0;
    while (total_received < size) {
        uint32_t remaining = size - total_received;
        size_t to_read = remaining < sizeof(buffer) ? remaining : sizeof(buffer);
        ssize_t r = readn(sockfd, buffer, to_read);
        if (r <= 0) {
            fclose(fp);
            return -1;
        }
        fwrite(buffer, 1, (size_t)r, fp);
        total_received += (uint32_t)r;
    }
    fclose(fp);
    rename(tmp_path, final_path);
    return 1;
}

int main(int argc, char *argv[]) {
    int portno;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    if (argc < 3) {
        fprintf(stderr, "usage: %s <hostname> <port>\n", argv[0]);
        exit(1);
    }

    signal(SIGINT, cleanup_and_exit);
    signal(SIGTERM, cleanup_and_exit);

    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) error("ERROR opening socket");

    server = gethostbyname(argv[1]);
    if (server == NULL) error("ERROR, no such host");

    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr_list[0], (char *)&serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);

    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    printf("[*] Connected to Server! Receiving Audio & Video stream...\n");

    int viewer_opened = 0;

    while (1) {
        int img_rc = recv_file(sockfd, "temp_frame.jpg", "stream_frame.jpg");
        if (img_rc < 0) break; 

        int aud_rc = recv_file(sockfd, "temp_audio.wav", "stream_audio.wav");
        if (aud_rc < 0) break;

        if (aud_rc == 1) {
            system("pkill aplay >/dev/null 2>&1");
            system("aplay -q stream_audio.wav >/dev/null 2>&1 &");
        }

        if (!viewer_opened && img_rc == 1) {
            system("feh --reload 1 stream_frame.jpg &");
            viewer_opened = 1;
        }
    }

    printf("[!] Server disconnected.\n");
    system("pkill feh >/dev/null 2>&1");
    system("pkill aplay >/dev/null 2>&1");
    close(sockfd);
    return 0;
}