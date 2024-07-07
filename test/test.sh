#!/bin/bash
try() {
  expected="$1"
  input="$2"

  ./build/9cc "$input" > ./build/tmp.s
  cc -o ./build/tmp ./build/tmp.s
  ./build/tmp
  actual="$?"

  if [ "$actual" = "$expected" ]; then
    echo "$input => $actual"
  else
    echo "$input => $expected expected, but got $actual"
    exit 1
  fi  
}

# 测试用例
try 0 "0"
try 48 "(3+3)*8"
try 13 "(6-2)/2*16-(1+2)/1*4-7"

echo OK