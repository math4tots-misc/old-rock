#ifndef rock_object_h
#define rock_object_h



typedef struct RK_Object RK_Object;

struct RK_Object {
  RK_Object *metatable;
  union {
    long integer;
    double floating;
    struct string {
      long length;
      char *buffer;
    };
  }
};


#endif/*rock_object_h*/
