#include <stdlib.h>
#include <stdio.h>

#include "BaseTree.h"

/* Method predeclarations */
/* Memory management */
NODE AllocNode(TREE pTree);
void FreeNode(NODE pNode);

/* Small pieces of useful functionality */
void DebugTree(TREE pTree);
void DebugNode(NODE pNode);
void AttachNodes(NODE pParent, NODE pChild);
void DetachNodes(NODE pParent, NODE pChild);

/* Internal functionality for basic tree operations */
NODE SearchNode(int nKey, TREE pTree);
unsigned char InsertNode(NODE pNode, TREE pTree);
unsigned char RemoveNode(NODE pNode, TREE pTree, unsigned char ucOriginal);

/* Used for traversal */
NODE GetFirst(TREE pTree);
NODE GetLast(TREE pTree);
NODE GetNext(NODE pNode);
NODE GetPrevious(NODE pNode);

/*
 * Allocate a tree structure and initialize all of its
 * internal members.
 */
TREE AllocTree(AllocTreeCallback cbAllocTree)
{
    TREE pTree;

    pTree = (TREE)malloc(sizeof(*pTree));
    pTree->pRoot = NULL;
    pTree->pFirst = NULL;
    pTree->nSize = 0;
    pTree->nIterators = 0;

    pTree->cbAllocNode = NULL;
    pTree->cbInsert = NULL;
    pTree->cbRemove = NULL;
    pTree->cbRotation = NULL;
    pTree->cbDebugTree = NULL;
    pTree->cbDebugNode = NULL;

    if (cbAllocTree != NULL)
    {
        pTree->pAuxiliary = cbAllocTree(pTree);
    }
    else
    {
        pTree->pAuxiliary = NULL;
    }

    return pTree;
}

/*
 * Try to free all memory used by a tree, but only if no
 * iterators are currently attached.
 */
unsigned char FreeTree(TREE pTree)
{
    if (pTree->nIterators > 0)
    {
        return FALSE;
    }

    while(pTree->nSize > 0)
    {
        RemoveNode(pTree->pFirst, pTree, TRUE);
    }

    free(pTree);
    return TRUE;
}

/* 
 * Allocate a node structure and all of its internal
 * members.
 */
NODE AllocNode(TREE pTree)
{
    NODE pNode;

    pNode = (NODE)malloc(sizeof(*pNode));
    pNode->pLeftChild = NULL;
    pNode->pRightChild = NULL;
    pNode->pParent = NULL;
    pNode->nKey = 0;
    pNode->pContent = NULL;

    if (pTree->cbAllocNode != NULL)
    {
        pNode->pAuxiliary = pTree->cbAllocNode(pNode);
    }
    else
    {
        pNode->pAuxiliary = NULL;
    }

    return pNode;
}

/*
 * Free a node's memory.
 */
void FreeNode(NODE pNode)
{
    free(pNode);
}

/*
 * Allocate an iterator structure and all of its internal
 * members.
 */
ITERATOR AllocIterator()
{
    ITERATOR pIter;

    pIter = (ITERATOR)malloc(sizeof(*pIter));
    pIter->pTree = NULL;
    pIter->pNode = NULL;
    pIter->ucDirection = FORWARD;

    return pIter;
}

/*
 * Free an iterator's memory.
 */
void FreeIterator(ITERATOR pIter)
{
    free(pIter);
}

/*
 * Print out debug information about a tree.
 */
void DebugTree(TREE pTree)
{
    printf("---- DEBUG TREE ----\n");
    printf("Root key: %d\n", pTree->pRoot);
    printf("First key: %d\n", pTree->pFirst);
    printf("Size: %d\n", pTree->nSize);
    printf("Iterator count: %d\n", pTree->nIterators);

    if (pTree->cbDebugTree != NULL)
    {
        pTree->cbDebugTree(pTree);
    }

    printf("\n");
}

/*
 * Print out debug information about a node.
 */
void DebugNode(NODE pNode)
{
    printf("---- DEBUG NODE ----\n");
    printf("Key: %d\n", pNode->nKey);
    printf("Left child key: %d\n", pNode->pLeftChild == NULL ? 0 : pNode->pLeftChild->nKey);
    printf("Right child key: %d\n", pNode->pRightChild == NULL ? 0 : pNode->pRightChild->nKey);
    printf("Parent key: %d\n", pNode->pParent == NULL ? 0 : pNode->pParent->nKey);

    if (pNode->pTree->cbDebugNode != NULL)
    {
        pNode->pTree->cbDebugNode(pNode);
    }

    printf("\n");
}

/*
 * Return the grandparent of a node, if it exists.
 * Returns NULL otherwise.
 */
NODE Grandparent(NODE pNode)
{
    if (pNode != NULL && pNode->pParent != NULL)
    {
        return pNode->pParent->pParent;
    }
    return NULL;
}

/*
 * Return the sibling of a node, if it exists.
 * Returns NULL otherwise.
 */
NODE Sibling(NODE pNode)
{
    if (pNode != NULL && pNode->pParent != NULL)
    {
        return pNode->nKey < pNode->pParent->nKey ? pNode->pParent->pRightChild : pNode->pParent->pLeftChild;
    }
    return NULL;
}

/*
 * Return the uncle of a node, if it exists.
 * Returns NULL otherwise.
 */
NODE Uncle(NODE pNode)
{
    if (pNode != NULL)
    {
        return Sibling(pNode->pParent);
    }
    return NULL;
}

/*
 * Establishes all links between two nodes based on
 * key relativity. Potentially destructive to current
 * links if need to be overwritten. Updates tree pFirst
 * pointer if necessary.
 */
void AttachNodes(NODE pParent, NODE pChild)
{
    if (pChild == NULL)
    {
        return;
    }

    pChild->pParent = pParent;

    if (pParent == NULL)
    {
        pChild->pTree->pRoot = pChild;
        if (pChild->pTree->pFirst == NULL)
        {
            pChild->pTree->pFirst = pChild;
        }
        return;
    }

    if (pChild->nKey < pParent->nKey)
    {
        pParent->pLeftChild = pChild;
        if (pParent->pTree->pFirst == pParent)
        {
            pParent->pTree->pFirst = pChild;
        }
    }
    else
    {
        pParent->pRightChild = pChild;
    }
}

/*
 * Removes the links between two nodes based on
 * key relativity.
 */
void DetachNodes(NODE pParent, NODE pChild)
{
    if (pChild->nKey < pParent->nKey)
    {
        pParent->pLeftChild = NULL;
    }
    else
    {
        pParent->pRightChild = NULL;
    }

    pChild->pParent = NULL;
}

/*
 * Performs a left rotation on the given node.
 */
NODE LeftRotation(NODE pNode)
{
    NODE pChildNode = pNode->pRightChild;
    pNode->pRightChild = NULL;
    AttachNodes(pNode, pChildNode->pLeftChild);
    AttachNodes(pNode->pParent, pChildNode);
    AttachNodes(pChildNode, pNode);

    if (pNode->pTree->cbRotation != NULL)
    {
        pNode->pTree->cbRotation(pNode);
    }

    return pChildNode;
}

/*
 * Performs a right rotation on the given node.
 */
NODE RightRotation(NODE pNode)
{
    NODE pChildNode = pNode->pLeftChild;
    pNode->pLeftChild = NULL;
    AttachNodes(pNode, pChildNode->pRightChild);
    AttachNodes(pNode->pParent, pChildNode);
    AttachNodes(pChildNode, pNode);

    if (pNode->pTree->cbRotation != NULL)
    {
        pNode->pTree->cbRotation(pNode);
    }

    return pChildNode;
}

/*
 * Search for the appropriate node and return its contents.
 */
void *Search(int nKey, TREE pTree)
{
    void *pContent = NULL;
    NODE pNode = SearchNode(nKey, pTree);
    if (pNode != NULL)
    {
        pContent = pNode->pContent;
    }
    return pContent;
}

/*
 * Search for the appropriate node and return it.
 */
NODE SearchNode(int nKey, TREE pTree)
{
    NODE pNode = pTree->pRoot;

    while (pNode != NULL)
    {
        if (nKey < pNode->nKey)
        {
            pNode = pNode->pLeftChild;
        }
        else if (nKey > pNode->nKey)
        {
            pNode = pNode->pRightChild;
        }
        else
        {
            return pNode;
        }
    }

    return NULL;
}

/*
 * Insert a new node at the appropriate location in the tree.
 * Insertion fails if the key already exists.
 */
unsigned char Insert(int nKey, void *pContent, TREE pTree)
{
    NODE pNode;
    unsigned char ucResponse;
    pNode = AllocNode(pTree);
    pNode->nKey = nKey;
    pNode->pContent = pContent;
    pNode->pTree = pTree;

    ucResponse = InsertNode(pNode, pTree);
    if (ucResponse == FALSE)
    {
        FreeNode(pNode);
    }
    return ucResponse;
}

/*
 * Insert a new node at the appropriate location in the tree.
 * Insertion fails if the key already exists.
 */
unsigned char InsertNode(NODE pNode, TREE pTree)
{
    NODE pCurrNode = pTree->pRoot, pPrevNode = NULL;

    while (pCurrNode != NULL)
    {
        pPrevNode = pCurrNode;
        if (pNode->nKey < pCurrNode->nKey)
        {
            pCurrNode = pCurrNode->pLeftChild;
        }
        else if (pNode->nKey > pCurrNode->nKey)
        {
            pCurrNode = pCurrNode->pRightChild;
        }
        else
        {
            return FALSE;
        }
    }

    AttachNodes(pPrevNode, pNode);
    pTree->nSize++;

    if (pTree->cbInsert != NULL)
    {
        pTree->cbInsert(pNode);
    }

    return TRUE;
}

/*
 * Remove a node from the tree.
 */
unsigned char Remove(int nKey, TREE pTree)
{
    NODE pNode = SearchNode(nKey, pTree);
    unsigned char ucResponse = RemoveNode(pNode, pTree, TRUE);

    if (ucResponse == TRUE)
    {
        pTree->nSize--;
    }

    return ucResponse;
}

/*
 * Remove a node from the tree. ucOriginal designates whether this is the
 * original call to the method, or whether it is a subsequent internally
 * called removal (for cases when a removed node has two child nodes).
 */
unsigned char RemoveNode(NODE pNode, TREE pTree, unsigned char ucOriginal)
{
    NODE pOtherNode = NULL, pChildNode;
    if (pNode == NULL)
    {
        return FALSE;
    }

    if (pTree->pFirst == pNode)
    {
        pTree->pFirst = GetNext(pNode);
    }

    if (pNode->pLeftChild == NULL || pNode->pRightChild == NULL)
    {
        pChildNode = pNode->pLeftChild == NULL ? pNode->pRightChild : pNode->pLeftChild;
        if (pNode->pParent == NULL)
        {
            pTree->pRoot = pChildNode;
            if (pChildNode != NULL)
            {
                pChildNode->pParent = NULL;
                pOtherNode = pChildNode;
            }
        }
        else
        {
            pOtherNode = pNode->pParent;
            DetachNodes(pOtherNode, pNode);
            AttachNodes(pOtherNode, pChildNode);
        }
    }
    else
    {
        pOtherNode = GetPrevious(pNode);
        RemoveNode(pOtherNode, pTree, FALSE);
        AttachNodes(pNode->pParent, pOtherNode);
        AttachNodes(pOtherNode, pNode->pLeftChild);
        AttachNodes(pOtherNode, pNode->pRightChild);
    }

    FreeNode(pNode);

    if (pTree->cbRemove != NULL)
    {
        pTree->cbRemove(pOtherNode, ucOriginal);
    }

    return TRUE;
}

/*
 * Return the first node in the tree.
 */
NODE GetFirst(TREE pTree)
{
    return pTree->pFirst;
}

/*
 * Return the last node in the tree.
 */
NODE GetLast(TREE pTree)
{
    NODE pNode = pTree->pRoot;
    while (pNode != NULL && pNode->pRightChild != NULL)
    {
        pNode = pNode->pRightChild;
    }
    return pNode;
}

/*
 * Return the next node in the tree.
 */
NODE GetNext(NODE pNode)
{
    int nPrevKey = pNode->nKey;
    if (pNode->pRightChild != NULL)
    {
        pNode = pNode->pRightChild;
        while (pNode->pLeftChild != NULL)
        {
            pNode = pNode->pLeftChild;
        }
        return pNode;
    }
    else
    {
        pNode = pNode->pParent;
        while (pNode != NULL && pNode->nKey < nPrevKey)
        {
            nPrevKey = pNode->nKey;
            pNode = pNode->pParent;
        }
        return pNode;
    }
}

/*
 * Return the previous node in the tree.
 */
NODE GetPrevious(NODE pNode)
{
    int nPrevKey = pNode->nKey;
    if (pNode->pLeftChild != NULL)
    {
        pNode = pNode->pLeftChild;
        while (pNode->pRightChild != NULL)
        {
            pNode = pNode->pRightChild;
        }
        return pNode;
    }
    else
    {
        pNode = pNode->pParent;
        while (pNode != NULL && pNode->nKey > nPrevKey)
        {
            nPrevKey = pNode->nKey;
            pNode = pNode->pParent;
        }
        return pNode;
    }
}

/*
 * Attach an iterator to one end of the tree depending
 * on the direction of the iterator.
 */
void Attach(ITERATOR pIter, TREE pTree)
{
    pIter->pTree = pTree;
    if (pIter->ucDirection == FORWARD)
    {
        pIter->pNode = GetFirst(pTree);
    }
    else
    {
        pIter->pNode = GetLast(pTree);
    }
    pTree->nIterators++;
}

/*
 * Attach an iterator to the end of the tree.
 */
void AttachEnd(ITERATOR pIter, TREE pTree)
{
    pIter->ucDirection = BACKWARD;
    Attach(pIter, pTree);
}

/*
 * Return the contents of the current location of the
 * iterator.
 */
void *Current(ITERATOR pIter)
{
    void *pContent = NULL;
    if (pIter->pNode != NULL)
    {
        DebugNode(pIter->pNode);
        pContent = pIter->pNode->pContent;
    }
    return pContent;
}

/*
 * Return the contents of the current location of the
 * iterator and advance the iterator to the next node
 * depending on iterator direction.
 */
void *Next(ITERATOR pIter)
{
    void *pContent = Current(pIter);

    if (pIter->pNode != NULL)
    {
        if (pIter->ucDirection == FORWARD)
        {
            pIter->pNode = GetNext(pIter->pNode);
        }
        else
        {
            pIter->pNode = GetPrevious(pIter->pNode);
        }
    }
    
    return pContent;
}

/*
 * Detach the iterator from the tree.
 */
void Detach(ITERATOR pIter)
{
    if (pIter->pTree != NULL)
    {
        pIter->pTree->nIterators--;
        pIter->pTree = NULL;
    }
}