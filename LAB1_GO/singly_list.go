package main

import (
	"bufio"
	"fmt"
	"io"
)

// SinglyNode — узел односвязного списка.
type SinglyNode struct {
	value string
	next  *SinglyNode
}

// SinglyList хранит указатели на первый и последний узлы.
type SinglyList struct {
	head *SinglyNode
	tail *SinglyNode
}

func (l *SinglyList) findNode(value string) *SinglyNode {
	for current := l.head; current != nil; current = current.next {
		if current.value == value {
			return current
		}
	}
	return nil
}

func (l *SinglyList) pushFront(value string) {
	node := &SinglyNode{value: value, next: l.head}
	l.head = node
	if l.tail == nil {
		l.tail = node
	}
}

func (l *SinglyList) pushBack(value string) {
	node := &SinglyNode{value: value}
	if l.tail == nil {
		l.head = node
		l.tail = node
		return
	}
	l.tail.next = node
	l.tail = node
}

func (l *SinglyList) insertBefore(key, value string) bool {
	if l.head == nil {
		return false
	}
	if l.head.value == key {
		l.pushFront(value)
		return true
	}

	// Ищем узел, который находится перед key.
	for current := l.head; current.next != nil; current = current.next {
		if current.next.value == key {
			current.next = &SinglyNode{value: value, next: current.next}
			return true
		}
	}
	return false
}

func (l *SinglyList) insertAfter(key, value string) bool {
	node := l.findNode(key)
	if node == nil {
		return false
	}

	newNode := &SinglyNode{value: value, next: node.next}
	node.next = newNode
	if node == l.tail {
		l.tail = newNode
	}
	return true
}

func (l *SinglyList) popFront() bool {
	if l.head == nil {
		return false
	}
	l.head = l.head.next
	if l.head == nil {
		l.tail = nil
	}
	return true
}

func (l *SinglyList) popBack() bool {
	if l.head == nil {
		return false
	}
	if l.head == l.tail {
		return l.popFront()
	}

	// У узла нет ссылки назад, поэтому ищем предпоследний узел.
	current := l.head
	for current.next != l.tail {
		current = current.next
	}
	l.tail = current
	l.tail.next = nil
	return true
}

func (l *SinglyList) removeBefore(key string) bool {
	if l.head == nil || l.head.value == key {
		return false
	}
	if l.head.next != nil && l.head.next.value == key {
		return l.popFront()
	}

	for current := l.head; current.next != nil && current.next.next != nil; current = current.next {
		if current.next.next.value == key {
			current.next = current.next.next
			return true
		}
	}
	return false
}

func (l *SinglyList) removeAfter(key string) bool {
	node := l.findNode(key)
	if node == nil || node.next == nil {
		return false
	}
	if node.next == l.tail {
		l.tail = node
	}
	node.next = node.next.next
	return true
}

func (l *SinglyList) removeValue(value string) bool {
	if l.head == nil {
		return false
	}
	if l.head.value == value {
		return l.popFront()
	}

	for current := l.head; current.next != nil; current = current.next {
		if current.next.value == value {
			if current.next == l.tail {
				l.tail = current
			}
			current.next = current.next.next
			return true
		}
	}
	return false
}

func (l *SinglyList) get(index int) (string, bool) {
	if index < 0 {
		return "", false
	}
	// Считаем позиции от головы списка.
	current := l.head
	for current != nil && index > 0 {
		current = current.next
		index--
	}
	if current == nil {
		return "", false
	}
	return current.value, true
}

func (l *SinglyList) contains(value string) bool {
	return l.findNode(value) != nil
}

func (l *SinglyList) print() {
	fmt.Print("[ ")
	for current := l.head; current != nil; current = current.next {
		fmt.Print(current.value, " ")
	}
	fmt.Println("]")
}

func printSinglyReverse(node *SinglyNode) {
	if node == nil {
		return
	}
	printSinglyReverse(node.next)
	fmt.Print(node.value, " ")
}

func (l *SinglyList) printReverse() {
	fmt.Print("[ ")
	printSinglyReverse(l.head)
	fmt.Println("]")
}

func (l *SinglyList) load(file io.Reader) bool {
	reader := bufio.NewReader(file)
	for {
		var value string
		if _, err := fmt.Fscan(reader, &value); err != nil {
			return err == io.EOF
		}
		l.pushBack(value)
	}
}

func (l *SinglyList) save(writer io.Writer) {
	for current := l.head; current != nil; current = current.next {
		fmt.Fprint(writer, current.value, " ")
	}
}
