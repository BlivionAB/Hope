#ifndef ELET_EXPORTTRIETREE_H
#define ELET_EXPORTTRIETREE_H


#include "Foundation/Utf8StringView.h"
#include "Foundation/List.h"
#include "../Instruction.h"


namespace elet::domain::compiler::instruction::output
{
    const size_t ALPHABET_SIZE = 57; // From A to z in ASCII


    struct SparseExportTrieNode;

    struct ExportTrieNode;


    enum ExportSymbolKind
    {
        EXPORT_SYMBOL_FLAGS_KIND_REGULAR = 0x00u,
        EXPORT_SYMBOL_FLAGS_KIND_THREAD_LOCAL = 0x01u,
        EXPORT_SYMBOL_FLAGS_KIND_ABSOLUTE = 0x02u,

        EXPORT_SYMBOL_FLAGS_KIND_MASK = 0x03u,
        EXPORT_SYMBOL_FLAGS_WEAK_DEFINITION = 0x04u,
        EXPORT_SYMBOL_FLAGS_REEXPORT = 0x08u,
        EXPORT_SYMBOL_FLAGS_STUB_AND_RESOLVER = 0x10u
    };


    struct ExportTrieEdge
    {
        Utf8StringView
        label;

        ExportTrieNode*
        node;

        ExportTrieEdge(Utf8StringView label, ExportTrieNode* node):
            label(label),
            node(node)
        { }
    };


    class ExportTrieNode
    {

    public:
        ExportTrieNode():
            _cumulativeString("")
        { }

        ExportTrieNode(Utf8StringView label):
            _cumulativeString(label)
        { }

        void
        addOrderedNodes(const FunctionRoutine* functionRoutine, List<ExportTrieNode*>& orderedNodes);

        void
        insertRoutine(FunctionRoutine* routine, uint64_t textStartOffset);

        bool
        updateOffset(uint32_t& offset);

        uint64_t
        address;

        uint64_t
        size;

        List<ExportTrieEdge*>
        edges;

        bool
        hasExportInfo;

        ExportSymbolKind
        flags;

        uint64_t
        trieOffset;

    private:

        Utf8StringView
        _cumulativeString;

        bool
        _ordered = false;

        ExportTrieNode*
        parent;

        FunctionRoutine*
        routine;
    };


    struct ChildPointer
    {
        Utf8String
        label;

        SparseExportTrieNode*
        child;

        ChildPointer(SparseExportTrieNode* child):
            child(child)
        { }
    };


    struct SparseExportTrieNode
    {
        SparseExportTrieNode*
        parent;

        List<ChildPointer*>
        children;

        Utf8StringView
        fromLabel;

        FunctionRoutine*
        functionRoutine;

        SparseExportTrieNode():
            parent(nullptr)
        { }

        SparseExportTrieNode(SparseExportTrieNode* parent):
            parent(parent)
        { }
    };

    struct SparseNodeMap
    {
        ExportTrieNode*
        node;

        SparseExportTrieNode*
        sparseTrieNode;

        SparseNodeMap(ExportTrieNode* node, SparseExportTrieNode* sparseTrieNode):
            node(node),
            sparseTrieNode(sparseTrieNode)
        { }
    };
}
#endif //ELET_EXPORTTRIETREE_H
