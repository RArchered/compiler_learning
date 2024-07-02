#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

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
  char *str; // 标记对于的文本列
};

// 全局变量，正在处理的Token
Token *token;

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
    error("不是'%c'", op);
  }
  // 预期成功，更新当前处理的token
  token = token->next;
}

// 预期下一个token是数值，否则报错
int expect_number() {
  if (token->kind != TK_NUM) {
    // 预期失败，执行编译报错
    error("不是数值");
  }
  // 预期成功，更新当前处理的token
  int re = token->val;
  token = token->next;
  return re;
}

bool at_eof() {
  return token->kind == TK_EOF;
}

// 错误打印，退出程序
void error(char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  vfprintf(stderr, fmt, ap);
  fprintf(stderr, "\n");
  exit(1);
}