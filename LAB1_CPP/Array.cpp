#include "Array.h"

#include <fstream>
#include <iostream>

using namespace std;

Array::Array() {
    size = 0;
}

bool Array::pushBack(const string& value) {
    // В статическом массиве нельзя выйти за границу в 200 элементов.
    if (size == MAX_SIZE) {
        return false;
    }

    data[size] = value;
    size++;
    return true;
}

bool Array::insert(int index, const string& value) {
    if (index < 0 || index > size || size == MAX_SIZE) {
        return false;
    }

    // Освобождаем нужную ячейку сдвигом элементов вправо.
    // В худшем случае сложность вставки равна O(n).
    for (int i = size; i > index; i--) {
        data[i] = data[i - 1];
    }

    data[index] = value;
    size++;
    return true;
}

bool Array::remove(int index) {
    if (index < 0 || index >= size) {
        return false;
    }

    // Закрываем пустое место сдвигом элементов влево: O(n).
    for (int i = index; i < size - 1; i++) {
        data[i] = data[i + 1];
    }

    size--;
    return true;
}

bool Array::set(int index, const string& value) {
    if (index < 0 || index >= size) {
        return false;
    }

    data[index] = value;
    return true;
}

string Array::get(int index) const {
    // Доступ по индексу выполняется за O(1).
    if (index < 0 || index >= size) {
        return "";
    }

    return data[index];
}

int Array::getSize() const {
    return size;
}

void Array::print() const {
    cout << "[ ";
    for (int i = 0; i < size; i++) {
        cout << data[i] << " ";
    }
    cout << "]\n";
}

bool Array::loadFromFile(const string& fileName) {
    ifstream file(fileName);
    return file.is_open() && load(file);
}

bool Array::load(istream& file) {
    // Значения могут быть разделены пробелами или переводами строки.
    string value;
    while (file >> value) {
        if (!pushBack(value)) {
            return false;
        }
    }
    return !file.bad();
}

bool Array::saveToFile(const string& fileName) const {
    ofstream file(fileName);
    if (!file) return false;

    save(file);
    file.close();
    return static_cast<bool>(file);
}

void Array::save(ostream& file) const {
    for (int i = 0; i < size; i++) {
        file << data[i] << '\n';
    }
}

void runArray(const string& fileName) {
    Array array;
    array.loadFromFile(fileName);

    int command = -1;
    while (command != 0) {
        cout << "\nМассив\n";
        cout << "1 - добавить в конец\n";
        cout << "2 - вставить по индексу\n";
        cout << "3 - получить по индексу\n";
        cout << "4 - удалить по индексу\n";
        cout << "5 - заменить по индексу\n";
        cout << "6 - вывести длину\n";
        cout << "7 - вывести массив\n";
        cout << "0 - выход\n";
        cout << "Команда: ";
        cin >> command;

        int index;
        string value;

        switch (command) {
            case 1:
                cout << "Значение: ";
                cin >> value;
                if (array.pushBack(value)) {
                    array.saveToFile(fileName);
                } else {
                    cout << "Массив заполнен\n";
                }
                break;
            case 2:
                cout << "Индекс и значение: ";
                cin >> index >> value;
                if (array.insert(index, value)) {
                    array.saveToFile(fileName);
                } else {
                    cout << "Неверный индекс или массив заполнен\n";
                }
                break;
            case 3:
                cout << "Индекс: ";
                cin >> index;
                if (index >= 0 && index < array.getSize()) {
                    cout << array.get(index) << '\n';
                } else {
                    cout << "Неверный индекс\n";
                }
                break;
            case 4:
                cout << "Индекс: ";
                cin >> index;
                if (array.remove(index)) {
                    array.saveToFile(fileName);
                } else {
                    cout << "Неверный индекс\n";
                }
                break;
            case 5:
                cout << "Индекс и новое значение: ";
                cin >> index >> value;
                if (array.set(index, value)) {
                    array.saveToFile(fileName);
                } else {
                    cout << "Неверный индекс\n";
                }
                break;
            case 6:
                cout << "Длина: " << array.getSize() << '\n';
                break;
            case 7:
                array.print();
                break;
        }
    }
}
