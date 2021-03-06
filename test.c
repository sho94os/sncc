
#define test(x, expect) test_impl(x, expect, #x, __LINE__)

// test

/*
 * C compiler test
 */

void printf(char* fmt);
void exit(int x);
int strcmp(char* a, char* b);
char* strtok(char* s1, char* s2);

void test_impl(int x, int expect, char* s, int line) {
  if (!(x == expect)) {
    printf("[ERROR] L%d %s: expect %d, but got %d", line, s, expect, x);
    exit(1);
  }
}

void infix_test() {
  test(1+2, 3);
  test(2-1, 1);
  test(1+10-2, 9);
  test(1+2*3, 7);
  test(7+2/2, 8);
  test((1+1)*2, 4);
  test((1+1)/2, 1);
  test(4 % 4, 0);
  test(4 % 5, 4);
  test(5 % 4, 1);
  test(9 == 9, 1);
  test(9 == 8, 0);
  test(!0, 1);
  test(!1, 0);
  test(!10, 0);
  test(2 <= 5-2, 1);
  test(10 <= 5-2, 0);

  test(10 & 7, 2);

  test(1 && 1, 1);
  test(1 && 0, 0);
  test(0 && 0, 0);
  test(0 || 1, 1);
  test(1 || 0, 1);
  test(0 || 0, 0);
  test(0 || 0 || 0 || 1, 1);
  test(1 && 1 && 1 && 0, 0);
  test(1 && 1 && 1 && 1, 1);
}

void op_test() {
  int a = 0;
  test(++a, 1);
  test(a, 1);
  test(++a, 2);
  test(a, 2);
  a += 2;
  test(a, 4);
  a *= 2;
  test(a, 8);

  int i = 0;
  test(i++, 0);
  test(i, 1);
  test(i++, 1);
  test(i, 2);
}

void variable_test() {
  int a = 9;
  int b = 8;
  test(a + b, 17);
  int c, d;
  c = 4;
  d = 5;
  test(c, 4);
  test(d, 5);
  int e, f = 1, 2;
  test(e, 1);
  test(f, 2);
}

int return_test_impl() {
  return 1+3*3;
}
void return_test() {
  test(return_test_impl(), 10);
}

int add(int a, int b) {
  return a + b;
}

int add8(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
  return a1 + a2 + a3 + a4 + a5 + a6 + a7 + a8;
}

void call_test() {
  test(add(4, 5), 9);
  test(add8(1, 1, 1, 1, 1, 1, 1, 1), 8);
}

int if_test_impl1() {
  if (1) return 9;
  if (0) return 5;
}

int if_test_impl2() {
  if (0) return 9;
  if (1) return 5;
}

int if_test_impl3() {
  if (0) {
    return 9;
  }
  if (1) {
    return 5;
  }
}

int if_test_impl4() {
  if (0) {
    return 0;
  } else if (0) {
    return 0;
  } else if (1) {
    return 1;
  } else {
    return 0;
  }
}

void if_test() {
  test(if_test_impl1(), 9);
  test(if_test_impl2(), 5);
  test(if_test_impl3(), 5);
  test(if_test_impl4(), 1);
}

void while_test() {
  int i = 0;
  while (i<10) {
    i++;
  }
  test(i, 10);
  i = 0;
  while (i<10) {
    if (i == 5) break;
    i++;
  }
  test(i, 5);
  i = 0;
  while (i<10) {
    if (i < 5) {
      i++;
      continue;
    }
    break;
  }
  test(i, 5);
}

int for_test_impl() {
  for (int i=0; i<10; i++) {
    if (i == 5) return i;
  }
}

void for_test() {
  for (int i=0; i<10; i++) {
  }
  test(i, 10);
  test(for_test_impl(), 5);
  for (int i=0; i<10; i++) {
    if (i == 5) break;
  }
  test(i, 5);
  for (int i=0; i<10; i++) {
    if (i < 9) continue;
    break;
  }
  test(i, 9);
}

void addr_test() {
  int x;
  int* p = &x;
  *p = 9;
  test(x, 9);

  int** pp = &p;
  **pp = 555;
  test(x, 555);
}

void pointer_arithmetic_test() {
  int arr[10];
  int* p = arr;
  *(p+0) = 1;
  *(p+1) = 2;
  *(p+2) = 3;
  *(p+3) = 4;
  test(arr[0] + arr[1] + arr[2] + arr[3], 10);
}

int fib(int n) {
  if (n < 2) {
    return n;
  } else {
    return fib(n-1) + fib(n-2);
  }
}

void fib_test() {
  test(fib(10), 55);
}

void sizeof_test() {
  test(sizeof(int), 4);
  test(sizeof(int*), 8);
  test(sizeof int, 4);
  test(sizeof int*, 8);
  int iarr[10];
  int* parr[10];
  test(sizeof iarr, 40);
  test(sizeof parr, 80);
}

void array_test() {
  int arr[10];
  arr[0] = 1;
  arr[1] = 2;
  arr[2] = 3;
  arr[3] = 4;
  test(arr[0] + arr[1] + arr[2] + arr[3], 10);

  *&arr[0] = 10;
  test(arr[0], 10);
}

int gx;
int garr[10];
void globalvar_test() {
  gx = 9;
  test(gx, 9);
  int* p = &gx;
  *p = 555;
  test(gx, 555);

  garr[0] = 1;
  garr[1] = 9;
  test(garr[0] + garr[1], 10);
}

void string_test() {
  char cc[3];
  cc[0] = 9;
  test(cc[0], 9);

  char* s = "yukaia";
  test(s[0], 121);
}

int N;
int S;
int dfs(int i, int sum, int cnt) {
  if (cnt == N) {
    if (sum == S) {
      return 1;
    }
  }
  if (i == 10) return 0;
  if (cnt == N) return 0;
  
  int acc = 0;
  acc = acc+dfs(i+1, sum+i, cnt+1);
  acc = acc+dfs(i+1, sum, cnt);
  return acc;
}

void aoj0030() {
  N = 3;
  S = 6;
  test(dfs(0, 0, 0), 3);
}

int GI = 9;
char* GS = "yukaia";
void globalinit_test() {
  test(GI, 9);
  test(GS[0], 121);
}

void* malloc(int size);

void malloc_test() {
  int* p = malloc(sizeof(int)*4);
  p[0] = 1;
  p[1] = 2;
  p[2] = 3;
  p[3] = 4;
  test(p[0] + p[1] + p[2] + p[3], 10);
}

struct mychar {
  char a;
  char b;
};
struct mychar3 {
  char a;
  char b;
  char c;
};
struct mycharptr {
  char a;
  char b;
  int* p;
};
struct myintc {
  char c;
  int x;
};
struct mychari {
  int x;
  char c;
};
struct mycwrap {
  struct mychar mc;
  int x;
};
struct myptr {
  int* p;
  struct myintc mic;
};
struct icci {
  int a;
  char b;
  char c;
  int d;
};

typedef struct _ptrint {
  int kind;
  void* fdecl;
  void* argdecls;
  void* body;
  int stacksize;
  void* vdecl;
  void* vinit;
  void* structtype;
} ptrint;

typedef enum _tokenkind {
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
} tokenkind;

typedef struct _token {
  tokenkind kind;
  int intval;
  char* strval;
  char* ident;
} token;

void struct_sizeof_test() {
  test(sizeof(struct mychar), 2);
  test(sizeof(struct mychar3), 3);
  test(sizeof(struct mycharptr), 16);
  test(sizeof(struct myintc), 8);
  test(sizeof(struct mychari), 8);
  test(sizeof(struct mycwrap), 8);
  test(sizeof(struct myptr), 16);
  test(sizeof(struct icci), 12);
  test(sizeof(ptrint), 64);
  test(sizeof(token), 24);
}

int pi_stacksize(ptrint* pi);

void struct_dot_test() {
  struct mychar mc;
  mc.a = 1;
  mc.b = 2;
  test(mc.a + mc.b, 3);

  struct myptr mp;
  mp.mic.c = 4;
  mp.mic.x = 5;
  test(mp.mic.c + mp.mic.x, 9);

  ptrint pi;
  pi.stacksize = 555;
  test(pi_stacksize(&pi), 555);
}

void struct_allow_test() {
  struct mychar mc;
  struct mychar* mcp = &mc;
  mcp->a = 1;
  mcp->b = 2;
  test(mcp->a + mcp->b, 3);

  struct myptr mp;
  struct myptr* mpp = &mp;
  mpp->mic.c = 4;
  mpp->mic.x = 5;
  test(mpp->mic.c + mpp->mic.x, 9);
}

struct incom {
  int x;
  struct incom* next;
};

struct incom* new_incom();

void struct_incomplete_test() {
  test(sizeof(struct incom), 16);
  struct incom ic;
  struct incom icnext;
  ic.next = &icnext;
  ic.next->x = 9;
  test(icnext.x, 9);

  struct incom* ic2 = new_incom();
  test(ic2->next->x, 9);
  test(new_incom()->next->x, 9);
}

enum tokenkind {
  TOKEN_IDENT,
  TOKEN_INTLIT,
  TOKEN_STRLIT
};
typedef enum _astkind {
  AST_IDENT,
  AST_INTLIT,
  AST_STRLIT
} astkind;

typedef struct _ast {
  astkind kind;
  char* ident;
} astree;

astree* parse_ast() {
  astree* ast = malloc(sizeof(astree));
  ast->kind = AST_IDENT;
  ast->ident = "yukari";
  return ast;
}

void parse_test() {
  astree* ast = parse_ast();
  test(strcmp(ast->ident, "yukari"), 0);
}

void enum_test() {
  enum tokenkind tk1 = TOKEN_IDENT;
  enum tokenkind tk2 = TOKEN_STRLIT;
  test(tk1, 0);
  test(tk2, 2);
  astkind ak1 = AST_IDENT;
  astkind ak2 = AST_STRLIT;
  test(ak1, 0);
  test(ak2, 2);
}

typedef struct _vector {
  void** data;
  int cap;
} vector;

vector* new_vector_cap(int cap) {
  vector* v = malloc(sizeof(vector));
  v->data = malloc(sizeof(void*)*cap);
  v->cap = cap;
  return v;
}

void typedef_vector_test() {
  vector* v = new_vector_cap(10);
  int a;
  int b;
  int c;
  v->data[0] = &a;
  v->data[1] = &b;
  v->data[2] = &c;
}

typedef int bool;
#define true 1
#define false 0

void typedef_bool_test() {
  bool t = true;
  bool f = false;
  test(t, 1);
  test(f, 0);
}

int addpp(int* a, int* b);
int addic(int a, char b);
int* padd(int a, char b);

void exfn_test() {
  int x = 4;
  int y = 5;
  test(addpp(&x, &y), 9);

  int x = 4;
  char y = 5;
  test(addic(x, y), 9);

  int x = 4;
  char y = 5;
  int* p = padd(x, y);
  test(*p, 9);
}

extern int a;

void charlit_test() {
  test('0', 48);
  test('a', 97);
  test('\'', 39);
  test('\\', 92);
  test('"', 34);
  test('(', 40);
  test(' ', 32);
}

void strtok_test() {
  char* s = malloc(sizeof(char)*10);
  s[0] = 'y';
  s[1] = 'u';
  s[2] = 'k';
  s[3] = 'a';
  s[4] = 'r';
  s[5] = 'i';
  s[6] = ' ';
  s[7] = 'i';
  s[8] = 'a';
  s[9] = '\0';
  char* deli = " ";
  test(strcmp(strtok(s, " "), "yukari"), 0);
  test(strcmp(strtok(0, " "), "ia"), 0);
}

void unsigned_test() {
  unsigned int x = 2147483646;
  test(x, 2147483646);
  test(x+1, 2147483647);
}

int main() {
  infix_test();
  op_test();
  variable_test();
  return_test();
  call_test();
  if_test();
  while_test();
  for_test();
  addr_test();
  pointer_arithmetic_test();
  fib_test();
  sizeof_test();
  array_test();
  globalvar_test();
  string_test();
  aoj0030();
  globalinit_test();
  malloc_test();
  struct_sizeof_test();
  struct_dot_test();
  struct_allow_test();
  struct_incomplete_test();
  parse_test();
  enum_test();
  typedef_vector_test();
  typedef_bool_test();
  exfn_test();
  charlit_test();
  strtok_test();
  unsigned_test();

  printf("[OK]\n");
  return 0;
}
