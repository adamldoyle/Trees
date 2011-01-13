/*
 * Implementation of an AVL tree as an extension to BinarySearchTree.
 *
 * Adam Doyle
 */

#ifndef __AVL_H__
#define __AVL_H__

#include "BinarySearchTree.h"


/* Represents extra node data required for AVL */
typedef struct AVLNode
{
    short int nHeight;
} *AVLNODE;


/* Allocate AVL tree */
TREE AVLAllocTree(void);

#endif /* __AVL_H__ */