#include <iostream>
int main(){
  char *buf;
  sprintf(buf, "%d", 0);
  std::cout << buf;
  return 0;
}