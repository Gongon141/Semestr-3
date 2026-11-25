#pragma once

#include <string>
#include <iosfwd>

using namespace std;

// Стек работает по принципу LIFO: последний вошёл, первый вышел.
class Stack {
private:
    struct Node {
        string value;
        Node* next;
    };

    Node* top; // Указатель на вершину стека.

public:
    Stack();
    ~Stack();

    void push(const string& value);
    bool pop(string& value);
    bool peek(string& value) const;
    void print() const;

    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    bool load(istream& file);
    void save(ostream& file) const;
};

void runStack(const string& fileName);
