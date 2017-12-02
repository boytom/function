#ifndef __POLY_H__
#define __POLY_H__

#include <cmath>
#include <cstdarg>
#include <cstdlib>
#include <ctype.h>
#include <cstring>
#include <string>

#include "list.h"
#include "stack.h"

/*
double  asin  (double);             / * ASIN  * /
double  sin   (double);             / * SIN   * /
double  sinh  (double);             / * SINH  * /
double  acos  (double);             / * ACOS  * /
double  cos   (double);             / * COS   * /
double  cosh  (double);             / * COSH  * /
double  atan  (double);             / * ATAN  * /
double  atan2 (double, double);     / * ATAN2 * /
double  tan   (double);             / * TAN   * /
double  tanh  (double);             / * TANH  * /
double  pow   (double, double);     / * POW   * /
double  sqrt  (double);             / * SQRT  * /
double  log   (double);             / * LOG   * /
double  log10 (double);             / * LOG10 * /
double  fabs  (double);             / * FABS  * /
*/

typedef enum {IDFUNC, IDOPT, IDVAR, IDCONST} IDType;
            // +    -    *    /    %    (    )      ,
typedef enum {ADD, SUB, MUL, DIV, REM, LBRA, RBRA, COMMA, OPTS} Opt; // COMMA是一个特殊的操作符，它不参与运算; OPTS:8个操作符

typedef enum {ASIN, SIN, SINH, ACOS, COS, COSH, ATAN, ATAN2, TAN, TANH, POW,
              SQRT, LOG, LOG10, FABS, FUNCS} Func; //FUNCS:15个函数
typedef enum {ALLRIGHT, IDERROR, FUNCERROR, VARERROR, CHERROR,
              POLYERROR, LBRAERROR, RBRAERROR, POLYEMPTY, IDTOLONG} PError;
  //  8 个操作符, 15 个函数, 
//enum {OPTS = 8, FUNCS = 15};

enum {KEYSIZE = 33}; // 标识符最多可以长 32 个字节

typedef struct {
  char id[KEYSIZE];
  Func func;
} FuncTable;

typedef struct {
  char id;
  Opt opt;
} OptTable;

typedef struct {
  Opt opt;
  int inpri;
  int outpri;
} PriTable;

typedef struct {
  IDType type;        // 类型：函数、操作符、变量、常量
  union {
    char id[KEYSIZE];    // 标识符：变量的名字
    int varaddr;               // 变量的地址
    Opt opt;                   // 操作符：+、-、*、/、% 的类型
    Func func;                 // 函数：函数的类型
    double opd;                // 操作数：常量的值
  };
} Node;

typedef struct {
  char id[KEYSIZE];
  int varaddr;
  double value;
} Vars;

typedef struct {
  PError errcode;
  char msg[KEYSIZE];
  int pos;
  bool setmsg(const char *newmsg, PError newerrcode, int newpos = -1) { 
      
      assert(newmsg != NULL);
      clear();
      errcode = newerrcode;
      strcpy(msg, newmsg);
      pos = newpos;
      return true;
  }
  bool setmsg(char newmsg, PError newerrcode, int newpos = -1) {

      clear();
      errcode = newerrcode;
      msg[0] = newmsg;
      pos = newpos;
      return true;
  }
  bool isallright() {
      return errcode == ALLRIGHT;
  }
  void clear(void) {
      memset(msg, 0, sizeof(msg));
      errcode = ALLRIGHT;
      pos = -1;
  }
} ErrMsg;

int cmp_func(const void *s1, const void *s2);
int cmp_opt(const void *s1, const void *s2);

class Poly {
public:
  static OptTable opt_table[OPTS];
  static PriTable pri_table[OPTS];
  static FuncTable func_table[FUNCS];

  explicit Poly(const char *newpoly = NULL, int len = 0);
  Poly(Poly &newpoly);
  ~Poly(void) { /* Nothing to do */ };
  bool set_poly(Poly &newpoly);
  bool set_poly(const char *newpoly, int len);
  const std::string &dump(void);
  const char *get_error_string(void) const;
  double calc(void);
  int nvars(void) const { return _nvars; }
  bool set_vars(const double vararray[], int n);
  void clear(void);
private:
  List<Node> _poly; // 多项式
  List<Vars> _vars; // 变量表 
  List<char> _str;
  // 实际的变量数，由 set_poly 设置，set_vars 用来检查设置的变量个数是否正确。
  int _nvars;
  ErrMsg _errmsg;

  bool copy_poly(Poly &newpoly);
  void sort_func_table(void);
  void sort_opt_table(void);
  bool do_parse(const char *newpoly, int len);
  double do_calc(List<Node> &newpoly);
  double do_base_calc(Opt opt, double left, double right);
  double do_func_calc(Func func, double left, double right);
  // 如果是常量，返回常量的值；如果是变量或函数，用tok返回其名字。
  int search_func(const char *fnuc); // 搜索到了返回值大于或等于 0
  int search_opt(const char opt);     // 搜索到了返回值大于或等于 0
};

#endif // poly.h
