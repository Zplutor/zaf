#include "logic/interpret/expression/number_interpreter.h"
#include <cctype>
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/operand_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

class InnerNumberInterpreter : public TerminalInterpreter {
public:
    InnerNumberInterpreter(wchar_t prefix, const std::wstring& chars, std::size_t radix) :
        prefix_(prefix),
        chars_(chars),
        radix_(radix) {

    }

    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {

        std::wstring text;
        auto status = InterpretText(context, text);
        if (status != InterpretStatus::Ok) {
            return status;
        }

        auto operand_node = std::make_shared<OperandNode>();
        operand_node->radix = radix_;
        operand_node->text = text;

        syntax_tree_builder.AddOperand(operand_node);
        return InterpretStatus::Ok;
    }

private:
    InterpretStatus InterpretText(InterpretContext& context, std::wstring& text) {

        wchar_t current_char = context.GetCurrentChar();

        if (prefix_ != 0) {

            if (std::tolower(current_char) != prefix_) {
                return InterpretStatus::Mismatched;
            }

            if (!context.Forward()) {
                return InterpretStatus::Error;
            }
        }

        auto status = GetMatchedNumberChars(context, text);
        if (status == InterpretStatus::Error) {
            return status;
        }

        if (status == InterpretStatus::Mismatched) {
            return prefix_ == 0 ? InterpretStatus::Mismatched : InterpretStatus::Error;
        }

        return InterpretStatus::Ok;
    }


    InterpretStatus GetMatchedNumberChars(InterpretContext& context, std::wstring& number_string) {

        do {

            wchar_t current_char = context.GetCurrentChar();
            if (chars_.find(std::tolower(current_char)) == std::wstring::npos) {
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

private:
    wchar_t prefix_;
    std::wstring chars_;
    std::size_t radix_;
};


NumberInterpreter::NumberInterpreter() {

    auto decimal_number_interpreter = std::make_shared<InnerNumberInterpreter>(0, L"0123456789", 10);
    AddRuleInterpreter(CreateRuleInterpreter({ decimal_number_interpreter }));

    auto hex_number_interpreter = std::make_shared<InnerNumberInterpreter>(L'x', L"0123456789abcdef", 16);
    AddRuleInterpreter(CreateRuleInterpreter({ hex_number_interpreter }));

    auto binary_number_interpreter = std::make_shared<InnerNumberInterpreter>(L'b', L"01", 2);
    AddRuleInterpreter(CreateRuleInterpreter({ binary_number_interpreter }));

    auto octal_number_interpreter = std::make_shared<InnerNumberInterpreter>(L'o', L"01234567", 8);
    AddRuleInterpreter(CreateRuleInterpreter({ octal_number_interpreter }));
}