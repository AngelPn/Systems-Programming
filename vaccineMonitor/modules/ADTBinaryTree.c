#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "../include/ADTBinaryTree.h"

struct binary_tree{
    int size;
    BTNode root;
    DestroyFunc destroy_item;
};

struct binary_tree_node{
    void *item;
    BTNode parent;
    BTNode left;
    BTNode right;
};

BinaryTree BT_create(){
    BinaryTree tree = malloc(sizeof(struct binary_tree));
    tree->size = 0;
    tree->root = NULL;
    return tree;
}

int BTIsNil(BTNode node){
	if (node == NULL) return 1;
	else return 0;
}

int BT_size(BinaryTree tree){
    return tree->size;
}

int maxDepth(BinaryTree tree, BTNode node){
    if(BTIsNil(node)) return 0;
	else{
		int LeftDepth= maxDepth(tree, node->left); /*Find the height of left subtree recursively*/
		int RightDepth= maxDepth(tree, node->right); /*Find the height od right subtree recursively*/
		/*Return the highest height*/
		if (LeftDepth>RightDepth) return LeftDepth+1; 
		else return RightDepth+1;
	}
}

int BT_height(BinaryTree tree){
    if(BTIsNil(tree->root) ){
		printf("Error! The tree is empty\n");
		exit(EXIT_FAILURE);
	}
	else return maxDepth(tree, tree->root);
}

BTNode BT_root(BinaryTree tree){
    return tree->root;
}

BTNode BT_parent(BinaryTree tree, BTNode node){
    return node->parent;
}

BTNode BT_childleft(BinaryTree tree, BTNode node){
    return node->left;
}

BTNode BT_childright(BinaryTree tree, BTNode node){
    return node->right;
}

void *BT_get_item(BinaryTree tree, BTNode node){
    return node->item;
}

void BT_set_item(BinaryTree tree, BTNode node, void * item, size_t item_sz){
    tree->destroy_item(node->item);
    node->item = malloc(sizeof(item_sz));
    memcpy(node->item, item, item_sz);
}

void BT_insert_root(BinaryTree tree, void * item, size_t item_sz){
    if (!BTIsNil(tree->root)){
		printf("Error! Root is not empty");
		return;
	}
	(tree->size)++; /*Increase the size*/
	tree->root = malloc(sizeof(struct binary_tree_node)); /*Allocate memory for a new node*/
	
    tree->root->item = malloc(sizeof(item_sz));
    memcpy(tree->root->item, item, item_sz);
    //Initialize children
    tree->root->parent = NULL;
	tree->root->left=NULL;
	tree->root->right=NULL;
}

void BT_insert_left(BinaryTree tree, BTNode node, void * item, size_t item_sz){
    if (!BTIsNil(node->left)){
		printf("Error! ChildLeft of the node is not empty\n");
		return;
	}
	(tree->size)++; /*Increase the size*/
	node->left = malloc(sizeof(struct binary_tree_node)); /*Allocate memory for a new node*/
	node->left->item = malloc(sizeof(item_sz));
    memcpy(node->left->item, item, item_sz);
	node->left->parent = node;
	node->left->left=NULL; /*Initialize children*/
	node->left->right=NULL;
}

void BT_insert_right(BinaryTree tree, BTNode node, void * item, size_t item_sz){
    if (!BTIsNil(node->right)){
		printf("Error! ChildRight of the node is not empty\n");
		return;
	}
	(tree->size)++; /*Increase the size*/
	node->right = malloc(sizeof(struct binary_tree_node)); /*Allocate memory for a new node*/
	node->right->item = malloc(sizeof(item_sz));
    memcpy(node->right->item, item, item_sz);
	node->right->parent = node;
	node->right->left=NULL; /*Initialize children*/
	node->right->right=NULL;
}

void BT_remove(BinaryTree tree, BTNode node){
    if(!BTIsNil(node->left) && !BTIsNil(node->right)){
        printf("Error! The node can't be removed because it has two children\n");
        return;
    }
    else{

    }
}

void BT_traverse_preorder(BinaryTree tree, BTNode node, BinaryTreeVisitFunc visit){
    if(node!= NULL){
        visit(tree, node);
        BT_traverse_preorder(tree, node->left, visit);
        BT_traverse_preorder(tree, node->right, visit);
    }
}

void BT_preorder(BinaryTree tree, BinaryTreeVisitFunc visit){
    BT_traverse_preorder(tree, BT_root(tree), visit);
}

void BT_traverse_inorder(BinaryTree tree, BTNode node, BinaryTreeVisitFunc visit){
    if(node!= NULL){
        BT_traverse_inorder(tree, node->left, visit);
        visit(tree, node);
        BT_traverse_inorder(tree, node->right, visit);
    }
}

void BT_inorder(BinaryTree tree, BinaryTreeVisitFunc visit){
    BT_traverse_inorder(tree, BT_root(tree), visit);
}

void BT_traverse_postorder(BinaryTree tree, BTNode node, BinaryTreeVisitFunc visit){
    if(node!= NULL){
        BT_traverse_postorder(tree, node->left, visit);
        BT_traverse_postorder(tree, node->right, visit);
        visit(tree, node);
    }
}

void BT_postorder(BinaryTree tree, BinaryTreeVisitFunc visit){
    BT_traverse_postorder(tree, BT_root(tree), visit);
}

void BT_levelorder(BinaryTree tree, BinaryTreeVisitFunc visit){
    BT_traverse_levelorder(tree, tree->root, visit);
}

void FreeNode(BinaryTree tree, BTNode node){
    (tree->destroy_item)(node->item);
	free(node);
}

void BT_destroy(BinaryTree tree){
    BT_postorder(tree, FreeNode);
}