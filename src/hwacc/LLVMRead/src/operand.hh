#ifndef __HWACC_OPERAND_HH__
#define __HWACC_OPERAND_HH__
//------------------------------------------//
#include "llvm/IR/Value.h"
#include "llvm/IR/GlobalVariable.h"
#include "llvm/IR/Constants.h"
#include "llvm/ADT/APSInt.h"
#include "llvm/ADT/APFloat.h"
#include "llvm/IR/Instructions.h"
#include <llvm-c/Core.h>
#include "debug_flags.hh"
#include "value.hh"
#include "registers.hh"
#include <map>
#include <memory>
#include <vector>
 
namespace SALAM
{


class Operand: public Value
{
    private:
        Register *lockedValue;
        bool set = false;

    protected:
    public:
        void initOperandReg();
        //Operand(uint64_t id);
        Operand(const SALAM::Value &copy_val);
        Operand(const Operand &copy_val);
        Operand& operator = (Operand &copy_val);
        ~Operand() = default;
        //Value *clone() { return new Operand(*this); }
        virtual void initialize(llvm::Value * irval, irvmap * irmap) override;
        void updateOperandRegister();
};

class Constant: public Value {
    private:
    protected:
        SALAM::valueListTy operands;
    public:
        Constant(uint64_t id);
        ~Constant() = default;
        //Value *clone() { return new Constant(*this); }
        virtual void initialize(llvm::Value * irval, irvmap * irmap, SALAM::valueListTy * values);
};

class GlobalConstant : public Constant {
    private:
    protected:
    public:
        GlobalConstant(uint64_t id);
        ~GlobalConstant() = default;
        //Value *clone() { return new GlobalConstant(*this); }
        virtual void initialize(llvm::Value * irval, irvmap * irmap, SALAM::valueListTy * values) override;
};

class Argument : public Value {
    private:
    protected:
    public:
        Argument(uint64_t id);
        ~Argument() = default;
        //Value *clone() { return new Argument(*this); }
        virtual void initialize(llvm::Value * irval, irvmap * irmap) override;
};

}

//------------------------------------------//
#endif //__HWACC_OPERAND_HH__