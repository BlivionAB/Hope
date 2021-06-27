#ifndef ELET_EXPORTINFOWRITER_H
#define ELET_EXPORTINFOWRITER_H


#include "DyldInfoWriter.h"
#include <set>
#include "ExportTrieTree.h"


namespace elet::domain::compiler::instruction::output
{


class DyldInfoWriter;
class MachoFileWriter;




class ExportInfoWriter
{

public:

    ExportInfoWriter(MachoFileWriter* machoWriter);

    void
    write();

private:

    MachoFileWriter*
    _machoFileWriter;

    ByteWriter*
    _bw;

    ExportTrieNode*
    createTrieTreeFromExports();

    void
    writeExports(const List<ExportTrieNode*>& orderedNodes);

    Utf8String
    getSymbolStringFromNode(const SparseExportTrieNode* pNode);

    void
    setOffsetInNodes(List<ExportTrieNode*>& orderedNodes) const;
};


}
#endif //ELET_EXPORTINFOWRITER_H
