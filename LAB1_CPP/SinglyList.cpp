#include "SinglyList.h"

#include <fstream>
#include <iostream>

using namespace std;

SinglyList::SinglyList() {
    head = nullptr;
    tail = nullptr;
}

SinglyList::~SinglyList() {
    // Освобождаем каждый динамически созданный узел.
    while (head != nullptr) {
        Node* node = head;
        head = head->next;
        delete node;
    }
}

SinglyList::Node* SinglyList::findNode(const string& value) const {
    // Последовательно идём от головы. Поиск имеет сложность O(n).
    Node* current = head;
    while (current != nullptr) {
        if (current->value == value) return current;
        current = current->next;
    }
    return nullptr;
}

void SinglyList::pushFront(const string& value) {
    // Новый узел становится головой и указывает на старую голову.
    Node* node = new Node{value, head};
    head = node;
    if (tail == nullptr) tail = node;
}

void SinglyList::pushBack(const string& value) {
    // tail позволяет не проходить весь список при добавлении в конец.
    Node* node = new Node{value, nullptr};
    if (tail == nullptr) {
        head = node;
        tail = node;
        return;
    }
    tail->next = node;
    tail = node;
}

bool SinglyList::insertBefore(const string& key, const string& value) {
    if (head == nullptr) return false;
    if (head->value == key) {
        pushFront(value);
        return true;
    }

    // В односвязном списке надо найти узел перед элементом key.
    Node* current = head;
    while (current->next != nullptr) {
        if (current->next->value == key) {
            current->next = new Node{value, current->next};
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SinglyList::insertAfter(const string& key, const string& value) {
    Node* node = findNode(key);
    if (node == nullptr) return false;

    Node* newNode = new Node{value, node->next};
    node->next = newNode;
    if (node == tail) tail = newNode;
    return true;
}

bool SinglyList::popFront() {
    if (head == nullptr) return false;

    Node* node = head;
    head = head->next;
    delete node;
    if (head == nullptr) tail = nullptr;
    return true;
}

bool SinglyList::popBack() {
    if (head == nullptr) return false;
    if (head == tail) return popFront();

    // У tail нет ссылки назад, поэтому ищем предпоследний узел: O(n).
    Node* current = head;
    while (current->next != tail) current = current->next;

    delete tail;
    tail = current;
    tail->next = nullptr;
    return true;
}

bool SinglyList::removeBefore(const string& key) {
    if (head == nullptr || head->value == key) return false;
    if (head->next != nullptr && head->next->value == key) return popFront();

    Node* current = head;
    while (current->next != nullptr && current->next->next != nullptr) {
        if (current->next->next->value == key) {
            Node* node = current->next;
            current->next = node->next;
            delete node;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SinglyList::removeAfter(const string& key) {
    Node* node = findNode(key);
    if (node == nullptr || node->next == nullptr) return false;

    Node* removed = node->next;
    node->next = removed->next;
    if (removed == tail) tail = node;
    delete removed;
    return true;
}

bool SinglyList::removeValue(const string& value) {
    if (head == nullptr) return false;
    if (head->value == value) return popFront();

    Node* current = head;
    while (current->next != nullptr) {
        if (current->next->value == value) {
            Node* node = current->next;
            current->next = node->next;
            if (node == tail) tail = current;
            delete node;
            return true;
        }
        current = current->next;
    }
    return false;
}

bool SinglyList::get(int index, string& value) const {
    if (index < 0) return false;

    // Для доступа по индексу идём от головы по ссылкам next.
    const Node* current = head;
    while (current != nullptr && index > 0) {
        current = current->next;
        --index;
    }
    if (current == nullptr) return false;

    value = current->value;
    return true;
}

bool SinglyList::contains(const string& value) const {
    return findNode(value) != nullptr;
}

void SinglyList::print() const {
    Node* current = head;
    cout << "[ ";
    while (current != nullptr) {
        cout << current->value << " ";
        current = current->next;
    }
    cout << "]\n";
}

void SinglyList::printReverseNode(Node* node) const {
    // Сначала рекурсия доходит до хвоста, затем печатает при возврате.
    if (node == nullptr) return;
    printReverseNode(node->next);
    cout << node->value << " ";
}

void SinglyList::printReverse() const {
    cout << "[ ";
    printReverseNode(head);
    cout << "]\n";
}

bool SinglyList::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    return file.is_open() && load(file);
}

bool SinglyList::load(istream& file) {
    string value;
    while (file >> value) pushBack(value);
    return !file.bad();
}

bool SinglyList::saveToFile(const string& fileName) const {
    ofstream file(fileName);
    if (!file) return false;

    save(file);
    file.close();
    return static_cast<bool>(file);
}

void SinglyList::save(ostream& file) const {
    Node* current = head;
    while (current != nullptr) {
        file << current->value << '\n';
        current = current->next;
    }
}

void runSinglyList(const string& fileName) {
    SinglyList list;
    list.loadFromFile(fileName);

    int command = -1;
    while (command != 0) {
        cout << "\nОдносвязный список\n";
        cout << "1 - добавить в начало\n2 - добавить в конец\n";
        cout << "3 - вставить перед значением\n4 - вставить после значения\n";
        cout << "5 - удалить из начала\n6 - удалить из конца\n";
        cout << "7 - удалить перед значением\n8 - удалить после значения\n";
        cout << "9 - удалить по значению\n10 - поиск\n";
        cout << "11 - вывести с начала\n12 - вывести с конца\n0 - выход\n";
        cout << "Команда: ";
        cin >> command;

        string key;
        string value;
        bool changed = false;

        switch (command) {
            case 1:
                cout << "Значение: "; cin >> value;
                list.pushFront(value); changed = true;
                break;
            case 2:
                cout << "Значение: "; cin >> value;
                list.pushBack(value); changed = true;
                break;
            case 3:
                cout << "Перед каким значением и что вставить: "; cin >> key >> value;
                changed = list.insertBefore(key, value);
                break;
            case 4:
                cout << "После какого значения и что вставить: "; cin >> key >> value;
                changed = list.insertAfter(key, value);
                break;
            case 5: changed = list.popFront(); break;
            case 6: changed = list.popBack(); break;
            case 7:
                cout << "Значение: "; cin >> key;
                changed = list.removeBefore(key);
                break;
            case 8:
                cout << "Значение: "; cin >> key;
                changed = list.removeAfter(key);
                break;
            case 9:
                cout << "Значение: "; cin >> value;
                changed = list.removeValue(value);
                break;
            case 10:
                cout << "Значение: "; cin >> value;
                cout << (list.contains(value) ? "Найдено\n" : "Не найдено\n");
                break;
            case 11: list.print(); break;
            case 12: list.printReverse(); break;
        }

        if (changed) {
            list.saveToFile(fileName);
        } else if (command >= 1 && command <= 9) {
            cout << "Операцию выполнить не удалось\n";
        }
    }
}
