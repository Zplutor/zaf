#include "logic/interpret/command_interpreter.h"
#include <cctype>
#include "logic/interpret/controller/controller_interpreter.h"
#include "logic/interpret/expression/expression_interpreter.h"
#include "logic/interpret/interpret_context.h"
#include "logic/interpret/rule_interpreter.h"
#include "logic/interpret/terminal_interpreter.h"
#include "logic/syntax_tree/operator_node.h"
#include "logic/syntax_tree/syntax_tree_builder.h"

CommandInterpreter::CommandInterpreter() {

    AddRuleInterpreter(CreateRuleInterpreter({ 
        std::make_shared<ExpressionInterpreter>(),
        std::make_shared<ControllerInterpreter>(),
    }));
}