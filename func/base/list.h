#ifndef __LIST_H__
#define __LIST_H__

#include <cassert>
#include <cstring>

template <class T>
class List {
public:
  enum {LINC = 128};
  explicit List(int newsize = LINC);
  List(List<T> &newlist);
  ~List(void) { delete[] _base; }
  T &operator[](int index) {
    assert(index >= 0 && index < _length);
    return _base[index];
  }
  bool insert_last(T &newelem);
  List<T> &operator=(const List<T> &newlist);
  bool copy(const T *str, int nelem);
  bool clear(void);
  int size(void) const { return _size; }
  int length(void) const { return _length; }
  bool is_empty(void) const { return _length == 0; }
  bool is_full(void) const { return _length == _size; }
private:
  int _size, _length;
  T *_base;
  void copy_list(const List<T> &newlist);
  bool do_insert(T &newelem);
};

template <class T> List<T>::List(int newsize) {

  _size = newsize;
  _base = new T[_size];
  memset(_base, 0, sizeof(T) * _size);
  _length = 0;
} // List

template <class T> List<T>::List(List<T> &newlist) {

  _base = NULL;
  _size = _length == 0;
  copy_list(newlist);
} // List

template <class T> bool List<T>::insert_last(T &newelem) {

  return do_insert(newelem);
} // insert_last

template <class T> List<T> &List<T>::operator=(const List<T> &newlist) {

  copy_list(newlist);
  return *this;
} // operator=

template <class T> bool List<T>::copy(const T *str, int nelem) {

  T *newbase;

  if(str == NULL || nelem == NULL)
    return false;

  if((newbase = new T[nelem]) == NULL)
    return false;
  delete[] _base;
  _base = newbase;
  memcpy(_base, str, nelem * sizeof(T));
  _size = _length = nelem;
  return true;
} // copy_list

template <class T> bool List<T>::clear(void) {

  memset(_base, 0, _size * sizeof(T));
  _length = 0;
  return true;
} // clear

template <class T> void List<T>::copy_list(const List<T> &newlist) {

  T *newbase;

  if((newbase = new T[newlist.size()]) == NULL)
    return;
  delete[] _base;
  _base = newbase;
  memcpy(_base, newlist._base, newlist.length() * sizeof(T));
  _length = newlist.length();
  _size = newlist.size();
} // copy_list

template <class T> bool List<T>::do_insert(T &newelem) {

  if(is_full()) {
    T *newbase = new T[_size + LINC];
    if(newbase == NULL)
      return false;
    memcpy(newbase, _base, length() * sizeof(T));
    delete[] _base;
    _base = newbase;
    _size += LINC;
  }
  _base[_length++] = newelem;
  return true;
} // do_insert

#endif

