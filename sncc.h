#ifndef _SNCC_H_
#define _SNCC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  void** data;
  int cap;
  int len;
} vector;

typedef struct {
  char* name;
  int pos;
} mappair;
typedef struct {
  vector* vector;
} map;

typedef enum {
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_LESSER,
  TOKEN_GREATER,
  TOKEN_ASSIGN,
  TOKEN_INTLIT,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_COMMA,
  TOKEN_SEMICOLON,
  TOKEN_IDENT
} tokenkind;

typedef struct {
  tokenkind kind;
  union {
    int intval;
    char* ident;
  };
} token;

typedef enum {
  AST_ADD,
  AST_SUB,
  AST_MUL,
  AST_DIV,
  AST_LESSER,
  AST_MINUS,
  AST_ASSIGN,
  AST_INTLIT,
  AST_IDENT,
  AST_CALL,
  AST_FUNCDEF,
  AST_IF
} astkind;

typedef struct {
  vector* vector;
} statement;

typedef struct {
  vector* vector;
} paramtypelist;

typedef struct {
  char* fdecl;
  paramtypelist argdecls;
  statement body;
} funcdecl;

typedef struct _astree {
  astkind kind;
  union {
    struct {
      struct _astree* left;
      struct _astree* right;
    };
    struct _astree* value;
    int intval;
    char* ident;
    struct {
      struct _astree* call;
      vector* arguments;
    };
    struct {
      struct _astree* ifcond;
      struct _astree* ifbody;
      struct _astree* elsebody;
    };
    statement stmt;
  };
} astree;

typedef struct {
  vector* tokens;
  int pos;
} tokenstream;

// utilities
#define error(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

// string.c
char* string_copy(char* s);

// vector.c
vector* new_vector_cap(int cap);
vector* new_vector();
void* vector_get(vector* v, int index);
void vector_set(vector* v, int index, void* elem);
void vector_push(vector* v, void* elem);

// map.c
map* new_map_cap(int cap);
map* new_map();
mappair* new_mappair(char* name, int pos);
int map_get(map* m, char* name);
void map_insert(map* m, char* name, int pos);

// lexer.c
vector* lexer();
char* token_to_kindstr(token* token);
char* token_to_str(token* token);

// parser.c
tokenstream* new_tokenstream(vector* tokens);
token* get_token(tokenstream* ts);
void next_token(tokenstream* ts);
astree* expression(tokenstream* ts);
// statement
statement parse_statement(tokenstream* ts);
astree* statement_get(statement st, int index);
int statement_len(statement st);
// funcdecl
char* paramtypelist_get(paramtypelist ptlist, int index);
int paramtypelist_len(paramtypelist ptlist);
funcdecl parse_funcdecl(tokenstream* ts);
char* ast_to_kindstr(astree* ast);

// codegen.c
void codegen(map* varmap, astree* ast);
void codegen_funcdecl(funcdecl fdecl);
// emit
void emit_global(char* name);
void emit_label(char* label);
void emit_pop(char* s);
void emit_return();
void emit_prologue(int localsize);
void emit_epilogue();

#endif
