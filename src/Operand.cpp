#include "Operand.h"
#include <sstream>
#include <algorithm>
#include <string.h>
#include "BasicBlock.h"
#include "Instruction.h"

std::string Operand::toStr() const
{
    return se->toStr();
}

void Operand::replaceAllUsesWith(Operand *newOperand)
{
    fprintf(stderr,"当前替换的操作数为%s\n",this->toStr().c_str());
    fprintf(stderr,"新操作数为%s\n",newOperand->toStr().c_str());
    for (auto use : uses) //遍历所有使用此操作数的指令
    {
        fprintf(stderr,"当前使用的指令是不是store指令 %d\n",use->isStore());
        for (int i = 0; i < use->getAllOperandsNum(); i++)
        {
            fprintf(stderr,"当前使用的操作数是 %s\n",use->getOperand(i)->toStr().c_str());
            if (use->getOperand(i) == this)//如果使用的操作数是当前要进行替换的操作数
            {
                //进行替换
                fprintf(stderr,"开始替换操作数 %s\n",use->getOperand(i)->toStr().c_str());
                use->setOperand(newOperand, i);
                fprintf(stderr,"替换之后的结果为%s\n",use->getOperand(i)->toStr().c_str());
                newOperand->addUse(use);//将新操作数加入到使用列表中
            }
        }
    }
        //uses.clear();
        //函数中清空 uses 列表的目的是为了确保当前操作数不再被任何指令使用。因为所有使用当前操作数的指令已经被替换为新的操作数，所以当前操作数的 uses 列表应该被清空。
}





void Operand::removeUse(Instruction *inst)
{
    auto i = std::find(uses.begin(), uses.end(), inst);
    if(i != uses.end())
        uses.erase(i);
}

