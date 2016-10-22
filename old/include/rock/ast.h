#ifndef rock_ast_h
#define rock_ast_h

#include "rock/file.h"

#define RK_BLOCK 1
#define RK_NAME 2

typedef struct RK_Expression RK_Expression;
typedef struct RK_Block RK_Block;
typedef struct RK_Name RK_Name;
typedef struct RK_StringLiteral RK_StringLiteral;

struct RK_Expression {
  RK_Token token;
  long type;
  union {
    RK_Block block;
    RK_Name name;
  }
};

struct RK_Block {
  long size;
  RK_Expression *buffer;
};

struct RK_Name {
  long size;
  char *buffer;
};

struct RK_StringLiteral {
  long size;
  char *buffer;
};

#endif/*rock_ast_h*/
