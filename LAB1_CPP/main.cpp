#include "Commands.h"

#include <exception>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {
    string fileName;
    string query;

    // По методичке один запуск программы выполняет один запрос.
    if (argc == 5) {
        for (int i = 1; i < argc; i += 2) {
            string option = argv[i];
            if (option == "--file") {
                fileName = argv[i + 1];
            } else if (option == "--query") {
                query = argv[i + 1];
            }
        }
    }

    if (fileName.empty() || query.empty()) {
        cerr << "Формат: ./dbms --file file.data --query 'SPUSH mystack item'\n";
        return 1;
    }

    try {
        string answer;
        if (!executeCommand(fileName, query, answer)) {
            cerr << "Ошибка: " << answer << '\n';
            return 1;
        }
        if (!answer.empty()) {
            cout << answer << '\n';
        }
    } catch (const exception& error) {
        cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }

    return 0;
}
