package main

import (
	"bufio"
	"fmt"
	"io"
)

// Full Binary Tree: каждый узел имеет либо 0, либо 2 ребёнка.
type TreeNode struct {
	value string
	left  *TreeNode
	right *TreeNode
}

type FullBinaryTree struct {
	root *TreeNode
}

// Поиск выполняется прямым обходом: корень, левая ветвь, правая ветвь.
func findTreeNode(node *TreeNode, value string) *TreeNode {
	if node == nil || node.value == value {
		return node
	}
	if found := findTreeNode(node.left, value); found != nil {
		return found
	}
	return findTreeNode(node.right, value)
}

func treeHeight(node *TreeNode) int {
	if node == nil {
		return 0
	}
	leftHeight := treeHeight(node.left)
	rightHeight := treeHeight(node.right)
	if leftHeight > rightHeight {
		return leftHeight + 1
	}
	return rightHeight + 1
}

func insertTreeAtLevel(node *TreeNode, level int, value string) bool {
	if node == nil {
		return false
	}
	if level == 1 {
		if node.left == nil {
			node.left = &TreeNode{value: value}
			return true
		}
		if node.right == nil {
			node.right = &TreeNode{value: value}
			return true
		}
		return false
	}

	if insertTreeAtLevel(node.left, level-1, value) {
		return true
	}
	return insertTreeAtLevel(node.right, level-1, value)
}

func (t *FullBinaryTree) insert(value string) {
	if t.root == nil {
		t.root = &TreeNode{value: value}
		return
	}

	// Ищем первое свободное место по уровням слева направо.
	height := treeHeight(t.root)
	for level := 1; level <= height; level++ {
		if insertTreeAtLevel(t.root, level, value) {
			return
		}
	}
}

func (t *FullBinaryTree) addChildren(parent, leftValue, rightValue string) bool {
	node := findTreeNode(t.root, parent)
	// Пару детей можно добавить только существующему листу.
	if node == nil || node.left != nil || node.right != nil {
		return false
	}
	node.left = &TreeNode{value: leftValue}
	node.right = &TreeNode{value: rightValue}
	return true
}

// link указывает на поле родителя, которое нужно обнулить при удалении листа.
func removeLastLeaf(link **TreeNode, level int, target *TreeNode) bool {
	node := *link
	if node == nil {
		return false
	}
	if level == 1 {
		target.value = node.value
		*link = nil
		return true
	}

	// На заданной глубине ищем первый лист справа.
	if removeLastLeaf(&node.right, level-1, target) {
		return true
	}
	return removeLastLeaf(&node.left, level-1, target)
}

func (t *FullBinaryTree) remove(value string) bool {
	target := findTreeNode(t.root, value)
	if target == nil {
		return false
	}
	return removeLastLeaf(&t.root, treeHeight(t.root), target)
}

func (t *FullBinaryTree) contains(value string) bool {
	return findTreeNode(t.root, value) != nil
}

func isFullTree(node *TreeNode) bool {
	// Пустая ветвь и лист удовлетворяют условию Full.
	if node == nil || node.left == nil && node.right == nil {
		return true
	}
	// Ровно один ребёнок нарушает условие Full.
	if node.left == nil || node.right == nil {
		return false
	}
	return isFullTree(node.left) && isFullTree(node.right)
}

func (t *FullBinaryTree) isFull() bool {
	return isFullTree(t.root)
}

func printTree(node *TreeNode, level int) {
	if node == nil {
		return
	}
	// Правую ветвь печатаем сверху, левую снизу.
	printTree(node.right, level+1)
	for i := 0; i < level; i++ {
		fmt.Print("    ")
	}
	fmt.Println(node.value)
	printTree(node.left, level+1)
}

func (t *FullBinaryTree) print() {
	if t.root == nil {
		fmt.Println("Дерево пусто")
		return
	}
	printTree(t.root, 0)
}

func saveTreeNode(writer io.Writer, node *TreeNode) {
	if node == nil {
		fmt.Fprint(writer, "#", " ")
		return
	}
	// Прямой обход и # сохраняют точную форму дерева.
	fmt.Fprint(writer, node.value, " ")
	saveTreeNode(writer, node.left)
	saveTreeNode(writer, node.right)
}

func loadTreeNode(reader *bufio.Reader) (*TreeNode, error) {
	var value string
	if _, err := fmt.Fscan(reader, &value); err != nil {
		return nil, err
	}
	if value == "#" {
		return nil, nil
	}

	left, err := loadTreeNode(reader)
	if err != nil {
		return nil, err
	}
	right, err := loadTreeNode(reader)
	if err != nil {
		return nil, err
	}
	return &TreeNode{value: value, left: left, right: right}, nil
}

func (t *FullBinaryTree) load(file io.Reader) bool {
	root, err := loadTreeNode(bufio.NewReader(file))
	if err != nil {
		return false
	}
	t.root = root
	return true
}

func (t *FullBinaryTree) save(writer io.Writer) {
	saveTreeNode(writer, t.root)
}
