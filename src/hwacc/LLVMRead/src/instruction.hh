#ifndef __HWACC_LLVM_INSTRUCTION_HH__
#define __HWACC_LLVM_INSTRUCTION_HH__

#include <cstdlib>
#include <iostream>
#include <llvm/IR/Value.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include "basic_block.hh"
#include "operand.hh"
#include "cycle_count.hh"
#include "debug_flags.hh"
#include "value.hh"


namespace SALAM {

class BasicBlock; // Do Not Remove


//---------------------------------------------------------------------------//
//--------- Instruction Base Class ------------------------------------------//
//---------------------------------------------------------------------------//


class Instruction : public Value
{
    private:
        valueListTy staticOperands;
        std::vector<Instruction *> dynamicDependencies;
        std::vector<Instruction *> dynamicUsers;
        uint64_t llvmOpCode;
        uint64_t cycleCount;
        bool dbg = false;
        
    protected:
        std::vector<Operands> operands;
        bool running = false;

        class Instruction_Debugger: public Debugger
        {
            public:
                Instruction_Debugger();
                ~Instruction_Debugger() = default;
                virtual void dumper(Instruction * inst);
        }; 

       Instruction_Debugger* inst_dbg;
    public:
        Instruction(uint64_t id); //
        Instruction(uint64_t id, uint64_t OpCode); //
        Instruction(uint64_t id, uint64_t OpCode, uint64_t cycles); //
        ~Instruction(); //
        virtual void initialize(llvm::Value * irval, irvmap * irmap, SALAM::valueListTy * valueList) { }; //
        void instantiate(llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList); //
        // Create initialize function for instruction - common item containers
        valueListTy getStaticOperands() const { return staticOperands; }
        std::shared_ptr<SALAM::Value> getStaticOperands(int i) const { return staticOperands.at(i); }
        void signalUsers();
        virtual void compute() { }
        virtual void commit() { }
        virtual void fetchDependencyVal(Instruction * dep) {} //TODO: This will be changed to purely virtual
        void dump() { if (dbg) inst_dbg->dumper(this); }
        std::shared_ptr<SALAM::Instruction> clone() const { return std::static_pointer_cast<SALAM::Instruction>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Instruction>(new SALAM::Instruction(*this)); }
        /*
            TODO: link up the findDynamicDeps function from llvm_interface
        */
};

//---------------------------------------------------------------------------//
//--------- Terminator Instructions -----------------------------------------//
//---------------------------------------------------------------------------//

// SALAM-BadInstruction // --------------------------------------------------//
class BadInstruction : public Instruction {
    // Used to draw hard dependencies, ie: ret
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        BadInstruction(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~BadInstruction() = default;
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::BadInstruction> clone() const { return std::static_pointer_cast<SALAM::BadInstruction>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::BadInstruction>(new SALAM::BadInstruction(*this)); }
};


std::shared_ptr<SALAM::Instruction>
createBadInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);

// SALAM-Ret // -------------------------------------------------------------//

class Ret : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
        
    protected:

    public:
        Ret(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Ret() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Ret> clone() const { return std::static_pointer_cast<SALAM::Ret>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Ret>(new SALAM::Ret(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createRetInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);

// SALAM-Br // --------------------------------------------------------------//

class Br : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
        std::shared_ptr<SALAM::Value> condition;
        std::shared_ptr<SALAM::Value> defaultDestination;
        std::shared_ptr<SALAM::Value> trueDestination;
        std::shared_ptr<SALAM::Value> falseDestination;
        bool conditional = false;

    protected:

    public:
        // Branch Constructor
        Br(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Br() = default;
        void initialize(llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        Br &isConditional(bool isConditional) { conditional = isConditional; return *this; }
        bool isConditional() { return conditional; }
        std::shared_ptr<SALAM::Value> destination();
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Br> clone() const { return std::static_pointer_cast<SALAM::Br>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Br>(new SALAM::Br(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createBrInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);

// SALAM-Switch // ----------------------------------------------------------//
typedef std::pair<std::shared_ptr<SALAM::Value>, std::shared_ptr<SALAM::Value>> caseArgs;
typedef std::vector< caseArgs> switchArgs;

class Switch : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
        // [0] [Switch Var, Default Dest]
        // [1] [ Case Var, Case Dest ] .... [n]
        switchArgs arguments;
    protected:

    public:
        Switch(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Switch() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        std::shared_ptr<SALAM::Value> defaultDest() { return arguments[0].second; }
        std::shared_ptr<SALAM::Value> destination(int switchVar);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Switch> clone() const { return std::static_pointer_cast<SALAM::Switch>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Switch>(new SALAM::Switch(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSwitchInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- Binary Operator Instructions ------------------------------------//
//---------------------------------------------------------------------------//

// SALAM-Add // -------------------------------------------------------------//

class Add : public Instruction
{
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Add(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Add() = default;
        void initialize(llvm::Value *irval,
                        SALAM::irvmap *irmap,
                        SALAM::valueListTy *valueList) override;
        void compute() override;
        void commit()       override;

        std::shared_ptr<SALAM::Add> clone() const { return std::static_pointer_cast<SALAM::Add>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Add>(new SALAM::Add(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createAddInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);

// SALAM-FAdd // ------------------------------------------------------------//

class FAdd : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FAdd(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FAdd() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FAdd> clone() const { return std::static_pointer_cast<SALAM::FAdd>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FAdd>(new SALAM::FAdd(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFAddInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Sub // -------------------------------------------------------------//

class Sub : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Sub(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Sub() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Sub> clone() const { return std::static_pointer_cast<SALAM::Sub>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Sub>(new SALAM::Sub(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSubInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FSub // -------------------------------------------------------------//

class FSub : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FSub(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FSub() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FSub> clone() const { return std::static_pointer_cast<SALAM::FSub>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FSub>(new SALAM::FSub(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFSubInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Mul // -------------------------------------------------------------//

class Mul : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
        SALAM::APIntRegister *op1, *op2;
    protected:

    public:
        Mul(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Mul() = default;
        void initialize (llvm::Value * irval,
                        SALAM::irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Mul> clone() const { return std::static_pointer_cast<SALAM::Mul>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Mul>(new SALAM::Mul(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createMulInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FMul // ------------------------------------------------------------//

class FMul : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FMul(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FMul() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FMul> clone() const { return std::static_pointer_cast<SALAM::FMul>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FMul>(new SALAM::FMul(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFMulInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-UDiv // ------------------------------------------------------------//

class UDiv : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        UDiv(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~UDiv() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::UDiv> clone() const { return std::static_pointer_cast<SALAM::UDiv>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::UDiv>(new SALAM::UDiv(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createUDivInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-SDiv // ------------------------------------------------------------//

class SDiv : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        SDiv(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~SDiv() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::SDiv> clone() const { return std::static_pointer_cast<SALAM::SDiv>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::SDiv>(new SALAM::SDiv(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSDivInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FDiv // ------------------------------------------------------------//

class FDiv : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FDiv(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FDiv() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FDiv> clone() const { return std::static_pointer_cast<SALAM::FDiv>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FDiv>(new SALAM::FDiv(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFDivInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-URem // ------------------------------------------------------------//

class URem : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        URem(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~URem() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::URem> clone() const { return std::static_pointer_cast<SALAM::URem>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::URem>(new SALAM::URem(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createURemInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-SRem // ------------------------------------------------------------//

class SRem : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        SRem(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~SRem() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::SRem> clone() const { return std::static_pointer_cast<SALAM::SRem>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::SRem>(new SALAM::SRem(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSRemInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FRem // ------------------------------------------------------------//

class FRem : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FRem(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FRem() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FRem> clone() const { return std::static_pointer_cast<SALAM::FRem>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FRem>(new SALAM::FRem(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFRemInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- Bitwise Binary Operator Instructions ----------------------------//
//---------------------------------------------------------------------------//

// SALAM-Shl // -------------------------------------------------------------//

class Shl : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Shl(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Shl() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Shl> clone() const { return std::static_pointer_cast<SALAM::Shl>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Shl>(new SALAM::Shl(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createShlInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-LShr // ------------------------------------------------------------//

class LShr : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        LShr(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~LShr() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::LShr> clone() const { return std::static_pointer_cast<SALAM::LShr>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::LShr>(new SALAM::LShr(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createLShrInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-AShr // ------------------------------------------------------------//

class AShr : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        AShr(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~AShr() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::AShr> clone() const { return std::static_pointer_cast<SALAM::AShr>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::AShr>(new SALAM::AShr(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createAShrInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-And // -------------------------------------------------------------//

class And : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        And(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~And() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::And> clone() const { return std::static_pointer_cast<SALAM::And>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::And>(new SALAM::And(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createAndInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Or // --------------------------------------------------------------//

class Or : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Or(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Or() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Or> clone() const { return std::static_pointer_cast<SALAM::Or>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Or>(new SALAM::Or(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createOrInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Xor // -------------------------------------------------------------//

class Xor : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Xor(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Xor() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Xor> clone() const { return std::static_pointer_cast<SALAM::Xor>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Xor>(new SALAM::Xor(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createXorInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- Memory Instructions ---------------------------------------------//
//---------------------------------------------------------------------------//

// SALAM-Load // ------------------------------------------------------------//

class Load : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:


    public:
        Load(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Load() = default;
        void initialize(llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Load> clone() const { return std::static_pointer_cast<SALAM::Load>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Load>(new SALAM::Load(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createLoadInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Store // -----------------------------------------------------------//

class Store : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Store (uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Store() = default;
        void initialize(llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Store> clone() const { return std::static_pointer_cast<SALAM::Store>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Store>(new SALAM::Store(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createStoreInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-GEP // -------------------------------------------------------------//

class GetElementPtr : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        GetElementPtr(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~GetElementPtr() = default;
        void initialize(llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        GetElementPtr &setA() { std::cout << "a\n"; return *this; }
        GetElementPtr &setB() { std::cout << "b\n"; return *this; }
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::GetElementPtr> clone() const { return std::static_pointer_cast<SALAM::GetElementPtr>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::GetElementPtr>(new SALAM::GetElementPtr(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createGetElementPtrInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- Other / Cast Instructions ---------------------------------------//
//---------------------------------------------------------------------------//

// SALAM-Trunc // -----------------------------------------------------------//

class Trunc : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Trunc(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Trunc() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Trunc> clone() const { return std::static_pointer_cast<SALAM::Trunc>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Trunc>(new SALAM::Trunc(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createTruncInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-ZExt // ------------------------------------------------------------//

class ZExt : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        ZExt(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~ZExt() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::ZExt> clone() const { return std::static_pointer_cast<SALAM::ZExt>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::ZExt>(new SALAM::ZExt(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createZExtInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-SExt // ------------------------------------------------------------//

class SExt : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        SExt(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~SExt() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::SExt> clone() const { return std::static_pointer_cast<SALAM::SExt>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::SExt>(new SALAM::SExt(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSExtInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FPToUI // ----------------------------------------------------------//
class FPToUI;
void initializeFPToUIInst(SALAM::FPToUI &salamInstruction);

class FPToUI : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FPToUI(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FPToUI() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FPToUI> clone() const { return std::static_pointer_cast<SALAM::FPToUI>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FPToUI>(new SALAM::FPToUI(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFPToUIInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FPToSI // ----------------------------------------------------------//

class FPToSI : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FPToSI(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FPToSI() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FPToSI> clone() const { return std::static_pointer_cast<SALAM::FPToSI>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FPToSI>(new SALAM::FPToSI(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFPToSIInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-UIToFP // ----------------------------------------------------------//

class UIToFP : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        UIToFP(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~UIToFP() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::UIToFP> clone() const { return std::static_pointer_cast<SALAM::UIToFP>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::UIToFP>(new SALAM::UIToFP(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createUIToFPInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-SIToFP // ----------------------------------------------------------//

class SIToFP : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        SIToFP(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~SIToFP() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::SIToFP> clone() const { return std::static_pointer_cast<SALAM::SIToFP>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::SIToFP>(new SALAM::SIToFP(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSIToFPInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FPTrunc // ---------------------------------------------------------//

class FPTrunc : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FPTrunc(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FPTrunc() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FPTrunc> clone() const { return std::static_pointer_cast<SALAM::FPTrunc>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FPTrunc>(new SALAM::FPTrunc(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFPTruncInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FPExt // -----------------------------------------------------------//

class FPExt : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FPExt(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FPExt() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FPExt> clone() const { return std::static_pointer_cast<SALAM::FPExt>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FPExt>(new SALAM::FPExt(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFPExtInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-PtrToInt // --------------------------------------------------------//

class PtrToInt : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        PtrToInt (uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~PtrToInt() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::PtrToInt> clone() const { return std::static_pointer_cast<SALAM::PtrToInt>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::PtrToInt>(new SALAM::PtrToInt(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createPtrToIntInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-IntToPtr // --------------------------------------------------------//

class IntToPtr : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        IntToPtr(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~IntToPtr() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::IntToPtr> clone() const { return std::static_pointer_cast<SALAM::IntToPtr>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::IntToPtr>(new SALAM::IntToPtr(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createIntToPtrInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- Other / Comparison Instructions ---------------------------------//
//---------------------------------------------------------------------------//

// SALAM-ICmp // ------------------------------------------------------------//

class ICmp : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        ICmp(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~ICmp() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::ICmp> clone() const { return std::static_pointer_cast<SALAM::ICmp>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::ICmp>(new SALAM::ICmp(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createICmpInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-FCmp // ------------------------------------------------------------//

class FCmp : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        FCmp(uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~FCmp() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::FCmp> clone() const { return std::static_pointer_cast<SALAM::FCmp>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::FCmp>(new SALAM::FCmp(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createFCmpInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- Other / Edge Instructions ---------------------------------------//
//---------------------------------------------------------------------------//

// SALAM-Phi // -------------------------------------------------------------//

typedef std::pair<std::shared_ptr<SALAM::Value>, std::shared_ptr<SALAM::BasicBlock> > phiNode;

//typedef std::pair<std::shared_ptr<SALAM::Value>, std::shared_ptr<SALAM::Value> > phiNode;
typedef std::vector< phiNode> phiArgs;

class Phi : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
        phiArgs arguments; // [Value, Previous BB]
    protected:

    public:
        Phi (uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Phi() = default;
        void initialize(llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute();
        std::weak_ptr<SALAM::Value> commit(std::shared_ptr<SALAM::BasicBlock> previousBB);
        std::shared_ptr<SALAM::Phi> clone() const { return std::static_pointer_cast<SALAM::Phi>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Phi>(new SALAM::Phi(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createPHIInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Call // ------------------------------------------------------------//

class Call : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
    protected:

    public:
        Call (uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Call() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Call> clone() const { return std::static_pointer_cast<SALAM::Call>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Call>(new SALAM::Call(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createCallInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
// SALAM-Select // ----------------------------------------------------------//

class Select : public Instruction {
    private:
        std::vector< std::vector<uint64_t> > conditions;
        std::shared_ptr<SALAM::Value> condition;
        std::shared_ptr<SALAM::Value> trueValue;
        std::shared_ptr<SALAM::Value> falseValue;
    protected:

    public:
        // ---- Constructor
        Select (uint64_t id,
            uint64_t OpCode,
            uint64_t cycles);
        ~Select() = default;
        void initialize (llvm::Value * irval,
                        irvmap * irmap,
                        SALAM::valueListTy * valueList);
        std::shared_ptr<SALAM::Value> evaluate();
        void compute()      override;
        void commit()       override;
        std::shared_ptr<SALAM::Select> clone() const { return std::static_pointer_cast<SALAM::Select>(createClone()); }
        virtual std::shared_ptr<SALAM::Value> createClone() const override { return std::shared_ptr<SALAM::Select>(new SALAM::Select(*this)); }
};

std::shared_ptr<SALAM::Instruction>
createSelectInst(uint64_t id,
              uint64_t OpCode,
              uint64_t cycles);
//---------------------------------------------------------------------------//
//--------- End Instruction Classes -----------------------------------------//
//---------------------------------------------------------------------------//

// std::shared_ptr<SALAM::Value> attach(llvm::Value * irval, irvmap * irmap);

}

#endif // __HWACC_LLVM_INSTRUCTION_HH__