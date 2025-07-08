#include "BinaryTree.h"
#include "ErrorHandler.h"



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
  if(root == NULL) {
    // if in a leaf
    TreeNode *node = createNode(data);

    return node;
  }

  // compare newNode data with current node 
  int result = cmp(data,root->data);

  // insert into left
  if(result < 0) {

    root->left = insertNode(root->left,data,cmp);}

  else if(result > 0) {
// insert into right
    root->right = insertNode(root->right,data,cmp);
  }
  return root;
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


// public insert a node
int insertBST(BinaryTree *tree,void *data) {
  if(tree==NULL) {
    displayError(ERR_LOADING_DATA,"[insertBST]:NULL tree pointer");
    return 0;
  }

  // insertion
  tree->root = insertNode(tree->root,data,tree->cmp);
  return 1;
}


void* findMaxData(TreeNode* root, CompareFunc cmp) {
  if(!root||!cmp) return NULL;

  void * maxData = root->data;
  void* maxLeft = findMaxData(root->left,cmp);

  if(maxLeft&&cmp(maxLeft,maxData)>0){
    maxData =maxLeft;
  }

  void* maxRight = findMaxData(root->right,cmp);
  if(maxRight&&cmp(maxRight,maxData)>0){
    maxData =maxRight;
  }

  return maxData;
}
// search
// void * searchBST(BinaryTree *tree, const void* data) {
//   if(!tree||!tree->root||!data) return NULL;
  
//   TreeNode *current= tree->root;

//   while (current)
//   {
//     int cmp = tree->cmp(data,current->data);
//     if(cmp == 0) {
//       return current->data;
//     }
//     current = (cmp<0) ? current->left : current->right;
//   }
//   return NULL;
  
// }
