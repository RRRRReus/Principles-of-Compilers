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
    bool save=false;//是否需要保存
    virtual ~Instruction();
    BasicBlock *getParent();
    bool isUncond() const {return instType == UNCOND;};//是否为无条件分支
    bool isCond() const {return instType == COND;};//是否为条件分支
    bool isAlloca() const {return instType == ALLOCA;};//是否为分配指令
    bool isBinary() const {return instType == BINARY;};//是否为二元指令
    bool isLoad() const {return instType == LOAD;};//是否为加载指令
    bool isStore() const {return instType == STORE;};//是否为存储指令
    bool isCmp() const {return instType == CMP;};//是否为比较指令
    bool isRet() const {return instType == RET;};//是否为返回指令
    bool isCall() const {return instType == CALL;};//是否为函数调用指令
    bool isZext() const {return instType == ZEXT;};//是否为零扩展指令
    bool isGep() const {return instType == GEP;};//是否为数组访问指令
    bool isBitcast() const {return instType == BITCAST;};//是否为类型转换指令
    bool isFptoi() const {return instType == FPTOI;};//是否为浮点数到整数的转换指令
    bool isSitof() const {return instType == SITOF;};//是否为整数到浮点数的转换指令
    bool isPhi() const {return instType == PHI;};//是否为phi指令
    int getInstType() const {return instType;};//获取指令类型
    void setParent(BasicBlock *);
    void setNext(Instruction *);
    void setPrev(Instruction *);
    Instruction *getNext();
    Instruction *getPrev();
    virtual Operand *getDef() { return nullptr; }//获取定义 
    virtual std::vector<Operand *> getUse() { return {}; }
    //virtual void setUse(Operand *op, int index) {}//设置指令中使用的操作数，即指令的输入
    int getAllOperandsNum() { return operands.size(); }//获取操作数数量
    Operand *getOperand(int index) { return operands[index]; }//获取操作数
    void setOperand(Operand *op, int index) { operands[index] = op; }//设置所有操作数
    virtual void output() const = 0;
    void optimize(){};
    virtual bool hasSideEffects() const { return false; } // 默认没有副作用（死代码消除时使用）
protected:
    unsigned instType;//指令类型
    unsigned opcode;//操作码
    Instruction *prev;//指向前一条指令
    Instruction *next;//指向后一条指令
    BasicBlock *parent;//指向所属基本块
    std::vector<Operand*> operands;//操作数
    enum {  BINARY, 
            COND, 
            UNCOND,
            RET, 
            LOAD, 
            STORE, 
            CMP, 
            ALLOCA, 
            CALL,//增加函数调用的call
            ZEXT,//增加零扩展指令
            GEP//增加数组访问指令
            ,BITCAST//增加类型转换指令
            ,FPTOI // 增加浮点数到整数的转换指令
            ,SITOF // 增加整数到浮点数的转换指令
            ,PHI //增加phi指令


    };
};

// meaningless instruction, used as the head node of the instruction list.
//无意义指令，用作指令列表的头节点。
class DummyInstruction : public Instruction
{
public:
    DummyInstruction() : Instruction(-1, nullptr) {};
    DummyInstruction(BasicBlock *insert_bb) : Instruction(-1, insert_bb) {};
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
 * 
 *   `dst` = alloca `se->type`, align 4
 * 
 * @param dst 分配结果将存储的目标操作数。
 * @param se 与此分配关联的符号条目。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 */
    AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb = nullptr);
    ~AllocaInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    SymbolEntry *getSymbolEntry() { return se; }//用于在优化时获取符号表项，用于直接获取alloca对象的类型
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
class LoadInstruction : public Instruction//将内存指针operandp[1]指向的值赋给operand[0]寄存器
{
public:
/**
 * @brief 构造一个新的 LoadInstruction 对象。
 * 
 *   dst = load `dst->type`, `src_addr->type` `src_addr`, align 4
 * @param dst 加载结果将存储的目标操作数。
 * @param src_addr 源地址操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * @return 一个新的 LoadInstruction 对象。
 */
    LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb = nullptr);
    ~LoadInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    void setDef(Operand *def) { operands[0] = def; }//又乱加访问器方法
    std::vector<Operand *> getUse() { return {operands[1]}; }//获取指令中使用的操作数，即指令的输入
    //void setUse(Operand *op, int index) {operands[index + 1] = op;}//设置指令中使用的操作数，即指令的输入
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
 * 
 *  store `src->type` `src`, `dst_addr->type` `dst_addr`, align 4
 * @param dst_addr 存储地址操作数。
 * @param src 源操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * @return 一个新的 StoreInstruction 对象。
 * @note 该指令将 src 中的数据存储到 dst_addr 指向的内存地址中。
 */
    StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb = nullptr);
    ~StoreInstruction();
    void output() const;
    Operand *getDef() { return operands[0]; }
    std::vector<Operand *> getUse() { return {operands[0], operands[1]}; }
    void setDef(Operand *op) { operands[0] = op; } // 将current指向op
    bool hasSideEffects() const override { 
        //如果store指令的目标是全局变量
        if(dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getSymbolEntry())->isGlobal())
            return true;
        else
        return false; 
    } // Store 指令有副作用
    
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
    enum {SUB, ADD,MUL,DIV,MOD, AND, OR,XOR};
    Operand *getDef() { return operands[0]; }//获取结果操作数
    std::vector<Operand *> getUse() { return {operands[1], operands[2]}; }//获取源操作数
    bool canBeCalculated();//是否可以计算
    Operand *CalculatedResult();//计算结果
    
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
    bool canBeCalculated();//是否可以计算
    Operand *CalculatedResult();//计算结果

    
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
/**
 * @brief 构造一个新的 UncondBrInstruction 对象。从insert_bb末尾无条件跳转到branch前面。
 * @param branch 分支目标基本块。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 */
    UncondBrInstruction(BasicBlock*, BasicBlock *insert_bb = nullptr);
    void output() const;
    void setBranch(BasicBlock *);
    BasicBlock *getBranch();
    BasicBlock **patchBranch() {return &branch;};
    BasicBlock *getBranchBB(){return branch;}
    bool hasSideEffects() const override { return true; } // Uncond 指令有副作用
    
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
/**
 * @brief 构造一个新的 CondBrInstruction 对象。
 * @param true_branch 条件为真时的目标基本块。
 * @param false_branch 条件为假时的目标基本块。
 * @param cond 条件操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 */
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
    BasicBlock *getTrueBB(){return true_branch;}
    BasicBlock *getFlaseBB(){return false_branch;}
    bool hasSideEffects() const override { return true; } // Cond 指令有副作用

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
/**
 * @brief 构造一个新的 RetInstruction 对象。
 * @param src 返回值操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 */
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
    bool hasSideEffects() const override { return true; } // Ret 指令有副作用
};


/**
 * @class CallInstruction
 * @brief 表示编译器中间表示中的函数调用指令。
 * 
 * 该类负责处理函数调用操作。
 * 它继承自基类 Instruction。
 * 
 */
class CallInstruction : public Instruction
{
public:
/**
 * @brief 构造一个新的 CallInstruction 对象。
 * @param dst 返回值操作数。
 * @param funcSE 函数符号表项。
 * @param args 实参操作数列表。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 */
    CallInstruction(Operand *dst, IdentifierSymbolEntry *funcSE, const std::vector<Operand *> &args, BasicBlock *insert_bb = nullptr);
    CallInstruction(Operand *dst, FunctionSymbolEntry *library_funcSE, const std::vector<Operand *> &args, BasicBlock *insert_bb = nullptr);
    ~CallInstruction();
    void output() const;
    Operand *getDef() { return operands.empty() ? nullptr : operands[0]; }//获取返回值操作数
    std::vector<Operand *> getUse() { return std::vector<Operand *>(operands.begin() + 1, operands.end()); }//获取所有实参操作数
    bool hasSideEffects() const override { return true; } // Call 指令有副作用

private:
    IdentifierSymbolEntry *funcSE=nullptr;
    FunctionSymbolEntry *library_funcSE=nullptr;
};
/**
 * @class ZextInstruction
 * @brief 表示编译器中间表示中的零扩展指令。
 *
 * 该类负责处理零扩展操作。
 * 它继承自基类 Instruction。
 */
class ZextInstruction : public Instruction
{
public:
    ZextInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    void output() const override;
    Operand *getDef() override;
    std::vector<Operand *> getUse() override;
    bool canBeCalculated();//是否可以计算
    Operand *CalculatedResult();//计算结果

    
};
class GetElementPtrInstruction : public Instruction
{
public:
    
    GetElementPtrInstruction(Operand *dst,Operand *element, Operand *src, const std::vector<Operand *> &indices, BasicBlock *insert_bb = nullptr);
    void output() const override;
    Operand *getDef() override;
    std::vector<Operand *> getUse() override;
    

private:
    std::vector<Operand *> indices;
};
/**
 * @class BitcastInstruction
 * @brief 表示编译器中间表示中的 bitcast 指令。
 *
 * 该类负责处理 bitcast 操作。
 * 它继承自基类 Instruction。
 */
class BitcastInstruction : public Instruction
{
public:
    BitcastInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    void output() const override;
    Operand *getDef() override;
    std::vector<Operand *> getUse() override;
    
};

/**
 * @class FpToSiInstruction
 * @brief 表示编译器中间表示中的浮点数到有符号整数的转换指令。
 *
 * 该类负责处理浮点数到有符号整数的转换操作。
 * 它继承自基类 Instruction。
 */
class FpToSiInstruction : public Instruction
{
public:
    FpToSiInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~FpToSiInstruction();
    void output() const override;
    Operand *getDef() override {return operands[0];}
    std::vector<Operand *> getUse() override {return {operands[1]};}
    

};

/**
 * @class SiToFpInstruction
 * @brief 表示编译器中间表示中的有符号整数到浮点数的转换指令。
 *
 * 该类负责处理有符号整数到浮点数的转换操作。
 * 它继承自基类 Instruction。
 */
class SiToFpInstruction : public Instruction
{
public:
    SiToFpInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb = nullptr);
    ~SiToFpInstruction();
    void output() const override;
    Operand *getDef() override {return operands[0];}
    std::vector<Operand *> getUse() override {return {operands[1]};}
    
};
/**
 * @class PhiInstruction
 * @brief 表示编译器中间表示中的phi指令。
 *
 * 该类负责处理phi操作。
 * 它继承自基类 Instruction。
 */
class PhiInstruction : public Instruction
{
public:
    PhiInstruction(Operand *dst, const std::vector<std::pair<Operand *, BasicBlock *>> &incoming, BasicBlock *insert_bb = nullptr);
    PhiInstruction(Operand *dst, BasicBlock *insert_bb = nullptr);
    void addIncoming(Operand *op, BasicBlock *bb);
    void removeIncoming(BasicBlock *bb);
    void output() const override;
    Operand *getDef() override;
    std::vector<Operand *> getUse() override;
    std::vector<std::pair<Operand *, BasicBlock *>> incoming;

private:
    
};


#endif