/*********************************************************************************
 * FILENAME         distribution.h
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
 * LAST CHANGE      17. NOV 2014
 *
 ********************************************************************************/

 #ifndef DISTRIBUTION_H
 #define DISTRIBUTION_H

typedef struct sElement {
    int iEmpty;
    int iRowBegin;
    int iRowEnd;
    int iColBegin;
    int iColEnd;

} sElement;

typedef struct sTree {
    int iTreeSize;
    int iHeight;
    sElement **ppElement;
} sTree;

typedef struct sJob {
    int iJob;
    int iRowBegin;
    int iRowEnd;
    int iColBegin;
    int iColEnd;
} sJob;

typedef struct sJobList {
    int iTotalJobs;
    sJob **ppJob;
} sJobList;

/*
* DESCRIPTION - Distributes the output matrix' area
*               to a certain number by using a tree
*               structure. Let's say you have a Matrix
*               and you want to distribute the elements
*               to three processes. In the sketch '#'
*               symbolizes a matrix element.
*                   
*               INPUT
*               |−−−−−−−−−−−−−−−−−−−−−−−|
*               | # # # # # # # # # # # |
*               | # # # # # # # # # # # |   
*               | # # # # # # # # # # # |
*               | # # # # # # # # # # # |
*               | # # # # # # # # # # # |
*               |−−−−−−−−−−−−−−−−−−−−−−−|
*
*               OUTPUT
*               |−−−−−−−−−−−−−−−−−−−−−−−−−|
*               | # # # # # |             |
*               | # # # # # | # # # # # # |   
*               |−−−−−−−−−−−| # # # # # # |
*               |           | # # # # # # |
*               | # # # # # | # # # # # # |
*               | # # # # # | # # # # # # |
*               | # # # # # |             |
*               |−−−−−−−−−−−−−−−−−−−−−−−−−|
*               
*               if you want to have 4 processes of course
*               in the next step the right big part is divided
*               into two new fields.
*               
* PARAMETER   - integer: #rows 
*             - integer: #columns
*             - sJobList*: must contain information about
*               how many jobs you want to have. Output
*               is written here.
* RETURN      - void
*/
void vDistributeOutputMatrix(int, int, sJobList*);

/*
* DESCRIPTION - counts the needed tree height for
*               a given job count. Used by vDistributeOutputMatrix
* PARAMETER   - integer: job count
* RETURN      - integer: tree height
*/
int iTreeHeight(int);

/*
* DESCRIPTION - takes the finished tree and searches for the leafs.
*               the leafs are transformed to the sJobList format.
* PARAMETER   - sTree*: the tree
*             - sJobList*: output written here
* RETURN      - void
*/
void vLeafs2Jobs(sTree*, sJobList*);

/*
* DESCRIPTION - count the leafs of a tree
* PARAMETER   - sTree*: the tree
* RETURN      - integer: number of leafs
*/
int iCountLeafs(sTree*);

/*
* DESCRIPTION - divides one leaf of the tree into two
*               new leafs by keeping the tree perfectly
*               balanced. Furthermore the leaf's elements
*               are updated that the elements of the output
*               matrix are well distributed.
* PARAMETER   - sTree*: the tree
* RETURN      - void
*/
void vDivideLeaf(sTree*);

/*
* DESCRIPTION - Allocates the memory for a JobList with 
*               a number of jobs.
* PARAMETER   - integer: job count
*             - sJobList*: the sJobList* Object for allocation
* RETURN      - integer: 1 allocation failed. 0 allocation successful 
* USE         - sJobList myJobList;
*               int jobCount = 10; // e.g.
*               vAllocateJobList(jobCount, &myJobList);
*/
int vAllocateJobList(int, sJobList*);

/*
* DESCRIPTION - Free the memory of a JobList
* PARAMETER   - sJobList*: the Job List
* RETURN      - void
*/
void vFreeJobList(sJobList*);

/*
* DESCRIPTION - Prints a job List
* PARAMETER   - sJobList*: the job list
* RETURN      - void
*/
void vPrintJobList(sJobList*);
 #endif
