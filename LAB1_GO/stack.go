package main

import (
	"bufio"
	"fmt"
	"io"
)

// Стек работает по принципу LIFO: последний вошёл, первый вышел.
type StackNode struct {
	value string
	next  *StackNode
}

type Stack struct {
	top *StackNode
}

func (s *Stack) push(value string) {
	s.top = &StackNode{value: value, next: s.top}
}

func (s *Stack) pop() (string, bool) {
	if s.top == nil {
		return "", false
	}
	value := s.top.value
	s.top = s.top.next
	return value, true
}

func (s *Stack) peek() (string, bool) {
	if s.top == nil {
		return "", false
	}
	return s.top.value, true
}

func (s *Stack) print() {
	fmt.Print("Вершина -> ")
	for current := s.top; current != nil; current = current.next {
		fmt.Print(current.value, " ")
	}
	fmt.Println()
}

func (s *Stack) load(file io.Reader) bool {
	// Вершина записана первой, поэтому строим цепочку с её конца.
	var last *StackNode
	reader := bufio.NewReader(file)
	for {
		var value string
		if _, err := fmt.Fscan(reader, &value); err != nil {
			return err == io.EOF
		}
		node := &StackNode{value: value}
		if s.top == nil {
			s.top = node
		} else {
			last.next = node
		}
		last = node
	}
}

func (s *Stack) save(writer io.Writer) {
	for current := s.top; current != nil; current = current.next {
		fmt.Fprint(writer, current.value, " ")
	}
}
