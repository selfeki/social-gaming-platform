#include "DSLInterpreter.h"

#include <numeric>
#include <string_view>

namespace gameSpecification::rule {


void InterpretVisitor::visitImpl(const ForEach& forEach) {
    scope.push(&forEach);
    /*
    for each expression in elemList:
      set loop variable to that expression by updating context
      for each rule:
        visit rule
  */
    auto elemList = boost::get<ExpList>(forEach.elemList);
    auto element = boost::get<std::string_view>(forEach.elem);
    for (const auto& exp : elemList.list) {
        context.map[element] = exp;
        for (const auto& rule : forEach.rules) {
            rule->accept(*this);
        }
    }
    if (!needUserInput) {
        scope.pop();
    }
}

std::string
interpolateString(const std::string_view str) {
    // parses strings like
    // "Round {round}. Choose your weapon!"
    // to "Round 1. Choose your weapon!"
}

Expression
evaluateExpression(const std::string_view str) {
    // parses string like
    // "configuration.Rounds.upfrom(1)"
    // into an actual expression
}

State Interpreter::operator()(InputVote const& inputVote) const {    // To change to consider Time Out
    if (State(state).stateMap.getDataMap()[DataPacket("input_recieved")].getData().compare("true") != 0) {
        Rule promptMessage((MessageRule(inputVote.messageTosend, inputVote.players)));
        State st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptMessage);
        std::vector<std::string> vectorOfChoices;
        for (DataPacket choice : inputVote.choices) {
            vectorOfChoices.push_back(choice.getData());
        }
        std::string choicesString = std::accumulate(vectorOfChoices.begin(), vectorOfChoices.end(), std::string(" "));
        Rule promptChoices((MessageRule(DataPacket(choicesString), inputVote.players)));
        st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptChoices);
        return State(st);
    }
    MapOfArithmeticVariables choicesMap(std::pair<DataPacket, ArithmeticValue>(DataPacket("None"), ArithmeticValue(0)));
    for (Player& p1 : players.getPlayers()) {
        DataPacket choice = State(state).stateMap.getDataMap().find(*(p1.name()))->second;
        if (choicesMap.getDataMap().find(choice) == choicesMap.getDataMap().end()) {
            choicesMap.insertNew(choice, ArithmeticValue(1));
        } else {
            choicesMap.getDataMap()[choice] = choicesMap.getDataMap()[choice].getData() + 1;
        }
    }
    DataPacket bestChoice = std::max_element(choicesMap.getDataMap().begin(), choicesMap.getDataMap().end(),
        [](const std::pair<DataPacket, ArithmeticValue> pair1, const std::pair<DataPacket, ArithmeticValue> pair2) {
            return pair1.second.getData() < pair2.second.getData();
        })->first;
    State st(state);
    st.stateMap.insertNew(inputVote.resultVariable, bestChoice);
    st.stateMap.getDataMap()[DataPacket("input_recieved")] = DataPacket("false");
    return st;
}
State Interpreter::operator()(InputText const& iT) const {
    if (State(state).stateMap.getDataMap()[DataPacket("input_recieved")].getData().compare("true") != 0) {
        Rule promptMessage((MessageRule(iT.messageTosend, iT.player)));
        State st = boost::apply_visitor(Interpreter(players.getPlayers(), state), promptMessage);
        return st;
    }
    State st(state);
    DataPacket dataRecieved = st.stateMap.getDataMap()[DataPacket("input_recieved")];
    st.stateMap.insertNew(iT.resultVariable, dataRecieved);
    st.stateMap.getDataMap()[DataPacket("input_recieved")] = DataPacket("false");
    return st;
}


// Question: difference b/w list and list expression in following example?

// { "rule": "foreach",
//   "list": <<list, list expression, or name of a list object>>,
//   "element": << name for list element object within the rules below >>
//   "rules": [
//     << Rules to execute on every element of the given list >>
//   ]
// }

}    // namespace gameSpecification::rule
