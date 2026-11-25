package main

import (
	"bufio"
	"fmt"
	"io"
)

// Очередь работает по принципу FIFO: первый вошёл, первый вышел.
type QueueNode struct {
	value string
	next  *QueueNode
}

type Queue struct {
	first *QueueNode
	last  *QueueNode
}

func (q *Queue) push(value string) {
	node := &QueueNode{value: value}
	if q.last == nil {
		q.first = node
		q.last = node
		return
	}
	q.last.next = node
	q.last = node
}

func (q *Queue) pop() (string, bool) {
	if q.first == nil {
		return "", false
	}
	value := q.first.value
	q.first = q.first.next
	if q.first == nil {
		q.last = nil
	}
	return value, true
}

func (q *Queue) peek() (string, bool) {
	if q.first == nil {
		return "", false
	}
	return q.first.value, true
}

func (q *Queue) print() {
	fmt.Print("Начало -> ")
	for current := q.first; current != nil; current = current.next {
		fmt.Print(current.value, " ")
	}
	fmt.Println("<- конец")
}

func (q *Queue) load(file io.Reader) bool {
	reader := bufio.NewReader(file)
	for {
		var value string
		if _, err := fmt.Fscan(reader, &value); err != nil {
			return err == io.EOF
		}
		q.push(value)
	}
}

func (q *Queue) save(writer io.Writer) {
	for current := q.first; current != nil; current = current.next {
		fmt.Fprint(writer, current.value, " ")
	}
}
