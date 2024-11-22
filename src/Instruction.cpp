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

UncondBrInstruction::UncondBrInstruction(BasicBlock *to, BasicBlock *insert_bb) : Instruction(UNCOND, insert_bb)
{
    fprintf(stderr,"UncondBrInstruction::UncondBrInstruction\n");
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
    fprintf(stderr,"CondBrInstruction::CondBrInstruction\n");
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
    // bool isFloat = src.find('.') != std::string::npos;//这个判断方法对吗？？？？
    // if (isFloat) {
    //     double value = std::stod(src);//将字符串转换为double
    //     uint64_t ieee754;
    //     std::memcpy(&ieee754, &value, sizeof(value));//将double转换为uint64_t
    //     std::stringstream ss;
    //     ss << std::hex << ieee754;
    //     src = ss.str();
    // }
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


CallInstruction::~CallInstruction() {}

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
        if(operands[2]->getSymbolEntry()->getType()->isIntArray())
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
