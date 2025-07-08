#ifndef BINARYTREE_H
#define BINARYTREE_H

#include <stdio.h>
#include <stdlib.h>

// Function Pointers:
typedef int (*CompareFunc)(const void *, const void *);
typedef void (*PrintFunc)(const void *);
typedef void (*FreeFunc)(void *);

// TreeNode
typedef struct TreeNode
{
  struct TreeNode *left;
  struct TreeNode *right;
  void *data;
} TreeNode;

// configiration for binary tree

typedef struct BinaryTree
{
  TreeNode *root;
  
  CompareFunc cmp;
  PrintFunc print;
  FreeFunc destroy;
} BinaryTree;

// Functions
// init a BinaryTree with function pointers
BinaryTree initTree(CompareFunc cmp, PrintFunc print, FreeFunc destroy);

// insert data to the tree
TreeNode *insertNode(TreeNode *root, void *data, CompareFunc cmp);

// insert data
int insertBST(BinaryTree *bst, void *data);

// count nodes
int countNodes(TreeNode *root);

// destroy the tree
void destroyTree(TreeNode *root, FreeFunc destroy);


void inorderBST(BinaryTree *tree, void (*printFunc)(const void *));

// find maximum node with custom cmp function
void* findMaxData(TreeNode* root, CompareFunc cmp);




// inorder Traversal
// void inorderTraversal(TreeNode *root, PrintFunc print);

// search data
// void *searchBST(BinaryTree *tree, const void *data);

#endif // BINARYTREE_H