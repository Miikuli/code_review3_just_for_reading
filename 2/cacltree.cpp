#include "CalcTree.h" // Подключаем заголовочный файл с объявлениями

#include <string>      // Для работы со строками
#include <vector>      // Для использования динамических массивов (векторов)
#include <stack>       // Для использования стека в алгоритмах
#include <cmath>       // Для математических функций, таких как pow (возведение в степень)
#include <iomanip>     // Для форматирования вывода (например, setw)
#include <algorithm>   // Для различных алгоритмов, например, all_of

// Реализация конструктора узла дерева
Node::Node(std::string val) : value(val), left(nullptr), right(nullptr) {}

// Реализация функции для определения приоритета операторов
int get_priority(const std::string& op) {
    if (op == "+" || op == "-") return 1;   // Сложение и вычитание имеют низкий приоритет
    if (op == "*" || op == "/" || op == "%") return 2; // Умножение, деление и остаток имеют средний приоритет
    if (op == "^") return 3;                  // Возведение в степень имеет высокий приоритет
    return 0;                                 // Для других символов (не операторов) приоритет 0
}

// Реализация функции для проверки, является ли строка числом
bool is_number(const std::string& s) {
    // Проверяем, что все символы в строке являются цифрами
    return std::all_of(s.begin(), s.end(), ::isdigit);
}

// Реализация функции для проверки, является ли строка оператором
bool is_operator(const std::string& s) {
    // Проверяем, соответствует ли строка одному из поддерживаемых операторов
    return s == "+" || s == "-" || s == "*" || s == "/" || s == "%" || s == "^";
}

// Реализация функции для лексического анализа выражения
std::vector<std::string> tokenize(const std::string& expression) {
    std::vector<std::string> tokens;       // Вектор для хранения полученных токенов
    std::string current_token;           // Временная строка для построения токена
    for (char c : expression) {          // Итерируемся по каждому символу входного выражения
        if (isspace(c)) {                // Если символ - пробел
            if (!current_token.empty()) { // Если мы накопили токен, добавляем его в вектор
                tokens.push_back(current_token);
                current_token.clear();    // Очищаем временную строку для следующего токена
            }
        } else if (isdigit(c)) {         // Если символ - цифра
            current_token += c;           // Добавляем его к текущему токену (может быть частью многозначного числа)
        } else if (is_operator(std::string(1, c)) || c == '(' || c == ')') {
            // Если символ - оператор или скобка
            if (!current_token.empty()) { // Сначала добавляем накопленный токен (если есть)
                tokens.push_back(current_token);
                current_token.clear();
            }
            tokens.push_back(std::string(1, c)); // Затем добавляем сам оператор или скобку как отдельный токен
        } else if (c == 'x') {           // Если символ - переменная 'x'
            if (!current_token.empty()) { // Сначала добавляем накопленный токен (если есть)
                tokens.push_back(current_token);
                current_token.clear();
            }
            tokens.push_back("x");        // Добавляем 'x' как отдельный токен
        }
    }
    if (!current_token.empty()) {        // Добавляем последний накопленный токен (если есть)
        tokens.push_back(current_token);
    }
    return tokens;                       // Возвращаем вектор полученных токенов
}

// Реализация функции для построения дерева выражения из постфиксной нотации
Node* build_expression_tree(std::vector<std::string>& postfix_tokens) {
    std::stack<Node*> node_stack; // Стек для временного хранения узлов дерева
    for (const std::string& token : postfix_tokens) { // Итерируемся по токенам в постфиксной форме
        if (is_number(token) || token == "x") {
            // Если токен - число или переменная, создаем новый узел и помещаем его в стек
            node_stack.push(new Node(token));
        } else if (is_operator(token)) {
            // Если токен - оператор, извлекаем два верхних операнда из стека
            Node *right_operand = node_stack.top();
            node_stack.pop();
            Node *left_operand = node_stack.top();
            node_stack.pop();
            // Создаем новый узел для оператора, делая извлеченные операнды его потомками
            Node *op_node = new Node(token);
            op_node->left = left_operand;
            op_node->right = right_operand;
            // Помещаем новый узел оператора обратно в стек
            node_stack.push(op_node);
        }
    }
    return node_stack.top(); // В конце в стеке останется только корень построенного дерева
}

// Реализация функции для преобразования инфиксной нотации в постфиксную
std::vector<std::string> infix_to_postfix(const std::vector<std::string>& infix_tokens) {
    std::vector<std::string> postfix_tokens; // Вектор для хранения токенов в постфиксной форме
    std::stack<std::string> operator_stack;  // Стек для временного хранения операторов

    for (const std::string& token : infix_tokens) { // Итерируемся по токенам в инфиксной форме
        if (is_number(token) || token == "x") {
            // Если токен - число или переменная, сразу добавляем его в постфиксный вектор
            postfix_tokens.push_back(token);
        } else if (token == "(") {
            // Если токен - открывающая скобка, помещаем ее в стек операторов
            operator_stack.push(token);
        } else if (token == ")") {
            // Если токен - закрывающая скобка, выталкиваем операторы из стека в постфиксный вектор
            // до тех пор, пока не встретим открывающую скобку
            while (!operator_stack.empty() && operator_stack.top() != "(") {
                postfix_tokens.push_back(operator_stack.top());
                operator_stack.pop();
            }
            if (!operator_stack.empty() && operator_stack.top() == "(") {
                operator_stack.pop(); // Удаляем открывающую скобку из стека
            }
        } else if (is_operator(token)) {
            // Если токен - оператор, выталкиваем из стека операторы с более высоким или равным приоритетом
            // в постфиксный вектор, чтобы обеспечить правильный порядок операций
            while (!operator_stack.empty() && get_priority(operator_stack.top()) >= get_priority(token)) {
                postfix_tokens.push_back(operator_stack.top());
                operator_stack.pop();
            }
            operator_stack.push(token); // Помещаем текущий оператор в стек
        }
    }

    // После обработки всех токенов выталкиваем оставшиеся операторы из стека в постфиксный вектор
    while (!operator_stack.empty()) {
        postfix_tokens.push_back(operator_stack.top());
        operator_stack.pop();
    }

    return postfix_tokens; // Возвращаем вектор токенов в постфиксной форме
}

// Реализация функции для вычисления значения выражения
int evaluate(Node* node, int x_value) {
    if (!node) return 0; // Если узел пустой, возвращаем 0 (для корректной обработки)
    if (is_number(node->value)) {
        // Если узел содержит число, преобразуем строку в целое число и возвращаем его
        return std::stoi(node->value);
    } else if (node->value == "x") {
        // Если узел содержит переменную 'x', возвращаем заданное значение x_value
        return x_value;
    } else {
        // Если узел содержит оператор, рекурсивно вычисляем значения левого и правого поддеревьев
        int left_val = evaluate(node->left, x_value);
        int right_val = evaluate(node->right, x_value);
        // В зависимости от оператора выполняем соответствующую арифметическую операцию
        if (node->value == "+") return left_val + right_val;
        if (node->value == "-") return left_val - right_val;
        if (node->value == "*") return left_val * right_val;
        if (node->value == "/") return left_val / right_val;
        if (node->value == "%") return left_val % right_val;
        if (node->value == "^") return std::pow(left_val, right_val);
    }
    return 0; // Никогда не должно достичь этой точки при корректном дереве
}

// Реализация функции для преобразования поддеревьев x * A в A * x
Node* transform_tree(Node* node) {
    if (!node) return nullptr; // Если узел пустой, возвращаем пустой указатель

    // Рекурсивно обрабатываем левое и правое поддеревья
    node->left = transform_tree(node->left);
    node->right = transform_tree(node->right);

    // Если текущий узел - умножение и его левый потомок - 'x', меняем местами левого и правого потомков
    if (node->value == "*" && node->left && node->left->value == "x") {
        std::swap(node->left, node->right);
    }

    return node; // Возвращаем (возможно, преобразованный) текущий узел
}

// Реализация вспомогательной функции для рекурсивной печати дерева
void print_tree_helper(Node* node, int level, std::ofstream& outfile) {
    if (!node) return
