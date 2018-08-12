#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "sncc.h"

bool eq_ident(token* id, char* s) {
  return id != NULL && id->kind == TOKEN_IDENT && strcmp(id->ident, s) == 0;
}

//
// ast
//

astree* new_ast(astkind kind) {
  astree* ast = (astree*)malloc(sizeof(astree));
  ast->kind = kind;
  ast->typ = NULL;
  return ast;
}

astree* new_ast_prefix(astkind kind, astree* value) {
  astree* ast = new_ast(kind);
  ast->value = value;
  return ast;
}

astree* new_ast_infix(astkind kind, astree* left, astree* right) {
  astree* ast = new_ast(kind);
  ast->left = left;
  ast->right = right;
  return ast;
}

astree* new_ast_intlit(int x) {
  astree* ast = new_ast(AST_INTLIT);
  ast->intval = x;
  return ast;
}

astree* new_ast_ident(char* id) {
  astree* ast = new_ast(AST_IDENT);
  ast->ident = id;
  return ast;
}

char* ast_to_kindstr(astree* ast) {
  if (ast->kind == AST_ADD) {
    return "AST_ADD";
  } else if (ast->kind == AST_SUB) {
    return "AST_SUB";
  } else if (ast->kind == AST_MUL) {
    return "AST_MUL";
  } else if (ast->kind == AST_DIV) {
    return "AST_DIV";
  } else if (ast->kind == AST_MINUS) {
    return "AST_MINUS";
  } else if (ast->kind == AST_LESSER) {
    return "AST_LESSER";
  } else if (ast->kind == AST_ASSIGN) {
    return "AST_ASSIGN";
  } else if (ast->kind == AST_ADDR) {
    return "AST_ADDR";
  } else if (ast->kind == AST_DEREF) {
    return "AST_DEREF";
  } else if (ast->kind == AST_VARDECL) {
    return "AST_VARDECL";
  } else if (ast->kind == AST_CALL) {
    return "AST_CALL";
  } else if (ast->kind == AST_STATEMENT) {
    return "AST_STATEMENT";
  } else if (ast->kind == AST_INTLIT) {
    return "AST_INTLIT";
  } else if (ast->kind == AST_IDENT) {
    return "AST_IDENT";
  } else if (ast->kind == AST_IF) {
    return "AST_IF";
  } else if (ast->kind == AST_WHILE) {
    return "AST_WHILE";
  } else {
    assert(0);
  }
}

//
// typenode
//

typenode* new_typenode(typekind kind) {
  typenode* tn = malloc(sizeof(typenode));
  tn->kind = kind;
  tn->ptrof = NULL;
  return tn;
}

typenode* new_ptrnode(typenode* typ) {
  typenode* tn = new_typenode(TYPE_PTR);
  tn->ptrof = typ;
  return tn;
}

//
// tokenstream
//

tokenstream* new_tokenstream(vector* tokens) {
  tokenstream* ts = (tokenstream*)malloc(sizeof(tokenstream));
  ts->tokens = tokens;
  ts->pos = 0;
  return ts;
}
token* get_token(tokenstream* ts) {
  if (ts->pos >= ts->tokens->len) {
    return NULL;
  }
  return (token*)vector_get(ts->tokens, ts->pos);
}
void next_token(tokenstream* ts) {
  ts->pos++;
}

//
// parser
//

astree* parser_top(tokenstream* ts);

astree* factor(tokenstream* ts) {
  token* t = get_token(ts);
  if (t == NULL) error("require more token");

  if (t->kind == TOKEN_LPAREN) {
    next_token(ts);
    astree* ret = expression(ts);
    token* t = get_token(ts);
    if (t == NULL || t->kind != TOKEN_RPAREN) error("\"(\" should be ended by \")\".");
    next_token(ts);
    return ret;
  } else if (t->kind == TOKEN_INTLIT) {
    next_token(ts);
    return new_ast_intlit(t->intval);
  } else if (t->kind == TOKEN_IDENT) {
    next_token(ts);
    return new_ast_ident(t->ident);
  } else {
    return expression(ts);
  }
}

astree* callexpr(tokenstream* ts) {
  astree* call = factor(ts);
  token* lparen = get_token(ts);
  if (lparen == NULL) return call;
  if (lparen->kind != TOKEN_LPAREN) return call;
  next_token(ts);

  vector* args = new_vector();
  if (get_token(ts)-> kind != TOKEN_RPAREN) {
    vector_push(args, expression(ts));
    for (;;) {
      if (get_token(ts)->kind == TOKEN_RPAREN) break;
      token* t = get_token(ts); next_token(ts);
      if (t->kind != TOKEN_COMMA) error("expected comma by call, but got %s", token_to_str(t));
      vector_push(args, expression(ts));
    }
  }

  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("function call expected )rparen.");

  astree* ast = new_ast(AST_CALL);
  ast->call = call;
  ast->arguments = args;
  return ast;
}

// infix parser for lesser, greater, and lesser/greater equal.
astree* infix_lge(tokenstream* ts) {
  astree* left = callexpr(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_LESSER) {
      next_token(ts);
      astree* right = callexpr(ts);
      left = new_ast_infix(AST_LESSER, left, right);
    } else if (t->kind == TOKEN_GREATER) {
      next_token(ts);
      astree* right = callexpr(ts);
      left = new_ast_infix(AST_LESSER, right, left);
    } else {
      break;
    }
  }
  return left;
}

astree* infix_muldiv(tokenstream* ts) {
  astree* left = infix_lge(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_MUL) {
      next_token(ts);
      astree* right = infix_lge(ts);
      left = new_ast_infix(AST_MUL, left, right);
    } else if (t->kind == TOKEN_DIV) {
      next_token(ts);
      astree* right = infix_lge(ts);
      left = new_ast_infix(AST_DIV, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* infix_addsub(tokenstream* ts) {
  astree* left = infix_muldiv(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_ADD) {
      next_token(ts);
      astree* right = infix_muldiv(ts);
      left = new_ast_infix(AST_ADD, left, right);
    } else if (t->kind == TOKEN_SUB) {
      next_token(ts);
      astree* right = infix_muldiv(ts);
      left = new_ast_infix(AST_SUB, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* prefix_addsub_deref(tokenstream* ts) {
  token* t = get_token(ts);
  if (t != NULL && t->kind == TOKEN_SUB) {
    next_token(ts);
    astree* value = infix_addsub(ts);
    return new_ast_prefix(AST_MINUS, value);
  } else if (t != NULL && t->kind == TOKEN_MUL) {
    next_token(ts);
    astree* value = infix_addsub(ts);
    return new_ast_prefix(AST_DEREF, value);
  } else if (t != NULL && t->kind == TOKEN_AND) {
    next_token(ts);
    astree* value = infix_addsub(ts);
    return new_ast_prefix(AST_ADDR, value);
  } else {
    return infix_addsub(ts);
  }
}

astree* infix_assign(tokenstream* ts) {
  astree* left = prefix_addsub_deref(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_ASSIGN) {
      next_token(ts);
      astree* right = prefix_addsub_deref(ts);
      left = new_ast_infix(AST_ASSIGN, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* expression(tokenstream* ts) {
  return infix_assign(ts);
}

astree* parse_if(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "if")) return NULL;
  next_token(ts);

  astree* ast = new_ast(AST_IF);

  token* lparen = get_token(ts); next_token(ts);
  if (lparen == NULL || lparen->kind != TOKEN_LPAREN) error("if expected (lparen.");
  ast->ifcond = expression(ts);
  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("if expected )rparen.");
  ast->ifbody = parse_compound(ts);

  ast->elsebody = NULL;
  if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_IDENT && strcmp(get_token(ts)->ident, "else") == 0) {
    next_token(ts);
    ast->elsebody = parse_compound(ts);
  }

  return ast;
}

astree* parse_while(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "while")) return NULL;
  next_token(ts);

  astree* ast = new_ast(AST_WHILE);

  token* lparen = get_token(ts); next_token(ts);
  if (lparen == NULL || lparen->kind != TOKEN_LPAREN) error("while expected (lparen.");
  ast->whilecond = expression(ts);
  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("while expected )rparen.");
  ast->whilebody = parse_compound(ts);

  return ast;
}

astree* parse_for(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "for")) return NULL;
  next_token(ts);

  token* lparen = get_token(ts); next_token(ts);
  if (lparen == NULL || lparen->kind != TOKEN_LPAREN) error("for expected (lparen.");

  token* semicolon;

  astree* forinit = expression(ts);
  semicolon = get_token(ts); next_token(ts);
  if (semicolon == NULL || semicolon->kind != TOKEN_SEMICOLON) error("for expected ;semicolon.");

  astree* forcond = expression(ts);
  semicolon = get_token(ts); next_token(ts);
  if (semicolon == NULL || semicolon->kind != TOKEN_SEMICOLON) error("for expected ;semicolon.");

  astree* fornext = expression(ts);

  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("for expected )rparen.");

  astree* forbody = parse_compound(ts);
  astree* bodyast = new_ast(AST_STATEMENT);
  bodyast->stmt = new_vector();
  vector_push(bodyast->stmt, (void*)forbody);
  vector_push(bodyast->stmt, (void*)fornext);

  astree* whileast = new_ast(AST_WHILE);
  whileast->whilecond = forcond;
  whileast->whilebody = bodyast;

  astree* ast = new_ast(AST_STATEMENT);
  ast->stmt = new_vector();
  vector_push(ast->stmt, (void*)forinit);
  vector_push(ast->stmt, (void*)whileast);

  return ast;
}

astree* parse_vardecl(tokenstream* ts) {
  paramtype* pt = parse_paramtype(ts);
  if (pt == NULL) return NULL;
  astree* ast = new_ast(AST_VARDECL);
  ast->vardecl = pt;
  return ast;
}

vector* parse_statement(tokenstream* ts) {
  vector* v = new_vector();
  for (;;) {
    if (get_token(ts) == NULL || get_token(ts)->kind == TOKEN_RBRACKET) break;
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_SEMICOLON) {
      next_token(ts);
      continue;
    }
    astree* ifast = parse_if(ts);
    if (ifast != NULL) {
      vector_push(v, (void*)ifast);
      continue;
    }
    astree* whileast = parse_while(ts);
    if (whileast != NULL) {
      vector_push(v, (void*)whileast);
      continue;
    }
    astree* forast = parse_for(ts);
    if (forast != NULL) {
      vector_push(v, (void*)forast);
      continue;
    }
    astree* declast = parse_vardecl(ts);
    if (declast != NULL) {
      vector_push(v, (void*)declast);
      continue;
    }

    vector_push(v, (void*)parse_compound(ts));
  }
  return v;
}

astree* parse_compound(tokenstream* ts) {
  if (get_token(ts) == NULL) return NULL;
  if (get_token(ts)->kind == TOKEN_LBRACKET) {
    next_token(ts);
    astree* ast = new_ast(AST_STATEMENT);
    ast->stmt = parse_statement(ts);
    if (get_token(ts) == NULL || get_token(ts)->kind != TOKEN_RBRACKET) error("expect }lbracket in compound statement.");
    next_token(ts);
    return ast;
  } else {
    astree* ast = expression(ts);
    if (get_token(ts) == NULL || get_token(ts)->kind != TOKEN_SEMICOLON) error("expect ;semicolon as end of statement");
    next_token(ts);
    return ast;
  }
}

//
// funcdecl
//

paramtype* parse_paramtype(tokenstream* ts) {
  if (!eq_ident(get_token(ts), "int")) return NULL;
  next_token(ts);
  typenode* tn = new_typenode(TYPE_INT);
  for (;;) {
    if (get_token(ts) != NULL && get_token(ts)->kind == TOKEN_MUL) {
      next_token(ts);
      tn = new_ptrnode(tn);
    } else {
      break;
    }
  }
  token* t = get_token(ts); next_token(ts);
  if (t->kind != TOKEN_IDENT) error("expected identifier in parameter.");
  paramtype* pt = malloc(sizeof(paramtype));
  pt->typ = tn;
  pt->name = t->ident;
  return pt;
}

vector* parse_paramtype_list(tokenstream* ts) {
  vector* ptlist = new_vector();
  for (;;) {
    if (get_token(ts) == NULL) break;
    if (get_token(ts)->kind != TOKEN_IDENT) break;
    paramtype* pt = parse_paramtype(ts);
    if (pt == NULL) error("expect type declaration in parameters.");
    vector_push(ptlist, (void*)pt);
    token* t = get_token(ts);
    if (t == NULL) error("require more token.");
    if (t->kind != TOKEN_COMMA) break;
    next_token(ts);
  }
  return ptlist;
}

funcdecl parse_funcdecl(tokenstream* ts) {
  funcdecl fdecl;
  fdecl.fdecl = parse_paramtype(ts);
  token* lparen = get_token(ts); next_token(ts);
  if (lparen == NULL || lparen->kind != TOKEN_LPAREN) error("function decl expect \"(\", but got %s", token_to_str(lparen));
  fdecl.argdecls = parse_paramtype_list(ts);
  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("function decl expect \")\", but got %s", token_to_str(rparen));

  token* lbracket = get_token(ts); next_token(ts);
  if (lbracket == NULL || lbracket->kind != TOKEN_LBRACKET) error("function decl expect \"{\", but got %s", token_to_str(lbracket));
  fdecl.body = parse_statement(ts);
  token* rbracket = get_token(ts); next_token(ts);
  if (rbracket == NULL || rbracket->kind != TOKEN_RBRACKET) error("function decl expect \"}\", but got %s", token_to_str(rbracket));

  return fdecl;
}
