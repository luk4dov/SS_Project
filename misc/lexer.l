%{
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parser.tab.h"

%}

%option noyywrap
%option nounput
%option noinput

DIGIT       [0-9]
HEXDIGIT    [0-9A-Fa-f]
LETTER      [a-zA-Z_]
ALPHANUM    [a-zA-Z0-9_]

%%

\#.*$    { return NEWLINE; }

\.extern            { return EXTERN;  }
\.global            { return GLOBAL;  }
\.section           { return SECTION; }
\.word              { return WORD;    }
\.end               { return END;     }
\.ascii             { return ASCII;   }
\.equ               { return EQU;     }
\.weak              { return WEAK;    }

halt                { return HALT;  } 
int                 { return INT;   } 
iret                { return IRET;  } 
call                { return CALL;  } 
ret                 { return RET;   } 
jmp                 { return JMP;   } 
beq                 { return BEQ;   } 
bne                 { return BNE;   } 
bgt                 { return BGT;   } 
push                { return PUSH;  } 
pop                 { return POP;   } 
xchg                { return XCHG;  } 
add                 { return ADD;   } 
sub                 { return SUB;   } 
mul                 { return MUL;   } 
div                 { return DIV;   }
not                 { return NOT;   } 
and                 { return AND;   } 
or                  { return OR;    } 
xor                 { return XOR;   }
shl                 { return SHL;   }
shr                 { return SHR;   } 
ld                  { return LD;    } 
st                  { return ST;    } 
csrrd               { return CSRRD; }
csrwr               { return CSRWR; }


%r0                 { yylval.regn = 0; return REGISTER;  }
%r1                 { yylval.regn = 1; return REGISTER;  }
%r2                 { yylval.regn = 2; return REGISTER;  }
%r3                 { yylval.regn = 3; return REGISTER;  }
%r4                 { yylval.regn = 4; return REGISTER;  }
%r5                 { yylval.regn = 5; return REGISTER;  }
%r6                 { yylval.regn = 6; return REGISTER;  }
%r7                 { yylval.regn = 7; return REGISTER;  }
%r8                 { yylval.regn = 8; return REGISTER;  }
%r9                 { yylval.regn = 9; return REGISTER;  }
%r10                { yylval.regn = 10; return REGISTER; }
%r11                { yylval.regn = 11; return REGISTER; }
%r12                { yylval.regn = 12; return REGISTER; }
%r13                { yylval.regn = 13; return REGISTER; }
%r14                { yylval.regn = 14; return REGISTER; }
%r15                { yylval.regn = 15; return REGISTER; }
%sp                 { yylval.regn = 14; return REGISTER; }
%pc                 { yylval.regn = 15; return REGISTER; }

%cause              { yylval.str = strdup(yytext); return CSR; }
%handler            { yylval.str = strdup(yytext); return CSR; }
%status             { yylval.str = strdup(yytext); return CSR; }

0x{HEXDIGIT}+         { yylval.num = strtol(yytext + 2, NULL, 16); return NUMBER; }
{DIGIT}+              {         yylval.num = atoi(yytext); return NUMBER;         }

{LETTER}{ALPHANUM}* { yylval.str = strdup(yytext); return IDENTIFIER; }

\"[^\"]*\" {
        yylval.str = strdup(yytext + 1); 
        yylval.str[strlen(yylval.str)-1] = '\0';
        return STRING_LITERAL;
}

\[      { return OBRACKET; }
\]      { return CBRACKET; }
\+      { return PLUS; }
\-      { return MINUS; }

\{      { return '{'; }
\}      { return '}'; }


,       {  return COMMA;  }
:       {  return COLON;  }
\$       {  return DOLLAR; }

[ \t]+              { /* ignore whitespace */     }
\n                  { yylineno++; return NEWLINE; }


        /* Error */
.   { printf("Unexpected character: %s\n", yytext); }

%%

int yylex(void);