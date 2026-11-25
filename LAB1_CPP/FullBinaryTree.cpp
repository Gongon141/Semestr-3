#include "FullBinaryTree.h"

#include <iostream>

using namespace std;

FullBinaryTree::FullBinaryTree() {
    root = nullptr;
}

FullBinaryTree::~FullBinaryTree() {
    clear(root);
}

void FullBinaryTree::clear(Node* node) {
    // Обход postorder: сначала удаляем детей, затем сам узел.
    if (node == nullptr) return;
    clear(node->left);
    clear(node->right);
    delete node;
}

FullBinaryTree::Node* FullBinaryTree::findNode(Node* node,
                                               const string& value) const {
    // Прямой обход: корень, левое поддерево, правое поддерево.
    if (node == nullptr || node->value == value) return node;

    Node* found = findNode(node->left, value);
    if (found != nullptr) return found;
    return findNode(node->right, value);
}

int FullBinaryTree::height(Node* node) const {
    // Высота равна единице плюс максимум высот двух поддеревьев.
    if (node == nullptr) return 0;

    int leftHeight = height(node->left);
    int rightHeight = height(node->right);
    return 1 + (leftHeight > rightHeight ? leftHeight : rightHeight);
}

bool FullBinaryTree::insertAtLevel(Node* node,
                                   int level,
                                   const string& value) {
    if (node == nullptr) return false;

    if (level == 1) {
        // Сначала заполняем левую ветвь, затем правую.
        if (node->left == nullptr) {
            node->left = new Node{value, nullptr, nullptr};
            return true;
        }
        if (node->right == nullptr) {
            node->right = new Node{value, nullptr, nullptr};
            return true;
        }
        return false;
    }

    if (insertAtLevel(node->left, level - 1, value)) return true;
    return insertAtLevel(node->right, level - 1, value);
}

void FullBinaryTree::insert(const string& value) {
    if (root == nullptr) {
        root = new Node{value, nullptr, nullptr};
        return;
    }

    // Просматриваем уровни сверху вниз и вставляем в первое свободное место.
    int treeHeight = height(root);
    for (int level = 1; level <= treeHeight; level++) {
        if (insertAtLevel(root, level, value)) return;
    }
}

bool FullBinaryTree::addChildren(const string& parent,
                                 const string& leftValue,
                                 const string& rightValue) {
    Node* node = findNode(root, parent);

    // Пару можно добавить только существующему листу.
    if (node == nullptr || node->left != nullptr || node->right != nullptr) {
        return false;
    }

    node->left = new Node{leftValue, nullptr, nullptr};
    node->right = new Node{rightValue, nullptr, nullptr};
    return true;
}

bool FullBinaryTree::removeLastLeaf(Node*& node, int level, Node* target) {
    if (node == nullptr) return false;

    // На самом глубоком уровне каждый узел — лист.
    if (level == 1) {
        if (node != target) target->value.swap(node->value);
        delete node;
        node = nullptr;
        return true;
    }

    // Идём справа налево, чтобы снять крайний правый лист этого уровня.
    if (removeLastLeaf(node->right, level - 1, target)) return true;
    return removeLastLeaf(node->left, level - 1, target);
}

bool FullBinaryTree::remove(const string& value) {
    Node* target = findNode(root, value);
    if (target == nullptr) return false;

    return removeLastLeaf(root, height(root), target);
}

bool FullBinaryTree::contains(const string& value) const {
    return findNode(root, value) != nullptr;
}

bool FullBinaryTree::isFull(Node* node) const {
    // Пустая ветвь и лист удовлетворяют условию Full.
    if (node == nullptr) return true;

    if (node->left == nullptr && node->right == nullptr) return true;
    // Ровно один ребёнок нарушает условие Full.
    if (node->left == nullptr || node->right == nullptr) return false;

    // Внутренний узел имеет двух детей, проверяем оба поддерева.
    return isFull(node->left) && isFull(node->right);
}

bool FullBinaryTree::isFull() const {
    return isFull(root);
}

void FullBinaryTree::print(Node* node, int level) const {
    if (node == nullptr) return;

    // Правую ветвь печатаем выше корня, левую ниже.
    print(node->right, level + 1);
    for (int i = 0; i < level; i++) cout << "    ";
    cout << node->value << '\n';
    print(node->left, level + 1);
}

void FullBinaryTree::print() const {
    if (root == nullptr) {
        cout << "Дерево пусто\n";
        return;
    }
    print(root, 0);
}

void FullBinaryTree::saveNode(ostream& file, Node* node) const {
    if (node == nullptr) {
        file << "#\n";
        return;
    }

    // Прямой обход и метки # позволяют восстановить точную форму дерева.
    file << node->value << '\n';
    saveNode(file, node->left);
    saveNode(file, node->right);
}

FullBinaryTree::Node* FullBinaryTree::loadNode(istream& file) {
    string value;
    if (!(file >> value) || value == "#") return nullptr;

    // Читаем узлы в том же порядке, в котором saveNode их записал.
    Node* node = new Node{value, nullptr, nullptr};
    node->left = loadNode(file);
    node->right = loadNode(file);
    return node;
}

bool FullBinaryTree::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    return file.is_open() && load(file);
}

bool FullBinaryTree::load(istream& file) {
    clear(root);
    root = loadNode(file);
    return !file.bad();
}

bool FullBinaryTree::saveToFile(const string& fileName) const {
    ofstream file(fileName);
    if (!file) return false;

    save(file);
    file.close();
    return static_cast<bool>(file);
}

void FullBinaryTree::save(ostream& file) const {
    saveNode(file, root);
}

void runFullBinaryTree(const string& fileName) {
    FullBinaryTree tree;
    tree.loadFromFile(fileName);

    int command = -1;
    while (command != 0) {
        cout << "\nFull Binary Tree\n";
        cout << "1 - добавить один элемент\n";
        cout << "2 - добавить двух детей к листу\n";
        cout << "3 - найти элемент\n";
        cout << "4 - проверить свойство Full\n";
        cout << "5 - вывести дерево\n";
        cout << "0 - выход\nКоманда: ";
        cin >> command;

        string value;
        string parent;
        string leftValue;
        string rightValue;

        switch (command) {
            case 1:
                cout << "Значение: ";
                cin >> value;
                if (value == "#") {
                    cout << "Символ # используется для записи пустой ветви\n";
                    break;
                }
                tree.insert(value);
                tree.saveToFile(fileName);
                break;
            case 2:
                cout << "Родитель, левый и правый ребёнок: ";
                cin >> parent >> leftValue >> rightValue;
                if (leftValue == "#" || rightValue == "#") {
                    cout << "Символ # использовать нельзя\n";
                } else if (tree.addChildren(parent, leftValue, rightValue)) {
                    tree.saveToFile(fileName);
                } else {
                    cout << "Родитель не найден или уже имеет детей\n";
                }
                break;
            case 3:
                cout << "Значение: ";
                cin >> value;
                cout << (tree.contains(value) ? "Найдено\n" : "Не найдено\n");
                break;
            case 4:
                cout << (tree.isFull() ? "Дерево Full\n" : "Дерево не Full\n");
                break;
            case 5:
                tree.print();
                break;
        }
    }
}
