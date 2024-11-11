#ifndef __IRBUILDER_H__
#define __IRBUILDER_H__

class Unit;
class Function;
class BasicBlock;
/**
 * @class IRBuilder
 * @brief 表示编译器中间表示中的IR构建器。
 * 把unit和insertBB放在一起，方便生成IR。
 */
class IRBuilder
{
private:
    Unit *unit; //表示当前的编译单元
    BasicBlock *insertBB;   // The current basicblock that instructions should be inserted into. // 当前指令应插入的基本块。

public:
    IRBuilder(Unit*unit) : unit(unit){};
    void setInsertBB(BasicBlock*bb){insertBB = bb;};
    Unit* getUnit(){return unit;};
    BasicBlock* getInsertBB(){return insertBB;};
};

#endif