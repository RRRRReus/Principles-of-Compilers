#ifndef __BASIC_BLOCK_H__
#define __BASIC_BLOCK_H__
#include <vector>
#include <set>
#include "Instruction.h"
#include "AsmBuilder.h"

class Function;

class BasicBlock
{
    typedef std::vector<BasicBlock *>::iterator bb_iterator;//这行代码定义了一个类型别名 bb_iterator，它是一个 std::vector<BasicBlock *>::iterator 类型的别名

private:
    std::vector<BasicBlock *> pred, succ;//前驱和后继
    std::vector<BasicBlock *> reversePred,reverseSucc;//反向前驱和后继

    Instruction *head;//指向第一条指令
    Instruction *optimizeHead;//优化后的链表头
    Function *parent;//指向所属函数
    int no;//基本块编号


public:
    bool reachable=false;//是否可达
    std::vector<BasicBlock *> DOMpred, DOMsucc;//支配树的前驱和后继
    std::vector<BasicBlock *> DomFrontier;//支配边界
    std::vector<BasicBlock *> reverseDOMpred,reverseDOMsucc;//反支配树的前驱和后继
    std::vector<BasicBlock *> reverseDomFrontier;//反支配边界(反支配树的支配边界)
    BasicBlock *while_cond;//如果该基本块是while循环的循环体，则while_cond指向循环条件
    BasicBlock *while_end;//如果该基本块是while循环的循环体，则while_end指向循环结束
    BasicBlock(Function *);
    ~BasicBlock();
    void insertFront(Instruction *);
    void insertBack(Instruction *);
    void insertBefore(Instruction *, Instruction *);
    void remove(Instruction *);
    bool empty() const { return head->getNext() == head;}
    void output() const;
    void optimize();
    void refresh();
    bool succEmpty() const { return succ.empty(); };
    bool predEmpty() const { return pred.empty(); };
    void addSucc(BasicBlock *);//添加后继
    void removeSucc(BasicBlock *);//删除后继
    void addPred(BasicBlock *);//添加前驱
    void removePred(BasicBlock *);//删除前驱
    void cleanPred(){pred.clear();}
    void cleanSucc(){succ.clear();}
    void safeRemoveAllSucc();//安全删除所有后继
    void safeRemoveAllPred();//安全删除所有前驱
    int getNo() { return no; };
    Function *getParent() { return parent; };
    Instruction* begin() { return head->getNext();};
    Instruction* end() { return head;};
    Instruction* rbegin() { return head->getPrev();};//反向迭代器
    Instruction* rend() { return head;};
    bb_iterator succ_begin() { return succ.begin(); };
    bb_iterator succ_end() { return succ.end(); };//后继的开始和结束
    bb_iterator pred_begin() { return pred.begin(); };//前驱的开始和结束
    bb_iterator pred_end() { return pred.end(); };
    int getNumOfPred() const { return pred.size(); };
    int getNumOfSucc() const { return succ.size(); };

    Instruction *getHead() { return head; }
        // 设置支配树的前驱和后继
    void setDOMpred(const std::vector<BasicBlock*>& predBlocks) { DOMpred = predBlocks; }
    void setDOMsucc(const std::vector<BasicBlock*>& succBlocks) { DOMsucc = succBlocks; }
        // 设置反支配树的前驱和后继
    void setReverseDOMpred(const std::vector<BasicBlock*>& predBlocks) { reverseDOMpred = predBlocks; }
    void setReverseDOMsucc(const std::vector<BasicBlock*>& succBlocks) { reverseDOMsucc = succBlocks; }//????

    // 获取的前驱和后继
    std::vector<BasicBlock*>& getPred() { return pred; }
    std::vector<BasicBlock*>& getSucc() { return succ; }
    std::vector<BasicBlock*>& getReversePred() { return reversePred; }
    std::vector<BasicBlock*>& getReverseSucc() { return reverseSucc; }

    Instruction* getTerminal();//获取基本块的终结指令

    
    void genMachineCode(AsmBuilder*);
};

#endif