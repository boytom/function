#include <iostream>

#include "stdafx.h"
#include "poly.h"

OptTable Poly::opt_table[OPTS]
      = {{'+', ADD }, {'-', SUB  },
         {'*', MUL }, {'/', DIV  },
         {'%', REM }, {'(', LBRA },
         {')', RBRA}, {',', COMMA}};

/*
���ȼ��ж�
     ��
     +  -  *  /  %  (  )
�� + >  >  <  <  <  <  >
   - >  >  <  <  <  <  >
   / >  >  >  >  >  <  >
   % >  >  >  >  >  <  >
   ( <  <  <  <  <  <  >
   ) -  -  -  -  -  -  -          ')' ������ջ
*/

/*
���ȼ�����
         ��             ��
+        4              3
-        4              3
*        6              5
/        6              5
%        6              5
(        2              7
)        -              1       ')' ������ջ�����ȼ�
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
// ����ĸ˳���������к�����
  qsort(func_table, sizeof(func_table) / sizeof(func_table[0]),
	  sizeof(func_table[0]), cmp_func);
} // sort_func_table

void Poly::sort_opt_table(void) {
// �� ASCII ���С�������в�������
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

  // ����������
    static std::string dump_string;
    int i, j;
    char buf[1024];

    dump_string.clear();

    dump_string += get_error_string();
    dump_string += '\n';

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "----------------------------------------------------------------\n����������%d\n", _vars.length());
    dump_string += buf;

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "��������                             ������ַ       ����ֵ\n");
    dump_string += buf;

    for(i = 0; i < _vars.length(); i++) {
        memset(buf, 0, sizeof(buf));
        sprintf(buf, "%-*.*s %10d%20lf\n", KEYSIZE - 1, KEYSIZE - 1, _vars[i].id, _vars[i].varaddr, _vars[i].value);
        dump_string += buf;
    }

    memset(buf, 0, sizeof(buf));
    sprintf(buf, "----------------------------------------------------------------\n�������ʽ\n");
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
    sprintf(buf, "\n----------------------------------------------------------------\n�������ĺ������ʽ�������õ�ַ��ʽ��\n");
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

  static char *errstr[] = {"û�д���", "��ʶ��Ҫ����ĸ��ͷ", "����ʶ��ĺ���",
      "�ͺ��������ı���", "����ʶ����ַ�", "������ȱ���Ҳ��������������������",
      "û����������������ƥ��", "û����������������ƥ��", "���ʽ�ǿյ�", "��ʶ������̫��"
  };

  static char msg[128 + KEYSIZE]; // 128 ���㹻���
  memset(msg, 0, sizeof(msg));
  sprintf(msg, "���� %d: %s: %s λ��: %d", _errmsg.errcode, errstr[_errmsg.errcode], _errmsg.msg, _errmsg.pos);
  return msg;
} // get_error_string

double Poly::calc(void) {

  // ��Ӽ�����룺���㺯�����ʽֵ
  return do_calc(_poly);
} // calc

double Poly::do_calc(List<Node> &newpoly) {

  // ��Ӽ�����룺���㺯�����ʽֵ�ĺ��ĺ���
    
    Stack<Opt> opt_stack;
    Stack<double> opd_stack;
    int i, j;
    Opt opt;
    double left, right, result;
    List<Node> temppoly;

    for(i = 0; i < newpoly.length(); i++) {
        switch(newpoly[i].type) {
            case IDCONST:                // ����
                opd_stack.push(newpoly[i].opd);
                break;
            case IDVAR:                  // ����
                opd_stack.push(_vars[newpoly[i].varaddr].value);
                break;
            case IDOPT:                  // ������
                if(opt_stack.is_empty()) {
                    opt_stack.push(newpoly[i].opt);
                    break;
                }
                opt_stack.top(opt);
                if(pri_table[opt].inpri < pri_table[newpoly[i].opt].outpri) // ջ�����ȼ��ߣ�ջ���������ջ
                    opt_stack.push(_poly[i].opt);
                else {    // ջ�����ȼ��ߣ�����ջ�ڵ������������
                    while(pri_table[opt].inpri > pri_table[newpoly[i].opt].outpri && opt != LBRA && !opt_stack.is_empty()) {
                        opt_stack.pop(opt);
                        opd_stack.pop(right);
                        opd_stack.pop(left);
                        result = do_base_calc(opt, left, right);
                        opd_stack.push(result);
                        opt_stack.top(opt);
                    }
                    if(newpoly[i].opt == RBRA)   // �����������
                       opt_stack.pop(opt);       // �������ų�ջ
                    else
                       opt_stack.push(newpoly[i].opt); // ջ���������ջ
                }
                break;
            case IDFUNC:                // ����
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

  // ���ñ�����ֵ
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
  Stack<Opt> opt_stack; // �����������ƥ���ջ
  Stack<int> i_stack;   // �����洢������λ�õ�ջ

  _errmsg.clear();
  _poly.clear();
  _vars.clear();
  _nvars = 0;
  // �����������ʽ�ĺ���
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
    if((pos = search_opt(_str[i])) >= 0) { // ʶ�����һ��������

      node.type = IDOPT;
      node.opt = opt_table[pos].opt;

      // ���������ǲ��ǵ�һ��������
      if(_poly.is_empty() && ((node.opt >= ADD && node.opt <= REM) || node.opt == RBRA)) {
          //��һ����������+��-��*������ %,���� ),���Ǵ����
          _errmsg.setmsg(_str[prei], POLYERROR, prei); // _str[prei] �ǳ���Ĳ�����
          return false;
      }
      if(node.opt == LBRA) {  // ��������ջ
          opt_stack.push(LBRA);
          i_stack.push(prei);
      }
      else if(node.opt == RBRA) { // ���������ţ������žͳ�ջ
          if(opt_stack.is_empty()) {  // ջ�գ������Ų�ƥ��
              _errmsg.setmsg(_str[i], RBRAERROR, prei);
              return false;
          }
          opt_stack.pop();
          i_stack.pop();
      }
      _poly.insert_last(node);           // ���
      // ����������������ǲ��ǲ�����
      do {
          i++;
      } while(i < _str.length() && isspace(_str[i]));
      if(i >= _str.length() && node.opt != RBRA) {
          //�������ʽ�Բ�������β,���������������������
          _errmsg.setmsg(_str[prei], POLYERROR, prei); // _str[prei] �ǳ���Ĳ�����
          return false;
      }
      if(i >= _str.length()) // ���ʽ�ѿ�
          continue;
      pos = search_opt(_str[i]); // pos >= 0 ˵�� _str[i] �Ǹ�������
      if(pos >= 0 && node.opt >= ADD && node.opt <= REM && (opt_table[pos].opt == RBRA || opt_table[pos].opt >= ADD && opt_table[pos].opt <= REM)) {
          //�������� +��-��* ���� /�������� (��+��-��* ���� /����˵��������û�в�����
          _errmsg.setmsg(_str[prei], POLYERROR, prei);  // _str[prei] �ǳ���Ĳ�����
          return false;
      }
      if(pos >=0 && node.opt == LBRA && (opt_table[pos].opt == RBRA || (opt_table[pos].opt >= ADD && opt_table[pos].opt <= REM))) {
          // ������������������(ֻ������������������������������)
          _errmsg.setmsg(_str[prei], POLYERROR, prei);  // _str[prei] �ǳ���Ĳ�����
          return false;
      }
    }
    else if(isdigit(_str[i]) || _str[i] == '.') { // ʶ�����һ�����ֳ���
      double val = 0.0;
      int j = 0;

      while(i < _str.length() && isdigit(_str[i])) 
        val = val * 10.0 + double(_str[i++] - '0');
      while(i < _str.length() && isspace(_str[i]))
        i++;
      if(i < _str.length() && _str[i] == '.') {   // С����
        double n = 1;
        i++;
        while(i < _str.length() && (isdigit(_str[i]) || isspace(_str[i]))) {
          if(isdigit(_str[i]))
            val += double(_str[i] - '0') / (n *= 10.0);
          i++;
        }
        if(i < _str.length() && isalpha(_str[i])) {
            //std::cout << "��ʶ��Ҫ����ĸ��ͷ" << std::endl;
           _errmsg.setmsg(_str[prei], IDERROR, prei);
           return false;
        }
      }
      else if(i < _str.length() && isalpha(_str[i])) {
        //std::cout << "��ʶ��Ҫ����ĸ��ͷ" << std::endl;
        _errmsg.setmsg(_str[prei], IDERROR, prei);
        return false;
      }
      node.type = IDCONST;
      node.opd = val;
      _poly.insert_last(node);
    }
    else if(isalpha(_str[i])) { // ����ĸ��ͷ�ı�ʶ���Ǻ��������
      // ʶ�����ͱ�ʶ��
      int j = 0;
      Opt opt;

      while(i< _str.length() && (isalnum(_str[i]) || isspace(_str[i]))
            && j < KEYSIZE - 1) { // KEYSIZE = 33, KEYSIZE - 1 = 32,���ʶ���������� 32 ���ַ�
        if(isalnum(_str[i]))
          node.id[j++] = _str[i];
        i++;
      }

      if(i < _str.length()) {
        pos = search_opt(_str[i]); // ��������ʶ������Ĳ�����
        if(pos < 0) {             // ���治��һ��������
          if(isalnum(_str[i])) {  // �������ĸ�����֣���˵����ʶ��̫����
            _errmsg.setmsg(_str[i], IDTOLONG, i);
            return false;
          }
          continue;               // �����Ƿ��ַ�
        }
        opt = opt_table[pos].opt;
      }
      pos = search_func(node.id);
      if(i >= _str.length() && pos >= 0) { // ��������ʶ���Ǻ�����,������û�в�����,��Ϊ���Ǻͺ��������ı���
          _errmsg.setmsg(node.id, VARERROR, prei);
          return false;
      }
      if(i >= _str.length() || (opt != LBRA && pos < 0)) {
        // �Ѿ����˱��ʽ��β����治��һ�������ţ������ʶ���Ǹ�����
		int addr;

        node.type = IDVAR;
		for(addr = 0; addr < _vars.length(); addr++) // ������������ǲ����Ѿ��Ǽǹ���
			if(strcmp(_vars[addr].id, node.id) == 0)
				break;
		if(addr >= _vars.length()) { // ������һ���µı���
	        Vars var;

            var.value = 0.0;        // ��ֵ
            strcpy(var.id, node.id);
            var.varaddr = node.varaddr = _nvars++;
            _vars.insert_last(var);
		}
		else // ��������Ǹ��ɵı���
			node.varaddr = addr;
		_poly.insert_last(node);
      }
      else if(i < _str.length() && opt == LBRA && pos >= 0) {
        // �����Ǹ������ţ������ʶ���Ǹ�����
        node.type = IDFUNC;
        node.func = func_table[pos].func;
        _poly.insert_last(node);
      }
      else if(i < _str.length() && opt == LBRA && pos < 0) {
        //std::cout << "����ʶ��ĺ��� " << node.id << std::endl;
        _errmsg.setmsg(node.id, FUNCERROR, prei);
        return false;
      }
      else if(i < _str.length() && opt != LBRA && pos >= 0) {
        //std::cout << "�ͺ��������ı��� " << node.id << std::endl;
        _errmsg.setmsg(node.id, VARERROR, prei);
        return false;
      }
    }
    else {
      //std::cout << "�˴���Ӧ���� " <<  _str[i] << std::endl;
      _errmsg.setmsg(_str[i], CHERROR, prei);
      return false;
    }
  }
  if(!opt_stack.is_empty()) { //ջ����Ӧ�ǿյģ�������գ���������һ��������û��������ƥ��
       i_stack.pop(pos);
      _errmsg.setmsg('(', LBRAERROR, pos); //����ط�һ����������,�������˸�����
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

