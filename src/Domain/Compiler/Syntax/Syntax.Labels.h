#ifndef ELET_SYNTAX_LABELS_H
#define ELET_SYNTAX_LABELS_H


#define LABEL__DECLARATION         (std::uint8_t)(1 << 0)
#define LABEL__TYPE_DECLARATION    (std::uint8_t)(1 << 1)
#define LABEL__VALUE_DECLARATION   (std::uint8_t)(1 << 2)
#define LABEL__BLOCK               (std::uint8_t)(1 << 3)
#define LABEL__NAMED_EXPRESSION    (std::uint8_t)(1 << 4)
#define LABEL__USING_STATEMENT     (std::uint8_t)(1 << 5)


#endif //ELET_SYNTAX_LABELS_H
