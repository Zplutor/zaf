#pragma once

#include "logic/interpret/interpreter.h"

class SymbolInterpreter : public Interpreter {

};


template<typename InterpreterType>
class SymbolProxyInterpreter : public SymbolInterpreter {
public:
    InterpretStatus Interpret(InterpretContext& context, SyntaxTreeBuilder& syntax_tree_builder) override {
        if (interpreter_ == nullptr) {
            interpreter_ = std::make_shared<InterpreterType>();
        }
        return interpreter_->Interpret(context, syntax_tree_builder);
    }

private:
    std::shared_ptr<InterpreterType> interpreter_;
};