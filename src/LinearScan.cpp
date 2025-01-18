#include <algorithm>
#include "LinearScan.h"
#include "MachineCode.h"
#include "LiveVariableAnalysis.h"

LinearScan::LinearScan(MachineUnit *unit)
{
    this->unit = unit;
    for (int i = 5; i < 11; i++)   //为了函数参数从5开始分配
        regs.push_back(i);
}

void LinearScan::allocateRegisters()
{
    for (auto &f : unit->getFuncs())
    {
        func = f;
        bool success;
        success = false;
        while (!success)        // repeat until all vregs can be mapped
        {
            fprintf(stderr, "allocateRegisters\n");
            computeLiveIntervals();
            fprintf(stderr, "computeLiveIntervals\n");
            success = linearScanRegisterAllocation();
            fprintf(stderr, "linearScanRegisterAllocation\n");
            if (success)        // all vregs can be mapped to real regs
                modifyCode();
            else                // spill vregs that can't be mapped to real regs
                genSpillCode();
        }
    }
}

void LinearScan::makeDuChains()
{
    LiveVariableAnalysis lva;
    lva.pass(func);
    du_chains.clear();
    int i = 0;
    std::map<MachineOperand, std::set<MachineOperand *>> liveVar;
    for (auto &bb : func->getBlocks())
    {
        liveVar.clear();
        for (auto &t : bb->getLiveOut())
            liveVar[*t].insert(t);
        int no;
        no = i = bb->getInsts().size() + i;
        for (auto inst = bb->getInsts().rbegin(); inst != bb->getInsts().rend(); inst++)
        {
            if(*inst == nullptr)
                continue;
            (*inst)->setNo(no--);
            for (auto &def : (*inst)->getDef())
            {
                if (def->isVReg())
                {
                    auto &uses = liveVar[*def];
                    du_chains[def].insert(uses.begin(), uses.end());
                    auto &kill = lva.getAllUses()[*def];
                    std::set<MachineOperand *> res;
                    set_difference(uses.begin(), uses.end(), kill.begin(), kill.end(), inserter(res, res.end()));
                    liveVar[*def] = res;
                }
            }
            for (auto &use : (*inst)->getUse())
            {
                if (use->isVReg())
                    liveVar[*use].insert(use);
            }
        }
    }
}

void LinearScan::computeLiveIntervals()
{
    makeDuChains();
    intervals.clear();
    for (auto &du_chain : du_chains)
    {
        int t = -1;
        for (auto &use : du_chain.second)
            t = std::max(t, use->getParent()->getNo());
        Interval *interval = new Interval({du_chain.first->getParent()->getNo(), t, false, 0, 0, {du_chain.first}, du_chain.second});
        intervals.push_back(interval);
    }
    
    for (auto& interval : intervals) {
        auto uses = interval->uses;
        auto begin = interval->start;
        auto end = interval->end;
        for (auto block : func->getBlocks()) {
            auto liveIn = block->getLiveIn();
            auto liveOut = block->getLiveOut();
            bool in = false;
            bool out = false;
            for (auto use : uses)
                if (liveIn.count(use)) {
                    in = true;
                    break;
                }
            for (auto use : uses)
                if (liveOut.count(use)) {
                    out = true;
                    break;
                }
            if (in && out) {
                begin = std::min(begin, (*(block->begin()))->getNo());
                end = std::max(end, (*(block->rbegin()))->getNo());
            } else if (!in && out) {
                for (auto i : block->getInsts())
                    if (i->getDef().size() > 0 &&
                        i->getDef()[0] == *(uses.begin())) {
                        begin = std::min(begin, i->getNo());
                        break;
                    }
                end = std::max(end, (*(block->rbegin()))->getNo());
            } else if (in && !out) {
                begin = std::min(begin, (*(block->begin()))->getNo());
                int temp = 0;
                for (auto use : uses)
                    if (use->getParent()->getParent() == block)
                        temp = std::max(temp, use->getParent()->getNo());
                end = std::max(temp, end);
            }
        }
        interval->start = begin;
        interval->end = end;
    }
    bool change;
    change = true;
    while (change)
    {
        change = false;
        std::vector<Interval *> t(intervals.begin(), intervals.end());
        for (size_t i = 0; i < t.size(); i++)
            for (size_t j = i + 1; j < t.size(); j++)
            {
                Interval *w1 = t[i];
                Interval *w2 = t[j];
                if (**w1->defs.begin() == **w2->defs.begin())
                {
                    std::set<MachineOperand *> temp;
                    set_intersection(w1->uses.begin(), w1->uses.end(), w2->uses.begin(), w2->uses.end(), inserter(temp, temp.end()));
                    if (!temp.empty())
                    {
                        change = true;
                        w1->defs.insert(w2->defs.begin(), w2->defs.end());
                        w1->uses.insert(w2->uses.begin(), w2->uses.end());
                        // w1->start = std::min(w1->start, w2->start);
                        // w1->end = std::max(w1->end, w2->end);
                        auto w1Min = std::min(w1->start, w1->end);
                        auto w1Max = std::max(w1->start, w1->end);
                        auto w2Min = std::min(w2->start, w2->end);
                        auto w2Max = std::max(w2->start, w2->end);
                        w1->start = std::min(w1Min, w2Min);
                        w1->end = std::max(w1Max, w2Max);
                        auto it = std::find(intervals.begin(), intervals.end(), w2);
                        if (it != intervals.end())
                            intervals.erase(it);
                    }
                }
            }
    }
    sort(intervals.begin(), intervals.end(), compareStart);
}

bool LinearScan::linearScanRegisterAllocation()
{
    // Todo
    /*
        active ←{}
        foreach live interval i, in order of increasing start point
            ExpireOldIntervals(i)
            if length(active) = R then
                SpillAtInterval(i)
            else
                register[i] ← a register removed from pool of free registers
                add i to active, sorted by increasing end point
    */

//    //初始化 active 列表
//     active.clear();//？？？？？？？？？？？？？需要吗？？？？？

//     // 遍历 intervals 列表，按照开始位置递增排序
//     for (auto interval : intervals)
//     {
//         // 1. 遍历 active 列表，移除结束时间早于当前区间开始时间的 interval
//         expireOldIntervals(interval);

//         // 2. 判断 active 列表中 interval 的数目和可用的物理寄存器数目是否相等
//         if (active.size() == regs.size())
//         {
//             // (a) 若相等，进行寄存器溢出操作
//             Interval* spill = active.back(); // 获取 active 列表中最后一个 interval

//             if (spill->end > interval->end)
//             {
//                 // 如果 active 列表中的活跃区间结束时间更晚
//                 spill->spill = true; // 置位其 spill 标志位
//                 interval->rreg = spill->rreg; // 将其占用的寄存器分配给当前区间
//                 active.pop_back(); // 从 active 列表中移除
//                 active.push_back(interval); // 将当前区间插入到 active 列表中
//                 std::sort(active.begin(), active.end(), [](Interval* a, Interval* b) { return a->end < b->end; }); // 按结束时间排序
//             }
//             else
//             {
//                 // 如果当前区间的结束时间更晚
//                 interval->spill = true; // 置位其 spill 标志位
//             }
//         }
//         else
//         {
//             // (b) 若不相等，为当前区间分配物理寄存器
//             interval->rreg = regs.back(); // 从空闲寄存器池中获取一个寄存器
//             regs.pop_back(); // 移除已分配的寄存器
//             active.push_back(interval); // 将当前区间插入到 active 列表中
//             std::sort(active.begin(), active.end(), [](Interval* a, Interval* b) { return a->end < b->end; }); // 按结束时间排序
//         }
//     }

     //return true;

    bool success = true;
    active.clear();
    regs.clear();
    for (int i = 4; i < 11; i++)
    {
        regs.push_back(i);
    }
    for (auto &i : intervals)
    {
        expireOldIntervals(i);
        if (regs.empty())
        {
            spillAtInterval(i);
            success = false;
        }
        else
        {
            i->rreg = regs.front();
            regs.erase(regs.begin());
            active.push_back(i);
            sort(active.begin(), active.end(), compareEnd);
        }
    }
    return success;

}

void LinearScan::modifyCode()
{
    for (auto &interval : intervals)
    {
        func->addSavedRegs(interval->rreg);
        for (auto def : interval->defs)
            def->setReg(interval->rreg);
        for (auto use : interval->uses)
            use->setReg(interval->rreg);
    }
}

void LinearScan::genSpillCode()
{
    for(auto &interval:intervals)
    {
        if(!interval->spill)
            continue;
        // TODO
        /* HINT:
         * The vreg should be spilled to memory.
         * 1. insert ldr inst before the use of vreg
         * 2. insert str inst after the def of vreg
         */ 

        // // 1. 遍历其 USE 指令的列表，在 USE 指令前插入 ldr 指令
        // for (auto use : interval->uses)
        // {
        //     MachineOperand* spillOperand = new MachineOperand(MachineOperand::REG, interval->rreg);
        //     MachineOperand* stackOperand = new MachineOperand(MachineOperand::MEM, interval->disp);
        //     MachineInstruction* loadInst = new LoadMInstruction(use->getBlock(), spillOperand, stackOperand);
        //     use->getBlock()->InsertBefore(use, loadInst);
        // }

        // // 2. 遍历其 DEF 指令的列表，在 DEF 指令后插入 str 指令
        // for (auto def : interval->defs)
        // {
        //     MachineOperand* spillOperand = new MachineOperand(MachineOperand::REG, interval->rreg);
        //     MachineOperand* stackOperand = new MachineOperand(MachineOperand::MEM, interval->disp);
        //     MachineInstruction* storeInst = new StoreMInstruction(def->getBlock(), spillOperand, stackOperand);
        //     def->getBlock()->InsertAfter(def, storeInst);
        // }


        interval->disp = -func->AllocSpace(4);
        auto off = new MachineOperand(MachineOperand::IMM, interval->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto use : interval->uses)
        {
            auto temp = new MachineOperand(*use);
            MachineOperand *operand = nullptr;
            if (operand)
            {
                auto inst = new LoadMInstruction(use->getParent()->getParent(), temp, fp, new MachineOperand(*operand));
                use->getParent()->insertBefore(inst);
            }
            else
            {
                auto inst = new LoadMInstruction(use->getParent()->getParent(), temp, fp, off);
                use->getParent()->insertBefore(inst);
            }
        }
         for (auto def : interval->defs)
        {
            auto temp = new MachineOperand(*def);
            MachineOperand *operand = nullptr;
            MachineInstruction *inst1 = nullptr, *inst = nullptr;
            if (operand)
            {
                inst = new StoreMInstruction(def->getParent()->getParent(), temp, fp, new MachineOperand(*operand));
            }
            else
            {
                inst = new StoreMInstruction(def->getParent()->getParent(), temp, fp, off);
            }
            if (inst1)
            {
                inst1->insertAfter(inst);
            }
            else
            {
                def->getParent()->insertAfter(inst);
            }
        }


    }
    

    

}

void LinearScan::expireOldIntervals(Interval *interval)
{
    // Todo
    /*
        foreach interval j in active, in order of increasing end point
            if endpoint[j] ≥ startpoint[i] then
                return
            remove j from active
            add register[j] to pool of free registers
    */
   auto it = active.begin();
    while (it != active.end())
    {
        if ((*it)->end >= interval->start)
        {
            return;
        }
        regs.push_back((*it)->rreg);
        it = active.erase(find(active.begin(), active.end(), *it));
        sort(regs.begin(), regs.end());
    }

}

void LinearScan::spillAtInterval(Interval *interval)
{
    // Todo
    /*
        spill ← last interval in active
        if endpoint[spill] > endpoint[i] then
            register[i] ← register[spill]
            location[spill] ← new stack location
            remove spill from active
            add i to active, sorted by increasing end point
        else
            location[i] ← new stack location

    */

   auto spill = active.back();
    if (spill->end > interval->end)
    {
        spill->spill = true;
        interval->rreg = spill->rreg;
        active.push_back(interval);
        sort(active.begin(), active.end(), compareEnd);
    }
    else
    {
        interval->spill = true;
    }


}

bool LinearScan::compareStart(Interval *a, Interval *b)
{
    return a->start < b->start;
}

bool LinearScan::compareEnd(Interval *a, Interval *b)
{
    return a->end < b->end;
}

int LinearScan::allocateStackSpace()
{
    // 实现栈空间分配逻辑
    static int stackOffset = 0;
    stackOffset -= 4; // 每次分配 4 字节的栈空间
    return stackOffset;
}