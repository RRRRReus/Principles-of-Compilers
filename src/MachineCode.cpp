#include "MachineCode.h"
#include "Type.h"
#include <cstring>
#include <sstream>
#include <vector>
#include <iomanip>
extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
    if(tp == MachineOperand::IMM)//如果是立即数
        this->val = val;    //值为val
    else                    //如果不是立即数
        this->reg_no = val; //寄存器号为val
}

MachineOperand::MachineOperand(std::string label)
{
    this->type = MachineOperand::LABEL;
    this->label = label;
}

bool MachineOperand::operator==(const MachineOperand&a) const
{
    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

bool MachineOperand::operator<(const MachineOperand&a) const
{
    if(this->type == a.type)
    {
        if(this->type == IMM)
            return this->val < a.val;
        return this->reg_no < a.reg_no;
    }
    return this->type < a.type;

    if (this->type != a.type)
        return false;
    if (this->type == IMM)
        return this->val == a.val;
    return this->reg_no == a.reg_no;
}

void MachineOperand::PrintReg()
{
    switch (reg_no)
    {
    case 11:
        fprintf(yyout, "fp");
        break;
    case 13:
        fprintf(yyout, "sp");
        break;
    case 14:
        fprintf(yyout, "lr");
        break;
    case 15:
        fprintf(yyout, "pc");
        break;
    default:
        fprintf(yyout, "r%d", reg_no);
        break;
    }
}

void MachineOperand::output() 
{
    /* HINT：print operand
    * Example:
    * immediate num 1 -> print #1;
    * register 1 -> print r1;
    * lable addr_a -> print addr_a; */
    switch (this->type)
    {
    case IMM:
        fprintf(yyout, "#%d", this->val);
        break;
    case VREG:
        fprintf(yyout, "v%d", this->reg_no);
        break;
    case REG:
        PrintReg();
        break;
    case LABEL:
            // 变量输出 addr_<变量名>
            if (this->label.substr(0, 2) == ".L")
                fprintf(yyout, "%s", this->label.c_str());
            else
                fprintf(yyout, "=%s", this->label.c_str());  //原先是addr_变量名，现在是=变量名，不知道为何只有后者可以通过048
        break;
    default:
        break;
    }
}

std::string MachineOperand::outputDebug()
{
    std::string result;
    switch (this->type)
    {
    case IMM:
        result = "#" + std::to_string(this->val);
        break;
    case VREG:
        result = "v" + std::to_string(this->reg_no);
        break;
    case REG:
        switch (reg_no)
        {
        case 11:
            result = "fp";
            break;
        case 13:
            result = "sp";
            break;
        case 14:
            result = "lr";
            break;
        case 15:
            result = "pc";
            break;
        default:
            result = "r" + std::to_string(reg_no);
            break;
        }
        break;
    case LABEL:
        if (this->label.substr(0, 2) == ".L")
            result = this->label;
        else
            result = "addr_" + this->label;
        break;
    default:
        break;
    }
    return result;
}

bool MachineOperand::isValidImm() 
{
        // ARM 架构中，立即数必须是 0 到 255 之间的数，或者是通过旋转操作得到的数
        if (val >= 0 && val <= 255) {
            return true;
        }
        // 检查通过旋转操作得到的数
        for (int i = 0; i < 32; i += 2) {
            unsigned int rotated = (val >> i) | (val << (32 - i));
            if (rotated <= 255) {
                return true;
            }
        }
        return false;
}

void MachineInstruction::PrintCond()
{
    // TODO
    //OK???
    switch (cond)
    {
    case LT:
        fprintf(yyout, "lt");
        break;
    case LE:
        fprintf(yyout, "le");
        break;
    case GT:
        fprintf(yyout, "gt");
        break;
    case GE:
        fprintf(yyout, "ge");
        break;
    case EQ:
        fprintf(yyout, "eq");
        break;
    case NE:    
        fprintf(yyout, "ne");
        break;

    default:
        break;
    }
}



//在指令列表中的当前指令之前插入一个新的指令
void MachineInstruction::insertBefore(MachineInstruction *inst)
{
    auto &instructions = parent->getInsts();
    auto it = std::find(instructions.begin(), instructions.end(), this);
    instructions.insert(it, inst);
}
//在指令列表中的当前指令之后插入一个新的指令
void MachineInstruction::insertAfter(MachineInstruction *inst)
{
    auto &instructions = parent->getInsts();
    auto it = std::find(instructions.begin(), instructions.end(), this);
    instructions.insert(++it, inst);
}





BinaryMInstruction::BinaryMInstruction(
    MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::BINARY;
    this->op = op;  //运算符
    this->cond = cond;      //条件
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    src2->setParent(this);
}

void BinaryMInstruction::output() 
{
    fprintf(stderr, "已进入BinaryMInstruction::output函数\n");
    switch (this->op)
    {
    case BinaryMInstruction::ADD:
        fprintf(yyout, "\tadd ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::SUB:
        fprintf(yyout, "\tsub ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::MUL:
        fprintf(yyout, "\tmul ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::DIV: {
        // 使用 SDIV 指令计算商
        fprintf(yyout, "\tsdiv ");
        this->def_list[0]->output();  // 目标寄存器存储商值
        fprintf(yyout, ", ");
        this->use_list[0]->output();  // 被除数
        fprintf(yyout, ", ");
        this->use_list[1]->output();  // 除数
        fprintf(yyout, "\n");
        break;
    }
    case BinaryMInstruction::MOD: {
        // 使用 SDIV 和 MUL/SUB 实现取模
        // 1. 计算商值：temp1 = use_list[0] / use_list[1]
        fprintf(yyout, "\tsdiv r12, ");
        this->use_list[0]->output();  // 被除数
        fprintf(yyout, ", ");
        this->use_list[1]->output();  // 除数
        fprintf(yyout, "\n");

        // 2. 计算商乘积：temp2 = temp1 * use_list[1]
        fprintf(yyout, "\tmul r13, r12, ");
        this->use_list[1]->output();  // 除数
        fprintf(yyout, "\n");

        // 3. 计算余数：dst = use_list[0] - temp2
        fprintf(yyout, "\tsub ");
        this->def_list[0]->output();  // 存储余数的目标寄存器
        fprintf(yyout, ", ");
        this->use_list[0]->output();  // 被除数
        fprintf(yyout, ", r13\n");
        break;
    }
    case BinaryMInstruction::AND:
        fprintf(yyout, "\tand ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::OR:
        fprintf(yyout, "\torr ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    case BinaryMInstruction::XOR:
        fprintf(yyout, "\teor ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
    default:
        break;
    }
    fprintf(stderr, "已退出BinaryMInstruction::output函数\n");
}



LoadMInstruction::LoadMInstruction(MachineBlock* p,
    MachineOperand* dst, MachineOperand* src1, MachineOperand* src2,
    int cond)
{
    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src1);
    if (src2)
        this->use_list.push_back(src2);
    dst->setParent(this);
    src1->setParent(this);
    if (src2)
        src2->setParent(this);
}


void LoadMInstruction::output()
{
    fprintf(stderr, "已进入LoadMInstruction::output函数\n");
    fprintf(yyout, "\tldr ");
    this->def_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[0]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "[");

    this->use_list[0]->output();
    if( this->use_list.size() > 1 )
    {
        fprintf(yyout, ", ");
        this->use_list[1]->output();
    }

    if(this->use_list[0]->isReg()||this->use_list[0]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");
    fprintf(stderr, "已退出LoadMInstruction::output函数\n");
}

StoreMInstruction::StoreMInstruction(MachineBlock* p,
    MachineOperand* src1, MachineOperand* src2, MachineOperand* src3, 
    int cond)
{
    // TODO

    this->parent = p;
    this->type = MachineInstruction::LOAD;
    this->op = -1;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);


    if (src3)
    {
        this->use_list.push_back(src3);
        src3->setParent(this);
    }

}

void StoreMInstruction::output()
{
    // TODO



    fprintf(stderr, "已进入StoreMInstruction::output函数\n");

    // 判断是否为浮点数存储
    //if (use_list[0]->getSymbolEntry()->getType()->isAllFloat()) {
        // 如果是浮点数类型，使用 FST 指令
   //     fprintf(yyout, "\tfst ");
    //} else {
        // 否则，使用 STR 指令
        fprintf(yyout, "\tstr ");
    //}


    this->use_list[0]->output();
    fprintf(yyout, ", ");

    // Load immediate num, eg: ldr r1, =8
    if(this->use_list[1]->isImm())
    {
        fprintf(yyout, "=%d\n", this->use_list[0]->getVal());
        return;
    }

    // Load address
    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "[");

    this->use_list[1]->output();
    if( this->use_list.size() > 2 )
    {
        fprintf(yyout, ", ");
        this->use_list[2]->output();
    }

    if(this->use_list[1]->isReg()||this->use_list[1]->isVReg())
        fprintf(yyout, "]");
    fprintf(yyout, "\n");

}

MovMInstruction::MovMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, MachineOperand* src,
    int cond)
{
    // TODO


    this->parent = p;
    this->type = MachineInstruction::MOV;
    this->op = op;
    this->cond = cond;
    this->def_list.push_back(dst);
    this->use_list.push_back(src);
    fprintf(stderr, "mov的src->val是%d\n",src->getVal());
    fprintf(stderr, "mov的dst->val是%d\n",dst->getVal());

    dst->setParent(this);
    src->setParent(this);
    
}

void MovMInstruction::output() 
{
    // TODO
    switch (op)
    {
    case MovMInstruction::MOV:
        fprintf(yyout, "\tmov ");
        break;
    case MovMInstruction::EQ:
        fprintf(yyout, "\tmoveq ");
        break;
    case MovMInstruction::NE:
        fprintf(yyout, "\tmovne ");
        break;
    case MovMInstruction::LT:
        fprintf(yyout, "\tmovlt ");
        break;
    case MovMInstruction::LE:
        fprintf(yyout, "\tmovle ");
        break;
    case MovMInstruction::GT:
        fprintf(yyout, "\tmovgt ");
        break;
    case MovMInstruction::GE:
        fprintf(yyout, "\tmovge ");
        break;

    default:
        fprintf(yyout, "\tmov ");

        break;
    }

    this->def_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[0]->output();
    fprintf(yyout, "\n");

}

BranchMInstruction::BranchMInstruction(MachineBlock* p, int op, 
    MachineOperand* dst, 
    int cond)
{
    // TODO


    this->parent = p;
    this->type = MachineInstruction::BRANCH;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(dst);
    dst->setParent(this);

}

void BranchMInstruction::output()
{
    // TODO

    fprintf(stderr, "已进入BranchMInstruction::output函数\n");

    if(this->op==BranchMInstruction::BX)
    {
        std::vector<MachineOperand*> stack_list;
        for (int regno : this->parent->getParent()->getSavedRegs())
        {
            stack_list.push_back(new MachineOperand(MachineOperand::REG, regno));
        }
        MachineInstruction* cur_inst = new StackMInstrcuton(nullptr, StackMInstrcuton::POP, stack_list);
        cur_inst->output();
    }

    fprintf(yyout, "\t");
    switch (this->op)
    {
    case MachineInstruction::EQ:
        fprintf(yyout, "beq ");
        break;
    case MachineInstruction::NE:
        fprintf(yyout, "bne ");
        break;
    case MachineInstruction::LT:
        fprintf(yyout, "blt ");
        break;
    case MachineInstruction::LE:
        fprintf(yyout, "ble ");
        break;
    case MachineInstruction::GT:
        fprintf(yyout, "bgt ");
        break;
    case MachineInstruction::GE:
        fprintf(yyout, "bge ");
        break;
    case BranchMInstruction::BX:
        fprintf(yyout, "bx ");
        break;
    case BranchMInstruction::BL:
        fprintf(yyout, "bl ");
        break;
    default:
        fprintf(yyout, "b ");
        break;
    }
    if(this->op!=BranchMInstruction::BL)
        this->use_list[0]->output();
    else
        fprintf(yyout, "%s", this->use_list[0]->getLabel().c_str());
    fprintf(yyout, "\n");

}

CmpMInstruction::CmpMInstruction(MachineBlock* p, 
    MachineOperand* src1, MachineOperand* src2, 
    int cond)
{
    // TODO


    this->parent = p;
    this->type = MachineInstruction::CMP;
    this->op = -1;
    this->cond = cond;
    this->use_list.push_back(src1);
    this->use_list.push_back(src2);
    src1->setParent(this);
    src2->setParent(this);

}

void CmpMInstruction::output()
{
    // TODO
    // Jsut for reg alloca test
    // delete it after test

    fprintf(stderr, "已进入CmpMInstruction::output函数\n");
    fprintf(stderr,"use_list[0]->val:%d\n",this->use_list[0]->getVal());
    fprintf(stderr,"use_list[1]->val:%d\n",this->use_list[1]->getVal());
    
    fprintf(yyout, "\tcmp ");
    this->use_list[0]->output();
    fprintf(stderr, "已输出第一个操作数\n");
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(stderr, "已输出第二个操作数\n");
    fprintf(yyout, "\n");
    fprintf(stderr, "已退出CmpMInstruction::output函数\n");

}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, 
    MachineOperand* src,
    int cond)
{
    // TODO
    fprintf(stderr, "已进入StackMInstrcuton::StackMInstrcuton函数\n");
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(src);
    src->setParent(this);

}

StackMInstrcuton::StackMInstrcuton(MachineBlock *p, int op, std::vector<MachineOperand *> stack_list, int cond)
{
    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    this->use_list = stack_list;
    for (auto src : stack_list)
    {
        src->setParent(this);
    }
}

void StackMInstrcuton::output()
{
    // TODO
    if(use_list.size()==0)
        return;
    fprintf(stderr, "已进入StackMInstrcuton::output函数\n");

    fprintf(yyout, "\t");
    switch (this->op)
    {
    case StackMInstrcuton::PUSH:
        fprintf(yyout, "push ");
        break;
    case StackMInstrcuton::POP:
        fprintf(yyout, "pop ");
        break;
    default:
        break;
    }
    fprintf(yyout, "{");
    int len = int(this->use_list.size());
    for (int i = 0; i < len; i++)
    {
        this->use_list[i]->output();
        if (i != len - 1)
            fprintf(yyout, ", ");
    }
    fprintf(yyout, "}\n");

}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p; 
    this->sym_ptr = sym_ptr; 
    this->stack_size = 0;
    this->saved_regs = {11, 14};
};

void MachineBlock::output()
{

    fprintf(yyout, ".L%d:\n", this->no);
    fprintf(stderr, "MachineBlock::output已输出基本块%d\n", this->no);
    for(auto iter : inst_list)
    {
        if(iter==nullptr)
            fprintf(stderr, "iter为空\n");
        else
            iter->output();
            
      
        
    }

    
}

void MachineFunction::output()
{
    const char *func_name = this->sym_ptr->toStr().c_str() + 1;
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);

        // 保存被调用者保存的寄存器（包括 fp 和 lr）
    std::vector<MachineOperand*> stack_list;
    MachineInstruction* push_inst = nullptr;
    for (int regno : saved_regs)
    {
        fprintf(stderr, "正在保存 regno: %d\n", regno);
        stack_list.push_back(new MachineOperand(MachineOperand::REG, regno));
    }
    if (!stack_list.empty())
    {
        push_inst = new StackMInstrcuton(
            this->getBlocks()[0], StackMInstrcuton::PUSH, stack_list); // push {fp, lr, ...}
        this->getBlocks()[0]->InsertFront(push_inst);
    }
    fprintf(stderr, "stacklist 中现在有 %ld\n", stack_list.size());

    // 更新 fp = sp
    MachineInstruction* first_inst = new MovMInstruction(
        this->getBlocks()[0], MovMInstruction::MOV,
        new MachineOperand(MachineOperand::REG, 11),  // fp
        new MachineOperand(MachineOperand::REG, 13)); // sp
    this->getBlocks()[0]->InsertAfter(first_inst, push_inst);

    // 分配栈空间 (用于局部变量和溢出的参数)
    MachineInstruction* sub_inst = nullptr;
    if (this->stack_size > 0)
    {
        sub_inst = new BinaryMInstruction(
            this->getBlocks()[0], BinaryMInstruction::SUB,
            new MachineOperand(MachineOperand::REG, 13), // sp
            new MachineOperand(MachineOperand::REG, 13), // sp
            new MachineOperand(MachineOperand::IMM, this->getStackSize()));
        this->getBlocks()[0]->InsertAfter(sub_inst, first_inst);
    }



    // // 处理函数参数，将 `r0-r3` 和栈上的参数存储到分配的局部变量中
    // int current_offset = 0; // 参数存储的初始偏移量
    // int param_index = 0;

    // for (long unsigned int i = 0; i < this->params.size(); i++)
    // {
    //     if (param_index < 4)
    //     {
    //         // 参数在 r0-r3 中，直接存储到栈中
    //         current_offset -= 4;
    //         auto store_inst = new StoreMInstruction(
    //             this->getBlocks()[0],
    //             new MachineOperand(MachineOperand::REG, param_index),  // r0, r1, r2, r3
    //             new MachineOperand(MachineOperand::REG, 13),          // sp
    //             new MachineOperand(MachineOperand::IMM, current_offset));
    //         this->getBlocks()[0]->InsertAfter(store_inst, push_inst);
    //         push_inst = store_inst; // 更新插入点
    //     }
    //     else
    //     {
    //         // 参数已经在栈中，计算其偏移，无需额外存储
    //         current_offset -= 4;
    //     }
    //     param_index++;
    // }

    // 遍历输出每个基本块
    for (auto iter : block_list)
        iter->output();

    fprintf(stderr, "MachineFunction::output 已输出函数 %s\n", func_name);
}


std::string convertToLongFormat(const std::string& initValue) {
    std::istringstream iss(initValue);
    std::string token;
    std::string result;
    
    // 解析字符串并处理每个元素
    while (std::getline(iss, token, ',')) {
        fprintf(stderr, "当前token是%s\n",token.c_str());
        // 提取整数值
        size_t pos = token.find("i32");
        if (pos != std::string::npos) {
            std::string value = token.substr(pos + 3); // 获取 i32 后的数字
            int num = std::stoi(value);
            
            // 格式化输出并添加到结果字符串中
            std::ostringstream formatted;
            formatted << "\t.long\t" << num << "\t\t@ 0x" << std::hex << num << std::dec;
            result += formatted.str() + "\n";
        }
    }

    return result;
}
void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // You need to print global variable/const declarition code;


     // 遍历所有全局变量和常量
    for (auto global : global_list)
    {
        fprintf(stderr,"当前%s是否为全局常量：%d\n",global->getLabel().c_str(),global->isGlobalConst());
        if (global->isLabel()) // 确保是全局变量的 LABEL 类型
        {
            // 获取全局变量/常量的名称
            std::string varName = global->getLabel();

            // 判断是否为常量
            bool isConst = global->isGlobalConst(); // 假设 `MachineOperand` 有 `isConst()` 方法

            // 获取初始值
            std::string initValue = global->getInitialValue(); // 假设全局变量直接存储初始值
            if (initValue.empty()) {
                initValue = "0"; // 默认初始值
            }

            // 检查初始值是否是浮点数（假设浮点数用十六进制表示）
            bool isHexFloat = initValue.find("0x") == 0;

            // 根据是否为常量选择段类型
            if (isConst)
            {
                fprintf(yyout, "\t.section\t.rodata\n"); // 常量放在只读数据段
            }
            else
            {
                fprintf(yyout, "\t.data\n"); // 变量放在数据段
            }

            // 输出类型声明和全局符号
            fprintf(yyout, "\t.type\t%s,%%object\t\t@ @%s\n", varName.c_str(), varName.c_str());
            fprintf(yyout, "\t.global\t%s\n", varName.c_str());

            // 输出对齐和初值
            if (isHexFloat)
            {
                // 浮点数：对齐到 8 字节
                fprintf(yyout, "\t.p2align\t3\n");
                fprintf(yyout, "%s:\n", varName.c_str());
                fprintf(yyout, "\t.quad\t%s\t\t@ %s\n", initValue.c_str(), initValue.c_str());
                fprintf(yyout, "\t.size\t%s, 8\n", varName.c_str());
            }
           else
            {
                // 整数：对齐到 4 字节
                fprintf(yyout, "\t.p2align\t2\n");
                fprintf(yyout, "%s:\n", varName.c_str());
                if(global->getSymbolEntry()->getType()->isInt())
                {
                    // 将 initValue 转换为整数
                    int intValue = std::stoi(initValue);

                    // 以十六进制格式输出
                    fprintf(yyout, "\t.long\t%d\t\t@ 0x%x\n", intValue, intValue);
                    fprintf(yyout, "\t.size\t%s, 4\n", varName.c_str());
                }
                else
                {
                    if(initValue=="zeroinitializer")
                    {
                        fprintf(yyout, "\t.zero\t%d\n",dynamic_cast<IntArrayType*>(global->getSymbolEntry()->getType())->getStackSize());
                    }
                    else
                        fprintf(yyout, "%s", convertToLongFormat(initValue).c_str());
                    fprintf(yyout, "\t.size\t%s, %d\n", varName.c_str(),dynamic_cast<IntArrayType*>(global->getSymbolEntry()->getType())->getStackSize());
                }

            }

            // 如果是变量，生成地址符号
                std::string addrName = "addr_" + varName;
                fprintf(yyout, "\t.global\t%s\n", addrName.c_str());
                fprintf(yyout, "%s:\n", addrName.c_str());
                fprintf(yyout, "\t.word\t%s\n", varName.c_str());

            // 空行分隔
            fprintf(yyout, "\n");
        }
    }
}

void MachineUnit::output()
{
    // TODO
    /* Hint:
    * 1. You need to print global variable/const declarition code;
    * 2. Traverse all the function in func_list to print assembly code;
    * 3. Don't forget print bridge label at the end of assembly code!! */
    fprintf(yyout, "\t.arch armv8-a\n");
    fprintf(yyout, "\t.arch_extension crc\n");
    fprintf(yyout, "\t.arm\n");
    PrintGlobalDecl();
    for(auto iter : func_list)
        iter->output();
    
    // //打印全局变量
    // for(auto iter : global_list)
    //     iter->output();

    
    //常量呢？？？？



    // fprintf(yyout, "\t.global getint\n");
    // //fprintf(yyout, "\t.type getint, %%function\n");

    // fprintf(yyout, "\t.global putint\n");
    // //fprintf(yyout, "\t.type putint, %%function\n");

    // fprintf(yyout, "\t.global putch\n");
    // //fprintf(yyout, "\t.type putch, %%function\n");

    // fprintf(yyout, "\t.global putarray\n");
    // //fprintf(yyout, "\t.type putarray, %%function\n");

    // fprintf(yyout, "\t.global getfarray\n");

    // //fprintf(yyout, "\t.type getfarray, %%function\n");

    // fprintf(yyout, "\t.global putfarray\n");
    // //fprintf(yyout, "\t.type putfarray, %%function\n");

    // fprintf(yyout, "\t.global getch\n");
    // //fprintf(yyout, "\t.type getch, %%function\n");

    // fprintf(yyout, "\t.global putfloat\n");
    // //fprintf(yyout, "\t.type putfloat, %%function\n");

    // fprintf(yyout, "\t.global getfloat\n");
    // //fprintf(yyout, "\t.type getfloat, %%function\n");


    // fprintf(yyout, "\t.extern getint\n");
    // //fprintf(yyout, "\t.type getint, %%function\n");

    // fprintf(yyout, "\t.extern putint\n");
    // //fprintf(yyout, "\t.type putint, %%function\n");

    // fprintf(yyout, "\t.extern putch\n");
    // //fprintf(yyout, "\t.type putch, %%function\n");

    // fprintf(yyout, "\t.extern putarray\n");
    // //fprintf(yyout, "\t.type putarray, %%function\n");

    // fprintf(yyout, "\t.extern getfarray\n");

    // //fprintf(yyout, "\t.type getfarray, %%function\n");

    // fprintf(yyout, "\t.extern putfarray\n");
    // //fprintf(yyout, "\t.type putfarray, %%function\n");

    // fprintf(yyout, "\t.extern getch\n");
    // //fprintf(yyout, "\t.type getch, %%function\n");

    // fprintf(yyout, "\t.extern putfloat\n");
    // //fprintf(yyout, "\t.type putfloat, %%function\n");

    // fprintf(yyout, "\t.extern getfloat\n");
    // //fprintf(yyout, "\t.type getfloat, %%function\n");






	// fprintf(yyout,".ident	\"Ubuntu clang version 14.0.0-1ubuntu1.1\"\n");
	// fprintf(yyout,".section	\".note.GNU-stack\",\"\",%%progbits\n");
	// fprintf(yyout,".addrsig\n");
	// fprintf(yyout,".addrsig_sym putint\n");
	// fprintf(yyout,".addrsig_sym putch\n");
	// fprintf(yyout,".eabi_attribute	30, 6	@ Tag_ABI_optimization_goals\n");



    fprintf(stderr, "MachineUnit::output已输出\n");
}



