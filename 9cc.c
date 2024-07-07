#include <stdlib.h>

#include "tokenize.c"


// 语法树的基本节点的类型
typedef enum {
  ND_ADD, // +
  ND_SUB, // -
  ND_MUL, // *
  ND_DIV, // /
  ND_NUM, // 整数
} NodeKind;

typedef struct Node Node;

// 语法树的基本节点
struct Node {
  NodeKind kind; //节点的类型
  Node *lhs; // 左节点类型
  Node *rhs; // 右节点类型
  int val; // 节点为整数时的值
};

Node *expr();
Node *mul();
Node *term();

// 创建新节点
Node *new_node(NodeKind kind, Node *lhs, Node *rhs) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = kind;
  node->lhs = lhs;
  node->rhs = rhs;
  return node;
}

Node *new_node_num(int val) {
  Node *node = calloc(1, sizeof(Node));
  node->kind = ND_NUM;
  node->val = val;
  return node;
}

/*
 * 实现文法中的运算子，运算子通过消耗token来建立语法树
 * expr = mul ("+"mul | "-"mul)*
 * mul = term ("*"term | "/"term)*
 * term = num | "("expr")"
 */

Node *expr() {
  Node *node = mul();
  // 遍历token，直到token消耗完毕
  for (;;) {
    if (consume('+')) {
      node = new_node(ND_ADD, node, mul());
    } else if (consume('-')) {
      node = new_node(ND_SUB, node, mul());
    } else {
      return node;
    }
  }
}

Node *mul() {
  Node *node = term();
  // 遍历token，知道周围的乘除法token被完全消耗
  for (;;) {
    if (consume('*')) {
      node = new_node(ND_MUL, node, term());
    } else if (consume('/')) {
      node = new_node(ND_DIV, node, term());
    } else {
      return node;
    }
  }
}

Node *term() {
  if(consume('(')) {
    Node *node = expr();
    expect(')');
    return node;
  }
  return new_node_num(expect_number());
}

void gen(Node *node) {
  // 递归终止节点，数字节点压栈
  if (node->kind == ND_NUM) {
    printf("  push %d\n", node->val);
    return;
  }

  // 左右分别入栈
  gen(node->lhs);
  gen(node->rhs);

  printf("  pop rdi\n");
  printf("  pop rax\n");

  switch (node->kind) {
    case ND_ADD:
      printf("  add rax, rdi\n");
      break;
    case ND_SUB:
      printf("  sub rax, rdi\n");
      break;
    case ND_MUL:
      printf("  imul rax, rdi\n");
      break;
    case ND_DIV:
      printf("  cqo\n");
      printf("  idiv rdi\n");
      break;  
  }
  // 结果压入堆栈
  printf("  push rax\n");
}

int main(int argc, char **argv) {
  if (argc != 2) {
    fprintf(stderr, "参数数量错误\n");
    return 1;
  }
  // 文本转token
  token = tokenize(argv[1]);
  Node *node = expr();

  // 输出前半部分
  printf(".intel_syntax noprefix\n");
  printf(".global main\n");
  printf("main:\n");

  // 生成抽象语法树指令
  gen(node);

  printf("  pop rax\n");
  printf("  ret\n");
  return 0;
}
