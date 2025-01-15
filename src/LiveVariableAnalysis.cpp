#include "LiveVariableAnalysis.h"
#include "MachineCode.h"
#include <algorithm>

void LiveVariableAnalysis::pass(MachineUnit *unit)
{
    for (auto &func : unit->getFuncs())
    {
        computeUsePos(func);
        computeDefUse(func);
        iterate(func);
    }
}

void LiveVariableAnalysis::pass(MachineFunction *func)
{
    fprintf(stderr, "LiveVariableAnalysis::pass\n");
    computeUsePos(func);
    fprintf(stderr, "computeUsePos\n");
    computeDefUse(func);
    fprintf(stderr, "computeDefUse\n");
    iterate(func);
    fprintf(stderr, "iterate\n");
}

void LiveVariableAnalysis::computeDefUse(MachineFunction *func)
{
    for (auto &block : func->getBlocks())
    {
        for (auto inst = block->getInsts().begin(); inst != block->getInsts().end(); inst++)
        {
            if (*inst == nullptr)
                continue;
            auto user = (*inst)->getUse();
            std::set<MachineOperand *> temp(user.begin(), user.end());
            set_difference(temp.begin(), temp.end(),
                           def[block].begin(), def[block].end(), inserter(use[block], use[block].end()));
            auto defs = (*inst)->getDef();
            for (auto &d : defs)
                def[block].insert(all_uses[*d].begin(), all_uses[*d].end());
        }
    }
}

void LiveVariableAnalysis::iterate(MachineFunction *func)
{
    // 清空每个块的 LiveIn 集合
    for (auto &block : func->getBlocks())
        block->getLiveIn().clear();

    bool change = true;
    while (change)
    {
        fprintf(stderr, "\n--- Starting a new iteration ---\n");
        change = false;
        
        // 遍历每个基本块
        for (auto &block : func->getBlocks())
        {
            // 清空 LiveOut 集合
            block->getLiveOut().clear();

            // 合并所有后继块的 LiveIn 集合到当前块的 LiveOut
            for (auto &succ : block->getSuccs())
            {
                block->getLiveOut().insert(succ->getLiveIn().begin(), succ->getLiveIn().end());
            }

            // 保存原始的 LiveIn 用于比较变化
            auto oldLiveIn = block->getLiveIn();

            // 更新当前块的 LiveIn
            block->getLiveIn() = use[block];

            // 计算 LiveIn = use ∪ (LiveOut - def)
            std::vector<MachineOperand *> temp;
            set_difference(block->getLiveOut().begin(), block->getLiveOut().end(),
                           def[block].begin(), def[block].end(), inserter(block->getLiveIn(), block->getLiveIn().end()));

            // 调试：打印当前块的 LiveIn 和 LiveOut
            fprintf(stderr, "\n--- Block %d ---\n", block->getNo());

            // // 打印 Old LiveIn 和 New LiveIn
            // fprintf(stderr, "  Old LiveIn: ");
            // if (oldLiveIn.empty()) {
            //     fprintf(stderr, "None\n");
            // } else {
            //     for (auto &operand : oldLiveIn)
            //         fprintf(stderr, "%s ", operand->outputDebug().c_str());
            //     fprintf(stderr, "\n");
            // }

            // fprintf(stderr, "  New LiveIn: ");
            // if (block->getLiveIn().empty()) {
            //     fprintf(stderr, "None\n");
            // } else {
            //     for (auto &operand : block->getLiveIn())
            //         fprintf(stderr, "%s ", operand->outputDebug().c_str());
            //     fprintf(stderr, "\n");
            // }

            // // 打印 LiveOut
            // fprintf(stderr, "  LiveOut: ");
            // if (block->getLiveOut().empty()) {
            //     fprintf(stderr, "None\n");
            // } else {
            //     for (auto &operand : block->getLiveOut())
            //         fprintf(stderr, "%s ", operand->outputDebug().c_str());
            //     fprintf(stderr, "\n");
            // }

            // // 如果 LiveIn 发生变化，设置 change 为 true
            // if (oldLiveIn != block->getLiveIn())
            // {
            //     change = true;
            //     fprintf(stderr, "  *** LiveIn changed, continuing iteration. ***\n");
            // }
            // else
            // {
            //     fprintf(stderr, "  LiveIn did not change.\n");
            // }
        }

        // 如果没有变化，结束循环
        if (!change)
            fprintf(stderr, "\n--- No changes detected, terminating iteration. ---\n");
    }
}

void LiveVariableAnalysis::computeUsePos(MachineFunction *func)
{
    for (auto &block : func->getBlocks())
    {
        for (auto &inst : block->getInsts())
        {
            if(inst==nullptr)
                continue;   
            auto uses = inst->getUse();
            for (auto &use : uses)
                all_uses[*use].insert(use);
        }
    }
}
