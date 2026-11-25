#pragma once

#include <string>
#include <iosfwd>

using namespace std;

// Двусвязный список. Узел хранит ссылки на соседа слева и справа.
class DoublyList {
private:
    struct Node {
        string value;
        Node* previous;
        Node* next;
    };

    Node* head; // Первый элемент.
    Node* tail; // Последний элемент.

    Node* findNode(const string& value) const;
    // Общий метод удаления уже найденного узла.
    void removeNode(Node* node);

public:
    DoublyList();
    ~DoublyList();

    void pushFront(const string& value);
    void pushBack(const string& value);
    bool insertBefore(const string& key, const string& value);
    bool insertAfter(const string& key, const string& value);
    bool popFront();
    bool popBack();
    bool removeBefore(const string& key);
    bool removeAfter(const string& key);
    bool removeValue(const string& value);
    bool contains(const string& value) const;
    bool get(int index, string& value) const;
    // Благодаря previous список можно читать в обратную сторону.
    void printForward() const;
    void printBackward() const;

    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    bool load(istream& file);
    void save(ostream& file) const;
};

void runDoublyList(const string& fileName);
