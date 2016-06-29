#include <stdio.h>

#include <string.h>
#include <stdlib.h>

// バグ報告や要望は shomasd(at)gmail.com もしくはtwitterの @tsukimizake774 まで。
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
  tmp -> _data = malloc(sizeof(char) * length);
  strcpy(tmp -> _data, s);
  tmp -> _dataLength = length;
  tmp -> _bufferLength = length;
  return tmp;
}

// length個のStringをまとめて作って配列にする。mallocを何度も呼ぶのは遅いので、たくさんのStringを生成したい時はこちら。(TODO)
/* String** strsCreat(int length, char const * const * const ss){ */

/* } */

// バッファサイズは余裕をもって増やしていく。文字数10なら16, 文字数100なら128。最小値をセットできるようにしてもいいかも。
int _strMinBuffLength(int dataLength){
  int res = 1;
  while(res <= dataLength){
    res *= 2;
  }
  return res;
}

// 長さnewBufSizeにメモリを再確保する。十分な長さが確保されていても再確保する。値はセットし直す。
void _strReallocate(String * s, int const newBufSize){
  char * tmp = realloc(s -> _data, sizeof(char) * newBufSize);
  if(tmp != NULL){ // 別の場所に確保された場合(reallocのman参照)
    s -> _data = tmp;
  }
  s -> _bufferLength = newBufSize;
  
}

// 多めにメモリを確保したり、確保する必要がなければしなかったりする。
void _strReallocateMaybe(String * s, int const newDataSize){
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
void strConcat(String* lhs, String const* const rhs){
  int newDatLength = strLength(lhs) + strLength(rhs);
  int newBufLength = _strMinBuffLength(newDatLength);
  _strReallocateMaybe(lhs, newBufLength);
  strcat(lhs -> _data, rhs -> _data);
  lhs -> _dataLength = newDatLength;
  lhs -> _bufferLength = newBufLength;
}

// lhsをrhsで書き替える
void strWriteC(String * dest, char const * const src){
  int newDataLen = strlen(src);
  _strReallocateMaybe(dest, newDataLen);
  strcpy(dest -> _data, src);
  dest -> _dataLength = newDataLen;
}

// lhsをrhsで書き替える
void strWrite(String * dest, String const * const src){
  int newDataLen = src -> _dataLength;
  _strReallocateMaybe(dest, newDataLen);
  strcpy(dest -> _data, src -> _data);
  dest -> _dataLength = newDataLen;
}

// freeする。
void strDelete(String * const s){
  free((void * )s -> _data);
  free((void * )s);
}

//destのidx番目にsrcを挿入する。成功すれば1、idxが大きすぎる場合-1を返す。(Cでのエラーチェックはよくわからん)
int strInsert(String * dest, int idx, String const* const src){

  int newsize = strLength(dest) + strLength(src);
  int destLength = strLength(dest);

  if(idx>destLength){
    return -1;
  }

  char * tmp = malloc(sizeof(char) * destLength);

  strcpy(tmp, dest -> _data);
  _strReallocateMaybe(dest, newsize);

  /* printf("tmp:%s\n", tmp); */
  size_t iter = 0;

  // destの0からidx番目までを書き込む
  for(size_t i = 0; i<idx; ++i){
    dest -> _data [iter]= tmp[i];
    ++iter;
  }

  // srcを書き込む
  for(size_t i = 0; i<strLength(src); ++i){
    dest -> _data [iter]= src -> _data[i];
    ++iter;
  }

  // destのidxから最後までを書き込む
  for(size_t i = idx; i<destLength; ++i){
    dest -> _data[iter] = tmp[i];
    ++iter;
  }
  dest -> _dataLength = newsize;
  return 1;
}

//バッファサイズを実際のデータサイズまで縮める。メモリを節約したいときに。
void strShrinkToFit(String * s){
  if(s -> _bufferLength > s -> _dataLength){
    _strReallocate(s, s -> _dataLength);
  }
}

// strがqueryを含むか判定。含んでいたら始まりのインデックスを返す。
int strFind(String const * const str, String const * const query){
  char const * const matchHead = strstr(strData(str), strData(query));
  if(matchHead != NULL){
    char const * const strHead = strData(str);
    return (matchHead - strHead) / sizeof(char);
  }
  return -1;
}

// 文字列切り出し。インデックスがおかしい場合NULLを返す。
String * strSubst(String * s, int begin, int end){

  if(begin>end){
    return NULL;
  }
  if(end>strLength(s)){
    return NULL;
  }

  int length = end - begin;
  char * tmp = malloc(sizeof(char) * length); // tmp使わずすませたい(TODO)
  strncpy(tmp, strData(s) + sizeof(char) * begin, length);

  String * res = strCreat(tmp);
  free((void*)tmp);
  return res;
}


int main(int argc, char** argv){
  // 以下つかいかた

  // strCreatでオブジェクト生成
  String* s1 = strCreat("eueu");
  printf("s1:%s\n", strData(s1));
  String * s2 = strCreat("gaugau");

  // strConcatで1つめに2つめをくっつける。バッファサイズ気にしなくていいよ!!!
  strConcat(s1, s2);
  printf("s1:%s\n", strData(s1));
  printf("s1->length:%d\n", strLength(s1));

  // strWriteCでchar * 型の値を書きこむ
  strWriteC(s1, "hogehugahogehuga");
  printf("s1:%s\n", strData(s1));
  printf("s1->length:%d\n", strLength(s1));

  // strWriteでString * 型の(略
  String * s3 = strCreat("ugougougougo");
  strWrite(s1, s3);
  printf("s1:%s\n", strData(s1));
  printf("s1->length:%d\n", strLength(s1));

  // strInsertで挿入
  strInsert(s1, 9, s2);
  printf("s1:%s\n", strData(s1));
  printf("s1->length:%d\n", strLength(s1));

  // strFindで探す。インデックスが返る。
  printf("s1:%s\n", strData(s1));
  printf("s2:%s\n", strData(s2));
  printf("found:%d\n", strFind(s1, s2));

  //strSubstで部分文字列取得。
  printf("%s\n", strData(strSubst(s1, 9, 15)));

  // freeのかわりにstrDeleteで捨てないとリークするので注意
  strDelete(s1);
  strDelete(s2);
  strDelete(s3);
}

