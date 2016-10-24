#ifndef rock_file_h
#define rock_file_h

typedef struct RK_File RK_File;
typedef struct RK_Token RK_Token;

struct RK_File {
  char *filename;
  char *contents;
};

struct RK_Token {
  RK_File *file;
  long position;
};

#endif/*rock_file_h*/
