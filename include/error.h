#ifndef ERROR_H
#define ERROR_H

/* perror if code == invalid_code */
void handle_error(char *error_msg, int code, int invalid_code);

/* perror and exit if code == invalid_code */
void exit_with_error(char *error_msg, int code, int invalid_code);

#endif