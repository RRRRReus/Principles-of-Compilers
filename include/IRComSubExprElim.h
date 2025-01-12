#ifndef __IRCOMSUBEXPRELIM_H__
#define __IRCOMSUBEXPRELIM_H__

#include "Unit.h"

struct Expr
{
    Instruction *inst;
    Expr(Instruction *inst) : inst(inst){};
    // 用于调用find函数
    bool operator==(const Expr &other) const
    {
        //fprintf(stderr,"比较两个表达式是否相同\n");
        // TODO: 判断两个表达式是否相同
        // 两个表达式相同 <==> 两个表达式对应的指令的类型和操作数均相同
        if (!this->inst->haveSameOperator(other.inst))
            return false;
        
        
        const auto &thisOperands = this->inst->getUse();
        const auto &otherOperands = other.inst->getUse();

        if (thisOperands.size() != otherOperands.size())
            return false;
        if(this->inst->canBeSwapped()||other.inst->canBeSwapped())
        {
            
            Operand *op1=this->inst->getUse()[0];
            Operand *op2=this->inst->getUse()[1];
            Operand *op3=other.inst->getUse()[0];
            Operand *op4=other.inst->getUse()[1];
            if(op1==op4&&op2==op3)
            {
                fprintf(stderr,"发现交换可复用的表达式\n");
                return true;
            }
                
            if(op1==op3&&op2==op4)
            {
                fprintf(stderr,"发现不交换可复用的表达式\n");
                return true;
            }
   

        }
        for (size_t i = 0; i < thisOperands.size(); ++i)
        {
            //fprintf(stderr,"thisOperands[i] %s\n",thisOperands[i]->toStr().c_str());
            //fprintf(stderr,"otherOperands[i] %s\n",otherOperands[i]->toStr().c_str());

            if (thisOperands[i] != otherOperands[i])
                return false;
        }
        fprintf(stderr,"发现不交换可复用的表达式\n");
        return true;
        
    };
};

class IRComSubExprElim
{
private:
    Unit *unit;

    std::vector<Expr> exprVec;// 记录所有的表达式
    std::map<Instruction *, int> ins2Expr; // 记录每个指令对应的表达式id
    std::map<BasicBlock *, std::set<int>> genBB;// 记录每个基本块的gen
    std::map<BasicBlock *, std::set<int>> killBB;// 记录每个基本块的kill
    std::map<BasicBlock *, std::set<int>> inBB;// 记录每个基本块的in
    std::map<BasicBlock *, std::set<int>> outBB;// 记录每个基本块的out

    // 跳过无需分析的指令
    bool skip(Instruction *);

    // 局部公共子表达式消除
    bool localCSE(Function *);

    // 全局公共子表达式消除
    bool globalCSE(Function *);
    void calGenKill(Function*);
    void calInOut(Function*);
    bool removeGlobalCSE(Function*);

public:
    IRComSubExprElim(Unit *unit);
    ~IRComSubExprElim();
    void pass();
};

#endif