#include <stdlib.h>
#include <stdio.h>

#include "BaseTree.h"

#ifdef WIN32
#define BTSCAN scanf_s
#else
#define BTSCAN scanf
#endif

int main(int argc, char** argv)
{
    TREE pTree = AllocTree(NULL);
    void *pContents;
    ITERATOR pIter;

    int nChoice = 0, nKey;

    while (nChoice != -1)
    {
        printf("1: Insert\n");
        printf("2: Remove\n");
        printf("3: Print\n");
        printf("4: Print all keys\n");
        printf("5: Print all keys reverse\n");
        printf("6: Mass insert\n");
        printf("7: Mass remove\n");
        printf("Else: Quit\n\n");
        
        BTSCAN("%d", &nChoice);

        switch(nChoice)
        {
        case 1:
            printf("Key: ");
            BTSCAN("%d", &nKey);
            Insert(nKey, "temp", pTree);
            break;
        case 2:
            printf("Key: ");
            BTSCAN("%d", &nKey);
            Remove(nKey, pTree);
            break;
        case 3:
            printf("Key: " );
            BTSCAN("%d", &nKey);
            pContents = Search(nKey, pTree);
            if (pContents != NULL)
            {
                printf("Contents: %s\n", pContents);
            }
            else
            {
                printf("Not found\n");
            }
            break;
        case 4:
            pIter = AllocIterator();
            Attach(pIter, pTree);

            while ((pContents = Next(pIter)) != NULL)
            {
                printf("Contents: %d\n", pContents);
            }

            Detach(pIter);
            FreeIterator(pIter);
            break;
        case 5:
            pIter = AllocIterator();
            AttachEnd(pIter, pTree);

            while ((pContents = Next(pIter)) != NULL)
            {
                printf("Contents: %d\n", pContents);
            }

            Detach(pIter);
            FreeIterator(pIter);
            break;
        case 6:
            for (nKey = 100; nKey > 0; nKey--)
            {
                Insert(nKey, "temp", pTree);
            }
            break;
        case 7:
            for (nKey = 100; nKey > 0; nKey--)
            {
                Remove(nKey, pTree);
            }
            break;
        default:
            nChoice = -1;
            break;
        }
        printf("\n");
    }
    

    return 0;
}