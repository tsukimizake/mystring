#include "mystring.h"
#include <assert.h>

int main(int argc, char** argv){
  // 以下つかいかた

  // strCreatでオブジェクト生成
  String * s1 = strCreat("eueu");
  String * s2 = strCreat("gaugau");

  // strConcatで1つめに2つめをくっつける。バッファサイズ気にしなくていいよ!!!
  strConcat(s1, s2);
  assert(strcmp(strData(s1), "eueugaugau") == 0);

  // strWriteCでchar * 型の値を書きこむ
  strWriteC(s1, "hogehugahogehuga");
  assert(strcmp(strData(s1), "hogehugahogehuga") == 0);

  // strWriteでString * 型の(略
  String * s3 = strCreat("ugougougougo");
  strWrite(s1, s3);
  assert(strcmp(strData(s1), "ugougougougo") == 0);

  // strInsertで挿入
  strInsert(s1, 9, s2);
  assert(strcmp(strData(s1), "ugougougogaugauugo") == 0);
  strInsert(s3, 100, s2);
  assert(strcmp(strData(s3), "ugougougougogaugau") == 0);

  // strFindで探す。インデックスが返る。
  assert(strFind(s1, s2) == 9);

  //strSubstで部分文字列取得。
  assert(strcmp(strData(strSubst(s1, 9, 6)), "gaugau") == 0);

  // freeのかわりにstrDeleteで捨てないとリークするので注意
  strDelete(s1);
  strDelete(s2);
  strDelete(s3);
}

