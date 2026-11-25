#pragma once

#include <string>
#include <iosfwd>

using namespace std;

// Очередь работает по принципу FIFO: первый вошёл, первый вышел.
class Queue {
private:
    struct Node {
        string value;
        Node* next;
    };

    Node* first; // Из этой стороны удаляем.
    Node* last;  // В эту сторону добавляем.

public:
    Queue();
    ~Queue();

    void push(const string& value);
    bool pop(string& value);
    bool peek(string& value) const;
    void print() const;

    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    bool load(istream& file);
    void save(ostream& file) const;
};

void runQueue(const string& fileName);
