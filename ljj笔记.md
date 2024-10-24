
## 函数定义 (FunctionDef)：

在函数定义中，参数列表中的每个参数都是一个标识符（Id），表示参数的名称和类型。
因此，参数列表的类型是 std::vector<Id*>，其中每个 Id* 表示一个参数。

函数定义需要干什么：

函数定义时需要识别形参列表，每一个形参都应是一个标识符Id，并且这些形参需要加入一个新的符号表symboltable，变成符号表项，有自己独有的作用域。

每次调用symboltable的构造函数构造新的符号表时，在构造函数中，都会传入上一个符号表，并且将这个符号表的prev指向上一个符号表，将这个符号表的level相对上一个 **+1** ，以此达到符号表层级逐渐增加的效果。

## 函数调用 (FuncCall)：

在函数调用中，参数列表中的每个参数是一个表达式（ExprNode），表示实际传递给函数的值。
因此，参数列表的类型是 std::vector<ExprNode*>，其中每个 ExprNode* 表示一个实际参数表达式。

# Node
Node 类是抽象语法树（AST）中的基本节点类。它是所有具体语法节点的基类

Node 类定义了一个纯虚函数 output(int level)，要求所有派生类实现该函数。这确保了所有具体节点类都具有输出自身信息的能力。

Node 类包含一个静态计数器 counter（用于生成唯一的序号） 和一个实例变量 seq（int 存储节点的唯一序号），用于为每个节点分配唯一的序号。这在调试和输出时非常有用，可以唯一标识每个节点。
## ExprNode
ExprNode 类是所有具体表达式节点的基类，提供了与表达式相关的基本功能。

ExprNode 类包含一个指向符号表项的指针 symbolEntry，用于存储与该表达式相关的符号信息。

SymbolEntry *symbolEntry：指向符号表项的指针，存储与该表达式相关的符号信息。符号表项包含了与变量、常量、函数等相关的详细信息。
### BinaryExpr
BinaryExpr 类表示二元表达式节点


### Constant
常量节点

### Id
标识符节点

### FuncCall
函数调用节点

包括一个实参vector、一个函数名

```cpp
Id *func;   // 函数名
std::vector<ExprNode*> args;    // 函数参数（注意类型应为 表达式 ）
```

## StmtNode


### ExprStmt
表达式语句类，成员为一个表达式节点，ExprNode *expr;  表示该语句中的表达式

### CompoundStmt
复合语句节点

在编程语言中，**复合语句是由多个语句组成的语句块，通常用大括号 {} 包围**。复合语句允许将多个语句组合在一起，使它们在语法上被视为一个单独的语句。

### SeqNode
序列语句节点

在编程语言中，**序列语句是由多个语句按顺序排列而成的语句块**。序列语句允许将多个语句按顺序执行，使它们在语法上被视为一个单独的语句。

### DeclStmt
声明语句类

### IfStmt


### IfElseStmt

### ReturnStmt

### AssignStmt
赋值语句

ExprNode *lval; //左值  
ExprNode *expr; //右值
### FunctionDef
函数定义语句

注意此处的参数应该为形参， std::vector<Id*> params; // 参数列表  



### WhileStmt

### EmptyStmt