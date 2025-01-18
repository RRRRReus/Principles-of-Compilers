/**
 * linear scan register allocation
 */

#ifndef _LINEARSCAN_H__
#define _LINEARSCAN_H__
#include <set>
#include <map>
#include <vector>
#include <list>

class MachineUnit;
class MachineOperand;
class MachineFunction;


class LinearScan
{
private:
    struct Interval
    {
        int start;//区间的开始和结束位置。
        int end;
        bool spill; // 是否需要将虚拟寄存器溢出到内存。
        int disp;   // 在栈中的偏移量
        int rreg;   // 映射到的实际寄存器
        std::set<MachineOperand *> defs; //定义该寄存器的操作数集合。
        std::set<MachineOperand *> uses;//使用该寄存器的操作数集合。
    };
    MachineUnit *unit;
    MachineFunction *func;  //指向当前处理的 MachineFunction 的指针
    std::vector<int> regs;  //可用寄存器的列表
    std::map<MachineOperand *, std::set<MachineOperand *>> du_chains; //定义-使用链
    std::vector<Interval*> intervals;   //所有寄存器分配区间的列表。
    std::vector<Interval*> active; // 当前活跃的寄存器分配区间的列表

    static bool compareStart(Interval*a, Interval*b);   //比较区间的开始位置。
    static bool compareEnd(Interval *a, Interval *b);   //比较区间的结束位置。
    void expireOldIntervals(Interval *interval);
    void spillAtInterval(Interval *interval);
    void makeDuChains();
    void computeLiveIntervals();
    bool linearScanRegisterAllocation();
    void modifyCode();
    void genSpillCode();
    int allocateStackSpace();
public:
    LinearScan(MachineUnit *unit);
    void allocateRegisters();
};

#endif