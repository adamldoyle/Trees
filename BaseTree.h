/*
 * Basic implementation of a binary search tree with appropriate callbacks
 * to allow for extension into specific tree types (e.g. AVL).
 *
 * Adam Doyle
 */

#ifndef __BASETREE_H__
#define __BASETREE_H__


/* Boolean */
#define TRUE 1
#define FALSE 0

/* Iterator direction */
#define FORWARD 0
#define BACKWARD 1


/* Predeclarations */
struct Tree;
struct Node;


/* Definitions for callbacks that can be defined for the tree */
/* Called after tree allocation, accepts the new tree and must return the tree's auxiliary object */
typedef void *(*AllocTreeCallback)(struct Tree *pTree);
/* Called after node allocation, accepts the new node and must return the node's auxiliary object */
typedef void *(*AllocNodeCallback)(struct Node *pNode);
/* Called after a successful insertion of the given node */
typedef void (*InsertCallback)(struct Node *pNode);
/* Called after a successful removal of the given node, passing in the replacement node */
typedef void (*RemoveCallback)(struct Node *pNode, unsigned char ucOriginal);
/* Called after rotating on the given node */
typedef void (*RotationCallback)(struct Node *pNode);
/* Called after debug printing a tree */
typedef void (*DebugTreeCallback)(struct Tree *pTree);
/* Called after debug printing a node */
typedef void (*DebugNodeCallback)(struct Node *pNode);


/* Represents a single, basic node of the tree */
typedef struct Node
{
    struct Tree *pTree;
    struct Node *pLeftChild;
    struct Node *pRightChild;
    struct Node *pParent;
    int nKey;
    void *pContent; /* Content payload */
    void *pAuxiliary; /* Optional auxiliary data for the node */
} *NODE;

/* Represents a single, basic tree */
typedef struct Tree
{
    NODE pRoot;
    NODE pFirst;
    int nSize;
    int nIterators; /* Number of current iterators attached */
    void *pAuxiliary; /* Optional auxiliary data for the tree */

    AllocNodeCallback cbAllocNode;
    InsertCallback cbInsert;
    RemoveCallback cbRemove;
    RotationCallback cbRotation;
    DebugTreeCallback cbDebugTree;
    DebugNodeCallback cbDebugNode;
} *TREE;

/* Represents an iterator for traversing the tree */
typedef struct Iterator
{
    TREE pTree;
    NODE pNode;
    unsigned char ucDirection;
} *ITERATOR;


/* Memory management */
TREE AllocTree(AllocTreeCallback cbAllocTree);
unsigned char FreeTree(TREE pTree);
ITERATOR AllocIterator(void);
void FreeIterator(ITERATOR pIter);

/* Small pieces of useful functionality */
NODE Grandparent(NODE pNode);
NODE Sibling(NODE pNode);
NODE Uncle(NODE pNode);
NODE RightRotation(NODE pNode);
NODE LeftRotation(NODE pNode);

/* Basic tree operations */
void *Search(int nKey, TREE pTree);
unsigned char Insert(int nKey, void *pContent, TREE pTree);
unsigned char Remove(int nKey, TREE pTree);

/* Iterator operations */
void Attach(ITERATOR pIter, TREE pTree);
void AttachEnd(ITERATOR pIter, TREE pTree);
void *Current(ITERATOR pIter);
void *Next(ITERATOR pIter);
void Detach(ITERATOR pIter);

#endif /* __BASETREE_H__ */