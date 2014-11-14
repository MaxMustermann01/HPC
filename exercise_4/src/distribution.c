/*********************************************************************************
 * FILENAME         distribution.c
 *
 * DESCRIPTION      These functions are part of the submission to exercises of 
 *                  the "Introduction to High Percformance Computing" (Intro HPC) 
 *                  lecture of the University of Heidelberg.
 * 
 *                  Exercise 4 - Parallel Matrix Multiplication
 * 
 * AUTHORS          Klaus Naumann
 *                  Christoph Klein
 *                  Günther Schindler
 *
 * LAST CHANGE      14. NOV 2014
 *
 ********************************************************************************/

#include "distribution.h"
#include <stdlib.h>
#include <stdio.h>

void vDistributeOutputMatrix(int iJobCount, int iRows, int iCols, sJobList *pJobList) {

    /* initialize tree */
    sTree *tree;

    tree = (sTree *) malloc(sizeof(sTree*)); 
    if (tree == NULL)
        exit(EXIT_FAILURE);
  
    /* set maximum height of the tree */
    tree->iHeight = iTreeHeight(iJobCount); 

    /* 
    * total number of elements in a perfect balanced tree
    * with height H is equal to 2^(H+1) - 1
    * This is realized by bitwise shift operator.
    */
    tree->iTreeSize = (1 << (tree->iHeight + 1)) - 1;

    /* allocation for the elements */
    tree->ppElement = (sElement **) malloc(tree->iTreeSize * sizeof(sElement **)); 
    if (tree->ppElement == NULL)
        exit(EXIT_FAILURE);

    /* initialization of the elements */
    int i;
    for (i = 0; i < tree->iTreeSize; i++) {

        /* allocation of elements */
        tree->ppElement[i] = (sElement *) malloc(sizeof(sElement *));
        if (tree->ppElement[i] == NULL)
            exit(EXIT_FAILURE);

        /* mark every element as empty */
        tree->ppElement[i]->iEmpty = 1;
    }
    
    /* mark first element */
    tree->ppElement[0]->iEmpty = 0;
    tree->ppElement[0]->iRowBegin = 0;
    tree->ppElement[0]->iRowEnd = iRows;
    tree->ppElement[0]->iColBegin = 0;
    tree->ppElement[0]->iColEnd = iCols;

    /* add nodes to the tree until #leafs == #iJobCount */
    while (iCountLeafs(tree) != iJobCount) {
        vDivideLeaf(tree);
    }
     
}

int iCountLeafs(sTree *pTree) {

    int iLeafs = 0;
    int i;
    int iEmp;
    int iChild;
    int iSize = pTree->iTreeSize;

    for (i = 0; i < iSize; i++) {
        iEmp = pTree->ppElement[i]->iEmpty;
        if (iEmpty) break;
        iChild = 2 * i + 1;
        if (iChild >= iSize) 
            iLeafs++;
        else if (iChild < iSize) {
            if (pTree->ppElement[iChild]->iEmpty)
                iLeafs++;
        }
    }
    
    return iLeafs;
}

void vDivideLeaf(sTree *pTree) {

    int iEmp = 0;
    int iFstEmp = 0;
    int iParent;
    int iBeginA, iBeginB, iEndA, iEndB;
    int iRowDiff, iColDiff;

    /* get index of first empty element */
    while (!iEmp) {

        if (iFstEmp >= pTree->iTreeSize)
            exit(EXIT_FAILURE);

        ++iFstEmp;
        iEmp = pTree->ppElement[iFstEmp]->iEmpty;
    }

    /* get parent of first empty index */
    iParent = (iFstEmp - 1) / 2;

    /* set children limits */
    iRowDiff = pTree->ppElement[iParent]->iRowEnd - pTree->ppElement[iParent]->iRowBegin;
    iColDiff = pTree->ppElement[iParten]->iColEnd - pTree->ppElement[iParent]->iColBegin;

    if (iRowDiff >= iColDiff) {
        /* divide the rows */
        iBeginA = pTree->ppElement[iParent]->iRowBegin;
        iEndA = iRowDiff/2 + pTree->ppElement[iParent]->iRowBegin;
        iBeginB = iEndA;
        iEndB = pTree->ppElement[iParent]->iRowEnd;

        /* set the values A */
        pTree->ppElement[iFstEmp]->isEmpty = 0;
        pTree->ppElement[iFstEmp]->iRowBegin = iBeginA;
        pTree->ppElement[iFstEmp]->iRowEnd = iEndA;
        pTree->ppElement[iFstEmp]->iColBegin = pTree->ppElement[iParent]->iColBegin;
        pTree->ppElement[iFstEmp]->iColEnd = pTree->ppElement[iParent]->iColEnd;

        /* set the values B */
        pTree->ppElement[iFstEmp+1]->isEmpty = 0;
        pTree->ppElement[iFstEmp+1]->iRowBegin = iBeginB;
        pTree->ppElement[iFstEmp+1]->iRowEnd = iEndB;
        pTree->ppElement[iFstEmp+1]->iColBegin = pTree->ppElement[iParent]->iColBegin;
        pTree->ppElement[iFstEmp+1]->iColEnd = pTree->ppElement[iParent]->iColEnd;
    }
    else {
        /* divide the columns */
        iBeginA = pTree->ppElement[iParent]->iColBegin;
        iEndA = iColDiff/2 + pTree->ppElement[iParent]->iColBegin;
        iBeginB = iEndA;
        iEndB = pTree->ppElement[iParent]->iColEnd;

        /* set the values A */
        pTree->ppElement[iFstEmp]->isEmpty = 0;
        pTree->ppElement[iFstEmp]->iColBegin = iBeginA;
        pTree->ppElement[iFstEmp]->iColEnd = iEndA;
        pTree->ppElement[iFstEmp]->iRowBegin = pTree->ppElement[iParent]->iRowBegin;
        pTree->ppElement[iFstEmp]->iRowEnd = pTree->ppElement[iParent]->iRowEnd;

        /* set the values B */
        pTree->ppElement[iFstEmp+1]->isEmpty = 0;
        pTree->ppElement[iFstEmp+1]->iColBegin = iBeginB;
        pTree->ppElement[iFstEmp+1]->iColEnd = iEndB;
        pTree->ppElement[iFstEmp+1]->iRowBegin = pTree->ppElement[iParent]->iRowBegin;
        pTree->ppElement[iFstEmp+1]->iRowEnd = pTree->ppElement[iParent]->iRowEnd;
    }
}

int iTreeHeight(int iJobCount) {
    
    int i = 1; // loop var
    int h = 0; // tree height
    while (i < iJobCount) {
        i = i << 1;
        ++h;
    }
    return h;
}
