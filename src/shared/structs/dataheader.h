#pragma once


struct DataHeader {
  unsigned short prefix;
  unsigned short type;
  unsigned long  len;
  unsigned short postfix;
};


struct EmptyHeader {
};
