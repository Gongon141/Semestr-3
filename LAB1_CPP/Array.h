#pragma once

#include <string>
#include <iosfwd>

using namespace std;

// Обычный массив фиксированного размера.
// size хранит количество занятых ячеек, а MAX_SIZE задаёт предел.
class Array {
private:
    static const int MAX_SIZE = 200;

    string data[MAX_SIZE];
    int size;

public:
    Array();

    // Операции, которые изменяют размер, возвращают false при ошибке.
    bool pushBack(const string& value);
    bool insert(int index, const string& value);
    bool remove(int index);
    bool set(int index, const string& value);
    string get(int index) const;
    int getSize() const;
    void print() const;

    // Файл нужен для сохранения данных между запусками программы.
    bool loadFromFile(const string& fileName);
    bool saveToFile(const string& fileName) const;
    bool load(istream& file);
    void save(ostream& file) const;
};

void runArray(const string& fileName);
