#include "wakelltype.h"

#ifdef __cplusplus
extern "C" {
#endif

void error_print_error_token(YYLTYPE* location);
void error_open_file(const char* filename);
void error_set_buffer(const char* buffer);
void error_close_file();

#ifdef __cplusplus
}
#endif
