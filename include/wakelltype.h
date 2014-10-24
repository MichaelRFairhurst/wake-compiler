#ifndef YYLTYPE_IS_DECLARED
#define YYLTYPE_IS_DECLARED 1
#define yylltype YYLTYPE

#ifdef __cplusplus
extern "C" {
#endif
typedef struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
} YYLTYPE;
#ifdef __cplusplus
}
#endif

#endif
