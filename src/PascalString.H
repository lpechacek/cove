#ifndef USING_PCH
#include <string.h>
#endif /* USING_PCH */

template<int maxSize> class pascal_string
{
  char length;
  char content[maxSize];

  public:
  pascal_string(): length(0) {}
  pascal_string(const char* initString)
  {
    int initStringLength = strlen(initString);
    if(initStringLength > maxSize)
    {
      initStringLength = maxSize;
    }
    memcpy(content, initString, initStringLength);
    length = initStringLength;
  }
  pascal_string& operator=(const char* initString)
  {
    return *new(this) pascal_string(initString);
  }
};
