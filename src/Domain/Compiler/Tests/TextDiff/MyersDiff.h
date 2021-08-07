#ifndef ELET_MYERSDIFF_H
#define ELET_MYERSDIFF_H


#include <cstdlib>
#include <optional>
#include <Foundation/Utf8String.h>
#include <Foundation/Utf8StringView.h>
#include <Foundation/List.h>
#include <queue>

namespace elet::domain::compiler::test
{


using namespace foundation;


struct Point
{
    unsigned int
    x;

    unsigned int
    y;

    Point(unsigned int x, unsigned int y):
        x(x),
        y(y)
    { }
};


struct LineMove
{
    Point
    start;

    Point
    end;

    LineMove(Point start, Point end):
        start(start),
        end(end)
    { }
};


struct Box
{
    int64_t
    left;

    int64_t
    top;

    int64_t
    right;

    int64_t
    bottom;

    int64_t
    width () const
    {
        return right - left;
    }

    int64_t
    height() const
    {
        return bottom - top;
    }

    int64_t
    size() const
    {
        return width() + height();
    }

    int64_t
    delta() const
    {
        return width() - height();
    }

    Box(int64_t left, int64_t top, int64_t right, int64_t bottom):
        left(left),
        top(top),
        right(right),
        bottom(bottom)
    { }
};


enum class EditType
{
    Insert,
    Delete,
    Equal,
};


struct EditLine
{
    Utf8StringView
    content;

    unsigned int
    number;
};


struct Edit
{
    EditType
    type;

    std::optional<EditLine>
    oldLine;

    std::optional<EditLine>
    newLine;

    Edit(EditType type, std::optional<EditLine> oldLine, std::optional<EditLine> newLine):
        type(type),
        oldLine(oldLine),
        newLine(newLine)
    { }
};



class MyersDiff
{

public:

    List<Edit>
    diffText(Utf8String text1, Utf8String text2);

    List<Edit>
    diffText(Utf8String text1, Utf8String text2, bool& isDiffing);

private:

    List<Utf8String>
    a;

    List<Utf8String>
    b;

    List <Edit>
    diffLines(List <Utf8String> a, List <Utf8String> b, bool& isDiffing);

    std::optional<LineMove>
    midpoint(const Box& box);

    std::optional<List<Point>>
    findPath(int64_t left, int64_t top, int64_t right, int64_t bottom);

    std::optional<LineMove>
    forwards(const Box& box, List <int64_t>& vf, List <int64_t>& vb, int64_t d, int64_t max_d);

    std::optional<LineMove>
    backwards(const Box& box, List <int64_t>& vf, List <int64_t>& vb, int64_t d, int64_t max_d);

    void
    visitSnakes(std::function<void (const Point, const Point)> callback);

    Point
    walkDiagonal(int64_t x1, int64_t y1, int64_t x2, int64_t y2, std::function<void (const Point, const Point)> callback);
};

}

#endif //ELET_MYERSDIFF_H
