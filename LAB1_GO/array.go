package main

import (
	"bufio"
	"fmt"
	"io"
)

const maxArraySize = 200

// Array — статический массив на 200 строк.
type Array struct {
	data [maxArraySize]string
	size int
}

func (a *Array) pushBack(value string) bool {
	if a.size == maxArraySize {
		return false
	}
	a.data[a.size] = value
	a.size++
	return true
}

func (a *Array) insert(index int, value string) bool {
	if index < 0 || index > a.size || a.size == maxArraySize {
		return false
	}

	// Сдвигаем элементы вправо и освобождаем нужную ячейку.
	for i := a.size; i > index; i-- {
		a.data[i] = a.data[i-1]
	}
	a.data[index] = value
	a.size++
	return true
}

func (a *Array) remove(index int) bool {
	if index < 0 || index >= a.size {
		return false
	}

	// После удаления закрываем пустое место сдвигом влево.
	for i := index; i < a.size-1; i++ {
		a.data[i] = a.data[i+1]
	}
	a.size--
	a.data[a.size] = ""
	return true
}

func (a *Array) set(index int, value string) bool {
	if index < 0 || index >= a.size {
		return false
	}
	a.data[index] = value
	return true
}

func (a *Array) get(index int) (string, bool) {
	if index < 0 || index >= a.size {
		return "", false
	}
	return a.data[index], true
}

func (a *Array) print() {
	fmt.Print("[ ")
	for i := 0; i < a.size; i++ {
		fmt.Print(a.data[i], " ")
	}
	fmt.Println("]")
}

func (a *Array) load(file io.Reader) bool {
	reader := bufio.NewReader(file)
	for {
		var value string
		if _, err := fmt.Fscan(reader, &value); err != nil {
			return err == io.EOF
		}
		if !a.pushBack(value) {
			return false
		}
	}
}

func (a *Array) save(writer io.Writer) {
	for i := 0; i < a.size; i++ {
		fmt.Fprint(writer, a.data[i], " ")
	}
}
