#include "logic/interpret/expression/number_interpreter.h"
#include <cctype>
#include "logic/interpret/interpret_context.h"
#include "logic/syntax_tree/operand_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

static InterpretStatus GetMatchedNumberChars(InterpretContext& context, std::wstring& number_string);

InterpretStatus NumberInterpreter::Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) {

    std::wstring text;
    auto status = GetMatchedNumberChars(context, text);
    if (status != InterpretStatus::Ok) {
        return status;
    }

    auto operand_node = std::make_shared<OperandNode>();
    operand_node->text = text;

    syntax_tree_builder.AddOperand(operand_node);
    return InterpretStatus::Ok;
}


InterpretStatus GetMatchedNumberChars(InterpretContext& context, std::wstring& number_string) {

    const std::wstring chars(L"0123456789");

    do {

        wchar_t current_char = context.GetCurrentChar();
        if (chars.find(std::tolower(current_char)) == std::wstring::npos) {
            break;
        }

        number_string.append(1, current_char);

    } while (context.Forward());

    if (number_string.empty()) {
        return InterpretStatus::Mismatched;
    }
    else {
        return InterpretStatus::Ok;
    }
}
