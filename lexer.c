#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include <string.h>
#include "sncc.h"

FILE* input;

void init_lexer() {
  input = stdin;
}

token* new_token(tokenkind kind) {
  token* t = (token*)malloc(sizeof(token));
  t->kind = kind;
  return t;
}

token* new_intlit(int x) {
  token* t = new_token(TOKEN_INTLIT);
  t->intval = x;
  return t;
}
token* new_strlit(char* s) {
  token* t = new_token(TOKEN_STRLIT);
  t->strval = s;
  return t;
}
token* new_ident(char* s) {
  token* t = new_token(TOKEN_IDENT);
  t->ident = strdup(s);
  return t;
}

char* int_to_str(int x) {
  char* buf = (char*)malloc(6+1);
  snprintf(buf, 6, "%d", x);
  return buf;
}
bool isident(char c) {
  return isalpha(c) || c == '_';
}
bool isidenttail(char c) {
  return isident(c) || isdigit(c);
}

char* token_to_kindstr(token* token) {
  if (token->kind == TOKEN_ADD) {
    return "TOKEN_ADD";
  } else if (token->kind == TOKEN_SUB) {
    return "TOKEN_SUB";
  } else if (token->kind == TOKEN_MUL) {
    return "TOKEN_MUL";
  } else if (token->kind == TOKEN_DIV) {
    return "TOKEN_DIV";
  } else if (token->kind == TOKEN_LESSER) {
    return "TOKEN_LESSER";
  } else if (token->kind == TOKEN_GREATER) {
    return "TOKEN_GREATER";
  } else if (token->kind == TOKEN_LESSEREQ) {
    return "TOKEN_LESSEREQ";
  } else if (token->kind == TOKEN_GREATEREQ) {
    return "TOKEN_GREATEREQ";
  } else if (token->kind == TOKEN_ASSIGN) {
    return "TOKEN_ASSIGN";
  } else if (token->kind == TOKEN_ADDASSIGN) {
    return "TOKEN_ADDASSIGN";
  } else if (token->kind == TOKEN_MULASSIGN) {
    return "TOKEN_MULASSIGN";
  } else if (token->kind == TOKEN_LAND) {
    return "TOKEN_LAND";
  } else if (token->kind == TOKEN_LOR) {
    return "TOKEN_LOR";
  } else if (token->kind == TOKEN_INC) {
    return "TOKEN_INC";
  } else if (token->kind == TOKEN_EQ) {
    return "TOKEN_EQ";
  } else if (token->kind == TOKEN_NOT) {
    return "TOKEN_NOT";
  } else if (token->kind == TOKEN_DOT) {
    return "TOKEN_DOT";
  } else if (token->kind == TOKEN_ALLOW) {
    return "TOKEN_ALLOW";
  } else if (token->kind == TOKEN_AND) {
    return "TOKEN_AND";
  } else if (token->kind == TOKEN_INTLIT) {
    return "TOKEN_INTLIT";
  } else if (token->kind == TOKEN_STRLIT) {
    return "TOKEN_STRLIT";
  } else if (token->kind == TOKEN_LPAREN) {
    return "TOKEN_LPAREN";
  } else if (token->kind == TOKEN_RPAREN) {
    return "TOKEN_RPAREN";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "TOKEN_LBRACKET";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "TOKEN_RBRACKET";
  } else if (token->kind == TOKEN_LBRACE) {
    return "TOKEN_LBRACE";
  } else if (token->kind == TOKEN_RBRACE) {
    return "TOKEN_RBRACE";
  } else if (token->kind == TOKEN_COMMA) {
    return "TOKEN_COMMA";
  } else if (token->kind == TOKEN_SEMICOLON) {
    return "TOKEN_SEMICOLON";
  } else if (token->kind == TOKEN_IDENT) {
    return "TOKEN_IDENT";
  } else {
    assert(0);
  }
}
char* token_to_str(token* token) {
  if (token->kind == TOKEN_ADD) {
    return "+";
  } else if (token->kind == TOKEN_SUB) {
    return "-";
  } else if (token->kind == TOKEN_MUL) {
    return "*";
  } else if (token->kind == TOKEN_DIV) {
    return "/";
  } else if (token->kind == TOKEN_LESSER) {
    return "<";
  } else if (token->kind == TOKEN_GREATER) {
    return ">";
  } else if (token->kind == TOKEN_LESSEREQ) {
    return "<=";
  } else if (token->kind == TOKEN_GREATEREQ) {
    return ">=";
  } else if (token->kind == TOKEN_ASSIGN) {
    return "=";
  } else if (token->kind == TOKEN_ADDASSIGN) {
    return "+=";
  } else if (token->kind == TOKEN_MULASSIGN) {
    return "*=";
  } else if (token->kind == TOKEN_LAND) {
    return "&&";
  } else if (token->kind == TOKEN_LOR) {
    return "||";
  } else if (token->kind == TOKEN_INC) {
    return "++";
  } else if (token->kind == TOKEN_EQ) {
    return "==";
  } else if (token->kind == TOKEN_NOT) {
    return "!";
  } else if (token->kind == TOKEN_DOT) {
    return ".";
  } else if (token->kind == TOKEN_ALLOW) {
    return "->";
  } else if (token->kind == TOKEN_AND) {
    return "&";
  } else if (token->kind == TOKEN_LPAREN) {
    return "(";
  } else if (token->kind == TOKEN_RPAREN) {
    return ")";
  } else if (token->kind == TOKEN_LBRACE) {
    return "{";
  } else if (token->kind == TOKEN_RBRACE) {
    return "}";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "[";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "]";
  } else if (token->kind == TOKEN_COMMA) {
    return ",";
  } else if (token->kind == TOKEN_SEMICOLON) {
    return ";";
  } else if (token->kind == TOKEN_INTLIT) {
    return int_to_str(token->intval);
  } else if (token->kind == TOKEN_STRLIT) {
    return token->strval;
  } else if (token->kind == TOKEN_IDENT) {
    return token->ident;
  } else {
    assert(0);
  }
}

vector* lexer() {
  vector* tokenss = new_vector();

  for (;;) {
    char c = getc(input);
    if (c == EOF) {
      break;
    } else if (isdigit(c)) { // int literal
      char digitbuf[256] = {};
      digitbuf[0] = c;
      for (int i=1; ; i++) {
        assert(i < 256);
        char nc = getc(input);
        if (!isdigit(nc)) {
          ungetc(nc, input);
          vector_push(tokenss, new_intlit(atoi(digitbuf)));
          break;
        }
        digitbuf[i] = nc;
      }
    } else if (c == '#') { // preprocessor
      char identbuf[256] = {};
      for (int i=0; ; i++) {
        if (i >= 256) assert(false);
        char nc = getc(input);
        if (!isidenttail(nc)) {
          ungetc(nc, input);
          break;
        }
        identbuf[i] = nc;
      }
      if (strcmp(identbuf, "include") == 0) {
        for (;;) {
          char nc = getc(input);
          if (nc != ' ') {
            ungetc(nc, input);
            break;
          }
        }
        if (getc(stdin) != '"') error("include expect string literal.");
        char strbuf[1024] = {};
        for (int i=0; ; i++) {
          assert(i < 1024);
          char nc = getc(input);
          if (nc == '"') break;
          if (nc == EOF) error("expect end of string literal.");
          strbuf[i] = nc;
        }
        fprintf(stderr, "%s", strbuf);
        FILE* tmpf = input;
        input = fopen(strbuf, "r");
        vector* includetokens = lexer();
        for (int i=0; i<includetokens->len; i++) {
          vector_push(tokenss, vector_get(includetokens, i));
        }
        fclose(input);
        input = tmpf;
      } else {
        error("unsupported %s preprocessor.", identbuf);
      }
    } else if (c == '"') { // string literal
      char strbuf[1024] = {};
      for (int i=0; ; i++) {
        assert(i < 1024);
        char nc = getc(input);
        if (nc == '"') break;
        if (nc == EOF) error("expect end of string literal.");
        strbuf[i] = nc;
      }
      vector_push(tokenss, new_strlit(strdup(strbuf)));
    } else if (c == '+') {
      char nc = getc(input);
      if (nc == '+') {
        vector_push(tokenss, new_token(TOKEN_INC));
      } else if (nc == '=') {
        vector_push(tokenss, new_token(TOKEN_ADDASSIGN));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_ADD));
      }
    } else if (c == '-') {
      char nc = getc(input);
      if (nc == '>') {
        vector_push(tokenss, new_token(TOKEN_ALLOW));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_SUB));
      }
    } else if (c == '*') {
      char nc = getc(input);
      if (nc == '=') {
        vector_push(tokenss, new_token(TOKEN_MULASSIGN));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_MUL));
      }
    } else if (c == '/') {
      char nc = getc(input);
      if (nc == '/') {
        for (;;) {
          nc = getc(input);
          if (nc == EOF || nc == '\n') break;
        }
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_DIV));
      }
    } else if (c == '<') {
      char nc = getc(input);
      if (nc == '=') {
        vector_push(tokenss, new_token(TOKEN_LESSEREQ));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_LESSER));
      }
    } else if (c == '>') {
      char nc = getc(input);
      if (nc == '=') {
        vector_push(tokenss, new_token(TOKEN_GREATEREQ));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_GREATER));
      }
    } else if (c == '.') {
      vector_push(tokenss, new_token(TOKEN_DOT));
    } else if (c == '|') {
      char nc = getc(input);
      if (nc == '|') {
        vector_push(tokenss, new_token(TOKEN_LOR));
      } else {
        error("unsupporte | token in currently.");
      }
    } else if (c == '&') {
      char nc = getc(input);
      if (nc == '&') {
        vector_push(tokenss, new_token(TOKEN_LAND));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_AND));
      }
    } else if (c == '=') {
      char nc = getc(input);
      if (nc == '=') {
        vector_push(tokenss, new_token(TOKEN_EQ));
      } else {
        ungetc(nc, input);
        vector_push(tokenss, new_token(TOKEN_ASSIGN));
      }
    } else if (c == '!') {
      vector_push(tokenss, new_token(TOKEN_NOT));
    } else if (c == '(') {
      vector_push(tokenss, new_token(TOKEN_LPAREN));
    } else if (c == ')') {
      vector_push(tokenss, new_token(TOKEN_RPAREN));
    } else if (c == '[') {
      vector_push(tokenss, new_token(TOKEN_LBRACKET));
    } else if (c == ']') {
      vector_push(tokenss, new_token(TOKEN_RBRACKET));
    } else if (c == '{') {
      vector_push(tokenss, new_token(TOKEN_LBRACE));
    } else if (c == '}') {
      vector_push(tokenss, new_token(TOKEN_RBRACE));
    } else if (c == ',') {
      vector_push(tokenss, new_token(TOKEN_COMMA));
    } else if (c == ';') {
      vector_push(tokenss, new_token(TOKEN_SEMICOLON));
    } else if (c == ' ') {
      continue;
    } else if (c == '\n') {
      continue;
    } else if (isident(c)) { // identifier
      char identbuf[256] = {};
      identbuf[0] = c;
      for (int i=1; ; i++) {
        if (i >= 256) error("long length (>256) identifer is unsupported in currently."); // FIXME: long length identifer support.
        char nc = getc(input);
        if (!isidenttail(nc)) {
          ungetc(nc, input);
          vector_push(tokenss, new_ident(identbuf));
          break;
        }
        identbuf[i] = nc;
      }
    } else {
      error("unexpected token %c.", c);
    }
  }
  return tokenss;
}
