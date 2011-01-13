#include <stdlib.h>
#include <stdio.h>

#include "BinarySearchTree.h"
#include "AVL.h"

/* Method predeclarations */
/* Memory management */
void *AVLAllocNode(NODE pNode);

/* AVL height and balancing */
int AVLGetHeight(NODE pNode);
void AVLSetHeight(NODE pNode);
int AVLBalanceFactor(NODE pNode);
void AVLRotationSetHeights(NODE pNode);
void AVLBalance(NODE pNode);
void AVLRemoveBalance(NODE pNode, unsigned char ucOriginal);

/*
 * Allocate an AVL node and return it to be attached to a tree Node.
 */
void *AVLAllocNode(NODE pNode)
{
    AVLNODE pAVLNode;

    pAVLNode = (AVLNODE)malloc(sizeof(*pAVLNode));
    pAVLNode->nHeight = 0;

    return pAVLNode;
}

/*
 * Return an AVL node's height, -1 if the node is NULL.
 */
int AVLGetHeight(NODE pNode)
{
    return (pNode != NULL ? ((AVLNODE)pNode->pAuxiliary)->nHeight : -1);
}

/*
 * Set an AVL node's height as the max of its two children's height plus one.
 */
void AVLSetHeight(NODE pNode)
{
    int nLeftHeight = AVLGetHeight(pNode->pLeftChild);
    int nRightHeight = AVLGetHeight(pNode->pRightChild);
    ((AVLNODE)pNode->pAuxiliary)->nHeight = (nLeftHeight > nRightHeight ? nLeftHeight : nRightHeight) + 1;
}

/*
 * Return the balance factor of an AVL node (left child height minus right child height).
 */
int AVLBalanceFactor(NODE pNode)
{
    return AVLGetHeight(pNode->pLeftChild) - AVLGetHeight(pNode->pRightChild);
}

/*
 * Set the node and node's parent's height after rotation.
 */
void AVLRotationSetHeights(NODE pNode)
{
    AVLSetHeight(pNode);
    if (pNode->pParent != NULL)
    {
        AVLSetHeight(pNode->pParent);
    }
}

/*
 * Balance the AVL tree, performing rotations if necessary.
 */
void AVLBalance(NODE pNode)
{
    int nBalance = 0;
    int nChildBalance = 0;
    while (pNode != NULL)
    {
        AVLSetHeight(pNode);
        nBalance = AVLBalanceFactor(pNode);

        if (nBalance > 1) /* Heavy on left */
        {
            nChildBalance = AVLBalanceFactor(pNode->pLeftChild);
            if (nChildBalance > -1) /* right rotation */
                pNode = RightRotation(pNode);
            else /* double right rotation = left then right */
            {
                LeftRotation(pNode->pLeftChild);
                pNode = RightRotation(pNode);
            }
        }
        else if (nBalance < -1) /* Heavy on right */
        {
            nChildBalance = AVLBalanceFactor(pNode->pRightChild);
            if (nChildBalance < 1) /* left rotation */
                pNode = LeftRotation(pNode);
            else /* double left rotation = right then left */
            {
                RightRotation(pNode->pRightChild);
                pNode = LeftRotation(pNode);
            }
        }
        pNode = pNode->pParent;
    }
}

/*
 * Balance the AVL tree after removal (same as AVLBalance).
 */
void AVLRemoveBalance(NODE pNode, unsigned char ucOriginal)
{
    AVLBalance(pNode);
}

/*
 * Allocate a new tree and establish the appropriate callbacks.
 */
TREE AVLAllocTree()
{
    TREE pTree = AllocTree(NULL);
    pTree->cbAllocNode = AVLAllocNode;
    pTree->cbInsert = AVLBalance;
    pTree->cbRemove = AVLRemoveBalance;
    pTree->cbRotation = AVLRotationSetHeights;
    return pTree;
}