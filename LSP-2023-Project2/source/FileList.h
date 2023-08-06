#ifndef __FILELIST_H__
#define __FILELIST_H__


// 틀린 문제 관리 링크드리스트 노드
typedef struct WrongNode
{
    char wrongProblem[FILELEN];
    double score;
    
    struct WrongNode *nextNode;

} WrongNode;

// 채점 결과 정렬 링크드리스트 노드
typedef struct SortNode
{
    char studentId[10];
    char studentResult[BUFLEN];
    char studentScore[FILELEN];

    struct SortNode *nextNode;
    struct SortNode *previousNode;

} SortNode;


#endif