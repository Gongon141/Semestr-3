#pragma once

#include <string>
#include <iosfwd>

using namespace std;

// Односвязный список. Каждый узел знает только следующий узел.
class SinglyList {
private:
    // Node является одним элементом списка.
    struct Node {
        string value;
        Node* next;
    };

    Node* head; // Первый элемент.
    Node* tail; // Последний элемент.

    Node* findNode(const string& value) const;
    void printReverseNode(Node* node) const;

public:
    SinglyList();
    ~SinglyList();

    // Четыре способа вставки: в начало, в конец, до и после элемента.
    void pushFront(const string& value);
    void pushBack(const string& value);
    bool insertBefore(const string& key, const string& value);
    bool insertAfter(const string& key, const string& value);
    // Четыре способа удаления: из начала, из конца, до и после элемента.
    bool popFront();
    bool popBack();
    bool removeBefore(const string& key);
    bool removeAfter(const string& key);
    bool removeValue(const string& value);
    bool contains(const string& value) const;
    bool get(int index, string& value) const;
    void print() const;
    void printReverse() const;

    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    bool load(istream& file);
    void save(ostream& file) const;
};

void runSinglyList(const string& fileName);
