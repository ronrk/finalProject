#include "../headers/BinaryTree.h"
#include <stdio.h>
#include <stdlib.h>

// Create a new TreeNode
TreeNode* createNode(void* data)
{
  // Allocate memory newNode
  TreeNode* node = malloc(sizeof(TreeNode));
  // Check Allocation
  if(node == NULL) {
    perror("Failed to allocate TreeNode");
    return NULL;
  }
  // Init treeNode
  node->data = data;
  node->left = node->right = NULL;
  return node;
}

// insert node into tree recursive
TreeNode* insertNode (TreeNode* root, void* data,CompareFunc cmp) 
{

  // printf("[insertNode] Start - root: %p\n", (void*)root);

  if(root == NULL) {
    // printf("[insertNode] Creating new node for data %p\n", data);
    // if in a leaf
    TreeNode *node = createNode(data);
    // printf("[insertNode] Node created: %p\n", (void*)node);
    return node;
  }

  // compare newNode data with current node 
//  printf("[insertNode] Comparing data: %p with root data: %p\n", data, root->data);

  int result = cmp(data,root->data);

  // printf("[insertNode] Comparison result: %d\n", result);

  // insert into left
  if(result < 0) {

  // printf("[insertNode] Going left\n");

    root->left = insertNode(root->left,data,cmp);}
  // insert into right

  else if(result > 0) {

  // printf("[insertNode] Going right\n");

    root->right = insertNode(root->right,data,cmp);
  }
// printf("[insertNode] Returning root: %p\n", (void*)root);
  return root;
}

// public insert a node
int insertBST(BinaryTree *tree,void *data) {
  if(tree==NULL) {
     printf("[insertBST] ERROR: NULL tree pointer\n");
    return 0;
  }

  // printf("[insertBST] Inserting data: %p into tree: %p\n", data, (void*)tree);
    // printf("[insertBST] Current root: %p\n", (void*)tree->root);
    // printf("[insertBST] Compare function: %p\n", (void*)tree->cmp);

  // insertion
  tree->root = insertNode(tree->root,data,tree->cmp);

  // printf("[insertBST] Insertion completed\n");
  return 1;
}

// inorder traversal
void inorderTraversal(TreeNode*root,PrintFunc print) {
  if(root == NULL) return;

  inorderTraversal(root->left,print);   //left
  print(root->data);                    //current
  inorderTraversal(root->right,print);  //right
}

// search
void * searchBST(BinaryTree *tree, const void* data) {
  if(!tree||!tree->root||!data) return NULL;

  TreeNode *current= tree->root;
  while (current)
  {
    int cmp = tree->cmp(data,current->data);
    if(cmp == 0) {
      return current->data;
    }
    current = (cmp<0) ? current->left : current->right;
  }
  return NULL;
  
}

// count nodes
int countNodes(TreeNode *root) {
  if(root==NULL) return 0;
  return 1+countNodes(root->left) +countNodes(root->right);
}

// destroy tree
void destroyTree(TreeNode *root,FreeFunc destroy) {
  if(root == NULL) return;

  destroyTree(root->left,destroy); // left
  destroyTree(root->right,destroy); // right

  if(destroy) 
    destroy(root->data);

  free(root);
}

// init BTS
BinaryTree initTree(CompareFunc cmp,PrintFunc print,FreeFunc destroy) {
  // printf("[initTree] Setting cmp: %p, print: %p, destroy: %p\n", cmp, print, destroy);

  BinaryTree tree;
  tree.root = NULL;
  tree.cmp = cmp;
  tree.print = print;
  tree.destroy = destroy;
  return tree;
}


static void inorderNode(TreeNode *node, void (*printFunc)(const void *)) {
    if (!node) return;
    inorderNode(node->left, printFunc);
    printFunc(node->data);
    inorderNode(node->right, printFunc);
}

void inorderBST(BinaryTree *tree, void (*printFunc)(const void *)) {
    if (!tree || !printFunc) return;
    inorderNode(tree->root, printFunc);
}