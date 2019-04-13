#include <stdio.h>
#include <string.h>
#include <stdlib.h>

void test_atoi() {
  const char * s = "v1g.2.3";
  int x = atoi(&s[1]);
  printf("%s --> %d \n", s, x);
}

void test_free_const_char() {
  const char * s = "v1.2.3";
  free((void *)s);
  printf("string is now %s \n", s);
}

int main() {
  test_free_const_char();
}
