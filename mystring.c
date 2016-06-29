#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct String{
  char * _data;
  int _dataLength;
  int _bufferLength;
} String;
int const strLength (String const* const s);
char const * strData (String const* const s);

// バッファサイズは倍々で増やしていく。
int _minBuffLength(int dataLength){
  int res = 1;
  while(res < dataLength){
    res *= 2;
  }
  return res;
}

String* strCreate(char const* const s){
  String * tmp = (String*) malloc(sizeof(String));
  
  int length = strlen(s);  
  tmp -> _data = malloc(sizeof(char) * length);
  strcpy(tmp -> _data, s);
  tmp -> _dataLength = length;
  tmp -> _bufferLength = length;
  return tmp;
}

void _strReallocate(String * s, int newBufSize){
  char const * tmp = malloc(sizeof(char) * strLength(s));
  tmp = strData(s);
  free((void*) s -> _data);
  s -> _data = malloc(sizeof(char) * newBufSize);
  s -> _data = (char*)tmp;
  s -> _bufferLength = newBufSize;
}

char const * strData (String const* const s){
  return s -> _data;
}

int const strLength (String const* const s){
  return s -> _dataLength;
}

void strConcat (String* lhs, String const* const rhs){
  int newDatLength = strLength(lhs) + strLength(rhs);
  _strReallocate(lhs, newDatLength);
  strcat(lhs -> _data, rhs -> _data);
  lhs -> _dataLength = newDatLength;
}

void strWriteC(String * dest, char const * const src){
  
}

void strWriteS(String * dest, String const * const src){
  
}

void strDelete (String const * s){
  free((void * )s -> _data);
  free((void * )s);
}

int main(int argc, char** argv){
  String* s1 = strCreate("eueu");
  printf("s1:%s\n", strData(s1));
  String * s2 = strCreate("gaugau");
  strConcat(s1, s2);
  printf("s1:%s\n", strData(s1));
  printf("s1->length:%d\n", strLength(s1));
  strDelete(s1);
  strDelete(s2);
}
