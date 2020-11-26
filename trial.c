#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>

#define BUFSIZE 256

int main(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s file\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        perror(argv[1]);
        exit(EXIT_FAILURE);
    }

    char buf[BUFSIZE];
    int bytes, pos;
    int ws = 0;

    while (0 < (bytes = read(fd, buf, BUFSIZE))) {
        //printf("Read %d bytes\n", bytes);
        for (pos = 0; pos < bytes; ++pos) {
            if (isspace(buf[pos])) {
                ws++;
            } else if (isalpha(buf[pos])) {
                buf[pos] = toupper(buf[pos]);
            } else if (buf[pos] == '\0') {
                write(STDOUT_FILENO, "!!!", 3);
            }

        }
        write(STDOUT_FILENO, buf, bytes);
    }
    // could check here whether bytes is negative
    //  bytes == 0  - end of file
    //  bytes <  0  - error

    printf("Whitespace chars: %d\n", ws);

    close(fd);
    return EXIT_SUCCESS;
}