package main

import (
	"fmt"
	"os"
)

func main() {
	if err := run(); err != nil {
		fmt.Fprintln(os.Stderr, err)
		os.Exit(1)
	}
}

func run() error {
	var fileName, query string
	if len(os.Args) == 5 {
		for i := 1; i < len(os.Args); i += 2 {
			if os.Args[i] == "--file" {
				fileName = os.Args[i+1]
			} else if os.Args[i] == "--query" {
				query = os.Args[i+1]
			}
		}
	}
	if fileName == "" || query == "" {
		return fmt.Errorf("Формат: ./dbms --file file.data --query 'SPUSH mystack item'")
	}

	// Пока выполняется запрос, второй процесс не может перезаписать базу.
	lock := fileName + ".lock"
	if err := os.Mkdir(lock, 0700); err != nil {
		if os.IsExist(err) {
			return fmt.Errorf("Ошибка: файл данных занят")
		}
		return fmt.Errorf("Ошибка: %w", err)
	}
	defer os.Remove(lock)

	if err := executeCommand(fileName, query); err != nil {
		return fmt.Errorf("Ошибка: %w", err)
	}
	return nil
}
