#include "BinaryTree.h"
#include "ErrorHandler.h"

// static functions
static TreeNode* createNode(void* data)
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

static void inorderNode(TreeNode *node, void (*printFunc)(const void *)) {
    if (!node) return;
    inorderNode(node->left, printFunc);
    printFunc(node->data);
    inorderNode(node->right, printFunc);
}


// public functions

BinaryTree initTree(CompareFunc cmp,PrintFunc print,FreeFunc destroy) {

  BinaryTree tree;
  tree.root = NULL;
  tree.cmp = cmp;
  tree.print = print;
  tree.destroy = destroy;
  return tree;
}

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

int insertBST(BinaryTree *tree,void *data) {
  if(tree==NULL) {
    displayError(ERR_LOADING_DATA,"[insertBST]:NULL tree pointer");
    return 0;
  }

  // insertion
  tree->root = insertNode(tree->root,data,tree->cmp);
  return 1;
}

int countNodes(TreeNode *root) {
  if(root==NULL) return 0;
  return 1+countNodes(root->left) +countNodes(root->right);
}

void destroyTree(TreeNode *root,FreeFunc destroy) {
  if(root == NULL) return;

  // printf("[DEBUG] visiting node: %p\n", (void*)root);
  // printf("[DEBUG] root->left: %p, root->right: %p\n", (void*)root->left, (void*)root->right);

  destroyTree(root->left,destroy); // left

  destroyTree(root->right,destroy); // right

  if(destroy) 
    // printf("[DEBUG] destroying car\n");
    destroy(root->data);


  // printf("[DEBUG] freeing node\n");
  free(root);
}

void * searchBST(BinaryTree *tree, const void* key) {
  if(!tree||!tree->root||!key) return NULL;
  
  TreeNode *current= tree->root;

  while (current)
  {
    int cmp = tree->cmp(key,current->data);
    if(cmp == 0) {
      return current->data;
    }
    current = (cmp<0) ? current->left : current->right;
  }
  return NULL;
  
}

void inorderBST(BinaryTree *tree, void (*printFunc)(const void *)) {
    if (!tree || !printFunc) return;
    inorderNode(tree->root, printFunc);
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

TreeNode* deleteNode(TreeNode* root, const void* key,CompareFunc cmp,FreeFunc destroy){
  if(!root) return NULL;
  int res = cmp(key,root->data);

  if(res < 0) {
    root->left = deleteNode(root->left,key,cmp,destroy);
  } else if (res > 0) {
    root->right = deleteNode(root->right,key,cmp,destroy);
  } else {
    // FOUND NODE TO REMOVE
    // node with 1 children or no children
    if(!root->left){
      TreeNode* tmp = root->right;
      if(destroy) destroy(root->data);
      free(root);
      return tmp;
    }

    // node with 2 children
    TreeNode* succ = root->right;
    while (succ->left)
    {
      succ = succ->left;
    }

    // copy data into current
    void* tmpData = root->data;
    root->data = succ->data;
    succ->data = tmpData;

    root->right = deleteNode(root->right,succ->data,cmp,destroy);
    

  }

  return root;
}

int deleteBST(BinaryTree* tree,const void* key){
  if(!tree || !tree->root) return 0;

  tree->root = deleteNode(tree->root,key,tree->cmp,tree->destroy);
  return 1;
}

