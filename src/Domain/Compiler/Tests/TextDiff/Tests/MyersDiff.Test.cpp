#include <gtest/gtest.h>
#include "../MyersDiff.h"
#include "../DiffPrinter.h"

namespace elet::domain::compiler::test
{


TEST(MyersDiff, Deletion)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D\nB\nA", "D\nB");
    diffPrinter.print(result);
}


TEST(MyersDiff, Insertion)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D", "D\nB");
    diffPrinter.print(result);
}


TEST(MyersDiff, Equal)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D", "D");
    diffPrinter.print(result);
}


TEST(MyersDiff, Mixed)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D\nC\nR", "D\nB\nE");
    diffPrinter.print(result);
}


}