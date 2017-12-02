#include <unistd.h>
#include <iostream>

#include "stack.h"

using namespace std;

enum {MAXSIZE = 1024};

int main(void) {

  Stack<char> ds;
  char ch;

  for(char a = 'a'; a <= 'z'; a++)
    ds.push(a);
  while(!ds.is_empty()) {
    ds.pop(ch);
    cout << ch;
  }
  cout << endl; 
  return 0;
}

