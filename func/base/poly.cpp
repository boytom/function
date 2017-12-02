#include <iostream>

#include "stdafx.h"
#include "poly.h"

OptTable Poly::opt_table[OPTS]
      = {{'+', ADD }, {'-', SUB  },
         {'*', MUL }, {'/', DIV  },
         {'%', REM }, {'(', LBRA },
         {')', RBRA}, {',', COMMA}};

/*
优先级判断
     外
     +  -  *  /  %  (  )
内 + >  >  <  <  <  <  >
   - >  >  <  <  <  <  >
   / >  >  >  >  >  <  >
   % >  >  >  >  >  <  >
   ( <  <  <  <  <  <  >
   ) -  -  -  -  -  -  -          ')' 不会入栈
*/

/*
优先级分配
         内             外
+        4              3
-        4              3
*        6              5
/        6              5
%        6              5
(        2              7
)        -              1       ')' 不会有栈内优先级
*/

PriTable Poly::pri_table[OPTS]
      = {{ADD, 4, 3}, {SUB, 4, 3},
         {MUL, 6, 5}, {DIV, 6, 5},
         {REM, 6, 5}, {LBRA, 2, 7},
         {RBRA, -1, 1}};

FuncTable Poly::func_table[FUNCS]
     = {{"asin", ASIN}, {"sin"  , SIN  },
        {"sinh", SINH}, {"acos" , ACOS },
        {"cos",  COS }, {"cosh" , COSH }, 
        {"atan", ATAN}, {"atan2", ATAN2}, 
        {"tan",  TAN }, {"tanh" , TANH },
        {"pow" , POW }, {"sqrt" , SQRT },
        {"log",  LOG }, {"log10", LOG10},
        {"fabs", FABS}};

int __cdecl cmp_func(const void *s1, const void *s2) {

  FuncTable *ps1, *ps2;
  ps1 = (FuncTable *)s1;
  ps2 = (FuncTable *)s2;
  return strcmp(ps1->id, ps2->id);
} // cmp_func

int __cdecl cmp_opt(const void *s1, const void *s2) {

  OptTable *ps1, *ps2;
  ps1 = (OptTable *)s1;
  ps2 = (OptTable *)s2;
  return ps1->id - ps2->id;
} // com_opt

Poly::Poly(Poly &newpoly) {

  clear();
  copy_poly(newpoly);
  sort_func_table();
  sort_opt_table();
} // Poly

Poly::Poly(const char *newpoly, int len) {

  clear();
  do_parse(newpoly, len);
  sort_func_table();
  sort_opt_table();
} // Poly

bool Poly::copy_poly(Poly &newpoly) {

  _poly = newpoly._poly;
  _nvars = newpoly._nvars;
  _str = newpoly._str;
  _errmsg.clear();
  return true;
} // copy_poly

void Poly::sort_func_table(void) {
// 按字母顺序升序排列函数表
  qsort(func_table, sizeof(func_table) / sizeof(func_table[0]),
	  sizeof(func_table[0]), cmp_func);
} // sort_func_table

void Poly::sort_opt_table(void) {
// 按 ASCII 码大小升序排列操作符表
  qsort(opt_table, sizeof(opt_table) / sizeof(opt_table[0]),
	  sizeof(opt_table[0]), cmp_opt);
} // sort_opt_table

bool Poly::set_poly(Poly &newpoly) {

  return copy_poly(newpoly);
} // set_poly

bool Poly::set_poly(const char *newpoly, int len) {

  clear();
  return do_parse(newpoly, len);
} // set_poly

const std::string &Poly::dump(void) {

  // 添加输出代码
    static std::string dump_string;
    int i, j;
    char buf[1024];

    dump_string.clear();

    dump_string += get_error_string();
    dump_string += '\n';

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "----------------------------------------------------------------\n变量个数：%d\n", _vars.length());
    dump_string += buf;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "变量名字                             变量地址       变量值\n");
    dump_string += buf;

    for(i = 0; i < _vars.length(); i++) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%-*.*s %10d%20lf\n", KEYSIZE - 1, KEYSIZE - 1, _vars[i].id, _vars[i].varaddr, _vars[i].value);
        dump_string += buf;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "----------------------------------------------------------------\n函数表达式\n");
    dump_string += buf;

    memset(buf, 0, sizeof(buf));
    for(i = 0; i < _str.length(); i++)
        dump_string += _str[i];

    memset(buf, 0, sizeof(buf));
    if(!_errmsg.isallright() && _errmsg.pos != -1)
        //printf("\n%*c", _errmsg.pos + 1, '^');
        sprintf(buf, "\n%*c", _errmsg.pos + 1, '^');
    dump_string += buf;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "\n----------------------------------------------------------------\n分析出的函数表达式（变量用地址表式）\n");
    dump_string += buf;

    for(i = 0; i < _poly.length(); i++) {
        memset(buf, 0, sizeof(buf));
        switch(_poly[i].type) {
            case IDFUNC:
                for(j = 0; j < sizeof(func_table) / sizeof(func_table[0]); j++)
                    if(_poly[i].func == func_table[j].func) {
                        dump_string += func_table[j].id;
                        break;
                    }
                break;
            case IDOPT:
                for(j = 0; j < sizeof(opt_table) / sizeof(opt_table[0]); j++)
                    if(_poly[i].opt == opt_table[j].opt) {
                        dump_string += opt_table[j].id; 
                        break;
                    }
                 break;
            case IDVAR:
                sprintf(buf, "%d", _poly[i].varaddr);
                dump_string += buf;
                break;
            case IDCONST:
                sprintf(buf, "\"%lf\"", _poly[i].opd);
                dump_string += buf;
                break;
        }
    }
    dump_string += '\n';
    return dump_string;
} // print_poly

const char *Poly::get_error_string(void) const {

  static char *errstr[] = {"没有错误", "标识符要以字母开头", "不能识别的函数",
      "和函数重名的变量", "不能识别的字符", "操作符缺少右操作数（或者左操作数）",
      "没有右括号与左括号匹配", "没有左括号与右括号匹配", "表达式是空的", "标识符名字太长"
  };

  static char msg[128 + KEYSIZE]; // 128 是足够大的
  memset(msg, 0, sizeof(msg));
  sprintf(msg, "错误 %d: %s: %s 位置: %d", _errmsg.errcode, errstr[_errmsg.errcode], _errmsg.msg, _errmsg.pos);
  return msg;
} // get_error_string

double Poly::calc(void) {

  // 添加计算代码：计算函数表达式值
  return do_calc(_poly);
} // calc

double Poly::do_calc(List<Node> &newpoly) {

  // 添加计算代码：计算函数表达式值的核心函数
    
    Stack<Opt> opt_stack;
    Stack<double> opd_stack;
    int i, j;
    Opt opt;
    double left, right, result;
    List<Node> temppoly;

    for(i = 0; i < newpoly.length(); i++) {
        switch(newpoly[i].type) {
            case IDCONST:                // 常量
                opd_stack.push(newpoly[i].opd);
                break;
            case IDVAR:                  // 变量
                opd_stack.push(_vars[newpoly[i].varaddr].value);
                break;
            case IDOPT:                  // 操作符
                if(opt_stack.is_empty()) {
                    opt_stack.push(newpoly[i].opt);
                    break;
                }
                opt_stack.top(opt);
                if(pri_table[opt].inpri < pri_table[newpoly[i].opt].outpri) // 栈外优先级高，栈外运算符进栈
                    opt_stack.push(_poly[i].opt);
                else {    // 栈内优先级高，弹出栈内的运算符，计算
                    while(pri_table[opt].inpri > pri_table[newpoly[i].opt].outpri && opt != LBRA && !opt_stack.is_empty()) {
                        opt_stack.pop(opt);
                        opd_stack.pop(right);
                        opd_stack.pop(left);
                        result = do_base_calc(opt, left, right);
                        opd_stack.push(result);
                        opt_stack.top(opt);
                    }
                    if(newpoly[i].opt == RBRA)   // 如果是右括号
                       opt_stack.pop(opt);       // 则左括号出栈
                    else
                       opt_stack.push(newpoly[i].opt); // 栈外运算符进栈
                }
                break;
            case IDFUNC:                // 函数
                temppoly.clear();
                j = i;
                for(i += 2; (newpoly[i].type == IDVAR || newpoly[i].type == IDCONST)
                            || (newpoly[i].type == IDOPT && newpoly[i].opt != COMMA && newpoly[i].opt != RBRA); i++)
                    temppoly.insert_last(newpoly[i]);

                left = do_calc(temppoly);
                right = 0;
                if(newpoly[i].opt == COMMA) {
                    temppoly.clear();
                    for(i++; (newpoly[i].type == IDVAR || newpoly[i].type == IDCONST)
                                    || ( newpoly[i].type == IDOPT && newpoly[i].opt != COMMA && newpoly[i].opt != RBRA); i++)
                    temppoly.insert_last(newpoly[i]);
                    right = do_calc(temppoly);
                }
                result = do_func_calc(newpoly[j].func, left, right);
                opd_stack.push(result);
                break;
        }
    }  
    while(!opt_stack.is_empty()) {
        opt_stack.pop(opt);
        opd_stack.pop(right);
        opd_stack.pop(left);
        result = do_base_calc(opt, left, right);
        opd_stack.push(result);
    }
    opd_stack.pop(result);
    return result;
} // do_calc

bool Poly::set_vars(const double vararray[], int n) {

  // 设置变量的值
  int i;

  if(n < nvars())
      return false;

  for(i = 0; i < nvars(); i++)
      _vars[i].value = vararray[i];
  return true;
} // set_vars

void Poly::clear(void) {

  _poly.clear();
  _str.clear();
  _vars.clear();
  _nvars = 0;
  memset(&_errmsg, 0, sizeof(ErrMsg));
  _errmsg.clear();
} // clear

bool Poly::do_parse(const char *newpoly, int len) {

  Node node;
  int i, pos, prei;
  Stack<Opt> opt_stack; // 用来检查括号匹配的栈
  Stack<int> i_stack;   // 用来存储左括号位置的栈

  _errmsg.clear();
  _poly.clear();
  _vars.clear();
  _nvars = 0;
  // 解析函数表达式的函数
  if(newpoly == NULL) {
      _errmsg.setmsg(' ', POLYEMPTY);
      return false;
  }
  if(!_str.copy(newpoly, len)) {
    _errmsg.setmsg(newpoly, POLYEMPTY);
    return false;
  }

  i = 0;

  while(i < _str.length()) {
    if(isspace(_str[i])) {
      i++;
      continue;
    }
     prei = i;
    memset(&node, 0, sizeof(node));
    if((pos = search_opt(_str[i])) >= 0) { // 识别出了一个操作符

      node.type = IDOPT;
      node.opt = opt_table[pos].opt;

      // 检查操作符是不是第一个操作符
      if(_poly.is_empty() && ((node.opt >= ADD && node.opt <= REM) || node.opt == RBRA)) {
          //第一个操作符是+、-、*、或者 %,或者 ),这是错误的
          _errmsg.setmsg(_str[prei], POLYERROR, prei); // _str[prei] 是出错的操作符
          return false;
      }
      if(node.opt == LBRA) {  // 左括号入栈
          opt_stack.push(LBRA);
          i_stack.push(prei);
      }
      else if(node.opt == RBRA) { // 遇到右括号，左括号就出栈
          if(opt_stack.is_empty()) {  // 栈空，则括号不匹配
              _errmsg.setmsg(_str[i], RBRAERROR, prei);
              return false;
          }
          opt_stack.pop();
          i_stack.pop();
      }
      _poly.insert_last(node);           // 填表
      // 下面检查操作符后面是不是操作数
      do {
          i++;
      } while(i < _str.length() && isspace(_str[i]));
      if(i >= _str.length() && node.opt != RBRA) {
          //函数表达式以操作符结尾,但这个操作符不是右括号
          _errmsg.setmsg(_str[prei], POLYERROR, prei); // _str[prei] 是出错的操作符
          return false;
      }
      if(i >= _str.length()) // 表达式已空
          continue;
      pos = search_opt(_str[i]); // pos >= 0 说明 _str[i] 是个操作符
      if(pos >= 0 && node.opt >= ADD && node.opt <= REM && (opt_table[pos].opt == RBRA || opt_table[pos].opt >= ADD && opt_table[pos].opt <= REM)) {
          //操作符是 +、-、* 或者 /，后面是 (、+、-、* 或者 /，则说明操作符没有操作数
          _errmsg.setmsg(_str[prei], POLYERROR, prei);  // _str[prei] 是出错的操作符
          return false;
      }
      if(pos >=0 && node.opt == LBRA && (opt_table[pos].opt == RBRA || (opt_table[pos].opt >= ADD && opt_table[pos].opt <= REM))) {
          // 不是两个左括号连用(只能是两个或者两个以上左括号连用)
          _errmsg.setmsg(_str[prei], POLYERROR, prei);  // _str[prei] 是出错的操作符
          return false;
      }
    }
    else if(isdigit(_str[i]) || _str[i] == '.') { // 识别出了一个数字常量
      double val = 0.0;
      int j = 0;

      while(i < _str.length() && isdigit(_str[i])) 
        val = val * 10.0 + double(_str[i++] - '0');
      while(i < _str.length() && isspace(_str[i]))
        i++;
      if(i < _str.length() && _str[i] == '.') {   // 小数点
        double n = 1;
        i++;
        while(i < _str.length() && (isdigit(_str[i]) || isspace(_str[i]))) {
          if(isdigit(_str[i]))
            val += double(_str[i] - '0') / (n *= 10.0);
          i++;
        }
        if(i < _str.length() && isalpha(_str[i])) {
            //std::cout << "标识符要以字母开头" << std::endl;
           _errmsg.setmsg(_str[prei], IDERROR, prei);
           return false;
        }
      }
      else if(i < _str.length() && isalpha(_str[i])) {
        //std::cout << "标识符要以字母开头" << std::endl;
        _errmsg.setmsg(_str[prei], IDERROR, prei);
        return false;
      }
      node.type = IDCONST;
      node.opd = val;
      _poly.insert_last(node);
    }
    else if(isalpha(_str[i])) { // 以字母开头的标识符是函数或变量
      // 识别函数和标识符
      int j = 0;
      Opt opt;

      while(i< _str.length() && (isalnum(_str[i]) || isspace(_str[i]))
            && j < KEYSIZE - 1) { // KEYSIZE = 33, KEYSIZE - 1 = 32,则标识符最多可以有 32 个字符
        if(isalnum(_str[i]))
          node.id[j++] = _str[i];
        i++;
      }

      if(i < _str.length()) {
        pos = search_opt(_str[i]); // 检查这个标识符后面的操作符
        if(pos < 0) {             // 后面不是一个操作符
          if(isalnum(_str[i])) {  // 如果是字母或数字，则说明标识符太长了
            _errmsg.setmsg(_str[i], IDTOLONG, i);
            return false;
          }
          continue;               // 其它非法字符
        }
        opt = opt_table[pos].opt;
      }
      pos = search_func(node.id);
      if(i >= _str.length() && pos >= 0) { // 最后这个标识符是函数名,但是它没有操作数,认为它是和函数重名的变量
          _errmsg.setmsg(node.id, VARERROR, prei);
          return false;
      }
      if(i >= _str.length() || (opt != LBRA && pos < 0)) {
        // 已经到了表达式结尾或后面不是一个左括号，这个标识符是个变量
		int addr;

        node.type = IDVAR;
		for(addr = 0; addr < _vars.length(); addr++) // 查找这个变量是不是已经登记过了
			if(strcmp(_vars[addr].id, node.id) == 0)
				break;
		if(addr >= _vars.length()) { // 遇到了一个新的变量
	        Vars var;

            var.value = 0.0;        // 初值
            strcpy(var.id, node.id);
            var.varaddr = node.varaddr = _nvars++;
            _vars.insert_last(var);
		}
		else // 这个变量是个旧的变量
			node.varaddr = addr;
		_poly.insert_last(node);
      }
      else if(i < _str.length() && opt == LBRA && pos >= 0) {
        // 后面是个左括号，这个标识符是个函数
        node.type = IDFUNC;
        node.func = func_table[pos].func;
        _poly.insert_last(node);
      }
      else if(i < _str.length() && opt == LBRA && pos < 0) {
        //std::cout << "不能识别的函数 " << node.id << std::endl;
        _errmsg.setmsg(node.id, FUNCERROR, prei);
        return false;
      }
      else if(i < _str.length() && opt != LBRA && pos >= 0) {
        //std::cout << "和函数重名的变量 " << node.id << std::endl;
        _errmsg.setmsg(node.id, VARERROR, prei);
        return false;
      }
    }
    else {
      //std::cout << "此处不应出现 " <<  _str[i] << std::endl;
      _errmsg.setmsg(_str[i], CHERROR, prei);
      return false;
    }
  }
  if(!opt_stack.is_empty()) { //栈现在应是空的，如果不空，则至少有一个左括号没有右括号匹配
       i_stack.pop(pos);
      _errmsg.setmsg('(', LBRAERROR, pos); //这个地方一定是左括号,所以用了个常量
      return false;
  }
  if(_poly.is_empty()) {
      _errmsg.setmsg(newpoly, POLYEMPTY);
      return false;
  }
  return true;
} // do_parse

double Poly::do_base_calc(Opt opt, double left, double right) {

    switch(opt) {
      case ADD:
          return left + right;
      case SUB:
          return left - right;
      case MUL:
          return left * right;
      case DIV:
          return left / right;
      case REM:
          return double((int)left % (int)right);
      default:
          _errmsg.setmsg(char(opt), CHERROR);
    }
    return 0.0;
} /* do_base_calc */
double Poly::do_func_calc(Func func, double left, double right) {

    double result = 0.0;

    switch(func) {
        case ASIN:
            result = asin(left);
            break;
        case SIN:
            result = sin(left);
            break;
        case SINH:
            result = sinh(left);
            break;
        case ACOS:
            result = acos(left);
            break;
        case COS:
            result = cos(left);
            break;
        case COSH:
            result = cosh(left);
            break;
        case ATAN:
            result = atan(left);
            break;
        case ATAN2:
            result = atan2(left, right);
            break;
        case TAN:
            result = tan(left);
            break;
        case TANH:
            result = tanh(left);
            break;
        case POW:
            result = pow(left, right);
            break;
        case SQRT:
            result = sqrt(left);
            break;
        case LOG:
            result = log(left);
            break;
        case LOG10:
            result = log10(left);
            break;
        case FABS:
            result = fabs(left);
            break;
      }
    return result;
} /* do_func_calc */

int Poly::search_func(const char *func) {

  int result, high, low, mid;

  low = 0;
  high = sizeof(func_table) / sizeof(func_table[0]) - 1;

  while(low <= high) {
    mid = (low + high) / 2;
    if((result = strcmp(func, func_table[mid].id)) < 0)
      high = mid - 1;
    else if(result > 0)
      low = mid + 1;
    else
      return mid;
  }
  return -1;
} // search_func

int Poly::search_opt(const char opt) {

  int result, high, low, mid;

  low = 0;
  high = sizeof(opt_table) / sizeof(opt_table[0]) - 1;

  while(low <= high) {
    mid =(low + high) / 2;
    if((result = opt - opt_table[mid].id) < 0)
      high = mid - 1;
    else if(result > 0)
      low = mid + 1;
    else
      return mid;
  }
  return -1;
} // search_opt

