# 类型检查和中间代码生成

相比于语法分析，Ast.h中每个类多了两个成员函数，typeCheck 和 genCode，分别用于类型检查和代码生成

## 在哪里被调用？

类型检查最简单的实现方式是在建立语法树的过程中进行相应的识别和处理，也可以在建树完成
后，自底向上遍历语法树进行类型检查。

main函数中调用ast.typeCheck(), 这是typecheck函数递归的开始，通过递归遍历整个语法树

## 类型检查
## 中间代码生成
genCode 函数用于生成中间代码或目标代码。代码生成是编译过程的最后一个阶段，目的是将抽象语法树（AST）转换为中间表示（IR）或机器代码。

在编译器设计中，**编译单元**和**基本块**是两个重要的概念，它们在中间表示（Intermediate Representation，IR）和代码生成过程中起着关键作用。

### 编译单元（Unit）
编译单元通常表示源代码文件或模块的一个独立部分。它是编译器处理的基本单位。每个编译单元包含一个或多个函数、全局变量和其他声明。编译器会将每个编译单元单独编译成目标代码或中间表示。

在 IRBuilder 类中，Unit 类表示当前的编译单元。它可能包含以下内容：

+ 全局变量
+ 函数列表
+ 类型信息
+ 其他编译相关的信息


### 基本块（BasicBlock）
基本块是控制流图（Control Flow Graph，CFG）中的一个节点。它是一个直线代码序列，没有分支（除了最后一条指令）和汇合（除了第一条指令）。基本块的特点是：

+ 进入基本块的控制流只能从基本块的第一条指令开始。
+ 离开基本块的控制流只能从基本块的最后一条指令结束。

基本块在优化和代码生成过程中非常重要，因为它们使得编译器可以更容易地分析和优化代码。



## 关系
在编译器设计中，`Unit`、Function、`BasicBlock`、Instruction和 Operand是中间表示（Intermediate Representation，IR）中的关键概念，它们之间有着层次化的关系。以下是它们之间的关系和作用：

### 1. Unit

`Unit` 通常表示一个编译单元，它是编译器处理的基本单位。一个 `Unit` 包含多个 Function，每个 Function表示一个函数或方法。

- **作用**：表示整个编译单元，包含全局变量和函数列表。
- **包含**：多个 Function对象。

### 2. Function

Function表示一个函数或方法。一个 

Function包含多个 `BasicBlock`，每个 `BasicBlock` 表示一个基本块。

- **作用**：表示一个函数或方法，包含函数的参数、局部变量和基本块列表。
- **包含**：多个 `BasicBlock` 对象。

### 3. BasicBlock

`BasicBlock` 表示一个基本块。一个 `BasicBlock` 包含多个 Instruction，每个 Instruction表示一条指令。

- **作用**：表示一个基本块，是控制流图（CFG）中的一个节点，包含一系列没有分支和汇合的指令。
- **包含**：多个 Instruction 对象。

### 4. Instruction

Instruction表示一条指令。一个 Instruction包含多个 Operand，每个 Operand表示一个操作数。

- **作用**：表示一条指令，包含操作码和操作数。
- **包含**：多个Operand对象。

### 5. Operand

Operand表示一个操作数。一个 Operand可以被多个 Instruction使用。

- **作用**：表示指令的操作数，可以是立即数、变量、寄存器等。
- **包含**：一个定义它的 

Instruction和使用它的 Instruction列表。

### 关系图

以下是它们之间关系的层次图：

```
Unit
 ├── Function
 │    ├── BasicBlock
 │    │    ├── Instruction
 │    │    │    ├── Operand
 │    │    │    └── Operand
 │    │    ├── Instruction
 │    │    │    ├── Operand
 │    │    │    └── Operand
 │    │    └── ...
 │    └── BasicBlock
 └── Function
```

## 中间代码生成中，函数定义形参是如何处理的

在gencode时，形参应被翻译为一条一条的alloca语句

形参是一条一条的DeclStmt语句，在语法分析的时候被链接为一个链表

在进行gencode时，通过FuncDef里的params遍历链表，获得所有参数。


## 全局变量怎么处理

### 要求
1. 全局变量声明中指定的初值表达式必须是常量表达式
2. 未显式初始化的局部变量，其值是不确定的；而未显式初始化的全局变量，
其（元素）值均被初始化为 0 或 0.0


## 问题

全局变量初始值如何设置？？？

## float中间代码
```
float a=1.2;

%1 = alloca float, align 4
store float 0x3FF3333340000000, float* %1, align 4
```
0x3FF3333340000000 为 1.2 的 IEEE 754 双精度浮点数表示。


1. 加减乘除应为浮点数加减乘除（fadd,fsub,fmul,fdiv）
2. float+int结果应为float


```
int a=1;
float s=a;

    %1 = alloca i32, align 4
    %2 = alloca float, align 4
    store i32 1, i32* %1, align 4
    %3 = load i32, i32* %1, align 4
    %4 = sitofp i32 %3 to float
    store float %4, float* %2, align 4
```


