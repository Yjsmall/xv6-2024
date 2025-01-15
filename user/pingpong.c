#include "kernel/types.h"
#include "user/user.h"

#define R 0
#define W 1

int check_error(const char *msg, int res) {
    if (res == -1) {
        fprintf(2, "error: %s\n", msg);
        exit(1);
    }
    return res;
}

#define CHECK_ERROR(func, ...) check_error(#func, func(__VA_ARGS__))

int main(int argc, char *argv[]) {
    int p2c[2], c2p[2];
    CHECK_ERROR(pipe, p2c);
    CHECK_ERROR(pipe, c2p);

    int pid = CHECK_ERROR(fork);

    if (pid == 0) {
        // child
        close(p2c[W]);
        close(c2p[R]);

        char buf[2];
        CHECK_ERROR(read, p2c[R], buf, 2);

        const char *msg = "C";
        printf("%d: received pong\n", getpid());
        CHECK_ERROR(write, c2p[W], msg, strlen(msg));

        close(p2c[R]);
        close(c2p[W]);
    } else {
        // parent
        close(p2c[R]);
        close(c2p[W]);

        const char *msg = "P";
        CHECK_ERROR(write, p2c[W], msg, strlen(msg));

        char buf[2];
        CHECK_ERROR(read, c2p[R], buf, strlen(msg));
        printf("%d: received ping\n", getpid());

        close(p2c[W]);
        close(c2p[R]);
    }

    exit(0);
}