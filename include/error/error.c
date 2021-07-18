#include "error.h"

#include <stdio.h>
#include <stdlib.h>

void handle_error(char *error_msg, int code, int invalid_code) {
    if (code == invalid_code)
        perror(error_msg);
}

void exit_with_error(char *error_msg, int code, int invalid_code) {
    if (code == invalid_code) {
        perror(error_msg);
        exit(1);
    }
}