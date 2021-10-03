#include <gtest/gtest.h>
#include "Domain/Compiler/TestHarness/TextDiff/MyersDiff.h"
#include "Domain/Compiler/TestHarness/TextDiff/DiffPrinter.h"

namespace elet::domain::compiler::test
{


TEST(MyersDiff, Deletion)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D\nB\nA", "D\nB");
    EXPECT_EQ(diffPrinter.print(result), "  1    1    D\n  2    2    B\n- 3         A\n");
}


TEST(MyersDiff, Insertion)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D", "D\nB");
    EXPECT_EQ(diffPrinter.print(result), "  1    1    D\n+      2    B\n");
}


TEST(MyersDiff, Equal)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D", "D");
    EXPECT_EQ(diffPrinter.print(result), "  1    1    D\n");
}


TEST(MyersDiff, Mixed)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D\nC\nR", "D\nB\nE");
    EXPECT_EQ(diffPrinter.print(result), "  1    1    D\n- 2         C\n- 3         R\n+      2    B\n+      3    E\n");
}


TEST(MyersDiff, ToEmpty)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("D\nB\nA", "");
    EXPECT_EQ(diffPrinter.print(result), "- 1         D\n- 2         B\n- 3         A\n");
}



TEST(MyersDiff, FromEmpty)
{
    MyersDiff myersDiff;
    DiffPrinter diffPrinter;
    auto result = myersDiff.diffText("", "D\nB\nA\nC");
    EXPECT_EQ(diffPrinter.print(result), "+      1    D\n+      2    B\n+      3    A\n+      4    C\n");
}

}