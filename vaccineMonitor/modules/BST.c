#include <stdio.h>
#include <stdlib.h>

#include "../include/BST.h"

/*helpful static extern variables*/
static BSTree z;
static BTNode y;
static int counter;

BSTree BSTCreate(){
	return BT_create();
}

BTNode BSTGetRoot(BSTree tree){
	return BT_root(tree);
}

BTNode BSTGetParent(BSTree tree, BTNode n){
	return BT_parent(tree, n);
}

BTNode BSTGetChildLeft(BSTree tree, BTNode n){
	return BT_childleft(tree,n);
}

BTNode BSTGetChildRight(BSTree tree, BTNode n){
	return BT_childright(tree, n);
}

void *BSTGetItem(BSTree tree, BTNode node){
	return BT_get_item(tree, node);
}

void BSTInsertRoot(BSTree tree, BTItem item){
	BT_insert_root(tree, item);
}

void BSTInsertR(BTNode node, BSTKeyType Key, BSTItem Item, compare CompareFunction){/*A recursive function that inserts item in BST*/
	BTNode childleft=BSTGetChildLeft(z,node); /*Get the children of the node we check*/
	BTNode childright=BSTGetChildRight(z,node);
	int unlocked= CompareFunction(Key, BSTGetItem(z,node)->key); /*Compare the key of item we want to insert with the key of root*/
	if( unlocked==0 ) return; /*If KeyItem= KeyRoot stop*/
	else if( unlocked>0 ){/*If KeyItem< KeyRoot go to the left subtree*/
		if(BSTIsNil(childleft)){ /*If the subtree is Nil insert*/
			BTItem x=(BTItem)malloc(sizeof(BTitem)); x->key=Key; x->item=Item;
			BTInsertLeft(z, node,x);
		}
		else BSTInsertR(childleft, Key, Item, CompareFunction);/*Otherwise, call BSTInsertR with the new node to check*/
	}
	else{ /*If KeyItem> KeyRoot go to the right subtree*/
		if(BSTIsNil(childright)){/*If the subtree is Nil insert*/
			BTItem x=(BTItem)malloc(sizeof(BTitem)); x->key=Key; x->item=Item;
			BTInsertRight(z, node,x);
		}
		else BSTInsertR(childright, Key, Item, CompareFunction);/*Otherwise, call BSTInsertR with the new node to check*/
	}
}

void BSTInsert(BSTree tree, char *Key, void *Item, compare CompareFunction){
	if(BSTGetRoot(tree) == NULL){
		BTItem x=(BTItem)malloc(sizeof(BTitem));
        x->key=Key;
        x->item=Item;
		BSTInsertRoot(tree, x);
	}
	else{
		z=tree;
		y=BSTGetRoot(tree);
		BSTInsertR(y, Key, Item, CompareFunction); /*Call the recursive function that inserts the new item in BST*/
	}
}

BTNode BSTSearchR(BTNode node, BSTKeyType Key, compare CompareFunction){/*A recursive function that search key in BST*/
	BTNode childleft=BSTGetChildLeft(z,node);/*Get the children of the node we check*/
	BTNode childright=BSTGetChildRight(z,node);
	int unlocked= CompareFunction(Key, BSTGetItem(z,node)->key);/*Compare the key of item we are searching for with the key of root*/
	if( unlocked>0 )/*If KeyItem< KeyRoot go to the left subtree*/
		node=BSTSearchR(childleft, Key, CompareFunction);
	else if(unlocked<0)/*If KeyItem> KeyRoot go to the right subtree*/
		node=BSTSearchR(childright, Key, CompareFunction);
	return node;
}

BTNode BSTSearch(BSTree tree, BSTKeyType Key, compare CompareFunction){
	z=tree;
	y=BSTGetRoot(tree);
	return BSTSearchR(y, Key, CompareFunction);/*Call the recursive function that search the key in BST*/
}

void GetLastRightNode(BTNode node){
	if(!counter){
		y= node;
		counter++;
	} 
}

void BSTRemove(BSTree tree, BSTKeyType Key, compare CompareFunction){
	BTNode remnode= BSTSearch(tree, Key, CompareFunction);/*Find the node we want to remove*/
	BTNode childright= BSTGetChildRight(tree, remnode);/*Get the subtrees of that node*/
	BTNode childleft= BSTGetChildLeft(tree, remnode);

	if(childleft==NULL && childright==NULL)
		BTRemove(tree, remnode);
	else if(BTIsNil(childleft) && BTIsNil(childright))
		BTRemove(tree, remnode);
	else if(BTIsNil(childright) || BTIsNil(childright))
		BTRemove(tree,remnode);
	else {
		TraverseInOrder(tree, childright, GetLastRightNode); /*Find the rightmost node of right subtree*/
		BSTChange(tree, remnode, BSTGetItem(tree, y)); /*Exchange the item*/
		counter=0; /*Reset counter*/
		BTRemove(tree,y);
	}
}


void BSTChange(BSTree tree, BTNode node, BTItem item){
	BTChange(tree,node,item);
}

void BSTPreOrder(BSTree tree, visit Function){
	BTPreOrder(tree,Function);
}

void BSTInOrder(BSTree tree, visit Function){
	BTInOrder(tree,Function);
}

void BSTPostOrder(BSTree tree, visit Function){
	BTPostOrder(tree, Function);
}

void BSTLevelOrder(BSTree tree, visit Function){
	BTLevelOrder(tree,Function);
}

void BSTDestroy(BSTree tree){
	BTDestroy(tree);
}