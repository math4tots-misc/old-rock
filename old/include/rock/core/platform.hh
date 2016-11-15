#ifndef rock_core_platform_hh
#define rock_core_platform_hh

namespace rock {

class Platform;
class Class;

extern Platform *platform;
extern Class *classPlatform;

// Contains information that the process can use to ask questions about
// the process.
class Platform {
public:
  virtual ~Platform();
  template <class T> T *unsafeCast() const { return static_cast<T>(this); }
  virtual std::string getName() const;
  virtual char getFileSeparator() const;
  virtual char getPathSeparator() const;
};

}

#endif//rock_core_platform_hh
