#include "./Aarch64Instructions.h"

namespace elet::domain::compiler::test::aarch
{
    OneOfInstruction::OneOfInstruction(AddShiftedRegisterInstruction add)
    {
        this->add = add;
    }


    OneOfInstruction::OneOfInstruction(AddSubImmediateInstruction addSubImmediate)
    {
        this->addSubImmediate = addSubImmediate;
    }


    OneOfInstruction::OneOfInstruction(AdrInstruction adr)
    {
        this->adr = adr;
    }


    OneOfInstruction::OneOfInstruction(AdrpInstruction adrp)
    {
        this->adrp = adrp;
    }


    OneOfInstruction::OneOfInstruction(AndImmediateInstruction _and)
    {
        this->_and = _and;
    }


    OneOfInstruction::OneOfInstruction(BInstruction b)
    {
        this->b = b;
    }


    OneOfInstruction::OneOfInstruction(BlInstruction bl)
    {
        this->bl = bl;
    }


    OneOfInstruction::OneOfInstruction(DivInstruction div)
    {
        this->div = div;
    }


    OneOfInstruction::OneOfInstruction(BrInstruction br)
    {
        this->br = br;
    }


    OneOfInstruction::OneOfInstruction(LdrInstruction ldr)
    {
        this->ldr = ldr;
    }


    OneOfInstruction::OneOfInstruction(LdrStrImmediateUnsignedOffsetInstruction ldrstr)
    {
        this->ldrstr = ldrstr;
    }


    OneOfInstruction::OneOfInstruction(LdrbStrbImmediateUnsignedOffsetInstruction ldrbstrb)
    {
        this->ldrbstrb = ldrbstrb;
    }


    OneOfInstruction::OneOfInstruction(LdrhStrhImmediateUnsignedOffsetInstruction ldrhstrh)
    {
        this->ldrhstrh = ldrhstrh;
    }


    OneOfInstruction::OneOfInstruction(LoadStorePairInstruction ldrpstrp)
    {
        this->ldrpstrp = ldrpstrp;
    }


    OneOfInstruction::OneOfInstruction(LdrshImmediateUnsignedOffsetInstruction ldrsh)
    {
        this->ldrsh = ldrsh;
    }


    OneOfInstruction::OneOfInstruction(LdrsbImmediateUnsignedOffsetInstruction ldrsb)
    {
        this->ldrsb = ldrsb;
    }


    OneOfInstruction::OneOfInstruction(MaddSubInstruction maddsub)
    {
        this->maddsub = maddsub;
    }


    OneOfInstruction::OneOfInstruction(MovkInstruction movk)
    {
        this->movk = movk;
    }


    OneOfInstruction::OneOfInstruction(MovnInstruction movn)
    {
        this->movn = movn;
    }


    OneOfInstruction::OneOfInstruction(MovzInstruction movz)
    {
        this->movz = movz;
    }


    OneOfInstruction::OneOfInstruction(NopInstruction nop)
    {
        this->nop = nop;
    }


    OneOfInstruction::OneOfInstruction(OrrImmediateInstruction orr)
    {
        this->orr = orr;
    }


    OneOfInstruction::OneOfInstruction(RetInstruction ret)
    {
        this->ret = ret;
    }


    OneOfInstruction::OneOfInstruction(SubShiftedRegisterInstruction sub)
    {
        this->sub = sub;
    }


    OneOfInstruction::OneOfInstruction(SxtbInstruction sxtb)
    {
        this->sxtb = sxtb;
    }


    OneOfInstruction::OneOfInstruction(SxthInstruction sxth)
    {
        this->sxth = sxth;
    }


    OneOfInstruction::OneOfInstruction(UdfInstruction udf)
    {
        this->udf = udf;
    }
}