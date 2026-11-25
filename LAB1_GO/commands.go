package main

import (
	"errors"
	"fmt"
	"strconv"
	"strings"
)

func indexOf(text string) (int, error) {
	index, err := strconv.ParseInt(text, 10, 32)
	return int(index), err
}

func booleanAnswer(value bool) string {
	if value {
		return "TRUE"
	}
	return "FALSE"
}

func executeCommand(base, query string) error {
	input := strings.NewReader(query)
	var operation, name string
	if _, err := fmt.Fscan(input, &operation, &name); err != nil {
		return errors.New("формат: КОМАНДА имя [аргументы]")
	}
	if strings.ContainsAny(name, "/\\") {
		return errors.New("имя структуры не должно содержать разделители пути")
	}

	// Четвёртая ячейка позволяет обнаружить лишний аргумент.
	var arguments [4]string
	argumentCount := 0
	for argumentCount < len(arguments) {
		if _, err := fmt.Fscan(input, &arguments[argumentCount]); err != nil {
			break
		}
		argumentCount++
	}

	db, err := openDatabase(base)
	if err != nil {
		return err
	}
	operation = strings.ToUpper(operation)
	const kinds = "MFLQST"
	if operation == "PRINT" {
		for _, kind := range kinds {
			entry := db.record(string(kind), name)
			if entry.exists() {
				return runCommand(string(kind), entry, "PRINT", arguments, argumentCount)
			}
		}
		return errors.New("структура не найдена")
	}
	if operation == "ISMEMBER" {
		operation = "TGET"
	}

	kind, command := operation[:1], operation[1:]
	if !strings.Contains(kinds, kind) {
		return errors.New("неизвестный тип структуры")
	}
	return runCommand(kind, db.record(kind, name), command, arguments, argumentCount)
}

// Первая буква команды определяет обработчик структуры.
func runCommand(kind string, entry record, command string, arguments [4]string, argumentCount int) error {
	if kind == "M" {
		return arrayCommand(entry, command, arguments, argumentCount)
	}
	if kind == "F" {
		return singlyCommand(entry, command, arguments, argumentCount)
	}
	if kind == "L" {
		return doublyCommand(entry, command, arguments, argumentCount)
	}
	if kind == "S" {
		return stackCommand(entry, command, arguments, argumentCount)
	}
	if kind == "Q" {
		return queueCommand(entry, command, arguments, argumentCount)
	}
	return treeCommand(entry, command, arguments, argumentCount)
}

func arrayCommand(entry record, command string, arguments [4]string, argumentCount int) error {
	var array Array
	if command == "CREATE" && argumentCount == 0 {
		if entry.exists() {
			return errors.New("массив с таким именем уже существует")
		}
		return entry.save(&array, "OK")
	}

	if err := entry.load(&array); err != nil {
		return err
	}
	if command == "PUSH" && argumentCount == 1 {
		if !array.pushBack(arguments[0]) {
			return errors.New("массив заполнен")
		}
		return entry.save(&array, arguments[0])
	}
	if command == "INSERT" && argumentCount == 2 {
		index, err := indexOf(arguments[0])
		if err != nil {
			return errors.New("неверная команда массива")
		}
		if !array.insert(index, arguments[1]) {
			return errors.New("неверный индекс")
		}
		return entry.save(&array, arguments[1])
	}
	if command == "GET" && argumentCount == 1 {
		index, err := indexOf(arguments[0])
		if err != nil {
			return errors.New("неверная команда массива")
		}
		value, ok := array.get(index)
		if !ok {
			return errors.New("неверный индекс")
		}
		fmt.Println(value)
		return nil
	}
	if command == "SET" && argumentCount == 2 {
		index, err := indexOf(arguments[0])
		if err != nil {
			return errors.New("неверная команда массива")
		}
		if !array.set(index, arguments[1]) {
			return errors.New("неверный индекс")
		}
		return entry.save(&array, arguments[1])
	}
	if command == "DEL" && argumentCount == 1 {
		index, err := indexOf(arguments[0])
		if err != nil {
			return errors.New("неверная команда массива")
		}
		value, ok := array.get(index)
		if !ok || !array.remove(index) {
			return errors.New("неверный индекс")
		}
		return entry.save(&array, value)
	}
	if command == "LEN" && argumentCount == 0 {
		fmt.Println(array.size)
		return nil
	}
	if command == "PRINT" && argumentCount == 0 {
		array.print()
		return nil
	}
	return errors.New("неверная команда массива")
}

func singlyCommand(entry record, command string, arguments [4]string, argumentCount int) error {
	var list SinglyList
	if err := entry.load(&list); err != nil {
		return err
	}
	if (command == "PUSH" || command == "PUSHBACK") && argumentCount == 1 {
		list.pushBack(arguments[0])
		return entry.save(&list, arguments[0])
	}
	if command == "PUSHFRONT" && argumentCount == 1 {
		list.pushFront(arguments[0])
		return entry.save(&list, arguments[0])
	}
	if command == "INSERTBEFORE" && argumentCount == 2 {
		if !list.insertBefore(arguments[0], arguments[1]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&list, arguments[1])
	}
	if command == "INSERTAFTER" && argumentCount == 2 {
		if !list.insertAfter(arguments[0], arguments[1]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&list, arguments[1])
	}
	if command == "DEL" && argumentCount == 1 {
		if !list.removeValue(arguments[0]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&list, "OK")
	}
	if command == "FIND" && argumentCount == 1 {
		fmt.Println(booleanAnswer(list.contains(arguments[0])))
		return nil
	}
	if command == "DELFRONT" && argumentCount == 0 {
		if !list.popFront() {
			return errors.New("список пуст")
		}
		return entry.save(&list, "OK")
	}
	if command == "DELBACK" && argumentCount == 0 {
		if !list.popBack() {
			return errors.New("список пуст")
		}
		return entry.save(&list, "OK")
	}
	if command == "DELBEFORE" && argumentCount == 1 {
		if !list.removeBefore(arguments[0]) {
			return errors.New("нет элемента перед указанным значением")
		}
		return entry.save(&list, "OK")
	}
	if command == "DELAFTER" && argumentCount == 1 {
		if !list.removeAfter(arguments[0]) {
			return errors.New("нет элемента после указанного значения")
		}
		return entry.save(&list, "OK")
	}
	if command == "GET" && argumentCount == 1 {
		index, err := indexOf(arguments[0])
		if err == nil {
			if value, ok := list.get(index); ok {
				fmt.Println(value)
				return nil
			}
		}
		return errors.New("неверный индекс")
	}
	if command == "PRINT" {
		list.print()
		return nil
	}
	if command == "REVERSE" {
		list.printReverse()
		return nil
	}
	return errors.New("неверная команда односвязного списка")
}

func doublyCommand(entry record, command string, arguments [4]string, argumentCount int) error {
	var list DoublyList
	if err := entry.load(&list); err != nil {
		return err
	}
	if (command == "PUSH" || command == "PUSHBACK") && argumentCount == 1 {
		list.pushBack(arguments[0])
		return entry.save(&list, arguments[0])
	}
	if command == "PUSHFRONT" && argumentCount == 1 {
		list.pushFront(arguments[0])
		return entry.save(&list, arguments[0])
	}
	if command == "INSERTBEFORE" && argumentCount == 2 {
		if !list.insertBefore(arguments[0], arguments[1]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&list, arguments[1])
	}
	if command == "INSERTAFTER" && argumentCount == 2 {
		if !list.insertAfter(arguments[0], arguments[1]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&list, arguments[1])
	}
	if command == "DEL" && argumentCount == 1 {
		if !list.removeValue(arguments[0]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&list, "OK")
	}
	if command == "FIND" && argumentCount == 1 {
		fmt.Println(booleanAnswer(list.contains(arguments[0])))
		return nil
	}
	if command == "DELFRONT" && argumentCount == 0 {
		if !list.popFront() {
			return errors.New("список пуст")
		}
		return entry.save(&list, "OK")
	}
	if command == "DELBACK" && argumentCount == 0 {
		if !list.popBack() {
			return errors.New("список пуст")
		}
		return entry.save(&list, "OK")
	}
	if command == "DELBEFORE" && argumentCount == 1 {
		if !list.removeBefore(arguments[0]) {
			return errors.New("нет элемента перед указанным значением")
		}
		return entry.save(&list, "OK")
	}
	if command == "DELAFTER" && argumentCount == 1 {
		if !list.removeAfter(arguments[0]) {
			return errors.New("нет элемента после указанного значения")
		}
		return entry.save(&list, "OK")
	}
	if command == "GET" && argumentCount == 1 {
		index, err := indexOf(arguments[0])
		if err == nil {
			if value, ok := list.get(index); ok {
				fmt.Println(value)
				return nil
			}
		}
		return errors.New("неверный индекс")
	}
	if command == "PRINT" {
		list.printForward()
		return nil
	}
	if command == "REVERSE" {
		list.printBackward()
		return nil
	}
	return errors.New("неверная команда двусвязного списка")
}

func stackCommand(entry record, command string, arguments [4]string, argumentCount int) error {
	var stack Stack
	if err := entry.load(&stack); err != nil {
		return err
	}
	if command == "PUSH" && argumentCount == 1 {
		stack.push(arguments[0])
		return entry.save(&stack, arguments[0])
	}
	if command == "POP" && argumentCount == 0 {
		value, ok := stack.pop()
		if !ok {
			return errors.New("стек пуст")
		}
		return entry.save(&stack, value)
	}
	if command == "PEEK" && argumentCount == 0 {
		value, ok := stack.peek()
		if !ok {
			return errors.New("стек пуст")
		}
		fmt.Println(value)
		return nil
	}
	if command == "PRINT" {
		stack.print()
		return nil
	}
	return errors.New("неверная команда стека")
}

func queueCommand(entry record, command string, arguments [4]string, argumentCount int) error {
	var queue Queue
	if err := entry.load(&queue); err != nil {
		return err
	}
	if command == "PUSH" && argumentCount == 1 {
		queue.push(arguments[0])
		return entry.save(&queue, arguments[0])
	}
	if command == "POP" && argumentCount == 0 {
		value, ok := queue.pop()
		if !ok {
			return errors.New("очередь пуста")
		}
		return entry.save(&queue, value)
	}
	if command == "PEEK" && argumentCount == 0 {
		value, ok := queue.peek()
		if !ok {
			return errors.New("очередь пуста")
		}
		fmt.Println(value)
		return nil
	}
	if command == "PRINT" {
		queue.print()
		return nil
	}
	return errors.New("неверная команда очереди")
}

func treeCommand(entry record, command string, arguments [4]string, argumentCount int) error {
	var tree FullBinaryTree
	if err := entry.load(&tree); err != nil {
		return err
	}
	if command == "INSERT" && argumentCount == 1 {
		if arguments[0] == "#" {
			return errors.New("символ # зарезервирован для пустой ветви")
		}
		tree.insert(arguments[0])
		return entry.save(&tree, arguments[0])
	}
	if (command == "ADDPAIR" || command == "ADDPPAIR") && argumentCount == 3 {
		if arguments[1] == "#" || arguments[2] == "#" {
			return errors.New("символ # зарезервирован для пустой ветви")
		}
		if !tree.addChildren(arguments[0], arguments[1], arguments[2]) {
			return errors.New("родитель не найден или уже имеет детей")
		}
		return entry.save(&tree, "OK")
	}
	if command == "DEL" && argumentCount == 1 {
		if !tree.remove(arguments[0]) {
			return errors.New("значение не найдено")
		}
		return entry.save(&tree, "OK")
	}
	if (command == "GET" || command == "MEMBER") && argumentCount == 1 {
		fmt.Println(booleanAnswer(tree.contains(arguments[0])))
		return nil
	}
	if command == "ISFULL" && argumentCount == 0 {
		fmt.Println(booleanAnswer(tree.isFull()))
		return nil
	}
	if command == "PRINT" {
		tree.print()
		return nil
	}
	return errors.New("неверная команда дерева")
}
