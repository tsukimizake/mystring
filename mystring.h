#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// バグ報告や要望は 適当にissue立ててください。
// とりあえずMITライセンスです。
// アンダースコアで始まる変数や関数を直接使った場合や、キャストでconstを外してデータを変更した場合の動作保証はできません

typedef struct String{
  char * _data;
  int _dataLength;
  int _bufferLength;
} String;

int const strLength(String const* const s);
char const * strData(String const* const s);


String* strCreat(char const* const s){
  String * tmp = (String*) malloc(sizeof(String));
  int length = strlen(s);
  tmp -> _data = (char*)malloc(sizeof(char) * length);
  strcpy(tmp -> _data, s);
  tmp -> _dataLength = length;
  tmp -> _bufferLength = length;
  return tmp;
}

// 何文字くらいまで増えそうか最初からわかっている場合はこちらを使うとmalloc回数を減せる
String* strCreatn(char const* const s, int initialLength){
  String * tmp = (String*) malloc(sizeof(String));
  int dataLength = strlen(s);
  tmp -> _data = (char*)malloc(sizeof(char) * initialLength);
  strcpy(tmp -> _data, s);
  tmp -> _dataLength = dataLength;
  tmp -> _bufferLength = initialLength;
  return tmp;
}

// length個のStringをまとめて作って配列にする。mallocを何度も呼ぶのは遅いので、たくさんのStringを生成したい時はこちら。(TODO)
/* String** strsCreat(int length, char const * const * const ss){ */

/* } */

// バッファサイズは余裕をもって増やしていく。文字数100なら128、300なら512。
int const _strMinBuffLength(int const dataLength){
  int res = 1;
  while(res <= dataLength){
    res *= 2;
  }
  return res;
}

// 長さnewBufSizeにメモリを再確保する。十分な長さが確保されていても再確保する。
void _strReallocate(String * const s, int const newBufSize){
  char * tmp = (char*)realloc(s -> _data, sizeof(char) * newBufSize);

  if(tmp == NULL){
    printf("failed to reallocate memory!\n");
    exit(1);
  }
  s -> _data = tmp;
  s -> _bufferLength = newBufSize;
  
}

// 多めにメモリを確保したり、確保する必要がなければしなかったりする。
void _strReallocateMaybe(String * const s, int const newDataSize){
  if(s -> _bufferLength >= newDataSize){ // バッファサイズが十分なら再確保しない
    /* printf("%d %d\n", s -> _bufferLength, newDataSize); */
    return;
  }
  int newBufSize = _strMinBuffLength(newDataSize);
  /* printf("%d\n", newBufSize); */
  _strReallocate(s, newBufSize); // バッファが足りないなら再確保
}

// ふつうのC言語文字列を返すインターフェース関数。
char const * strData(String const* const s){
  return s -> _data;
}

// 長さを返す。
int const strLength(String const* const s){
  return s -> _dataLength;
}

// lhsの右にrhsをくっつける。
void strConcat(String * const lhs, String const* const rhs){
  int newDatLength = strLength(lhs) + strLength(rhs);
  int newBufLength = _strMinBuffLength(newDatLength);
  _strReallocateMaybe(lhs, newBufLength);
  strcat(lhs -> _data, rhs -> _data);
  lhs -> _dataLength = newDatLength;
  lhs -> _bufferLength = newBufLength;
}

// lhsをrhsで書き替える
void strWriteC(String * const dest, char const * const src){
  int newDataLen = strlen(src);
  _strReallocateMaybe(dest, newDataLen);
  strcpy(dest -> _data, src);
  dest -> _dataLength = newDataLen;
}

// lhsをrhsで書き替える
void strWrite(String * const dest, String const * const src){
  int newDataLen = src -> _dataLength;
  _strReallocateMaybe(dest, newDataLen);
  strcpy(dest -> _data, src -> _data);
  dest -> _dataLength = newDataLen;
}

// freeする。
void strDelete(String const * const s){
  free((void * )s -> _data);
  free((void * )s);
}

//destのidx番目にsrcを挿入する。idxがdestのlengthより大きい場合、末尾に挿入する
int const strInsert(String * const dest, int const idx, String const* const src){

  int newsize = strLength(dest) + strLength(src);
  int destLength = strLength(dest);
  /* printf("destlen:%d\n", destLength); */
  if(idx>destLength){
    return strInsert(dest, destLength, src);
  }

  char * tmp = (char*)malloc(sizeof(char) * destLength);

  strcpy(tmp, dest -> _data);
  _strReallocateMaybe(dest, newsize);

  /* printf("tmp:%s\n", tmp); */
  size_t iter = 0;

  // destの0からidx番目までを書き込む
  for(size_t i = 0; i<idx; ++i){
    /* printf("%ld, %c ", iter,  tmp[i]); */
    dest -> _data[iter] = tmp[i];
    ++iter;
  }
  /* printf("\n"); */
  // srcを書き込む
  for(size_t i = 0; i<strLength(src); ++i){
    /* printf("%ld, %c ", iter,  src -> _data[i]); */
    dest -> _data[iter] = src -> _data[i];
    ++iter;
  }
  /* printf("\n"); */
  // destのidxから最後までを書き込む
  for(size_t i = idx; i<destLength; ++i){
    /* printf("%ld, %c ", iter,  tmp[i]); */
    dest -> _data[iter] = tmp[i];
    ++iter;
  }
  dest -> _data[iter] = '\0';
  dest -> _dataLength = newsize;
  return 0;
}

//バッファサイズを実際のデータサイズまで縮める。メモリを節約したいときに。
void strShrinkToFit(String * const s){
  if(s -> _bufferLength > s -> _dataLength){
    _strReallocate(s, s -> _dataLength);
  }
}

// strがqueryを含むか判定。含んでいたら始まりのインデックスを返す。
int const strFind(String const * const str, String const * const query){
  char const * const matchHead = strstr(strData(str), strData(query));
  if(matchHead != NULL){
    char const * const strHead = strData(str);
    return (matchHead - strHead) / sizeof(char);
  }
  return -1;
}

// 文字列切り出し。begin+countが文字列の長さを越える場合、最後までを返す。
String * strSubst(String const * const s, int const begin, int const count){
  if(begin + count > strLength(s)){
    return strSubst(s, begin, strLength(s));
  }
  
  char * tmp = (char*)malloc(sizeof(char) * count);
  /* printf("strdata:%s\n", strData(s)); */

  strncpy(tmp, strData(s) + begin, count);
  tmp[count] = '\0';
  /* printf("substtmp:%s\n", tmp); */
  String * res = strCreat(tmp);
  free((void*)tmp);
  return res;
}

