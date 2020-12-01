#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>

char *fread_contents(char *fname) {
    int fd = open(fname, O_RDONLY, S_IRUSR | S_IWUSR);
    struct stat st;

    if (fstat(fd, &st) == -1)
        perror("Couldn't get file size\n");

    size_t fsize = st.st_size;
    char *buff = (char *)malloc((fsize + 1) * sizeof(char));

    if (fsize != read(fd, buff, fsize)) {
        free(buff);
        close(fd);
        perror("Couln't read all of the contents from the file\n");
        return NULL;
    }
    buff[fsize] = '\0';
    close(fd);
    return buff;
}
