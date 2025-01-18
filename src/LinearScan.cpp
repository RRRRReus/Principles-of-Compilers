#include <algorithm>
#include "LinearScan.h"
#include "MachineCode.h"
#include "LiveVariableAnalysis.h"

LinearScan::LinearScan(MachineUnit *unit)
{
    this->unit = unit;
    for (int i = 4; i < 11; i++)   //为了函数参数从5开始分配
        regs.push_back(i);        //可用寄存器的列表
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
            computeLiveIntervals();//计算活跃区间
            fprintf(stderr, "computeLiveIntervals\n");
            success = linearScanRegisterAllocation();//线性扫描寄存器分配
            fprintf(stderr, "linearScanRegisterAllocation\n");
            if (success)        // all vregs can be mapped to real regs
                modifyCode();//如果成功，修改代码以反映寄存器分配 (modifyCode)；否则，生成溢出代码 (genSpillCode)。
            else                // spill vregs that can't be mapped to real regs
                genSpillCode();//linearScanRegisterAllocation()发生了spillAtInterval()，生成溢出代码
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

bool LinearScan::linearScanRegisterAllocation() //将虚拟寄存器映射到实际的物理寄存器
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

    bool success = true;
    active.clear();
    regs.clear();   
    for (int i = 4; i < 11; i++)
    {
        regs.push_back(i);//重新初始化可用寄存器的列表
    }
    for (auto &i : intervals)   //遍历所有的活跃区间
    {
        expireOldIntervals(i);  //解放旧寄存器，过期旧区间
        //如果活跃区间列表 active 的长度等于可用寄存器的数量 R，则溢出
        if (regs.empty())
        {
            spillAtInterval(i);//没有可用寄存器则溢出
            success = false;
        }
        else
        {
            //从可用寄存器列表中取出一个寄存器
            i->rreg = regs.front(); 
            regs.erase(regs.begin());

            //将区间 i 加入到活跃区间列表中
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
        func->addSavedRegs(interval->rreg);//分配指定好的寄存器（这个函数中都用到了哪些寄存器）
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
        if(!interval->spill)//不需要spill的区间
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
        //并创建相应的操作数 off和fp
        auto off = new MachineOperand(MachineOperand::IMM, interval->disp);
        auto fp = new MachineOperand(MachineOperand::REG, 11);
        for (auto use : interval->uses)
        {
            auto temp = new MachineOperand(*use);

                // LoadMInstruction格式为    ldr r0, [fp, #4]
                auto inst = new LoadMInstruction(use->getParent()->getParent(), temp, fp, off);
                use->getParent()->insertBefore(inst);
        }
        
        for (auto def : interval->defs)
        {
            auto temp = new MachineOperand(*def);
            MachineInstruction *inst1 = nullptr, *inst = nullptr;

            // LoadMInstruction格式为    ldr r0, [fp, #4]
            inst = new StoreMInstruction(def->getParent()->getParent(), temp, fp, off);

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

   //从活动区间列表 active 中移除那些已经结束的区间，
   //并将它们占用的寄存器释放回可用寄存器列表 regs 中
   auto it = active.begin();
    while (it != active.end())
    {
        //如果当前活动区间的结束位置大于等于新区间 interval 的开始位置，
        //则停止处理，因为后续的区间都不会过期
        if ((*it)->end >= interval->start)
        {
            return;
        }
        regs.push_back((*it)->rreg);
        it = active.erase(find(active.begin(), active.end(), *it));//移除it，并开始下一次迭代
        sort(regs.begin(), regs.end());//重新排序
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
    if (spill->end > interval->end) //如果 spill 区间的结束位置大于 interval 区间的结束位置
    {
        spill->spill = true;    //需要溢出，设置 spill 为 true
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