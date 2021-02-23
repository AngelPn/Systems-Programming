
#include "ADTBinaryTree.h"

// ADT Binary Search Tree

typedef int (*compare)(BSTKeyType A, BSTKeyType B);

typedef BinaryTree BSTree;

BSTree BSTCreate();
BTNode BSTGetRoot(BSTree tree);
BTNode BSTGetParent(BSTree tree, BTNode n);
BTNode BSTGetChildLeft(BSTree tree, BTNode n);
BTNode BSTGetChildRight(BSTree tree, BTNode n);

void *BSTGetItem(BSTree tree, BTNode node);

void BSTInsertRoot(BSTree tree, BTItem item);
void BSTInsertR(BTNode node, BSTKeyType Key, BSTItem Item, compare CompareFunction);
void BSTInsert(BSTree tree, BSTKeyType Key, BSTItem Item, compare CompareFunction);
BTNode BSTSearchR(BTNode node, BSTKeyType Key, compare CompareFunction);
BTNode BSTSearch(BSTree tree, BSTKeyType Key, compare CompareFunction);
void BSTRemove(BSTree tree, BSTKeyType Key, compare CompareFunction);
void BSTChange(BSTree tree, BTNode node, BTItem item);
void BSTPreOrder(BSTree tree, visit Function);
void BSTInOrder(BSTree tree, visit Function);
void BSTPostOrder(BSTree tree, visit Function);
void BSTLevelOrder(BSTree tree, visit Function);
void BSTDestroy(BSTree tree);