#pragma once

#include <fstream>
#include <string>
#include <iosfwd>

using namespace std;

// Full Binary Tree: у каждого узла должно быть 0 или 2 ребёнка.
class FullBinaryTree {
private:
    // В бинарном дереве у узла не больше двух потомков.
    struct Node {
        string value;
        Node* left;
        Node* right;
    };

    Node* root;

    // Вспомогательные рекурсивные методы.
    void clear(Node* node);
    Node* findNode(Node* node, const string& value) const;
    bool removeLastLeaf(Node*& node, int level, Node* target);
    bool isFull(Node* node) const;
    int height(Node* node) const;
    bool insertAtLevel(Node* node, int level, const string& value);
    void print(Node* node, int level) const;
    // При записи знак # обозначает пустую ветвь.
    void saveNode(ostream& file, Node* node) const;
    Node* loadNode(istream& file);

public:
    FullBinaryTree();
    ~FullBinaryTree();

    // Одиночная вставка может временно нарушить свойство Full.
    void insert(const string& value);
    // Вставка сразу двух детей сохраняет свойство Full.
    bool addChildren(const string& parent,
                     const string& leftValue,
                     const string& rightValue);
    bool remove(const string& value);
    bool contains(const string& value) const;
    bool isFull() const;
    void print() const;

    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    bool load(istream& file);
    void save(ostream& file) const;
};

void runFullBinaryTree(const string& fileName);
