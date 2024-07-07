#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdarg.h>

typedef enum {
  TK_RESERVED, // 符号
  TK_NUM, // 整数
  TK_EOF, // 结束标记
} TokenKind;

typedef struct Token Token;

// 标记结构
struct Token {
  TokenKind kind; // 标记类型
  Token *next; // 下一个Token，链状结构
  int val; // 标记为TK_NUM的数值
  char *str; // 标记对应的文本列
};

// 全局变量，正在处理的Token
Token *token;
// 全局变量，用户输入字符串
char *user_input;

// 错误位置
void error_at(char *loc, char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);

  int pos = loc - user_input;
  fprintf(stderr, "%s\n", user_input);
  fprintf(stderr, "%*s", pos, "");
  fprintf(stderr, "^ ");
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}

// 尝试消费一个符号
bool consume(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    return false;
  }
  // 成功消费，更新当前处理的token
  token = token->next;
  return true;
}

// 预期下一个token是符号op，否则报错
void expect(char op) {
  if (token->kind != TK_RESERVED || token->str[0] != op) {
    // 预期失败，执行编译报错
    error_at(token->str, "不是'%c'", op);
  }
  // 预期成功，更新当前处理的token
  token = token->next;
}

// 预期下一个token是数值，否则报错
int expect_number() {
  if (token->kind != TK_NUM) {
    // 预期失败，执行编译报错
    error_at(token->str, "不是数值");
  }
  // 预期成功，更新当前处理的token
  int re = token->val;
  token = token->next;
  return re;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 创建新的token，并连接
Token *new_token(TokenKind kind, Token *cur, char *str) {
  Token *token = calloc(1, sizeof(Token));
  token->kind = kind;
  token->str = str;
  cur->next = token;
  return token;
}

// 文本转标记
Token *tokenize(char *p) {
  // 栈上声明变量
  Token head;
  head.next = NULL;
  // 取栈地址，在作用域结束后，head的地址会被释放
  Token *cur = &head;

  // 直到读到零字符为止
  while (*p) {
    // 跳过空白字符
    if (isspace(*p)) {
      // 地址前进到下一个字节
      p++;
      continue;
    }
    // 识别当前字符是否是符号，包括+-*/()
    if (*p == '+' || *p == '-' || *p == '*' || *p == '/' || *p == '(' || *p == ')') {
      cur = new_token(TK_RESERVED, cur, p);
      p++;
      continue;
    }
    // 识别当前字符是否是数字
    if (isdigit(*p)) {
      
      cur = new_token(TK_NUM, cur, p);
      // 转换数字并将p赋值为结束位置
      cur->val = strtol(p, &p, 10);
      continue;
    }
    error_at(p, "标记解析失败");
  }

  new_token(TK_EOF, cur, p);
  return head.next;
}

// int main(int argc, char **argv) {
//   if (argc != 2) {
//     error("参数错误");
//     return 1;
//   }
//   // 解析token
//   token = tokenize(argv[1]);
//   printf(".intel_syntax noprefix\n");
//   printf(".global main\n");
//   printf("main:\n");

//   printf("  mov rax, %d\n", expect_number());
//   while (!at_eof()) {
//     if (consume('+')) {
//       printf("  add rax, %d\n", expect_number());
//       continue;
//     }
//     if (consume('-')) {
//       printf("  sub rax, %d\n", expect_number());
//       continue;
//     }
//     error("错误序列");
//   }
//   printf("  ret\n");
//   return 0;
// }