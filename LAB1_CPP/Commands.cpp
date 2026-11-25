#include "Commands.h"

#include "Array.h"
#include "DoublyList.h"
#include "FullBinaryTree.h"
#include "Queue.h"
#include "SinglyList.h"
#include "Stack.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <stdexcept>

using namespace std;

namespace {

namespace fs = filesystem;

class FileLock {
    fs::path directory;

public:
    explicit FileLock(const string& file) : directory(file + ".lock") {
        if (!fs::create_directory(directory)) {
            throw runtime_error("файл данных занят");
        }
    }

    ~FileLock() {
        error_code error;
        fs::remove(directory / "new.data", error);
        fs::remove(directory, error);
    }
};

// Одна строка базы хранит тип, имя и содержимое одной структуры.
class StoredStructure {
    string base;
    string recordKey;
    string payload;
    string otherRecords;
    string legacyFile;
    bool found = false;

public:
    StoredStructure(const string& baseFile, const string& name, char type)
        : base(baseFile), recordKey(string(1, type) + " " + name),
          legacyFile(baseFile + "." + name + "." + type) {
        ifstream input(base);
        if (!input && fs::exists(base)) {
            throw runtime_error("не удалось открыть файл данных");
        }

        string line;
        while (getline(input, line)) {
            if (line.compare(0, recordKey.size() + 1, recordKey + " ") == 0) {
                found = true;
                payload = line.substr(recordKey.size() + 1);
            } else {
                otherRecords += line + '\n';
            }
        }

        if (input.bad()) {
            throw runtime_error("не удалось прочитать файл данных");
        }
    }

    bool exists() const {
        return found || fs::is_regular_file(legacyFile);
    }

    template <class Structure>
    void load(Structure& structure) const {
        if (found) {
            istringstream input(payload);
            if (structure.load(input)) return;
        } else {
            // Читаем данные, которые сохранила предыдущая версия лабораторной.
            if (!fs::exists(legacyFile) || structure.loadFromFile(legacyFile)) return;
        }
        throw runtime_error("не удалось загрузить структуру");
    }

    template <class Structure>
    bool save(const Structure& structure, string& answer) const {
        ostringstream values;
        structure.save(values);

        string temporary = base + ".lock/new.data";
        ofstream output(temporary);
        output << otherRecords << recordKey << ' ';
        istringstream input(values.str());
        string value;
        while (input >> value) {
            output << value << ' ';
        }
        output << '\n';
        output.close();

        error_code error;
        if (output) {
            fs::rename(temporary, base, error);
            if (!error) return true;
        }

        answer = "не удалось сохранить файл данных";
        return false;
    }
};

bool readIndex(const string& text, int& index) {
    istringstream input(text);
    return (input >> index) && input.eof();
}

// Массив.
bool runArray(const StoredStructure& storage, const string& command,
              const string arguments[], int argumentCount, string& answer) {
    Array array;
    if (command == "CREATE" && argumentCount == 0) {
        if (storage.exists()) {
            answer = "массив с таким именем уже существует";
            return false;
        }
        answer = "OK";
        return storage.save(array, answer);
    }

    storage.load(array);
    int index;

    if (command == "PUSH" && argumentCount == 1) {
        if (!array.pushBack(arguments[0])) {
            answer = "массив заполнен";
            return false;
        }
        answer = arguments[0];
        return storage.save(array, answer);
    }

    if (command == "INSERT" && argumentCount == 2 && readIndex(arguments[0], index)) {
        if (!array.insert(index, arguments[1])) {
            answer = "неверный индекс";
            return false;
        }
        answer = arguments[1];
        return storage.save(array, answer);
    }

    if (command == "GET" && argumentCount == 1 && readIndex(arguments[0], index)) {
        answer = array.get(index);
        if (answer.empty()) {
            answer = "неверный индекс";
            return false;
        }
        return true;
    }

    if (command == "SET" && argumentCount == 2 && readIndex(arguments[0], index)) {
        if (!array.set(index, arguments[1])) {
            answer = "неверный индекс";
            return false;
        }
        answer = arguments[1];
        return storage.save(array, answer);
    }

    if (command == "DEL" && argumentCount == 1 && readIndex(arguments[0], index)) {
        answer = array.get(index);
        if (answer.empty() || !array.remove(index)) {
            answer = "неверный индекс";
            return false;
        }
        return storage.save(array, answer);
    }

    if (command == "LEN" && argumentCount == 0) {
        answer = to_string(array.getSize());
        return true;
    }

    if (command == "PRINT" && argumentCount == 0) {
        array.print();
        return true;
    }

    answer = "неверная команда массива";
    return false;
}

// Односвязный список.
bool runSinglyList(const StoredStructure& storage, const string& command,
                   const string arguments[], int argumentCount, string& answer) {
    SinglyList list;
    storage.load(list);

    if ((command == "PUSH" || command == "PUSHBACK") && argumentCount == 1) {
        list.pushBack(arguments[0]);
        answer = arguments[0];
        return storage.save(list, answer);
    }

    if (command == "PUSHFRONT" && argumentCount == 1) {
        list.pushFront(arguments[0]);
        answer = arguments[0];
        return storage.save(list, answer);
    }

    if (command == "INSERTBEFORE" && argumentCount == 2) {
        if (!list.insertBefore(arguments[0], arguments[1])) {
            answer = "значение не найдено";
            return false;
        }
        answer = arguments[1];
        return storage.save(list, answer);
    }

    if (command == "INSERTAFTER" && argumentCount == 2) {
        if (!list.insertAfter(arguments[0], arguments[1])) {
            answer = "значение не найдено";
            return false;
        }
        answer = arguments[1];
        return storage.save(list, answer);
    }

    if (command == "DEL" && argumentCount == 1) {
        if (!list.removeValue(arguments[0])) {
            answer = "значение не найдено";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELFRONT" && argumentCount == 0) {
        if (!list.popFront()) {
            answer = "список пуст";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELBACK" && argumentCount == 0) {
        if (!list.popBack()) {
            answer = "список пуст";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELBEFORE" && argumentCount == 1) {
        if (!list.removeBefore(arguments[0])) {
            answer = "нет элемента перед указанным значением";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELAFTER" && argumentCount == 1) {
        if (!list.removeAfter(arguments[0])) {
            answer = "нет элемента после указанного значения";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "GET" && argumentCount == 1) {
        int index;
        if (readIndex(arguments[0], index) && list.get(index, answer)) return true;
        answer = "неверный индекс";
        return false;
    }

    if (command == "PRINT") {
        list.print();
        return true;
    }
    if (command == "REVERSE") {
        list.printReverse();
        return true;
    }

    if (command == "FIND" && argumentCount == 1) {
        answer = list.contains(arguments[0]) ? "TRUE" : "FALSE";
        return true;
    }

    answer = "неверная команда односвязного списка";
    return false;
}

// Двусвязный список.
bool runDoublyList(const StoredStructure& storage, const string& command,
                   const string arguments[], int argumentCount, string& answer) {
    DoublyList list;
    storage.load(list);

    if ((command == "PUSH" || command == "PUSHBACK") && argumentCount == 1) {
        list.pushBack(arguments[0]);
        answer = arguments[0];
        return storage.save(list, answer);
    }

    if (command == "PUSHFRONT" && argumentCount == 1) {
        list.pushFront(arguments[0]);
        answer = arguments[0];
        return storage.save(list, answer);
    }

    if (command == "INSERTBEFORE" && argumentCount == 2) {
        if (!list.insertBefore(arguments[0], arguments[1])) {
            answer = "значение не найдено";
            return false;
        }
        answer = arguments[1];
        return storage.save(list, answer);
    }

    if (command == "INSERTAFTER" && argumentCount == 2) {
        if (!list.insertAfter(arguments[0], arguments[1])) {
            answer = "значение не найдено";
            return false;
        }
        answer = arguments[1];
        return storage.save(list, answer);
    }

    if (command == "DEL" && argumentCount == 1) {
        if (!list.removeValue(arguments[0])) {
            answer = "значение не найдено";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELFRONT" && argumentCount == 0) {
        if (!list.popFront()) {
            answer = "список пуст";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELBACK" && argumentCount == 0) {
        if (!list.popBack()) {
            answer = "список пуст";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELBEFORE" && argumentCount == 1) {
        if (!list.removeBefore(arguments[0])) {
            answer = "нет элемента перед указанным значением";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "DELAFTER" && argumentCount == 1) {
        if (!list.removeAfter(arguments[0])) {
            answer = "нет элемента после указанного значения";
            return false;
        }
        answer = "OK";
        return storage.save(list, answer);
    }

    if (command == "GET" && argumentCount == 1) {
        int index;
        if (readIndex(arguments[0], index) && list.get(index, answer)) return true;
        answer = "неверный индекс";
        return false;
    }

    if (command == "PRINT") {
        list.printForward();
        return true;
    }
    if (command == "REVERSE") {
        list.printBackward();
        return true;
    }

    if (command == "FIND" && argumentCount == 1) {
        answer = list.contains(arguments[0]) ? "TRUE" : "FALSE";
        return true;
    }

    answer = "неверная команда двусвязного списка";
    return false;
}

// Стек.
bool runStack(const StoredStructure& storage, const string& command,
              const string arguments[], int argumentCount, string& answer) {
    Stack stack;
    storage.load(stack);
    string value;

    if (command == "PUSH" && argumentCount == 1) {
        stack.push(arguments[0]);
        answer = arguments[0];
        return storage.save(stack, answer);
    }

    if (command == "POP" && argumentCount == 0) {
        if (!stack.pop(value)) {
            answer = "стек пуст";
            return false;
        }
        answer = value;
        return storage.save(stack, answer);
    }

    if (command == "PEEK" && argumentCount == 0) {
        if (!stack.peek(value)) {
            answer = "стек пуст";
            return false;
        }
        answer = value;
        return true;
    }

    if (command == "PRINT") {
        stack.print();
        return true;
    }

    answer = "неверная команда стека";
    return false;
}

// Очередь.
bool runQueue(const StoredStructure& storage, const string& command,
              const string arguments[], int argumentCount, string& answer) {
    Queue queue;
    storage.load(queue);
    string value;

    if (command == "PUSH" && argumentCount == 1) {
        queue.push(arguments[0]);
        answer = arguments[0];
        return storage.save(queue, answer);
    }

    if (command == "POP" && argumentCount == 0) {
        if (!queue.pop(value)) {
            answer = "очередь пуста";
            return false;
        }
        answer = value;
        return storage.save(queue, answer);
    }

    if (command == "PEEK" && argumentCount == 0) {
        if (!queue.peek(value)) {
            answer = "очередь пуста";
            return false;
        }
        answer = value;
        return true;
    }

    if (command == "PRINT") {
        queue.print();
        return true;
    }

    answer = "неверная команда очереди";
    return false;
}

// Дерево варианта 4.
bool runTree(const StoredStructure& storage, const string& command,
             const string arguments[], int argumentCount, string& answer) {
    FullBinaryTree tree;
    storage.load(tree);

    if (command == "INSERT" && argumentCount == 1) {
        if (arguments[0] == "#") {
            answer = "символ # зарезервирован для пустой ветви";
            return false;
        }
        tree.insert(arguments[0]);
        answer = arguments[0];
        return storage.save(tree, answer);
    }

    if ((command == "ADDPAIR" || command == "ADDPPAIR") && argumentCount == 3) {
        if (arguments[1] == "#" || arguments[2] == "#") {
            answer = "символ # зарезервирован для пустой ветви";
            return false;
        }
        if (!tree.addChildren(arguments[0], arguments[1], arguments[2])) {
            answer = "родитель не найден или уже имеет детей";
            return false;
        }
        answer = "OK";
        return storage.save(tree, answer);
    }

    if (command == "DEL" && argumentCount == 1) {
        if (!tree.remove(arguments[0])) {
            answer = "значение не найдено";
            return false;
        }
        answer = "OK";
        return storage.save(tree, answer);
    }

    if ((command == "GET" || command == "MEMBER") && argumentCount == 1) {
        answer = tree.contains(arguments[0]) ? "TRUE" : "FALSE";
        return true;
    }

    if (command == "ISFULL" && argumentCount == 0) {
        answer = tree.isFull() ? "TRUE" : "FALSE";
        return true;
    }

    if (command == "PRINT") {
        tree.print();
        return true;
    }

    answer = "неверная команда дерева";
    return false;
}

// Первая буква команды определяет тип структуры и суффикс файла.
bool runCommand(const string& base, const string& name, const string& operation,
                const string arguments[], int argumentCount, string& answer) {
    char type = operation[0];
    string command = operation.substr(1);
    const string types = "MFLQST";
    if (types.find(type) == string::npos) {
        answer = "неизвестный тип структуры";
        return false;
    }
    StoredStructure storage(base, name, type);

    if (type == 'M') {
        return runArray(storage, command, arguments, argumentCount, answer);
    }

    if (type == 'F') {
        return runSinglyList(storage, command, arguments, argumentCount, answer);
    }

    if (type == 'L') {
        return runDoublyList(storage, command, arguments, argumentCount, answer);
    }

    if (type == 'S') {
        return runStack(storage, command, arguments, argumentCount, answer);
    }

    if (type == 'Q') {
        return runQueue(storage, command, arguments, argumentCount, answer);
    }

    return runTree(storage, command, arguments, argumentCount, answer);
}
}  // namespace

bool executeCommand(const string& baseFile, const string& query, string& answer) {
    istringstream input(query);
    string operation;
    string name;

    // Например, в «MINSERT arr 1 B» arr — имя, а 1 и B — аргументы.
    if (!(input >> operation >> name)) {
        answer = "формат: КОМАНДА имя [аргументы]";
        return false;
    }

    // Четвёртый аргумент помогает отличить лишний ввод от команды с тремя аргументами.
    constexpr int argumentLimit = 4;
    string arguments[argumentLimit];
    int argumentCount = 0;
    while (argumentCount < argumentLimit && (input >> arguments[argumentCount])) {
        ++argumentCount;
    }

    for (char& letter : operation) {
        letter = static_cast<char>(toupper(static_cast<unsigned char>(letter)));
    }

    if (name.find_first_of("/\\") != string::npos) {
        answer = "имя структуры не должно содержать разделители пути";
        return false;
    }

    FileLock lock(baseFile);

    // У PRINT нет префикса. Ищем тип записи по имени.
    if (operation == "PRINT") {
        const string types = "MFLQST";
        for (char type : types) {
            StoredStructure storage(baseFile, name, type);
            if (storage.exists()) {
                string printOperation = string(1, type) + "PRINT";
                return runCommand(baseFile, name, printOperation,
                                  arguments, argumentCount, answer);
            }
        }

        answer = "структура не найдена";
        return false;
    }

    if (operation == "ISMEMBER") {
        operation = "TGET";
    }

    return runCommand(baseFile, name, operation, arguments, argumentCount, answer);
}
