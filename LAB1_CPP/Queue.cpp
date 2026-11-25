#include "Queue.h"

#include <fstream>
#include <iostream>

using namespace std;

Queue::Queue() {
    first = nullptr;
    last = nullptr;
}

Queue::~Queue() {
    string value;
    while (pop(value)) {
    }
}

void Queue::push(const string& value) {
    // Новый элемент добавляется в конец очереди за O(1).
    Node* node = new Node{value, nullptr};

    if (last == nullptr) {
        first = node;
        last = node;
        return;
    }

    last->next = node;
    last = node;
}

bool Queue::pop(string& value) {
    // Первый элемент покидает очередь за O(1).
    if (first == nullptr) return false;

    Node* node = first;
    value = node->value;
    first = first->next;
    delete node;

    // После удаления последнего элемента оба указателя должны стать пустыми.
    if (first == nullptr) last = nullptr;
    return true;
}

bool Queue::peek(string& value) const {
    if (first == nullptr) return false;
    value = first->value;
    return true;
}

void Queue::print() const {
    Node* current = first;
    cout << "Начало -> ";
    while (current != nullptr) {
        cout << current->value << " ";
        current = current->next;
    }
    cout << "<- конец\n";
}

bool Queue::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    return file.is_open() && load(file);
}

bool Queue::load(istream& file) {
    string value;
    while (file >> value) push(value);
    return !file.bad();
}

bool Queue::saveToFile(const string& fileName) const {
    ofstream file(fileName);
    if (!file) return false;

    save(file);
    file.close();
    return static_cast<bool>(file);
}

void Queue::save(ostream& file) const {
    Node* current = first;
    while (current != nullptr) {
        file << current->value << '\n';
        current = current->next;
    }
}

void runQueue(const string& fileName) {
    Queue queue;
    queue.loadFromFile(fileName);

    int command = -1;
    while (command != 0) {
        cout << "\nОчередь\n";
        cout << "1 - push\n2 - pop\n3 - посмотреть первый элемент\n";
        cout << "4 - вывести очередь\n0 - выход\nКоманда: ";
        cin >> command;

        string value;
        switch (command) {
            case 1:
                cout << "Значение: ";
                cin >> value;
                queue.push(value);
                queue.saveToFile(fileName);
                break;
            case 2:
                if (queue.pop(value)) {
                    cout << "Удалено: " << value << '\n';
                    queue.saveToFile(fileName);
                } else {
                    cout << "Очередь пуста\n";
                }
                break;
            case 3:
                if (queue.peek(value)) cout << "Первый: " << value << '\n';
                else cout << "Очередь пуста\n";
                break;
            case 4:
                queue.print();
                break;
        }
    }
}
