char [A-Za-z_.]
digit [0-9]
%{

#include <stdio.h>
#include <stdlib.h>
#include <alloca.h>
#include "demux.h"

#define STRING 1
#define EQUAL 2
#define NOTEQUAL 3
#define LEFT 4
#define RIGHT 5

%}

%%
{char}({char}|{digit})* return STRING;
"=" return EQUAL;
"!=" return NOTEQUAL;
"{" return LEFT;
"}" return RIGHT;
.|\n ;

%%

extern int optind;
extern char *optarg;

int yywrap()
{
  return 1;
}

void dump_rule(rule *r)
{
  cond *c;
  fprintf(stdout, "%s %s\n", r->name, r->ext);
  c = r->conds;
  while (c)
    {
      printf("    %d %d %d\n", c->left, c->operator, c->right);
      c = c->next;
    }
}

rule *parse_rules(char *f_name)
{
  rule *new_rule, *rules = NULL;
  int tmp;

  if (f_name && !(yyin = fopen(f_name, "r")))
    {
      fprintf(stderr, "Cannot open file %s.\n", optarg);
      exit(1);
    }
  while ((tmp = yylex()))
    {
      new_rule = malloc(sizeof(rule));
      new_rule->next = rules;
      new_rule->conds = NULL;
      new_rule->name = malloc(yyleng + 1);
      strcpy(new_rule->name, yytext);
      tmp = yylex();
      new_rule->ext = malloc(yyleng + 1);
      strcpy(new_rule->ext, yytext);
      tmp = yylex();
      while (tmp = yylex() && yytext[0] != '}')
	{
	  int op, right;
	  char txt[64];
	  strcpy(txt, yytext);
	  op = yyleng + 1;
	  tmp = yylex();
	  strcpy(txt + op, yytext);
	  right = op + yyleng + 1;
	  tmp = yylex();
	  strcpy(txt + right, yytext);
	  new_rule->conds = AddCondition(new_rule->conds, txt, txt + op, txt + right);
	}
      dump_rule(new_rule);
      rules = new_rule;
    }
  return rules;
}
