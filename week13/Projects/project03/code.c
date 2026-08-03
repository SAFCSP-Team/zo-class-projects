#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

int main() {
    int fd = open("najd", O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1) {
        printf("Failed to open file\n");
        return EXIT_FAILURE;
    }

    pid_t pid = fork();
    if (pid == -1) {
        printf("Failed to fork\n");
        close(fd);
        return EXIT_FAILURE;
    }

    if (pid > 0) {
        wait(NULL);
        
        int fd_read = open("najd", O_RDONLY);
        if (fd_read != -1) {
            char buffer[1024];
            ssize_t bytes_read = read(fd_read, buffer, sizeof(buffer) - 1);
            if (bytes_read > 0) {
                buffer[bytes_read] = '\0';
                printf("Parent Output:\n%s", buffer);
            }
            close(fd_read);
        }
        
        close(fd);
    } 
    else {
        if (dup2(fd, STDOUT_FILENO) == -1) {
            printf("Failed to dup2\n");
            close(fd);
            exit(EXIT_FAILURE);
        }

        close(fd);

        char *args[] = {"/bin/ls", "-l", NULL};
        execv(args[0], args);

        printf("Failed to execv\n");
        exit(EXIT_FAILURE);
    }

    return 0;
}