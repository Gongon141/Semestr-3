package main

import (
	"bufio"
	"fmt"
	"io"
)

// DoublyNode знает и предыдущий, и следующий узел.
type DoublyNode struct {
	value    string
	previous *DoublyNode
	next     *DoublyNode
}

type DoublyList struct {
	head *DoublyNode
	tail *DoublyNode
}

func (l *DoublyList) findNode(value string) *DoublyNode {
	for current := l.head; current != nil; current = current.next {
		if current.value == value {
			return current
		}
	}
	return nil
}

func (l *DoublyList) removeNode(node *DoublyNode) {
	// Соседи удаляемого узла начинают указывать друг на друга.
	if node.previous == nil {
		l.head = node.next
	} else {
		node.previous.next = node.next
	}

	if node.next == nil {
		l.tail = node.previous
	} else {
		node.next.previous = node.previous
	}
}

func (l *DoublyList) pushFront(value string) {
	node := &DoublyNode{value: value, next: l.head}
	if l.head != nil {
		l.head.previous = node
	} else {
		l.tail = node
	}
	l.head = node
}

func (l *DoublyList) pushBack(value string) {
	node := &DoublyNode{value: value, previous: l.tail}
	if l.tail != nil {
		l.tail.next = node
	} else {
		l.head = node
	}
	l.tail = node
}

func (l *DoublyList) insertBefore(key, value string) bool {
	node := l.findNode(key)
	if node == nil {
		return false
	}
	if node == l.head {
		l.pushFront(value)
		return true
	}

	newNode := &DoublyNode{value: value, previous: node.previous, next: node}
	node.previous.next = newNode
	node.previous = newNode
	return true
}

func (l *DoublyList) insertAfter(key, value string) bool {
	node := l.findNode(key)
	if node == nil {
		return false
	}
	if node == l.tail {
		l.pushBack(value)
		return true
	}

	newNode := &DoublyNode{value: value, previous: node, next: node.next}
	node.next.previous = newNode
	node.next = newNode
	return true
}

func (l *DoublyList) popFront() bool {
	if l.head == nil {
		return false
	}
	l.removeNode(l.head)
	return true
}

func (l *DoublyList) popBack() bool {
	if l.tail == nil {
		return false
	}
	// Благодаря previous удаление хвоста выполняется за O(1).
	l.removeNode(l.tail)
	return true
}

func (l *DoublyList) removeBefore(key string) bool {
	node := l.findNode(key)
	if node == nil || node.previous == nil {
		return false
	}
	l.removeNode(node.previous)
	return true
}

func (l *DoublyList) removeAfter(key string) bool {
	node := l.findNode(key)
	if node == nil || node.next == nil {
		return false
	}
	l.removeNode(node.next)
	return true
}

func (l *DoublyList) removeValue(value string) bool {
	node := l.findNode(value)
	if node == nil {
		return false
	}
	l.removeNode(node)
	return true
}

func (l *DoublyList) get(index int) (string, bool) {
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

func (l *DoublyList) contains(value string) bool {
	return l.findNode(value) != nil
}

func (l *DoublyList) printForward() {
	fmt.Print("[ ")
	for current := l.head; current != nil; current = current.next {
		fmt.Print(current.value, " ")
	}
	fmt.Println("]")
}

func (l *DoublyList) printBackward() {
	fmt.Print("[ ")
	for current := l.tail; current != nil; current = current.previous {
		fmt.Print(current.value, " ")
	}
	fmt.Println("]")
}

func (l *DoublyList) load(file io.Reader) bool {
	reader := bufio.NewReader(file)
	for {
		var value string
		if _, err := fmt.Fscan(reader, &value); err != nil {
			return err == io.EOF
		}
		l.pushBack(value)
	}
}

func (l *DoublyList) save(writer io.Writer) {
	for current := l.head; current != nil; current = current.next {
		fmt.Fprint(writer, current.value, " ")
	}
}
