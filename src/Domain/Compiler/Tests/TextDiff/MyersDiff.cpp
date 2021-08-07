#include "MyersDiff.h"
#include <vector>

namespace elet::domain::compiler::test
{


List<Edit>
MyersDiff::diffText(Utf8String text1, Utf8String text2)
{
    bool dummy;
    return diffLines(text1.toLines(), text2.toLines(), dummy);
}


List<Edit>
MyersDiff::diffText(Utf8String text1, Utf8String text2, bool& isDiffing)
{
    return diffLines(text1.toLines(), text2.toLines(), isDiffing);
}


List<Edit>
MyersDiff::diffLines(List<Utf8String> a, List<Utf8String> b, bool& isDiffing)
{
    this->a = a;
    this->b = b;

    List<Edit> diffs;

    visitSnakes([&](const Point previous, const Point current)
    {
        if (current.x == previous.x)
        {
            isDiffing = true;
            diffs.emplace(EditType::Insert, std::nullopt, std::make_optional<EditLine>({ b[previous.y], previous.y + 1 }));
        }
        else if (current.y == previous.y)
        {
            isDiffing = true;
            diffs.emplace(EditType::Delete, std::make_optional<EditLine>({ a[previous.x], previous.x + 1 }), std::nullopt);
        }
        else
        {
            diffs.emplace(EditType::Equal, std::make_optional<EditLine>({ a[previous.x], previous.x + 1 }), std::make_optional<EditLine>({ b[previous.y], previous.y + 1 }));
        }
    });
    return diffs;
}


void
MyersDiff::visitSnakes(std::function<void (const Point, const Point)> callback)
{
    auto path = findPath(0, 0, a.size(), b.size());
    if (!path)
    {
        return;
    }

    for (int i = 0; i < path->size() - 1; ++i)
    {
        auto current = (*path)[i];
        auto next = (*path)[i + 1];
        current = walkDiagonal(current.x, current.y, next.x, next.y, callback);

        if (next.x - current.x > next.y - current.y)
        {
            callback(current, { current.x + 1, current.y });
            current.x += 1;
        }
        else if (next.y - current.y > next.x - current.x)
        {
            callback(current, { current.x, current.y + 1 });
            current.y += 1;
        }

        walkDiagonal(current.x, current.y, next.x, next.y, callback);
    }
}


std::optional<LineMove>
MyersDiff::midpoint(const Box& box)
{
    if (box.size() == 0)
    {
        return std::nullopt;
    }
    size_t rest = box.size() % 2;
    size_t max_d = box.size() / 2 + rest;
    List<int64_t> vf;
    vf.reserve(box.size() * 2 + 1);
    vf[box.size() + 1] = box.left;
    List<int64_t> vb;
    vb.reserve(box.size() * 2 + 1);
    vb[box.size() + 1] = box.bottom;

    for (int64_t d = 0; d <= max_d; ++d)
    {
        auto forwardSnake = forwards(box, vf, vb, d, box.size());
        if (forwardSnake)
        {
            return forwardSnake;
        }
        auto backwardsSnake = backwards(box, vf, vb, d, box.size());
        if (backwardsSnake)
        {
            return backwardsSnake;
        }
    }
    throw std::runtime_error("Could not find snake.");
}


std::optional<List<Point>>
MyersDiff::findPath(int64_t left, int64_t top, int64_t right, int64_t bottom)
{
    Box box(left, top, right, bottom);
    auto snake = midpoint(box);

    if (!snake)
    {
        return std::nullopt;
    }

    auto s = *snake;
    Point& start = s.start;
    Point& end = s.end;

    auto head = findPath(box.left, box.top, start.x, start.y);
    auto tail = findPath(end.x, end.y, box.right, box.bottom);

    List<Point> points;
    if (head)
    {
        points.add(*head);
    }
    else
    {
        points.add(start);
    }
    if (tail)
    {
        points.add(*tail);
    }
    else
    {
        points.add(end);
    }
    return points;
}


std::optional<LineMove>
MyersDiff::forwards(const Box& box, List<int64_t>& vf, List<int64_t>& vb, int64_t d, int64_t max_d)
{
    for (int64_t k = d; k >= -d; k -= 2)
    {
        int64_t c = k - box.delta();
        int64_t px;
        int64_t py;
        int64_t x;
        int64_t y;
        if (k == -d || (k != d && vf[max_d + k - 1] < vf[max_d + k + 1]))
        {
            px = x = vf[max_d + k + 1];
        }
        else
        {
            px = vf[max_d + k - 1];
            x = px + 1;
        }

        y = box.top + (x - box.left) - k;
        py = (d == 0 || x != px) ? y : y - 1;

        while ((x < box.right) && (y < box.bottom) && (a[x] == b[y]))
        {
            x = x + 1;
            y = y + 1;
        }

        vf[max_d + k] = x;

        if (box.delta() % 2 != 0)
        {
            if (d != 0 && (c >= -(d - 1) && c <= (d - 1)) && y >= vb[max_d + c])
            {
                return LineMove({ static_cast<unsigned int>(px), static_cast<unsigned int>(py) }, { static_cast<unsigned int>(x), static_cast<unsigned int>(y) });
            }
        }
    }
    return std::nullopt;
}


std::optional<LineMove>
MyersDiff::backwards(const Box& box, List<int64_t>& vf, List<int64_t>& vb, int64_t d, int64_t max_d)
{
    for (int64_t c = d; c >= -d; c -= 2)
    {
        int64_t k = c + box.delta();
        int64_t px;
        int64_t py;
        int64_t x;
        int64_t y;
        if (c == -d || (c != d && vb[max_d + c - 1] > vb[max_d + c + 1]))
        {
            py = y = vb[max_d + c + 1];
        }
        else
        {
            py = vb[max_d + c - 1];
            y = py - 1;
        }

        x = box.left + (y - box.top) + k;
        px = (d == 0 || y != py) ? x : x + 1;
        while ((x > box.left) && (y > box.top) && (a[x - 1] == b[y - 1]))
        {
            x = x - 1;
            y = y - 1;
        }

        vb[max_d + c] = y;

        if (box.delta() % 2 == 0 && k >= -d && k <= d && x <= vf[max_d + k])
        {
            return LineMove({ static_cast<unsigned int>(x), static_cast<unsigned int>(y) }, { static_cast<unsigned int>(px), static_cast<unsigned int>(py) });
        }
    }
    return std::nullopt;
}


Point
MyersDiff::walkDiagonal(int64_t x1, int64_t y1, int64_t x2, int64_t y2, std::function<void (const Point, const Point)> callback)
{
    while (x1 < x2 && y1 < y2 && a[x1] == b[y1])
    {
        callback({ static_cast<unsigned int>(x1), static_cast<unsigned int>(y1), }, { static_cast<unsigned int>(x1 + 1), static_cast<unsigned int>(y1 + 1) });
        x1 = x1 + 1;
        y1 = y1 + 1;
    }
    return Point(x1, y1);
}


}