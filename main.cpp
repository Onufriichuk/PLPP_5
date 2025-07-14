#include <iostream>
#include <string>
#include <map>
#include <cmath>
#include <cctype>

struct UserFunc {
    std::string funcName;
    std::string arg1;
    std::string arg2;
    std::string funcBody;
};

class ExprEvaluator {
private:
    std::string exprStr;
    size_t index;
    std::map<std::string, double> vars;
    std::map<std::string, UserFunc> userFunctions;

public:
    ExprEvaluator() : index(0) {}

    double compute(const std::string& expression) {
        exprStr = expression;
        index = 0;

        skipSpaces();

        if (startsWith("var")) {
            return processVarAssignment();
        }

        if (startsWith("def")) {
            return processFuncDef();
        }

        return parseExpression();
    }

private:
    void skipSpaces() {
        while (index < exprStr.size() && std::isspace(exprStr[index])) {
            ++index;
        }
    }

    bool startsWith(const std::string& keyword) {
        skipSpaces();
        if (exprStr.compare(index, keyword.length(), keyword) == 0) {
            return true;
        }
        return false;
    }

    std::string readIdentifier() {
        skipSpaces();
        std::string id;
        while (index < exprStr.size() && (std::isalnum(exprStr[index]) || exprStr[index] == '_')) {
            id.push_back(exprStr[index]);
            ++index;
        }
        return id;
    }

    double readNumber() {
        skipSpaces();
        size_t startPos = index;
        bool dotFound = false;
        if (exprStr[index] == '-') {
            ++index;
        }
        while (index < exprStr.size() && (std::isdigit(exprStr[index]) || exprStr[index] == '.')) {
            if (exprStr[index] == '.') {
                if (dotFound) break;
                dotFound = true;
            }
            ++index;
        }
        return std::stod(exprStr.substr(startPos, index - startPos));
    }

    double processVarAssignment() {
        // skip "var"
        index += 3;
        skipSpaces();

        std::string varName = readIdentifier();
        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != '=') {
            throw std::runtime_error("Expected '=' after variable name");
        }
        ++index; // skip '='

        double val = parseExpression();
        vars[varName] = val;
        return val;
    }

    double processFuncDef() {
        // skip "def"
        index += 3;
        skipSpaces();

        std::string fnName = readIdentifier();
        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != '(') {
            throw std::runtime_error("Expected '(' after function name");
        }
        ++index; // skip '('

        std::string param1 = readIdentifier();
        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != ',') {
            throw std::runtime_error("Expected ',' between parameters");
        }
        ++index; // skip ','

        std::string param2 = readIdentifier();
        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != ')') {
            throw std::runtime_error("Expected ')' after parameters");
        }
        ++index; // skip ')'

        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != '{') {
            throw std::runtime_error("Expected '{' before function body");
        }
        ++index; // skip '{'

        std::string body;
        while (index < exprStr.size() && exprStr[index] != '}') {
            body.push_back(exprStr[index]);
            ++index;
        }

        if (index >= exprStr.size() || exprStr[index] != '}') {
            throw std::runtime_error("Expected '}' after function body");
        }
        ++index; // skip '}'

        userFunctions[fnName] = {fnName, param1, param2, body};
        std::cout << "Function " << fnName << " created." << std::endl;
        return 0;
    }

    double parseFactor() {
        skipSpaces();

        if (index >= exprStr.size()) {
            throw std::runtime_error("Unexpected end of expression");
        }

        if (exprStr[index] == '(') {
            ++index;
            double val = parseExpression();
            skipSpaces();
            if (exprStr[index] == ')') {
                ++index;
            } else {
                throw std::runtime_error("Expected ')'");
            }
            return val;
        }

        if (std::isalpha(exprStr[index])) {
            std::string id = readIdentifier();
            skipSpaces();

            if (index < exprStr.size() && exprStr[index] == '(') {
                return parseFuncCall(id);
            } else {
                if (vars.count(id)) {
                    return vars[id];
                } else {
                    throw std::runtime_error("Unknown variable: " + id);
                }
            }
        }

        return readNumber();
    }

    double parseTerm() {
        double left = parseFactor();
        while (true) {
            skipSpaces();
            if (index >= exprStr.size()) break;

            char op = exprStr[index];
            if (op == '*' || op == '/') {
                ++index;
                double right = parseFactor();
                if (op == '*') left *= right;
                else {
                    if (right == 0) throw std::runtime_error("Division by zero");
                    left /= right;
                }
            } else {
                break;
            }
        }
        return left;
    }

    double parseExpression() {
        double left = parseTerm();
        while (true) {
            skipSpaces();
            if (index >= exprStr.size()) break;

            char op = exprStr[index];
            if (op == '+' || op == '-') {
                ++index;
                double right = parseTerm();
                if (op == '+') left += right;
                else left -= right;
            } else {
                break;
            }
        }
        return left;
    }

    double parseFuncCall(const std::string& funcName) {
        ++index; // skip '('
        double arg1 = parseExpression();
        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != ',') {
            throw std::runtime_error("Expected ',' in function call");
        }
        ++index; // skip ','

        double arg2 = parseExpression();
        skipSpaces();

        if (index >= exprStr.size() || exprStr[index] != ')') {
            throw std::runtime_error("Expected ')' at end of function call");
        }
        ++index; // skip ')'

        if (funcName == "pow") return std::pow(arg1, arg2);
        if (funcName == "abs") return std::abs(arg1);
        if (funcName == "max") return std::max(arg1, arg2);
        if (funcName == "min") return std::min(arg1, arg2);

        if (userFunctions.count(funcName)) {
            return runUserFunc(funcName, arg1, arg2);
        }

        throw std::runtime_error("Unknown function: " + funcName);
    }

    double runUserFunc(const std::string& name, double a1, double a2) {
        UserFunc f = userFunctions[name];
        // Save current variables to restore after
        auto oldVars = vars;
        vars[f.arg1] = a1;
        vars[f.arg2] = a2;

        ExprEvaluator nestedEval;
        nestedEval.vars = vars;
        nestedEval.userFunctions = userFunctions;

        double res = nestedEval.compute(f.funcBody);
        vars = oldVars; // restore variables
        return res;
    }
};

int main() {
    ExprEvaluator interpreter;

    std::cout << "Arithmetic Interpreter v2" << std::endl;
    std::cout << "Supports: +, -, *, /, var assignment, user functions" << std::endl;
    std::cout << "Commands: var, def, quit" << std::endl;

    std::string line;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line == "quit" || line == "exit") break;
        if (line.empty()) continue;

        try {
            double result = interpreter.compute(line);
            if (line.find("def") == std::string::npos) {
                std::cout << result << std::endl;
            }
        } catch (const std::exception& ex) {
            std::cout << "Error: " << ex.what() << std::endl;
        }
    }
    return 0;
}
