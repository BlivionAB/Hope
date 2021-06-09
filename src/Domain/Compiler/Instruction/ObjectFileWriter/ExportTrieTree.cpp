#include "ExportTrieTree.h"
#include "../ByteWriter.h"

namespace elet::domain::compiler::instruction::output
{


void
ExportTrieNode::insertRoutine(FunctionRoutine* routine)
{
    size_t cumulativeStringSize = _cumulativeString.size();
    Utf8StringView tailString = routine->name.subString(cumulativeStringSize);
    for (ExportTrieEdge* edge : edges)
    {
        if (tailString.startsWith(edge->label))
        {
            edge->node->insertRoutine(routine);
            return;
        }

        size_t tailCommonSize = tailString.size();
        for (size_t i = tailCommonSize; i >= 0; --i)
        {
            size_t subStringSize = tailString.size() - i;
            Utf8StringView subString = tailString.subString(0, subStringSize);
            if (edge->label.startsWith(subString))
            {
                // The routine is already added
                if (subStringSize == edge->label.size())
                {
                    return;
                }

                size_t newCumulativeStringSize = cumulativeStringSize + tailCommonSize;

                // Split old edge
                auto newEdge = new ExportTrieEdge(edge->label.subString(subStringSize), edge->node);
                edge->label = subString;
                edge->node = new ExportTrieNode(edge->node->_cumulativeString.subString(0, newCumulativeStringSize));
                edge->node->edges.add(newEdge);

                // Add tail edge
                auto tailNode = new ExportTrieNode(routine->name.subString(0, newCumulativeStringSize));
                auto tailEdge = new ExportTrieEdge(tailString.subString(subStringSize), tailNode);
                edge->node->edges.add(tailEdge);

                return;
            }
        }
    }

    auto tailNode = new ExportTrieNode(routine->name);
    auto tailEdge = new ExportTrieEdge(routine->name, tailNode);
    tailNode->hasExportInfo = true;
    tailNode->address = routine->offset;
    edges.add(tailEdge);
}


void
ExportTrieNode::addOrderedNodes(const FunctionRoutine* functionRoutine, List<ExportTrieNode*>& orderedNodes)
{
    if (!_ordered) {
        orderedNodes.add(this);
        _ordered = true;
    }
    Utf8StringView partialStr = functionRoutine->name.subString(_cumulativeString.size());
    for (const ExportTrieEdge* edge : edges)
    {
        Utf8StringView label = edge->label;
        if (partialStr.startsWith(label))
        {
            edge->node->addOrderedNodes(functionRoutine, orderedNodes);
            return;
        }
    }
}


bool
ExportTrieNode::updateOffset(uint32_t& offset)
{
    uint32_t nodeSize = 1; // Length when no export info
    if (hasExportInfo)
    {
        nodeSize = ByteWriter::getUleb128Size(flags) + ByteWriter::getUleb128Size(address);
        nodeSize += ByteWriter::getUleb128Size(nodeSize);
    }

    // Compute size of all child edges.
    ++nodeSize; // Byte for number of children.
    for (const ExportTrieEdge* edge : edges)
    {
        nodeSize += edge->label.size() + 1 // String length.
            + ByteWriter::getUleb128Size(edge->node->trieOffset); // Offset len.
    }

    // On input, 'offset' is new preferred location for this node.
    bool result = (trieOffset != offset);

    // Store new location in node object for use by parents.
    trieOffset = offset;

    // Update offset for next iteration.
    offset += nodeSize;

    // Return true if trieOffset was changed.
    return result;
}


}