#pragma once

#include "common_types.h"

// ADT Binary Tree

//struct binary_tree and struct binary_tree_node are incomplete structs
typedef struct binary_tree* BinaryTree;
typedef struct binary_tree_node* BTNode;

typedef void (*BinaryTreeVisitFunc)(BinaryTree, BTNode);

//Creates and returns a new Binary Tree
BinaryTree BT_create();

//Returns size and height of Binary Tree
int BT_size(BinaryTree tree); /*size is the number of nodes that has the binary tree*/
int BT_heigth(BinaryTree tree);

//Returns the nodes: root, parent, childleft, childright
BTNode BT_root(BinaryTree tree);
BTNode BT_parent(BinaryTree tree, BTNode node);
BTNode BT_childleft(BinaryTree tree, BTNode node);
BTNode BT_childright(BinaryTree tree, BTNode node);

void *BT_get_item(BinaryTree tree, BTNode node);
void BT_set_item(BinaryTree tree, BTNode node, void * item, size_t item_sz);

void BT_insert_root(BinaryTree tree, void *item, size_t item_sz);
void BT_insert_left(BinaryTree tree, BTNode node, void * item, size_t item_sz);
void BT_insert_right(BinaryTree tree, BTNode node, void * item, size_t item_sz);
void BT_remove(BinaryTree tree, BTNode node);

//Traverse Binary Tree
void BT_preorder(BinaryTree tree, BinaryTreeVisitFunc visit);
void BT_inorder(BinaryTree tree, BinaryTreeVisitFunc visit);
void BT_postorder(BinaryTree tree, BinaryTreeVisitFunc visit);

void BT_Destroy(BinaryTree tree);

void BT_print(BinaryTree tree, PrintItem print);