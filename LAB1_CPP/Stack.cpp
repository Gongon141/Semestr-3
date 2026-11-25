#include "Stack.h"

#include <fstream>
#include <iostream>

using namespace std;

Stack::Stack() {
    top = nullptr;
}

Stack::~Stack() {
    string value;
    while (pop(value)) {
    }
}

void Stack::push(const string& value) {
    // Добавление всегда происходит в вершину за O(1).
    top = new Node{value, top};
}

bool Stack::pop(string& value) {
    // Удаляется только вершина. Это операция O(1).
    if (top == nullptr) return false;

    Node* node = top;
    value = node->value;
    top = top->next;
    delete node;
    return true;
}

bool Stack::peek(string& value) const {
    if (top == nullptr) return false;
    value = top->value;
    return true;
}

void Stack::print() const {
    Node* current = top;
    cout << "Вершина -> ";
    while (current != nullptr) {
        cout << current->value << " ";
        current = current->next;
    }
    cout << '\n';
}

bool Stack::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    return file.is_open() && load(file);
}

bool Stack::load(istream& file) {
    // В файле вершина записана первой. Добавляем узлы в конец цепочки,
    // чтобы после загрузки порядок стека не перевернулся.
    Node* last = nullptr;
    string value;
    while (file >> value) {
        Node* node = new Node{value, nullptr};
        if (top == nullptr) top = node;
        else last->next = node;
        last = node;
    }
    return !file.bad();
}

bool Stack::saveToFile(const string& fileName) const {
    ofstream file(fileName);
    if (!file) return false;

    save(file);
    file.close();
    return static_cast<bool>(file);
}

void Stack::save(ostream& file) const {
    Node* current = top;
    while (current != nullptr) {
        file << current->value << '\n';
        current = current->next;
    }
}

void runStack(const string& fileName) {
    Stack stack;
    stack.loadFromFile(fileName);

    int command = -1;
    while (command != 0) {
        cout << "\nСтек\n";
        cout << "1 - push\n2 - pop\n3 - посмотреть вершину\n";
        cout << "4 - вывести стек\n0 - выход\nКоманда: ";
        cin >> command;

        string value;
        switch (command) {
            case 1:
                cout << "Значение: ";
                cin >> value;
                stack.push(value);
                stack.saveToFile(fileName);
                break;
            case 2:
                if (stack.pop(value)) {
                    cout << "Удалено: " << value << '\n';
                    stack.saveToFile(fileName);
                } else {
                    cout << "Стек пуст\n";
                }
                break;
            case 3:
                if (stack.peek(value)) cout << "Вершина: " << value << '\n';
                else cout << "Стек пуст\n";
                break;
            case 4:
                stack.print();
                break;
        }
    }
}
