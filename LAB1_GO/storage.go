package main

import (
	"fmt"
	"io"
	"os"
	"path/filepath"
	"strings"
)

type storedStructure interface {
	load(io.Reader) bool
	save(io.Writer)
}

// Запись базы занимает одну строку: тип, имя и значения через пробел.
type database struct {
	path string
	data string
}

func openDatabase(path string) (database, error) {
	data, err := os.ReadFile(path)
	if err != nil && !os.IsNotExist(err) {
		return database{}, fmt.Errorf("не удалось прочитать файл данных: %w", err)
	}
	return database{path: path, data: string(data)}, nil
}

type record struct {
	path    string
	key     string
	value   string
	others  string
	oldPath string
	found   bool
}

func (db database) record(kind, name string) record {
	entry := record{
		path:    db.path,
		key:     kind + " " + name + " ",
		oldPath: db.path + "." + name + "." + kind,
	}
	var others strings.Builder
	for rest := db.data; rest != ""; {
		var line string
		line, rest, _ = strings.Cut(rest, "\n")
		if strings.HasPrefix(line, entry.key) {
			entry.found = true
			entry.value = line[len(entry.key):]
		} else {
			others.WriteString(line)
			others.WriteByte('\n')
		}
	}
	entry.others = others.String()
	return entry
}

func (entry record) exists() bool {
	if entry.found {
		return true
	}
	info, err := os.Stat(entry.oldPath)
	return err == nil && info.Mode().IsRegular()
}

func (entry record) load(structure storedStructure) error {
	values := entry.value
	if !entry.found {
		// Поддерживаем отдельные файлы, созданные предыдущей версией.
		data, err := os.ReadFile(entry.oldPath)
		if os.IsNotExist(err) {
			return nil
		}
		if err != nil {
			return fmt.Errorf("не удалось прочитать структуру: %w", err)
		}
		values = string(data)
	}
	if !structure.load(strings.NewReader(values)) {
		return fmt.Errorf("не удалось загрузить структуру")
	}
	return nil
}

func (entry record) save(structure storedStructure, answer string) error {
	var contents strings.Builder
	contents.WriteString(entry.others)
	contents.WriteString(entry.key)
	structure.save(&contents)
	contents.WriteByte('\n')

	// Подменяем файл целиком только после успешной записи.
	temporary := filepath.Join(entry.path+".lock", "new.data")
	defer os.Remove(temporary)
	if err := os.WriteFile(temporary, []byte(contents.String()), 0644); err != nil {
		return fmt.Errorf("не удалось сохранить файл данных: %w", err)
	}
	if err := os.Rename(temporary, entry.path); err != nil {
		return fmt.Errorf("не удалось сохранить файл данных: %w", err)
	}
	fmt.Println(answer)
	return nil
}
