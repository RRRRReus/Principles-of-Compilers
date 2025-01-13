#include "MachineCode.h"
extern FILE* yyout;

MachineOperand::MachineOperand(int tp, int val)
{
    this->type = tp;
    if(tp == MachineOperand::IMM)
        this->val = val;
    else 
        this->reg_no = val;
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
        if (this->label.substr(0, 2) == ".L")
            fprintf(yyout, "%s", this->label.c_str());
        else
            fprintf(yyout, "addr_%s", this->label.c_str());
    default:
        break;
    }
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
    // TODO: 
    // Complete other instructions
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
    case BinaryMInstruction::DIV:
        fprintf(yyout, "\tsdiv ");
        this->PrintCond();
        this->def_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[0]->output();
        fprintf(yyout, ", ");
        this->use_list[1]->output();
        fprintf(yyout, "\n");
        break;
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





    fprintf(yyout, "\tstr ");
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
    dst->setParent(this);
    src->setParent(this);
    
}

void MovMInstruction::output() 
{
    // TODO
    
    fprintf(yyout, "\tmov ");
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
    default:
        fprintf(yyout, "b ");
        break;
    }
    this->use_list[0]->output();
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


    fprintf(yyout, "\tcmp ");
    this->use_list[0]->output();
    fprintf(yyout, ", ");
    this->use_list[1]->output();
    fprintf(yyout, "\n");

}

StackMInstrcuton::StackMInstrcuton(MachineBlock* p, int op, 
    MachineOperand* src,
    int cond)
{
    // TODO

    this->parent = p;
    this->type = MachineInstruction::STACK;
    this->op = op;
    this->cond = cond;
    this->use_list.push_back(src);
    src->setParent(this);

}

void StackMInstrcuton::output()
{
    // TODO


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
    this->use_list[0]->output();
    fprintf(yyout, "\n");

}

MachineFunction::MachineFunction(MachineUnit* p, SymbolEntry* sym_ptr) 
{ 
    this->parent = p; 
    this->sym_ptr = sym_ptr; 
    this->stack_size = 0;
};

void MachineBlock::output()
{

    fprintf(yyout, ".L%d:\n", this->no);
    fprintf(stderr, "MachineBlock::output已输出基本块%d\n", this->no);
    for(auto iter : inst_list)
        iter->output();

    
}

void MachineFunction::output()
{
    const char *func_name = this->sym_ptr->toStr().c_str() + 1;
    fprintf(yyout, "\t.global %s\n", func_name);
    fprintf(yyout, "\t.type %s , %%function\n", func_name);
    fprintf(yyout, "%s:\n", func_name);
    // TODO
    /* Hint:
    *  1. Save fp
    *  2. fp = sp
    *  3. Save callee saved register
    *  4. Allocate stack space for local variable */
    
    // Traverse all the block in block_list to print assembly code.
    fprintf(stderr, "MachineFunction::output已输出函数%s\n", func_name);
    MachineInstruction* inst=nullptr;

    inst = new BinaryMInstruction(this->getBlocks()[0], BinaryMInstruction::SUB, new MachineOperand(MachineOperand::REG, 13), new MachineOperand(MachineOperand::REG, 13), new MachineOperand(MachineOperand::IMM, this->getStackSize()));
    this->getBlocks()[0]->InsertFront(inst);


    inst = new MovMInstruction(this->getBlocks()[0], -1, new MachineOperand(MachineOperand::REG, 11), new MachineOperand(MachineOperand::REG, 13));
    this->getBlocks()[0]->InsertFront(inst);


    // inst = new StackMInstrcuton(this->getBlocks()[0], StackMInstrcuton::PUSH, new MachineOperand(MachineOperand::REG, 11));
    // this->getBlocks()[0]->InsertFront(inst);

    for(auto iter : block_list)
        iter->output();

    
}

void MachineUnit::PrintGlobalDecl()
{
    // TODO:
    // You need to print global variable/const declarition code;
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

    fprintf(stderr, "MachineUnit::output已输出\n");
}



