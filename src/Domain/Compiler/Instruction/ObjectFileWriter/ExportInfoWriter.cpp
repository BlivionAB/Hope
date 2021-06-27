#include "ExportInfoWriter.h"
#include "ExportTrieTree.h"
#include <set>

namespace elet::domain::compiler::instruction::output
{


ExportInfoWriter::ExportInfoWriter(
    MachoFileWriter* machoWriter):

    _machoFileWriter(machoWriter),
    _bw(new ByteWriter(&machoWriter->output, &machoWriter->offset))
{

}


void
ExportInfoWriter::write()
{
    ExportTrieNode* rootNode = createTrieTreeFromExports();
    List<ExportTrieNode*> orderedNodes;
    for (const auto& exportedRoutine : _machoFileWriter->assemblyWriter->exportedRoutines)
    {
        rootNode->addOrderedNodes(exportedRoutine, orderedNodes);
    }
    setOffsetInNodes(orderedNodes);
    writeExports(orderedNodes);
}

void
ExportInfoWriter::setOffsetInNodes(List<ExportTrieNode*>& orderedNodes) const
{
    bool more;
    do
    {
        uint32_t offset = 0;
        more = false;
        for (ExportTrieNode* node : orderedNodes)
        {
            if (node->updateOffset(offset))
            {
                more = true;
            }
        }
    }
    while (more);
}


ExportTrieNode*
ExportInfoWriter::createTrieTreeFromExports()
{
    ExportTrieNode* root = new ExportTrieNode();
    for (const auto& exportedRoutine : _machoFileWriter->assemblyWriter->exportedRoutines)
    {
        root->insertRoutine(exportedRoutine, _machoFileWriter->textSegmentStartOffset);
    }
    return root;
}


void
ExportInfoWriter::writeExports(const List<ExportTrieNode*>& orderedNodes)
{
    for (const ExportTrieNode* node : orderedNodes)
    {
        if (node->hasExportInfo)
        {
            // Terminal Size
            _bw->writeUleb128(ByteWriter::getUleb128Size(node->address) + ByteWriter::getUleb128Size(node->flags));

            // Flags
            _bw->writeByte(EXPORT_SYMBOL_FLAGS_KIND_REGULAR);

            // Symbol offset
            _bw->writeUleb128(node->address);

            // Child count
            _bw->writeByte(0);
        }
        else
        {
            // Terminal Size
            _bw->writeByte(0);

            // Child count
            _bw->writeByte(node->edges.size());

            for (const ExportTrieEdge* edge : node->edges)
            {
                _bw->writeString(edge->label);
                _bw->writeUleb128(edge->node->trieOffset);
            }
        }
    }
}


Utf8String
ExportInfoWriter::getSymbolStringFromNode(const SparseExportTrieNode* node)
{
    return Utf8String(node->fromLabel.toString()) + getSymbolStringFromNode(node->parent);
}


}