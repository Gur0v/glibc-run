#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>

#include "config.h"

#define MAX_PATH_LEN 4096

int resolve_binary_path(const char *binary, char *resolved_path) {
    if (realpath(binary, resolved_path) == NULL) {
        fprintf(stderr, "Error: Failed to resolve binary path '%s': %s\n", binary, strerror(errno));
        return -1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int silent = 0;

    if (argc > 1 && strcmp(argv[1], "-s") == 0) {
        silent = 1;
        argv++;
        argc--;
    }

    if (argc < 2) {
        fprintf(stderr, "Usage: %s [-s] <path-to-glibc-binary> [args...]\n", argv[0]);
        return EXIT_FAILURE;
    }

    char loader_path[MAX_PATH_LEN];
    snprintf(loader_path, sizeof(loader_path), "%s/lib/ld-linux-x86-64.so.2", GLIBC_DIR);

    if (access(loader_path, F_OK) != 0) {
        fprintf(stderr, "Error: glibc loader not found at '%s'\n", loader_path);
        return EXIT_FAILURE;
    }

    char binary_path[MAX_PATH_LEN];
    if (resolve_binary_path(argv[1], binary_path) != 0) {
        return EXIT_FAILURE;
    }

    if (access(binary_path, X_OK) != 0) {
        fprintf(stderr, "Error: Binary '%s' is not executable or not found\n", binary_path);
        return EXIT_FAILURE;
    }

    char ld_library_path[MAX_PATH_LEN];
    snprintf(ld_library_path, sizeof(ld_library_path), "LD_LIBRARY_PATH=%s/lib:%s/usr/lib", GLIBC_DIR, GLIBC_DIR);
    if (putenv(ld_library_path) != 0) {
        fprintf(stderr, "Error: Failed to set LD_LIBRARY_PATH\n");
        return EXIT_FAILURE;
    }

    if (!silent) {
        printf("Running '%s' with glibc loader '%s'\n", binary_path, loader_path);
    }

    char *exec_args[argc + 2];
    exec_args[0] = loader_path;
    exec_args[1] = binary_path;
    for (int i = 2; i < argc; i++) {
        exec_args[i] = argv[i];
    }
    exec_args[argc] = NULL;

    execv(exec_args[0], exec_args);

    fprintf(stderr, "Error: Failed to execute binary '%s': %s\n", binary_path, strerror(errno));
    return EXIT_FAILURE;
}
