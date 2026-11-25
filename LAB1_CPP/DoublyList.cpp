#include "DoublyList.h"

#include <fstream>
#include <iostream>

using namespace std;

DoublyList::DoublyList() {
    head = nullptr;
    tail = nullptr;
}

DoublyList::~DoublyList() {
    // Все узлы были созданы через new, поэтому удаляем их вручную.
    while (head != nullptr) {
        Node* node = head;
        head = head->next;
        delete node;
    }
}

DoublyList::Node* DoublyList::findNode(const string& value) const {
    // Поиск начинается с головы и в худшем случае занимает O(n).
    Node* current = head;
    while (current != nullptr) {
        if (current->value == value) return current;
        current = current->next;
    }
    return nullptr;
}

void DoublyList::removeNode(Node* node) {
    // Соседи удаляемого узла начинают указывать друг на друга.
    if (node->previous == nullptr) head = node->next;
    else node->previous->next = node->next;

    if (node->next == nullptr) tail = node->previous;
    else node->next->previous = node->previous;

    delete node;
}

void DoublyList::pushFront(const string& value) {
    // Новый узел не имеет предыдущего элемента.
    Node* node = new Node{value, nullptr, head};
    if (head != nullptr) head->previous = node;
    else tail = node;
    head = node;
}

void DoublyList::pushBack(const string& value) {
    // Новый узел не имеет следующего элемента.
    Node* node = new Node{value, tail, nullptr};
    if (tail != nullptr) tail->next = node;
    else head = node;
    tail = node;
}

bool DoublyList::insertBefore(const string& key, const string& value) {
    Node* node = findNode(key);
    if (node == nullptr) return false;
    if (node == head) {
        pushFront(value);
        return true;
    }

    Node* newNode = new Node{value, node->previous, node};
    node->previous->next = newNode;
    node->previous = newNode;
    return true;
}

bool DoublyList::insertAfter(const string& key, const string& value) {
    Node* node = findNode(key);
    if (node == nullptr) return false;
    if (node == tail) {
        pushBack(value);
        return true;
    }

    Node* newNode = new Node{value, node, node->next};
    node->next->previous = newNode;
    node->next = newNode;
    return true;
}

bool DoublyList::popFront() {
    if (head == nullptr) return false;
    removeNode(head);
    return true;
}

bool DoublyList::popBack() {
    // Здесь удаление хвоста O(1), потому что у tail есть previous.
    if (tail == nullptr) return false;
    removeNode(tail);
    return true;
}

bool DoublyList::removeBefore(const string& key) {
    Node* node = findNode(key);
    if (node == nullptr || node->previous == nullptr) return false;
    removeNode(node->previous);
    return true;
}

bool DoublyList::removeAfter(const string& key) {
    Node* node = findNode(key);
    if (node == nullptr || node->next == nullptr) return false;
    removeNode(node->next);
    return true;
}

bool DoublyList::removeValue(const string& value) {
    Node* node = findNode(value);
    if (node == nullptr) return false;
    removeNode(node);
    return true;
}

bool DoublyList::get(int index, string& value) const {
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

bool DoublyList::contains(const string& value) const {
    return findNode(value) != nullptr;
}

void DoublyList::printForward() const {
    Node* current = head;
    cout << "[ ";
    while (current != nullptr) {
        cout << current->value << " ";
        current = current->next;
    }
    cout << "]\n";
}

void DoublyList::printBackward() const {
    // Движемся от хвоста к голове по указателям previous.
    Node* current = tail;
    cout << "[ ";
    while (current != nullptr) {
        cout << current->value << " ";
        current = current->previous;
    }
    cout << "]\n";
}

bool DoublyList::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    return file.is_open() && load(file);
}

bool DoublyList::load(istream& file) {
    string value;
    while (file >> value) pushBack(value);
    return !file.bad();
}

bool DoublyList::saveToFile(const string& fileName) const {
    ofstream file(fileName);
    if (!file) return false;

    save(file);
    file.close();
    return static_cast<bool>(file);
}

void DoublyList::save(ostream& file) const {
    Node* current = head;
    while (current != nullptr) {
        file << current->value << '\n';
        current = current->next;
    }
}

void runDoublyList(const string& fileName) {
    DoublyList list;
    list.loadFromFile(fileName);

    int command = -1;
    while (command != 0) {
        cout << "\nДвусвязный список\n";
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
            case 11: list.printForward(); break;
            case 12: list.printBackward(); break;
        }

        if (changed) {
            list.saveToFile(fileName);
        } else if (command >= 1 && command <= 9) {
            cout << "Операцию выполнить не удалось\n";
        }
    }
}
