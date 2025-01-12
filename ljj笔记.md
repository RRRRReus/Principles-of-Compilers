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

# 代码优化

## 死代码消除
1. 死代码消除（Dead Code Elimination）
1.1 算法思想
我们在死代码消除中希望去掉所有不活跃的变量。那么什么是不活跃呢？容易想到这意味着它定义的变量在接下来会被使用到。注意到，我们是在SSA阶段进行的这个优化，这意味着对于每个变量，它的def是它的每个use的必经节点。那么我们可以基于工作表算法写出伪代码：

while (存在某个没有使用点的变量v && 定值v的语句没有其他副作用) {
    删除定值v的这条语句
} 

1.2 需要维护的信息
我们使用HashMap<IRRegister> myMap来维护现有的变量，并使用WorkList。

同时，我们给出HashMap<IRRegister, HashMap<IRBaseInst>> useMap来记录所有变量的use，用HashMap<IRRegister, IRBaseInst> defMap来记录所有变量的use。

另外，我们注意到，函数的入参并不在我们的考量范围内（我们总不能消掉它们的def吧），所以我们需要用一个HashSet来记录当前函数的所有入参。

### 死代码消除（Dead Code Elimination）

#### 1.1 算法思想

死代码消除的目标是移除所有不活跃的变量和指令。不活跃的变量是指那些定义后从未被使用的变量。在静态单赋值（SSA）形式中，每个变量的定义（def）是其所有使用（use）的必经节点。因此，可以基于工作表算法来实现死代码消除。

伪代码如下：

```plaintext
while (存在某个没有使用点的变量v && 定值v的语句没有其他副作用) {
    删除定值v的这条语句
}
```

#### 1.2 需要维护的信息

为了实现上述算法，需要维护以下信息：

1. **变量的集合**：使用 `HashMap<IRRegister>` 来维护现有的变量。
2. **工作列表**：使用 `WorkList` 来存储需要处理的变量。
3. **变量的使用情况**：使用 `HashMap<IRRegister, HashMap<IRBaseInst>> useMap` 来记录所有变量的使用点。
4. **变量的定义情况**：使用 `HashMap<IRRegister, IRBaseInst> defMap` 来记录所有变量的定义点。
5. **函数的入参集合**：使用 `HashSet` 来记录当前函数的所有入参，因为入参的定义不能被消除。

### 详细解释

1. **变量的集合（myMap）**：
    - `myMap` 用于维护当前存在的所有变量。每当一个变量被定义时，它会被添加到 `myMap` 中。

2. **工作列表（WorkList）**：
    - `WorkList` 用于存储需要处理的变量。初始时，所有变量都被添加到 `WorkList` 中。每次处理一个变量时，如果发现它是不活跃的且没有副作用，则将其从 `WorkList` 中移除。

3. **变量的使用情况（useMap）**：
    - `useMap` 记录每个变量的所有使用点。对于每个变量 `v`，`useMap[v]` 是一个集合，包含所有使用 `v` 的指令。

4. **变量的定义情况（defMap）**：
    - `defMap` 记录每个变量的定义点。对于每个变量 `v`，`defMap[v]` 是定义 `v` 的指令。

5. **函数的入参集合**：
    - 函数的入参不能被消除，因此需要使用一个 `HashSet` 来记录所有入参。在处理变量时，如果变量是函数的入参，则跳过该变量。

### 实现步骤

1. **初始化**：
    - 将所有变量添加到 `myMap` 和 `WorkList` 中。
    - 记录每个变量的定义点和使用点，分别存储在 `defMap` 和 `useMap` 中。
    - 记录函数的入参。

2. **处理工作列表**：
    - 从 `WorkList` 中取出一个变量 `v`。
    - 如果 `v` 没有使用点且定义 `v` 的指令没有副作用，则删除定义 `v` 的指令，并更新 `defMap` 和 `useMap`。
    - 将受影响的变量重新添加到 `WorkList` 中。

3. **重复上述步骤**，直到 `WorkList` 为空。

### 示例代码

以下是一个简化的示例代码，展示如何实现死代码消除：

```cpp
#include <unordered_map>
#include <unordered_set>
#include <vector>

class IRRegister {};
class IRBaseInst {
public:
    virtual bool hasSideEffects() const = 0;
    virtual IRRegister* getDef() const = 0;
    virtual std::vector<IRRegister*> getUses() const = 0;
    virtual void remove() = 0;
};

void deadCodeElimination(std::vector<IRBaseInst*>& instructions, const std::unordered_set<IRRegister*>& functionParams) {
    std::unordered_map<IRRegister*, IRBaseInst*> defMap;
    std::unordered_map<IRRegister*, std::unordered_set<IRBaseInst*>> useMap;
    std::unordered_set<IRRegister*> workList;

    // 初始化 defMap 和 useMap
    for (IRBaseInst* inst : instructions) {
        IRRegister* def = inst->getDef();
        if (def) {
            defMap[def] = inst;
            workList.insert(def);
        }
        for (IRRegister* use : inst->getUses()) {
            useMap[use].insert(inst);
        }
    }

    // 处理工作列表
    while (!workList.empty()) {
        IRRegister* v = *workList.begin();
        workList.erase(workList.begin());

        if (useMap[v].empty() && defMap[v] && !defMap[v]->hasSideEffects() && functionParams.find(v) == functionParams.end()) {
            IRBaseInst* defInst = defMap[v];
            defInst->remove();
            defMap.erase(v);

            for (IRRegister* use : defInst->getUses()) {
                useMap[use].erase(defInst);
                if (useMap[use].empty()) {
                    workList.insert(use);
                }
            }
        }
    }
}
```

### 代码解释

1. **初始化**：
    - 遍历所有指令，填充 `defMap` 和 `useMap`。
    - 将所有变量添加到 `workList` 中。

2. **处理工作列表**：
    - 从 `workList` 中取出一个变量 `v`。
    - 如果 `v` 没有使用点且定义 `v` 的指令没有副作用且不是函数的入参，则删除定义 `v` 的指令。
    - 更新 `defMap` 和 `useMap`，并将受影响的变量重新添加到 `workList` 中。

通过这种方式，你可以实现死代码消除算法，移除所有不活跃的变量和指令。



## 死代码消除（另一种算法）

死代码消除（Dead Code Elimination, DCE）算法的目标是移除那些不会影响程序最终结果的指令。这个过程通常分为三个主要步骤：

1. 标记所有输出指令为活跃（alive），并将这些指令添加到工作列表（worklist）中。
2. 处理工作列表：从工作列表中取出指令，将指令的源操作数的定义指令标记为活跃，并将这些定义指令添加到工作列表中。重复这个过程，直到工作列表为空。
3. 遍历所有指令：如果指令不是活跃的，那么在定义-使用链（DU Chain）中删除该指令的定义和使用，并删除该指令；如果指令是活跃的，则跳过该指令。


### 详细解释
步骤 1：标记所有输出指令为活跃，并将这些指令添加到工作列表中
首先，遍历所有基本块中的所有指令，找到那些有副作用的指令（如 store、call、ret、br 等），将这些指令标记为活跃，并将它们添加到工作列表中。

步骤 2：处理工作列表
从工作列表中取出一个指令，将该指令的源操作数的定义指令标记为活跃，并将这些定义指令添加到工作列表中。重复这个过程，直到工作列表为空。

步骤 3：遍历所有指令
遍历所有基本块中的所有指令，如果指令不是活跃的，那么在定义-使用链中删除该指令的定义和使用，并删除该指令；如果指令是活跃的，则跳过该指令。


## 激进的死代码消除（Aggressive Dead Code Elimination）

### 算法思想
它的思想和传统的死代码消除最不一样的地方就在于：它对于死代码的定义不同。

它的定义相当于是递归的：初始，我们定义所有调用函数，函数返回，对存储器的操作为有效代码。之后，我们标记一下语句为有效的：

+ 对其他有效语句的use进行定值的语句
+ 其他有效语句控制依赖于的语句（至于这个是什么，我们待会儿说）

之后，我们迭代得到所有语句，并把剩下的都删除。那么接下来，我们首先展开控制依赖部分的内容，幸运的是，这一部分和支配树很像。

### 控制依赖
我们希望回答的问题是，控制流图上的两个节点
x,y中，x能否直接控制节点y的执行？

那么什么是控制执行呢？应该就是节点x有一个后继u能直接到达程序的exitBlock而不经过y。而它同时也有一个后继v使得v到exitBlock的每一条路径都经过y。

那么我们很容易就能得到控制依赖的等价定义。我们考虑CFG对应的反图，则在这张图上，x∈domFrontier(y)。因为x的前驱v被y直接支配，而它又能由u到达，因而x在y的支配边界上。

### 算法实现
我们需要维护的信息如下：
1. HashSet<IRBaseInst> live：所有活跃指令
2. HashSet<BasicBlock> liveBlock：所有有活跃指令的基本块
3. HashSet<entity> liveUse：所有活跃指令的use
4. HashSet<IRBaseInst> workList：用于迭代的工作表
5. HashSet<IRRegister, IRBaseInst> defMap：所有变量的def语句
首先，我们需要建出控制依赖图，这部分参考之前支配树构建的那期。

接下来，我们首先扫描该函数的所有基本块，将所有def收集到defMap中，同时把所有的store（代表修改全局变量，可能会在其他程序中用到）、所有的call、所有的ret加入workList。

然后，我们进行迭代。代码如下：
```java
while (!workList.isEmpty()) {
    IRBaseInst inst = workList.iterator().next();
    workList.remove(inst);
    live.add(inst);
    liveBlock.add(inst.parentBlock);
    liveUse.addAll(inst.uses());
    if (inst instanceof IRPhi irPhi) { // 对于一条phi指令，它的每一个前驱都应当被标注为活跃的
        for (var block : irPhi.blockMap) {
            if (block.terminal != null && !live.contains(block.terminal)) {
                workList.add(block.terminal);
                liveBlock.add(block);
            }
        }
    }
    for (var cdg_pred : inst.parentBlock.cdg_pred) { // 加入该块的所有控制依赖前驱
        if (cdg_pred.terminal != null && !live.contains(cdg_pred.terminal)) {
            workList.add(cdg_pred.terminal); // 注意已经加过的不用加了
        }
    }
    for (var use : inst.uses()) { // 对于其每个use的变量，将其def加入workList
        if (!(use instanceof IRRegister) || use instanceof IRGlobalVar) continue;
        IRBaseInst def = defMap.get(use);
        if (def != null && !live.contains(def)) {
            workList.add(def);
        }
    }
}
```
最后我们遍历所有指令，消去不活跃的phi指令和普通指令。

这里有一个细节，就是jump/branch这样的terminal的处理。如果一个块的terminal被标记为不活跃的，那么这个块应该跳到哪里呢？自然，它应当跳到它的后继中第一个活跃的块上。我们要在反支配树上寻找（反支配树就是我们根据CFG的反图建出的支配树）。

### gpt详细解释

激进的死代码消除（Aggressive Dead Code Elimination, ADCE）是一种高级的优化技术，用于删除程序中不会影响程序结果的代码。与传统的死代码消除不同，ADCE 通过递归地定义和标记有效代码来识别死代码。以下是对该算法的详细讲解。

#### 算法思想

ADCE 的核心思想是递归地定义和标记有效代码。初始状态下，所有调用函数、函数返回和对存储器的操作都被认为是有效代码。然后，通过以下规则递归地标记其他有效代码：

1. **对其他有效语句的 `use` 进行定值的语句**。
2. **其他有效语句控制依赖于的语句**。

#### 控制依赖

控制依赖用于确定控制流图（CFG）中的两个节点 `x` 和 `y` 之间的控制关系。具体来说，节点 `x` 是否直接控制节点 `y` 的执行。控制依赖的等价定义是：在 CFG 的反图中，`x` 属于 `y` 的支配边界（domFrontier）。

#### 算法实现

##### 维护的信息

1. **`live`**：所有有活跃指令。
2. **`liveBlock`**：所有有活跃指令的基本块。
3. **`liveUse`**：所有活跃指令的 `use`。
4. **`workList`**：用于迭代的工作表。
5. **`defMap`**：所有变量的 `def` 语句。

##### 步骤

1. **构建控制依赖图**：
    - 参考支配树构建的方法，构建控制依赖图。

2. **初始化 `defMap` 和 `workList`**：
    - 扫描函数的所有基本块，将所有 `def` 收集到 `defMap` 中。
    - 将所有 `store`（代表修改全局变量，可能会在其他程序中用到）、所有 `call`、所有 `ret` 加入 `workList`。

3. **迭代处理 `workList`**：
    - 迭代处理 `workList` 中的指令，标记活跃指令和基本块，并将相关的 `def` 和控制依赖前驱加入 `workList`。

4. **删除不活跃的指令**：
    - 遍历所有指令，删除不活跃的 `phi` 指令和普通指令。

#### 代码实现

以下是算法的代码实现：

```java
while (!workList.isEmpty()) {
    IRBaseInst inst = workList.iterator().next(); // 获取工作表中的下一个指令
    workList.remove(inst); // 从工作表中移除该指令
    live.add(inst); // 将该指令标记为活跃
    liveBlock.add(inst.parentBlock); // 将该指令所在的基本块标记为活跃
    liveUse.addAll(inst.uses()); // 将该指令的所有 use 标记为活跃

    if (inst instanceof IRPhi irPhi) { // 对于一条 phi 指令
        for (var block : irPhi.blockMap) { // 遍历 phi 指令的所有前驱块
            if (block.terminal != null && !live.contains(block.terminal)) {
                workList.add(block.terminal); // 将前驱块的终结指令加入工作表
                liveBlock.add(block); // 将前驱块标记为活跃
            }
        }
    }

    for (var cdg_pred : inst.parentBlock.cdg_pred) { // 遍历该块的所有控制依赖前驱
        if (cdg_pred.terminal != null && !live.contains(cdg_pred.terminal)) {
            workList.add(cdg_pred.terminal); // 将控制依赖前驱的终结指令加入工作表
        }
    }

    for (var use : inst.uses()) { // 遍历该指令的所有 use
        if (!(use instanceof IRRegister) || use instanceof IRGlobalVar) continue;
        IRBaseInst def = defMap.get(use); // 获取 use 的定义指令
        if (def != null && !live.contains(def)) {
            workList.add(def); // 将定义指令加入工作表
        }
    }
}
```

#### 详细解释

1. **初始化**：
    - 扫描函数的所有基本块，将所有 `def` 收集到 `defMap` 中。
    - 将所有 `store`、`call` 和 `ret` 指令加入 `workList`。

2. **迭代处理 `workList`**：
    - 从 `workList` 中取出一个指令 `inst`。
    - 将 `inst` 标记为活跃，并将其所在的基本块标记为活跃。
    - 将 `inst` 的所有 `use` 标记为活跃。
    - 对于 `phi` 指令，将其所有前驱块的终结指令加入 `workList`。
    - 将 `inst` 所在块的所有控制依赖前驱的终结指令加入 `workList`。
    - 对于 `inst` 的每个 `use`，将其定义指令加入 `workList`。

3. **删除不活跃的指令**：
    - 遍历所有指令，删除不活跃的 `phi` 指令和普通指令。




 #include "sysyruntimelibrary/sylib.h"