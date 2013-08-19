#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdint.h>

static const uint8_t NEWLINE = '\n';
static const uint8_t SPACE = 0x20;

uint8_t *tokenize (uint8_t *input)
{
    static int running = 0;

    switch (running) {
        case 0: goto L0;
        case 1: goto L1;
        case 2: goto L2;
    }

    L0:
    running = 1;
    static int ignore = 0;
    static int begin = 0;
    static int end = 0;
    static uint8_t *lex;
    static uint8_t *original;

    original = input;
    lex = input;

    while (*lex) {
        if (*lex == SPACE || *lex == NEWLINE) {
            if (ignore)
                goto L1;
            running = 2;
            original[end] = '\0';
            ignore = 1;
            return &original[begin];
            L2:
            begin = end + 1;
            running = 1;
        } else {
            if (ignore)
                begin = end;
            ignore = 0;
        }
        L1:
        end++;
        *lex = *(lex++);
    }
    
    running = 0;
    return NULL;
}

int main (int argc, char **argv)
{
    int fd; 
    int rc;
    uint8_t buff[4096];
    uint8_t *token;

    fd = open (argv[1], O_RDONLY);

    while ((rc = read(fd, buff, 4096)) > 0) {
        while ((token = tokenize (buff)) != NULL)
            printf("Token: %s\n", token);
    }

    close(fd);

    return 0;
}
