#ifndef ELET_AARCH64BASELINEPRINTER_H
#define ELET_AARCH64BASELINEPRINTER_H


#include "../BaselinePrinter.h"
#include "./AarchTypes.h"


namespace elet::domain::compiler::test::aarch
{

class Aarch64BaselinePrinter : public BaselinePrinter
{
public:

    List<Instruction*>
    instructions;

    Utf8String
    print() override;
};

}

#endif //ELET_AARCH64BASELINEPRINTER_H
