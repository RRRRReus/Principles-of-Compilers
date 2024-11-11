#ifndef __INSTRUCTION_H__
#define __INSTRUCTION_H__

#include "Operand.h"
#include <vector>
#include <map>

class BasicBlock;

//指令类
class Instruction
{
public:
    Instruction(unsigned instType, BasicBlock *insert_bb = nullptr);
    virtual ~Instruction();
    BasicBlock *getParent();
    bool isUncond() const {return instType == UNCOND;};//是否为无条件分支
    bool isCond() const {return instType == COND;};//是否为条件分支
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    Instruction *getNext();
    Instruction *getPrev();
    virtual Operand *getDef() { return nullptr; }//获取定义 
    virtual std::vector<Operand *> getUse() { return {}; }
    virtual void output() const = 0;
protected:
    unsigned instType;//指令类型
    unsigned opcode;//操作码
    Instruction *prev;//指向前一条指令
    Instruction *next;//指向后一条指令
    BasicBlock *parent;//指向所属基本块
    std::vector<Operand*> operands;//操作数
    enum {BINARY, COND, UNCOND, RET, LOAD, STORE, CMP, ALLOCA};
};

// meaningless instruction, used as the head node of the instruction list.
//无意义指令，用作指令列表的头节点。
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr) {};
    void output() const {};
};

/**
 * @class AllocaInstruction
 * @brief 表示编译器中间表示中的分配指令。
 *
 * 该类负责处理变量的内存分配。
 * 它继承自基类 Instruction。
 */
class AllocaInstruction : public Instruction
{
public:
/** 
 * @brief 构造一个新的 AllocaInstruction 对象。
 * @param dst 分配结果将存储的目标操作数。
 * @param se 与此分配关联的符号条目。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 */
    AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
private:
    SymbolEntry *se;
};

/**
 * @class LoadInstruction
 * @brief 表示编译器中间表示中的加载指令。
 *
 * 该类负责处理从内存中加载数据的操作。
 * 它继承自基类 Instruction。
 */
class LoadInstruction : public Instruction
{
public:
/**
 * @brief 构造一个新的 LoadInstruction 对象。
 * @param dst 加载结果将存储的目标操作数。
 * @param src_addr 源地址操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * @return 一个新的 LoadInstruction 对象。
 */
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1]}; }
};

/**
 * @class StoreInstruction
 * @brief 表示编译器中间表示中的存储指令。
 *
 * 该类负责处理将数据存储到内存中的操作。
 * 它继承自基类 Instruction。
 */
class StoreInstruction : public Instruction
{
public:
/**
 * @brief 构造一个新的 StoreInstruction 对象。
 * @param dst_addr 存储地址操作数。
 * @param src 源操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * @return 一个新的 StoreInstruction 对象。
 * @note 该指令将 src 中的数据存储到 dst_addr 指向的内存地址中。
 */
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
    std::vector<Operand *> getUse() { return {operands[0], operands[1]}; }
};

/**
 * @class BinaryInstruction
 * @brief 表示编译器中间表示中的二元指令。
 *
 * 该类负责处理二元运算操作。
 * 它继承自基类 Instruction。
 */
class BinaryInstruction : public Instruction
{
public:

/**
 * @brief 构造一个新的 BinaryInstruction 对象。
 * @param opcode 操作码。
 * @param dst 结果操作数。
 * @param src1 源操作数1。
 * @param src2 源操作数2。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 */
    BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~BinaryInstruction();
    void output() const;
    enum {SUB, ADD, AND, OR};
    Operand *getDef() { return operands[0]; }//获取定义
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }
};
/**
 * @class CmpInstruction
 * @brief 表示编译器中间表示中的比较指令。
 * 
 * 该类负责处理比较操作。
 * 它继承自基类 Instruction。
 * 
 */
class CmpInstruction : public Instruction
{
public:
/**
 * @brief 构造一个新的 CmpInstruction 对象。
 * @param opcode 操作码。
 * @param dst 结果操作数。
 * @param src1 源操作数1。
 * @param src2 源操作数2。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 */
    CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb = nullptr);
    ~CmpInstruction();
    void output() const;
    enum {E, NE, L, GE, G, LE};
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }
};

// unconditional branch
/**
 * @class UncondBrInstruction
 * @brief 表示编译器中间表示中的无条件分支指令。
 * 
 * 该类负责处理无条件分支操作。
 * 它继承自基类 Instruction。
 * 
 */
class UncondBrInstruction : public Instruction
{
public:
    UncondBrInstruction(BasicBlock*, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    BasicBlock **patchBranch() {return &branch;};
protected:
    BasicBlock *branch;
};

// conditional branch
/**
 * @class CondBrInstruction
 * @brief 表示编译器中间表示中的条件分支指令。
 * 
 * 该类负责处理条件分支操作。
 * 它继承自基类 Instruction。
 * 
 */
class CondBrInstruction : public Instruction
{
public:
    CondBrInstruction(BasicBlock*, BasicBlock*, Operand *, BasicBlock *insert_bb = nullptr);
    ~CondBrInstruction();
    void output() const;
    void setTrueBranch(BasicBlock*);
    BasicBlock* getTrueBranch();
    void setFalseBranch(BasicBlock*);
    BasicBlock* getFalseBranch();
    BasicBlock **patchBranchTrue() {return &true_branch;};
    BasicBlock **patchBranchFalse() {return &false_branch;};
    std::vector<Operand *> getUse() { return {operands[0]}; }
protected:
    BasicBlock* true_branch;
    BasicBlock* false_branch;
};
/**
 * @class RetInstruction
 * @brief 表示编译器中间表示中的返回指令。
 * 
 * 该类负责处理返回操作。
 * 它继承自基类 Instruction。
 * 
 */
class RetInstruction : public Instruction
{
public:
    RetInstruction(Operand *src, BasicBlock *insert_bb = nullptr);
    ~RetInstruction();
    std::vector<Operand *> getUse()
    {
        if (operands.size())
            return {operands[0]};
        else
            return {};
    }
    void output() const;
};

#endif