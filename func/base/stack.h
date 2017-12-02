#ifndef __STACK_H__
#define __STACK_H__

#include <cstring>

template <class T>
class Stack {
public:
  enum {INC = 128L};
  explicit Stack(long int newsize = INC);
  ~Stack(void) { delete[] _base; _base = _top = NULL; }
  bool push(const T &elem);
  bool pop(T &elem);
  bool pop(T *elem = NULL);
  bool top(T &elem);
  bool is_empty(void) const { return _top == _base; }
  bool is_full(void) const { return _top == (_base + _size - 1); }
  int length(void) const { return (long int)(_top - _base); }
  int size(void) const { return _size; }
private:
  int _size;
  T *_top;
  T *_base;
};

template <class T> Stack<T>::Stack(long int newsize) {

  _size = newsize;
  _base = new T[_size];
  _top = _base;
  memset(_base, 0, _size * sizeof(T));
} // Stack

template <class T> bool Stack<T>::push(const T &elem) {

  if(is_full()) {
    long int len = length();
    T *newbase = new T[_size + INC];
    if(newbase == NULL)
      return false;
    memcpy(newbase, _base, len * sizeof(T));
    delete[] _base;
    _base = newbase;
    _top = _base + len;
    _size += INC;
  }
  *_top++ = elem;
  return true;
} // push

template <class T> bool Stack<T>::pop(T &elem) {

  if(is_empty())
    return false;
  elem = *--_top;
  return true;
} // pop

template <class T> bool Stack<T>::pop(T *elem) {

  if(is_empty())
    return false;
  if(elem)
    *elem = *--_top;
  else
      --_top;
  return true;
} // pop

template <class T> bool Stack<T>::top(T &elem) {

  if(is_empty())
    return false;
  elem = *(_top - 1);
  return true;
} // top

#endif // stack.h

