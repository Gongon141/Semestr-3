#pragma once

#include <string>

using namespace std;

// Выполняет одну команду. При ошибке answer содержит сообщение для пользователя.
bool executeCommand(const string& baseFile, const string& query, string& answer);
