#include "Instruction.h"
#include "BasicBlock.h"
#include <iostream>
#include <sstream>
#include <string>
#include "Function.h"
#include "Type.h"
extern FILE* yyout;

Instruction::Instruction(unsigned instType, BasicBlock *insert_bb)
{
    prev = next = this;
    opcode = -1;
    this->instType = instType;
    if (insert_bb != nullptr)
    {
        insert_bb->insertBack(this);
        parent = insert_bb;
    }
}

Instruction::~Instruction()
{
    parent->remove(this);
}

BasicBlock *Instruction::getParent()
{
    return parent;
}

bool Instruction::haveSameOperator(Instruction *inst)
{
    //fprintf(stderr,"看是不是相同的操作符\n");
    if(this->instType!=inst->instType)
        return false;
    //fprintf(stderr,"看是不是相同的二元操作符\n");
    if(this->instType == BINARY&&inst->instType == BINARY)
    {
        if(dynamic_cast<BinaryInstruction*>(this)->opcode == dynamic_cast<BinaryInstruction*>(inst)->opcode)
        {
            //fprintf(stderr,"sizaizh>jhfladkjhgflaanrfggjk;lnnds;jsfhg\n");
            return true;
        }
            
        else
        {
            return false;       
        }
            
    }
    //fprintf(stderr,"看是不是相同的比较符\n");
    if(this->instType == CMP&&inst->instType == CMP)
    {
        if(dynamic_cast<CmpInstruction*>(this)->opcode == dynamic_cast<CmpInstruction*>(inst)->opcode)
            return true;
        else
            return false;
    }

    return false;
}

bool Instruction::canBeSwapped()
{
    if(this->instType == BINARY)
    {
        if(dynamic_cast<BinaryInstruction*>(this)->opcode == BinaryInstruction::ADD || dynamic_cast<BinaryInstruction*>(this)->opcode == BinaryInstruction::MUL)
            return true;
        else
            return false;
    }
    if(this->instType == CMP)
    {
        if(dynamic_cast<CmpInstruction*>(this)->opcode == CmpInstruction::E || dynamic_cast<CmpInstruction*>(this)->opcode == CmpInstruction::NE)
            return true;
        else
            return false;
    }
    return false;
    
}

void Instruction::setParent(BasicBlock *bb)
{
    parent = bb;
}

void Instruction::setNext(Instruction *inst)
{
    next = inst;
}

void Instruction::setPrev(Instruction *inst)
{
    prev = inst;
}

Instruction *Instruction::getNext()
{
    return next;
}

Instruction *Instruction::getPrev()
{
    return prev;
}

BinaryInstruction::BinaryInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb) : Instruction(BINARY, insert_bb)
{
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

BinaryInstruction::~BinaryInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void BinaryInstruction::output() const
{
    std::string s1, s2, s3, op, type;//s1为目的操作数，s2为源操作数1，s3为源操作数2，op为操作符，type为类型
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[0]->getType()->toStr();
    fprintf(stderr, "看这里的type是%s\n",type.c_str());
    switch (opcode)
    {
    case XOR:
        op = "xor";
        fprintf(stderr, "进入XOR,type是%s\n",type.c_str());
        break;
    case AND:
        op = "and";
        break;
    case OR:
        op = "or";
        break;

    case ADD:
        if(type == "float")
            op = "fadd";
        else
            op = "add";
        break;
    case SUB:
        if(type == "float")
            op = "fsub";
        else
            op = "sub";
        break;
    case MUL:
        if(type == "float")
            op = "fmul";
        else
            op = "mul";
        break;
    case DIV:
        if(type == "float")
            op = "fdiv";
        else
            op = "sdiv";
        break;
    case MOD:
        op = "srem";
        break;
        
    default:
        break;
    }
    fprintf(yyout, "  %s = %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

bool BinaryInstruction::canBeCalculated()
{
    
    if(operands[1]->getSymbolEntry()->isConstant() && operands[2]->getSymbolEntry()->isConstant())
    {
        fprintf(stderr,"oooop是%d\n",opcode);

        ConstantSymbolEntry *val1 = dynamic_cast<ConstantSymbolEntry *>(operands[1]->getSymbolEntry());
        ConstantSymbolEntry *val2 = dynamic_cast<ConstantSymbolEntry *>(operands[2]->getSymbolEntry());
        if(val1->getType()->isLongLong()||val2->getType()->isLongLong())
        {
            fprintf(stderr,"LONGLONG超限\n");
            return false;
        }
        if(val1->getType()->isInt())
        {

            int v1 = (val1)->getValue();
            int v2 = (val2)->getValue();
            fprintf(stderr,"op是%d\n",opcode);
            fprintf(stderr,"v1是%d\n",v1);
            fprintf(stderr,"v2是%d\n",v2);

            if(v1==-2147483648||v2==-2147483648)
            {
                fprintf(stderr,"运算超限\n");
                return false;
            }
            if(v1==2147483647||v2==2147483647)
            {
                fprintf(stderr,"运算超限\n");
                return false;
            }

        }

        return true;
    }
    return false;
}

Operand *BinaryInstruction::CalculatedResult()
{
    if(canBeCalculated())
    {
        ConstantSymbolEntry *val1 = dynamic_cast<ConstantSymbolEntry *>(operands[1]->getSymbolEntry());
        ConstantSymbolEntry *val2 = dynamic_cast<ConstantSymbolEntry *>(operands[2]->getSymbolEntry());
        ConstantSymbolEntry *result = new ConstantSymbolEntry(operands[0]->getType(), 0);
        if(val1->getType()->isInt())
        {
            int v1 = (val1)->getValue();
            int v2 = (val2)->getValue();
            int res = 0;
            switch (opcode)
            {
            case ADD:
                res = v1 + v2;
                break;
            case SUB:
                res = v1 - v2;
                break;
            case MUL:
                res = v1 * v2;
                break;
            case DIV:
                res = v1 / v2;
                break;
            case MOD:
                res = v1 % v2;
                break;
            case AND:
                res = v1 & v2;
                break;
            case OR:
                res = v1 | v2;
                break;
            case XOR:
                res = v1 ^ v2;
                break;
            default:
                break;
            }
            result->setIntValue(res);
        }
        else if(val1->getType()->isFloat())
        {
            float v1 = (val1)->getFloatValue();
            float v2 = (val2)->getFloatValue();
            float res = 0.0f;
            switch (opcode)
            {
            case ADD:
                res = v1 + v2;
                break;
            case SUB:
                res = v1 - v2;
                break;
            case MUL:
                res = v1 * v2;
                break;
            case DIV:
                res = v1 / v2;
                break;
            default:
                break;
            }
            result->setFloatValue(res);
        }
            return new Operand(result);
        }
    return nullptr;
}

CmpInstruction::CmpInstruction(unsigned opcode, Operand *dst, Operand *src1, Operand *src2, BasicBlock *insert_bb): Instruction(CMP, insert_bb){
    this->opcode = opcode;
    operands.push_back(dst);
    operands.push_back(src1);
    operands.push_back(src2);
    dst->setDef(this);
    src1->addUse(this);
    src2->addUse(this);
}

CmpInstruction::~CmpInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
    operands[2]->removeUse(this);
}

void CmpInstruction::output() const
{
    std::string s1, s2, s3, op, type;
    s1 = operands[0]->toStr();
    s2 = operands[1]->toStr();
    s3 = operands[2]->toStr();
    type = operands[1]->getType()->toStr();//能到这里来，说明两个操作数一定已经是同一种类型的了
    switch (opcode)
    {
    case E:
        op = "eq";
        if(type == "float")
            op = "oeq";
        break;
    case NE:
        op = "ne";
        if(type == "float")
            op = "one";
        break;
    case L:
        op = "slt";
        if(type == "float")
            op = "olt";
        break;
    case LE:
        op = "sle";
        if(type == "float")
            op = "ole";
        break;
    case G:
        op = "sgt";
        if(type == "float")
            op = "ogt";
        break;
    case GE:
        op = "sge";
        if(type == "float")
            op = "oge";
        break;
    default:
        op = "";
        break;
    }

    if(type == "float")
        fprintf(yyout, "  %s = fcmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
    else
    fprintf(yyout, "  %s = icmp %s %s %s, %s\n", s1.c_str(), op.c_str(), type.c_str(), s2.c_str(), s3.c_str());
}

bool CmpInstruction::canBeCalculated()
{
    if(operands[1]->getSymbolEntry()->isConstant() && operands[2]->getSymbolEntry()->isConstant())
        return true;
    return false;
}

Operand *CmpInstruction::CalculatedResult()
{
    if(canBeCalculated())
    {
        ConstantSymbolEntry *val1 = dynamic_cast<ConstantSymbolEntry *>(operands[1]->getSymbolEntry());
        ConstantSymbolEntry *val2 = dynamic_cast<ConstantSymbolEntry *>(operands[2]->getSymbolEntry());
        ConstantSymbolEntry *result = new ConstantSymbolEntry(operands[0]->getType(), 0);
        if(val1->getType()->isInt())
        {
            int v1 = (val1)->getValue();
            int v2 = (val2)->getValue();
            int res = 0;
            switch (opcode)
            {
            case E:
                res = v1 == v2;
                break;
            case NE:
                res = v1 != v2;
                break;
            case L:
                res = v1 < v2;
                break;
            case LE:
                res = v1 <= v2;
                break;
            case G:
                res = v1 > v2;
                break;
            case GE:
                res = v1 >= v2;
                break;
            default:
                break;
            }
            result->setIntValue(res);
        }
        else if(val1->getType()->isFloat())
        {
            float v1 = (val1)->getFloatValue();
            float v2 = (val2)->getFloatValue();
            int res = 0;
            switch (opcode)
            {
            case E:
                res = v1 == v2;
                break;
            case NE:
                res = v1 != v2;
                break;
            case L:
                res = v1 < v2;
                break;
            case LE:
                res = v1 <= v2;
                break;
            case G:
                res = v1 > v2;
                break;
            case GE:
                res = v1 >= v2;
                break;
            default:
                break;
            }
            result->setIntValue(res);
        }
        return new Operand(result);
    }
    return nullptr;
}

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    //fprintf(stderr,"UncondBrInstruction::UncondBrInstruction\n");
    branch = to;
}

void UncondBrInstruction::output() const
{
    fprintf(yyout, "  br label %%B%d\n", branch->getNo());
}

void UncondBrInstruction::setBranch(BasicBlock *bb)
{
    branch = bb;
}

BasicBlock *UncondBrInstruction::getBranch()
{
    return branch;
}

CondBrInstruction::CondBrInstruction(BasicBlock*true_branch, BasicBlock*false_branch, Operand *cond, BasicBlock *insert_bb) : Instruction(COND, insert_bb){
    
    this->true_branch = true_branch;
    this->false_branch = false_branch;
    cond->addUse(this);
    operands.push_back(cond);
    //fprintf(stderr,"CondBrInstruction::CondBrInstruction\n");
}

CondBrInstruction::~CondBrInstruction()
{
    operands[0]->removeUse(this);
}

void CondBrInstruction::output() const
{
    std::string cond, type;
    cond = operands[0]->toStr();
    type = operands[0]->getType()->toStr();
    int true_label = true_branch->getNo();
    int false_label = false_branch->getNo();
    fprintf(yyout, "  br %s %s, label %%B%d, label %%B%d\n", type.c_str(), cond.c_str(), true_label, false_label);
}

void CondBrInstruction::setFalseBranch(BasicBlock *bb)
{
    false_branch = bb;
}

BasicBlock *CondBrInstruction::getFalseBranch()
{
    return false_branch;
}

void CondBrInstruction::setTrueBranch(BasicBlock *bb)
{
    true_branch = bb;
}

BasicBlock *CondBrInstruction::getTrueBranch()
{
    return true_branch;
}

RetInstruction::RetInstruction(Operand *src, BasicBlock *insert_bb) : Instruction(RET, insert_bb)
{
    if(src != nullptr)
    {
        operands.push_back(src);
        src->addUse(this);
    }
}

RetInstruction::~RetInstruction()
{
    if(!operands.empty())
        operands[0]->removeUse(this);
}

void RetInstruction::output() const
{
    if(operands.empty())
    {
        fprintf(yyout, "  ret void\n");
    }
    else
    {
        std::string ret, type;
        ret = operands[0]->toStr();
        type = operands[0]->getType()->toStr();
        fprintf(yyout, "  ret %s %s\n", type.c_str(), ret.c_str());
    }
}

AllocaInstruction::AllocaInstruction(Operand *dst, SymbolEntry *se, BasicBlock *insert_bb) : Instruction(ALLOCA, insert_bb)
{
    operands.push_back(dst);//将dst加入操作数列表
    dst->setDef(this);//设置dst的定义
    this->se = se;
}

AllocaInstruction::~AllocaInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
}

void AllocaInstruction::output() const
{
    fprintf(stderr,"进入AllocaInstruction::output函数\n");
    std::string dst, type;
    dst = operands[0]->toStr();
    type = se->getType()->toStr();
    fprintf(yyout, "  %s = alloca %s, align 4\n", dst.c_str(), type.c_str());
}

LoadInstruction::LoadInstruction(Operand *dst, Operand *src_addr, BasicBlock *insert_bb) : Instruction(LOAD, insert_bb)
{
    operands.push_back(dst);//将dst加入操作数列表
    operands.push_back(src_addr);//将src_addr加入操作数列表
    dst->setDef(this);
    src_addr->addUse(this);
}

LoadInstruction::~LoadInstruction()
{
    operands[0]->setDef(nullptr);//将dst的定义设为空
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void LoadInstruction::output() const
{
    //fprintf(stderr,"进入LoadInstruction::output函数\n");
    std::string dst = operands[0]->toStr();//目的操作数
    std::string src = operands[1]->toStr();//源操作数
    std::string src_type;
    std::string dst_type;
    dst_type = operands[0]->getType()->toStr();//目的操作数的类型
    src_type = operands[1]->getType()->toStr();
    
    Type *Element=dynamic_cast<PointerType*>(operands[1]->getType())->getValueType();
    if(Element->isIntArray())
    {
        Type *newdst=new PointerType(TypeSystem::intType);
        src_type=newdst->toStr();
    }
        if(Element->isFloatArray())
    {
        Type *newdst=new PointerType(TypeSystem::floatType);
        src_type=newdst->toStr();

    }

    fprintf(yyout, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());
    //fprintf(stderr, "  %s = load %s, %s %s, align 4\n", dst.c_str(), dst_type.c_str(), src_type.c_str(), src.c_str());

}


StoreInstruction::StoreInstruction(Operand *dst_addr, Operand *src, BasicBlock *insert_bb) : Instruction(STORE, insert_bb)
{
    operands.push_back(dst_addr);
    operands.push_back(src);
    dst_addr->addUse(this);
    src->addUse(this);
    dst_addr->storeInsts.push_back(this);
}

StoreInstruction::~StoreInstruction()
{
    operands[0]->removeUse(this);
    operands[1]->removeUse(this);
}

void StoreInstruction::output() const
{
    fprintf(stderr, "进入StoreInstruction::output函数\n");
    std::string dst = operands[0]->toStr();
    std::string src = operands[1]->toStr();

    std::string dst_type = operands[0]->getType()->toStr();
    std::string src_type = operands[1]->getType()->toStr();

    if(dynamic_cast<PointerType*>(operands[0]->getType())!=nullptr)
    {
        Type *Element=dynamic_cast<PointerType*>(operands[0]->getType())->getValueType();
        if(Element->isIntArray())
        {
            Type *newdst=new PointerType(TypeSystem::intType);
            dst_type=newdst->toStr();
        }
            if(Element->isFloatArray())
        {
            Type *newdst=new PointerType(TypeSystem::floatType);
            dst_type=newdst->toStr();

        }

    }
    fprintf(yyout, "  store %s %s, %s %s, align 4\n", src_type.c_str(), src.c_str(), dst_type.c_str(), dst.c_str());
    //把src存给dst，后面为被赋值的
}

//函数调用命令
CallInstruction::CallInstruction(Operand *dst, IdentifierSymbolEntry *funcSE, const std::vector<Operand *> &args, BasicBlock *insert_bb)
    : Instruction(CALL, insert_bb), funcSE(funcSE)
{
    if (dst != nullptr) {
        operands.push_back(dst);
        dst->setDef(this);
    }
    for (auto arg : args) {
        operands.push_back(arg);
        arg->addUse(this);
    }
}

//函数调用命令2
CallInstruction::CallInstruction(Operand *dst, FunctionSymbolEntry *library_funcSE, const std::vector<Operand *> &args, BasicBlock *insert_bb)
    : Instruction(CALL, insert_bb), library_funcSE(library_funcSE)
{
    if (dst != nullptr) {
        operands.push_back(dst);
        dst->setDef(this);
    }
    for (auto arg : args) {
        operands.push_back(arg);
        arg->addUse(this);
    }
}


CallInstruction::~CallInstruction() {

}

void CallInstruction::output() const
{
    // 输出指令的字符串表示
    // 这里可以根据需要实现具体的输出逻辑
    std::string dst = operands[0]->toStr();//返回值操作数
    std::string func;
    std::string retType;
    if(funcSE==nullptr){
        func = library_funcSE->toStr();//函数名
        retType= dynamic_cast<FunctionType*>(library_funcSE->getType())->getRetType()->toStr();//由符号表获取返回值类型
    }
        

    else{
        func = funcSE->toStr();//函数名
        retType= dynamic_cast<FunctionType*>(funcSE->getType())->getRetType()->toStr();//由符号表获取返回值类型
    }
        
    //Type* retType=funcSE->getType();//返回值类型
    
    std::vector<std::string> args;//实参字符串列表
    std::vector<std::string> args_type;//实参类型列表
    for(long unsigned int i = 1; i < operands.size(); i++)
    {
        args.push_back(operands[i]->toStr());//将实参加入到args中
        args_type.push_back(operands[i]->getType()->toStr());//将实参类型加入到args_type中
    }
    if(retType!="void")
        fprintf(yyout, "  %s = call %s %s(", dst.c_str(), retType.c_str(), func.c_str()); 
    else
    {
        fprintf(yyout, "  call %s %s(" ,retType.c_str(), func.c_str()); 

    }

     // 输出实参
    for (size_t i = 0; i < args.size(); i++)
    {
        if (i > 0)
        {
            fprintf(yyout, ", ");
        }
        fprintf(yyout, "%s %s", args_type[i].c_str(), args[i].c_str());//函数实惨类型 + 实参
    }

    fprintf(yyout, ")\n");

}

void CallInstruction::genMachineCode(AsmBuilder *builder)
{

    fprintf(stderr,"进入CallInstruction::genMachineCode函数\n");
    //auto cur_func = builder->getFunction();
    auto cur_bb = builder->getBlock();
    MachineInstruction *call_inst = nullptr;
    if(this->operands.size()>5)
    {
        fprintf(stderr,"参数个数超过4个\n");
        //再说
    }
    else
    {


        for(int i=0;i<int(this->operands.size()-1);i++)
        {
            fprintf(stderr,"this->operands[i+1]->toStr()是%s\n",this->operands[i+1]->toStr().c_str());
            call_inst =new MovMInstruction(cur_bb,-1,genMachineReg(i),genMachineOperand(this->operands[i+1]));
            cur_bb->InsertInst(call_inst);
        }
        std::string funcname;
        if(funcSE)
            funcname=funcSE->getName();
        else
        {
            funcname=library_funcSE->getName();
        }
        call_inst = new BranchMInstruction(cur_bb,BranchMInstruction::BL,new MachineOperand(funcname));
        cur_bb->InsertInst(call_inst);
        fprintf(stderr,"this->getDef()->getSymbolEntry()->getType()是%s\n",this->getDef()->getSymbolEntry()->getType()->toStr().c_str());
        fprintf(stderr,"this->getDef()->getSymbolEntry()->getType()->isVoid()是%d\n",this->getDef()->getSymbolEntry()->getType()->isVoid());
        if(!(this->getDef()->getSymbolEntry()->getType()->isFuncVoid()))
        {

            call_inst = new MovMInstruction(cur_bb,-1,genMachineOperand(this->getDef()),genMachineReg(0));
            cur_bb->InsertInst(call_inst);
        }


    }
}

/**
 * @brief 构造一个新的 ZextInstruction 对象。
 * @param dst 目标操作数。
 * @param src 源操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 */
ZextInstruction::ZextInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb)
    : Instruction(ZEXT, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
    
}

/**
 * @brief 输出指令的字符串表示。
 */
void ZextInstruction::output() const
{
    fprintf(yyout, "  %s = zext %s %s to %s\n",
            operands[0]->toStr().c_str(),
            operands[1]->getType()->toStr().c_str(),
            operands[1]->toStr().c_str(),
            operands[0]->getType()->toStr().c_str());
}

/**
 * @brief 获取定义操作数。
 * @return 定义操作数。
 */
Operand *ZextInstruction::getDef()
{
    return operands[0];
}

/**
 * @brief 获取使用操作数。
 * @return 使用操作数的向量。
 */
std::vector<Operand *> ZextInstruction::getUse()
{
    return {operands[1]};
}

bool ZextInstruction::canBeCalculated()
{
    if(operands[1]->getSymbolEntry()->isConstant())
        return true;
    return false;
}

Operand *ZextInstruction::CalculatedResult()
{
    if(canBeCalculated())
    {
        ConstantSymbolEntry *result = new ConstantSymbolEntry(operands[0]->getType(), 0);

        ConstantSymbolEntry *val=dynamic_cast<ConstantSymbolEntry*>(operands[1]->getSymbolEntry());
        if(val->getValue()==1)
        {
            result->setIntValue(1);
        }
        else
        {
            result->setIntValue(0);
        }
        return new Operand(result);


    }


    return nullptr;
}

void ZextInstruction::genMachineCode(AsmBuilder *builder)
{
    auto cur_bb = builder->getBlock();
    MachineInstruction *zext_inst = nullptr;
    zext_inst = new MovMInstruction(cur_bb,-1,genMachineOperand(this->operands[0]),genMachineOperand(this->operands[1]));
    cur_bb->InsertInst(zext_inst);
    //这样真的不会出问题吗？？？直接移过去
}

/**
 * @brief 构造一个新的 GetElementPtrInstruction 对象。
 * @param dst 目标操作数。
 * @param src 源操作数。
 * @param indices 索引操作数的向量。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 * 
 *   `dst` = getelementptr inbounds `dst->type`, `src->type` `src`, `indices`
 */
GetElementPtrInstruction::GetElementPtrInstruction(Operand *dst,Operand *element, Operand *src, const std::vector<Operand *> &indices, BasicBlock *insert_bb)
    : Instruction(GEP, insert_bb), indices(indices)
{
    operands.push_back(dst);
    operands.push_back(src);
    operands.push_back(element);
    operands.insert(operands.end(), indices.begin(), indices.end());
    dst->setDef(this);
    src->addUse(this);
    for(auto index:indices)
    {
        index->addUse(this);
    }
    
}

/**
 * @brief 输出指令的字符串表示。
 */
void GetElementPtrInstruction::output() const
{
    fprintf(yyout, "  %s = getelementptr inbounds %s,%s %s",
            operands[0]->toStr().c_str(),
            operands[2]->getType()->toStr().c_str(),
            operands[1]->getType()->toStr().c_str(),
            operands[1]->toStr().c_str());
        if(operands[2]->getSymbolEntry()->getType()->isIntArray()||operands[2]->getSymbolEntry()->getType()->isFloatArray())
        {
            fprintf(yyout,", i32 0");
        }
    for (size_t i = 3; i < operands.size(); ++i)
    {
        fprintf(yyout, ", %s %s",
                operands[i]->getType()->toStr().c_str(),
                operands[i]->toStr().c_str());
    }
    fprintf(yyout, "\n");
}

/**
 * @brief 获取定义操作数。
 * @return 定义操作数。
 */
Operand *GetElementPtrInstruction::getDef()
{
    return operands[0];
}

/**
 * @brief 获取使用操作数。
 * @return 使用操作数的向量。
 */
std::vector<Operand *> GetElementPtrInstruction::getUse()
{
    return std::vector<Operand *>(operands.begin() + 1, operands.end());
}


/**
 * @brief 构造一个新的 BitcastInstruction 对象。
 * @param dst 目标操作数。
 * @param src 源操作数。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 */
BitcastInstruction::BitcastInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb)
    : Instruction(BITCAST, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
}

/**
 * @brief 输出指令的字符串表示。
 */
void BitcastInstruction::output() const
{
    fprintf(yyout, "  %s = bitcast %s %s to %s\n",
            operands[0]->toStr().c_str(),
            operands[1]->getType()->toStr().c_str(),
            operands[1]->toStr().c_str(),
            operands[0]->getType()->toStr().c_str());
}

/**
 * @brief 获取定义操作数。
 * @return 定义操作数。
 */
Operand *BitcastInstruction::getDef()
{
    return operands[0];
}

/**
 * @brief 获取使用操作数。
 * @return 使用操作数的向量。
 */
std::vector<Operand *> BitcastInstruction::getUse()
{
    return {operands[1]};
}

//浮点数转整数指令

FpToSiInstruction::FpToSiInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb)
    : Instruction(FPTOI, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

FpToSiInstruction::~FpToSiInstruction()
{
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void FpToSiInstruction::output() const
{
    fprintf(yyout, "  %s = fptosi %s %s to %s\n",
            operands[0]->toStr().c_str(),
            operands[1]->getType()->toStr().c_str(),
            operands[1]->toStr().c_str(),
            operands[0]->getType()->toStr().c_str());
}

//整数转浮点数指令
SiToFpInstruction::SiToFpInstruction(Operand *dst, Operand *src, BasicBlock *insert_bb)
    : Instruction(SITOF, insert_bb)
{
    operands.push_back(dst);
    operands.push_back(src);
    dst->setDef(this);
    src->addUse(this);
}

SiToFpInstruction::~SiToFpInstruction()
{   
    operands[0]->setDef(nullptr);
    if(operands[0]->usersNum() == 0)
        delete operands[0];
    operands[1]->removeUse(this);
}

void SiToFpInstruction::output() const
{
    fprintf(yyout, "  %s = sitofp %s %s to %s\n",
            operands[0]->toStr().c_str(),
            operands[1]->getType()->toStr().c_str(),
            operands[1]->toStr().c_str(),
            operands[0]->getType()->toStr().c_str());
}
/**
 * @brief 构造一个新的 PhiInstruction 对象。
 * @param dst 目标操作数。
 * @param incoming 输入边的操作数和基本块对的向量。
 * @param insert_bb 将插入此指令的基本块。默认为 nullptr。
 */
PhiInstruction::PhiInstruction(Operand *dst, const std::vector<std::pair<Operand *, BasicBlock *>> &incoming, BasicBlock *insert_bb)
    : Instruction(PHI, insert_bb), incoming(incoming)
{
    operands.push_back(dst);
    dst->setDef(this);
    for (const auto &pair : incoming)
    {
        operands.push_back(pair.first);
        pair.first->addUse(this);
    }
}

PhiInstruction::PhiInstruction(Operand *dst, BasicBlock *insert_bb): Instruction(PHI, insert_bb)
{
    operands.push_back(dst);
    dst->setDef(this);
    
}

void PhiInstruction::addIncoming(Operand *op, BasicBlock *bb)
{
    incoming.push_back(std::make_pair(op, bb));
    operands.push_back(op);
    op->addUse(this);
}

void PhiInstruction::removeIncoming(BasicBlock *bb)
{
    for (size_t i = 1; i < operands.size(); ++i)
    {
        if (incoming[i - 1].second == bb)
        {
            operands.erase(operands.begin() + i);
            incoming.erase(incoming.begin() + i - 1);
            //incoming[i - 1].first->removeUse(this);
            break;
        }
    }
    
}

/**
 * @brief 输出指令的字符串表示。
 */
void PhiInstruction::output() const
{
    std::string dst = operands[0]->toStr();
    std::string type = operands[0]->getType()->toStr();
    if(operands[0]->getType()->isPtr())
    {
        type=dynamic_cast<PointerType*>(operands[0]->getType())->getValueType()->toStr();
    }
    fprintf(yyout, "  %s = phi %s ", dst.c_str(), type.c_str());
    for(size_t i = 1; i < operands.size(); ++i)
    {
            fprintf(yyout, "[ ");
            fprintf(yyout, "%s, %%B%d", operands[i]->toStr().c_str(),incoming[i-1].second->getNo());
            fprintf(yyout, " ] ");
        if(i<operands.size()-1)
            fprintf(yyout, ",");
    }
    fprintf(yyout, "\n");
}

/**
 * @brief 获取定义操作数。
 * @return 定义操作数。
 */
Operand *PhiInstruction::getDef()
{
    return operands[0];
}

/**
 * @brief 获取使用操作数。
 * @return 使用操作数的向量。
 */
std::vector<Operand *> PhiInstruction::getUse()
{
    fprintf(stderr,"PhiInstruction::getUse\n");
    fprintf(stderr,"operands.size()是%ld\n",operands.size());
    std::vector<Operand *> uses;
    for (size_t i = 1; i < operands.size(); ++i)
    {
        uses.push_back(operands[i]);
        fprintf(stderr,"use:%s\n",operands[i]->toStr().c_str());
    }

    return uses;
}
MachineOperand* Instruction::genMachineOperand(Operand* ope)
{
    auto se = ope->getEntry();
    MachineOperand* mope = nullptr;
    if(se->isConstant())
    {   fprintf(stderr,"se->toStr()是%s\n",se->toStr().c_str());
        fprintf(stderr,"dynamic_cast<ConstantSymbolEntry*>(se)->toStr()是%s\n",dynamic_cast<ConstantSymbolEntry*>(se)->toStr().c_str());
        long long LL=dynamic_cast<ConstantSymbolEntry*>(se)->getValue();
        if(se->toStr()=="-2147483648")
            LL=-2147483648;
        fprintf(stderr,"dynamic_cast<ConstantSymbolEntry*>(se)->getValue())是%d\n",dynamic_cast<ConstantSymbolEntry*>(se)->getValue());
        mope = new MachineOperand(MachineOperand::IMM, LL);
        fprintf(stderr,"mope->getImm()是%d\n",mope->getVal());
    }
    else if(se->isTemporary())
        mope = new MachineOperand(MachineOperand::VREG, dynamic_cast<TemporarySymbolEntry*>(se)->getLabel());
    else if(se->isVariable())
    {
        auto id_se = dynamic_cast<IdentifierSymbolEntry*>(se);
        if(id_se->isGlobal())
            mope = new MachineOperand(id_se->toStr().substr(1).c_str());//去掉前面的@
        else
            exit(0);
    }
    return mope;
}

MachineOperand* Instruction::genMachineReg(int reg) 
{
    return new MachineOperand(MachineOperand::REG, reg);
}

MachineOperand* Instruction::genMachineVReg() 
{
    return new MachineOperand(MachineOperand::VREG, SymbolTable::getLabel());
}

MachineOperand* Instruction::genMachineImm(int val) 
{
    return new MachineOperand(MachineOperand::IMM, val);
}

MachineOperand* Instruction::genMachineLabel(int block_no)
{
    std::ostringstream buf;
    buf << ".L" << block_no;
    std::string label = buf.str();
    return new MachineOperand(label);
}

void AllocaInstruction::genMachineCode(AsmBuilder* builder)
{
    /* HINT:
    * Allocate stack space for local variabel
    * Store frame offset in symbol entry */
    auto cur_func = builder->getFunction();
    int offset=0;
    fprintf(stderr,"进入AllocaInstruction::genMachineCode函数\n");
    fprintf(stderr,"operands[0]->getEntry()->getType()->toStr()是%s\n",operands[0]->getEntry()->getType()->toStr().c_str());
    if(dynamic_cast<PointerType*>(operands[0]->getEntry()->getType())->getValueType()->isIntArray())
    {
        fprintf(stderr,"进入数组分配空间\n");
        offset = cur_func->AllocSpace(dynamic_cast<IntArrayType*>(dynamic_cast<PointerType*>(operands[0]->getEntry()->getType())->getValueType())->getStackSize());
        fprintf(stderr,"offset是%d\n",offset);
    }
    else if(dynamic_cast<PointerType*>(operands[0]->getEntry()->getType())->getValueType()->isFloatArray())
    {
        offset = cur_func->AllocSpace(dynamic_cast<FloatArrayType*>(dynamic_cast<PointerType*>(operands[0]->getEntry()->getType())->getValueType())->getStackSize());
    }
    else
    {
        offset = cur_func->AllocSpace(4);
    }
    dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->setOffset(-offset);
}

void LoadInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // Load global operand
    if(operands[1]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[1]->getEntry())->isGlobal())
    {
        auto dst = genMachineOperand(operands[0]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[1]);
        // example: load r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: load r1, [r0]
        cur_inst = new LoadMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Load local operand
    else if(operands[1]->getEntry()->isTemporary()
    && operands[1]->getDef()
    && operands[1]->getDef()->isAlloc())
    {
        // example: load r1, [r0, #4]
        auto dst = genMachineOperand(operands[0]);
        auto src1 = genMachineReg(11);
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[1]->getEntry())->getOffset());
        cur_inst = new LoadMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new LoadMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
}

void StoreInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO

     //fprintf(stderr,"进入StoreInstruction::genMachineCode函数\n");

    auto cur_block = builder->getBlock();
    MachineInstruction* cur_inst = nullptr;
    // store global operand
    if(operands[0]->getEntry()->isVariable()
    && dynamic_cast<IdentifierSymbolEntry*>(operands[0]->getEntry())->isGlobal())
    {
        fprintf(stderr,"StoreInstruction::genMachineCode函数中的全局变量\n");
        auto dst = genMachineOperand(operands[1]);
        auto internal_reg1 = genMachineVReg();
        auto internal_reg2 = new MachineOperand(*internal_reg1);
        auto src = genMachineOperand(operands[0]);
        fprintf(stderr,"StoreInstruction::genMachineCode全局变量函数结束\n");

        // example: Store r0, addr_a
        cur_inst = new LoadMInstruction(cur_block, internal_reg1, src);
        cur_block->InsertInst(cur_inst);
        // example: Store r1, [r0]


        if(operands[1]->getEntry()->isConstant())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new MovMInstruction(cur_block,-1, internal_reg, dst);
            cur_block->InsertInst(cur_inst);
            dst = new MachineOperand(*internal_reg);
        }

        cur_inst = new StoreMInstruction(cur_block, dst, internal_reg2);
        cur_block->InsertInst(cur_inst);
    }
    // Store local operand
    else if(operands[0]->getEntry()->isTemporary()
    && operands[0]->getDef()
    && operands[0]->getDef()->isAlloc())
    {
        //fprintf(stderr,"StoreInstruction::genMachineCode函数中的局部变量\n");
        // example: load r1, [r0, #4]
        MachineOperand* dst = genMachineOperand(operands[1]);
        if(operands[1]->getEntry()->isConstant())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new MovMInstruction(cur_block,-1, internal_reg, dst);
            cur_block->InsertInst(cur_inst);
            dst = new MachineOperand(*internal_reg);
        }
            
        auto src1 = genMachineReg(11);
        auto src2 = genMachineImm(dynamic_cast<TemporarySymbolEntry*>(operands[0]->getEntry())->getOffset());
                //fprintf(stderr,"StoreInstruction::genMachineCode局部变量函数结束\n");

        cur_inst = new StoreMInstruction(cur_block, dst, src1, src2);
        cur_block->InsertInst(cur_inst);
    }
    // Load operand from temporary variable
    else
    {
        // example: load r1, [r0]
        auto dst = genMachineOperand(operands[0]);
        auto src = genMachineOperand(operands[1]);
        cur_inst = new StoreMInstruction(cur_block, dst, src);
        cur_block->InsertInst(cur_inst);
    }
    //fprintf(stderr,"StoreInstruction::genMachineCode函数结束\n");
}

void BinaryInstruction::genMachineCode(AsmBuilder* builder)
{
    auto cur_block = builder->getBlock();
    auto dst = genMachineOperand(operands[0]);  // 目标操作数
    auto src1 = genMachineOperand(operands[1]); // 第一个源操作数
    auto src2 = genMachineOperand(operands[2]); // 第二个源操作数

    MachineInstruction* cur_inst = nullptr;

    // 处理第一个源操作数是立即数的情况
    if (src1->isImm())
    {
        if (!src1->isValidImm() || opcode == SUB || opcode == MUL || opcode == DIV || opcode == MOD|| opcode == AND || opcode == OR || opcode == XOR|| opcode ==ADD)
        {
            auto internal_reg = genMachineVReg();   // 生成虚拟寄存器
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, internal_reg, src1); // 将立即数加载到寄存器
            cur_block->InsertInst(cur_inst);    // 插入指令到当前块
            src1 = new MachineOperand(*internal_reg); // 将 src1 更新为寄存器操作数
        }
    }

    // 处理第二个源操作数是立即数的情况
    if (src2->isImm())
    {
        if (!src2->isValidImm() || opcode == MUL || opcode == DIV || opcode == MOD|| opcode == MOD|| opcode == AND || opcode == OR || opcode == XOR||opcode ==ADD)
        {
            auto internal_reg = genMachineVReg();   // 生成虚拟寄存器
            cur_inst = new MovMInstruction(cur_block, MovMInstruction::MOV, internal_reg, src2); // 将立即数加载到寄存器
            cur_block->InsertInst(cur_inst);    // 插入指令到当前块
            src2 = new MachineOperand(*internal_reg); // 将 src2 更新为寄存器操作数
        }
    }

    // 根据操作码生成对应的机器指令
    switch (opcode)
    {
    case ADD:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::ADD, dst, src1, src2);
        break;
    case SUB:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, src2);
        break;
    case MUL:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, dst, src1, src2);
        break;
    case DIV: {
        // 使用 SDIV 指令进行除法
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, dst, src1, src2);
        break;
    }
    case MOD: {
        // MOD 的实现：使用 SDIV 和 SUB
        // 1. 计算商值：temp1 = src1 / src2
        auto temp1 = genMachineVReg(); // 用于存储商值
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::DIV, temp1, src1, src2);
        cur_block->InsertInst(cur_inst);

        // 2. 计算商乘积：temp2 = temp1 * src2
        auto temp2 = genMachineVReg(); // 用于存储商乘积
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::MUL, temp2, temp1, src2);
        cur_block->InsertInst(cur_inst);

        // 3. 计算余数：dst = src1 - temp2
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::SUB, dst, src1, temp2);
        break;
    }
    case AND:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::AND, dst, src1, src2);
        break;
    case OR:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::OR, dst, src1, src2);
        break;
    case XOR:
        cur_inst = new BinaryMInstruction(cur_block, BinaryMInstruction::XOR, dst, src1, src2);
        break;
    default:
        break;
    }

    // 将生成的指令插入到当前块中
    cur_block->InsertInst(cur_inst);
}


void CmpInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO


    auto cur_block = builder->getBlock();
    auto src1 = genMachineOperand(operands[1]);
    auto src2 = genMachineOperand(operands[2]);
    MachineInstruction* cur_inst = nullptr;


    if(operands[1]->getEntry()->isConstant())
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new MovMInstruction(cur_block,-1, internal_reg, src1);
            cur_block->InsertInst(cur_inst);
            src1 = new MachineOperand(*internal_reg);
        }
    if(operands[2]->getEntry()->isConstant()&&dynamic_cast<ConstantSymbolEntry*>(operands[2]->getEntry())->getValue()>255)
        {
            auto internal_reg = genMachineVReg();
            cur_inst = new LoadMInstruction(cur_block, internal_reg, src2);
            cur_block->InsertInst(cur_inst);
            src2 = new MachineOperand(*internal_reg);
        }
    
    cur_inst = new CmpMInstruction(cur_block, src1, src2);
    cur_block->InsertInst(cur_inst);
    cur_inst = new MovMInstruction(cur_block, -1, genMachineOperand(operands[0]), genMachineImm(0));
    cur_block->InsertInst(cur_inst);

    cur_inst = new MovMInstruction(cur_block, opcode, genMachineOperand(operands[0]), genMachineImm(1));
    cur_block->InsertInst(cur_inst);
    builder->setCmpOpcode(opcode);


    

}

void UncondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO




    auto cur_block = builder->getBlock();
    std::string true_label = ".L" + std::to_string(branch->getNo());
    MachineOperand* true_src = new MachineOperand(true_label);
    MachineInstruction* cur_inst = new BranchMInstruction(cur_block, -1, true_src);
    cur_block->InsertInst(cur_inst);
    

}

void CondBrInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO


    auto cur_block = builder->getBlock();
    int opcode = builder->getCmpOpcode();
    MachineInstruction* cur_inst = nullptr;
    std::string true_label = ".L" + std::to_string(true_branch->getNo());
    std::string false_label = ".L" + std::to_string(false_branch->getNo());
    MachineOperand* true_src = new MachineOperand(true_label);
    MachineOperand* false_src = new MachineOperand(false_label);
    cur_inst = new BranchMInstruction(cur_block, opcode, true_src);
    cur_block->InsertInst(cur_inst);
    cur_inst = new BranchMInstruction(cur_block, -1,false_src);
    cur_block->InsertInst(cur_inst);

}

void RetInstruction::genMachineCode(AsmBuilder* builder)
{
    // TODO
    /* HINT:
    * 1. Generate mov instruction to save return value in r0
    * 2. Restore callee saved registers and sp, fp
    * 3. Generate bx instruction */
    auto cur_block = builder->getBlock();  
    MachineInstruction* cur_inst = nullptr;
    if(operands.empty())
    {
        //空的再说
    }
    else
    {
        auto src = genMachineOperand(operands[0]);
        cur_inst = new MovMInstruction(cur_block,-1, genMachineReg(0), src);
        cur_block->InsertInst(cur_inst);
        cur_inst = new MovMInstruction(cur_block,-1, genMachineReg(13), genMachineReg(11));
        cur_block->InsertInst(cur_inst);
        cur_inst = new BranchMInstruction(cur_block, BranchMInstruction::BX, genMachineReg(14));
        cur_block->InsertInst(cur_inst);

    }

}
