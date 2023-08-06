//add header files

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>

#include "ssu_score.h"
#include "blank.h"
#include "FileList.h"

extern struct ssu_scoreTable score_table[QNUM];
extern char id_table[SNUM][10];

struct ssu_scoreTable score_table[QNUM]; // 점수 테이블 구조체
char id_table[SNUM][10]; // 답안을 제출한 학생들의 학번 테이블

char stuDir[BUFLEN]; // 학생 답안 디렉토리 경로
char ansDir[BUFLEN]; // 정답 디렉토리 경로
char errorDir[BUFLEN]; // 에러 메시지 출력 경로
char csvName[BUFLEN]; // 채점 결과 저장 csv 파일명
char threadFiles[ARGNUM][FILELEN]; // lpthread 옵션을 적용할 파일 목록
char iIDs[ARGNUM][FILELEN]; // 학번 입력 인자 목록
char category[BUFLEN]; // s 옵션 관련 인자 관리

int sortType = 0; // s 옵션 관련 정렬 타입

// 옵션 활성화 여부 플래그
int nOption = false;
int mOption = false;
int cOption = false;
int pOption = false;
int tOption = false;
int eOption = false;
int sOption = false;

int cpArgFlag = false;

WrongNode *WrongList; // 틀린 문제 관리 링크드리스트
SortNode *SortList; // 채점 결과 정렬 링크드리스트

int WrongListCnt = 0;
int StudentTotal = 0;

void (*Sort_List)(int nodeCnt);
void (*do_pOption)(char *stdId, char (*ids)[FILELEN]);

int studentParams = 0;

int scoreTableCnt = 0;
int threadFileCnt = 0;

void ssu_score(int argc, char *argv[])
{
	char saved_path[BUFLEN];
	int i;

	FILE *fp;
	char scorePath[BUFLEN];
	char stdPath[MAX_PATH];
	char ansPath[MAX_PATH];
	struct stat statbuf;

	char path[MAX_PATH];

	int qCnt;
	int threadCnt;

	int tFlag;

	char resolvedPath[MAX_PATH];
	char temp[MAX_PATH];

	int c;


	// -h 옵션 입력 여부 검사
	// -h 옵션이 입력되었으면 usage 출력 후 종료
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			return;
		}
	}

	// -h 옵션 입력 여부 검사
	for(i = 0; i < argc; i++)
	{
		if(argv[i][0] == '-')
		{
			for(int k = 0; k < strlen(argv[i]); k++)
			{
				if(argv[i][k] == 'h')
				{
					print_usage();

					return;
				}
			}
		}
	}

	// 필수 입력 인자 검사
	if(argc < 3)
	{
		fprintf(stderr, "Need <STD_DIR> <ANS_DIR>>\n");

		return;
	}

	memset(saved_path, 0, BUFLEN);

	strcpy(stuDir, argv[1]);
	strcpy(ansDir, argv[2]);

	// 현재 작업 디렉토리 경로 얻기
	getcwd(saved_path, BUFLEN);


	memset(stdPath, 0, BUFLEN);
	memset(ansPath, 0, BUFLEN);
	

	realpath(stuDir, stdPath);
	realpath(ansDir, ansPath);

	// 학생 답안 디렉토리 stat 구조체 검사
	if(stat(stdPath, &statbuf) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", stdPath);

		return;
	}
	
	// 정답 디렉토리 stat 구조체 검사
	if(stat(ansPath, &statbuf) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", ansPath);

		return;
	}


	// 옵션 검사
	if(!check_option(argc, argv))
		exit(1);
	

	// 학생 답안 디렉토리로 이동
	if(chdir(stuDir) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN);			// 학상 답안 디렉토리에서의 절대경로 획득

	chdir(saved_path);				// 프로그램 실행 위치였던 saved_path로 이동
	
	// 정답 디렉토리로 이동
	if(chdir(ansDir) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN);			// 정답 디렉토리에서의 절대경로 획득

	chdir(saved_path);				// 프로그램 실행 위치였던 saved_path로 이동

	
	// c, p 옵션 실행
	if(cOption == true || pOption == true && strcmp(iIDs[0] , "") != 0)
	{
		chdir(stuDir);

		if(studentParams >= ARGNUM)
		{
			studentParams = ARGNUM;
		}

		for(int i = 0; i < studentParams; i++)
		{
			if(access(iIDs[i], F_OK) != 0)
			{
				fprintf(stderr, "%s doesn't exist in \"%s\"\n", iIDs[i], stuDir);

				return;
			}		
		}

		chdir(saved_path);
	}


	// m 옵션 실행
	if(mOption == true)
	{
		memset(path, 0, BUFLEN);

		snprintf(path, MAX_PATH, "%s/%s", ansDir, "score_table.csv");

		if(access(path, F_OK) < 0)
		{
			fprintf(stderr, "ScoreTable doesn't exist\n");
			
			return;
		}
	}

	set_scoreTable(ansDir); // 점수 테이블 준비
	set_idTable(stuDir); // 학번 테이블 준비

	// t 옵션 실행
	if(tOption == true && strcmp(threadFiles[0], "") != 0)
	{
		char tmp[FILELEN];

		char *strPtr;

		int i;
		int k;

		qCnt = scoreTableCnt;
		threadCnt = threadFileCnt;

		if(threadCnt >= ARGNUM)
		{
			threadCnt = ARGNUM;
		}

		tFlag = false;

		for(i = 0; i < threadCnt; i++)
		{
			tFlag = false;

			for(k = 0; k < qCnt; k++)
			{
				strcpy(tmp, score_table[k].qname);
				strPtr = strtok(tmp, ".");

				if(strcmp(threadFiles[i], strPtr) == 0)
				{
					tFlag = true;

					break;
				}
			}

			if(tFlag == false)
			{
				break;
			}
		}

		if(tFlag == false)
		{
			fprintf(stderr, "%s doesn't exist in %s\n", threadFiles[i], ansDir);

			return;
		}
	}

	// -m 옵션 플래그 검사
	if(mOption)
	{
		// 활성화 상태면 dp_mOption 실행
		if(do_mOption() == false)
		{
			return;
		}
	}

	printf("grading student's test papers..\n");

	// 채점 수행
	score_students();


	memset(resolvedPath, 0, MAX_PATH);
	memset(temp, 0, MAX_PATH);

	// n 옵션 관련 경로 생성
	if(nOption == true)
	{
		if(csvName[0] == '/')
		{
			snprintf(temp, MAX_PATH, "%s", csvName);
		}
		else
		{
			snprintf(temp, MAX_PATH, "%s/%s", saved_path, csvName);
		}
	}
	else
	{
		snprintf(temp, MAX_PATH, "%s/%s", ansDir, "score.csv");
	}

	realpath(temp, resolvedPath);

	printf("result saved.. (%s)\n", resolvedPath);

	if(eOption == true)
	{
		memset(resolvedPath, 0, MAX_PATH);

		realpath(errorDir, resolvedPath);

		printf("error saved.. (%s/)\n", resolvedPath);
	}

	return;
}

int check_option(int argc, char *argv[])
{
	int i, j, k;
	int c;
	int exist = 0;

	char *strPtr;
	char tmp[BUFLEN];
	char path[BUFLEN];
	struct stat statbuf;

	char newErrorDir[BUFLEN];
	char newFile[BUFLEN];
	char currentDir[BUFLEN];

	// getopt로 옵션 인자 추출
	while((c = getopt(argc, argv, "he:n:s:thmcp1")) != -1)		// 옵션은 e, n, s, t, h, m, c, p
	{
		switch(c){
			case 'h':

				print_usage();

				return false;
				
			break;

			case 'e':	// -e 옵션 입력된 경우

				if(eOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				eOption = true;		// -e 옵션 플래그 활성화
				strcpy(errorDir, optarg);	// 옵션 인자를 에러 메시지 출력 경로에 복사

				strcpy(tmp, errorDir);

				strcpy(path, tmp);

				// 경로 인자에 존재하지 않는 디렉토리가 있을 경우 디렉토리 생성
				strPtr = strrchr(tmp, '/');
				if(strPtr != NULL)
				{
					memset(newErrorDir, 0, BUFLEN);

					strcpy(tmp, path);
					
					if(tmp[0] == '/')
					{
						strcpy(newErrorDir, "/");
					}

					strPtr = strtok(tmp, "/");
					
					while(strPtr != NULL)
					{
						if(strcmp(newErrorDir, "") == 0)
							strcpy(newErrorDir, strPtr);
						else
							strcat(newErrorDir, strPtr);
						
						if(access(newErrorDir, F_OK) < 0)
						{
							mkdir(newErrorDir, 0755);
						}

						strcat(newErrorDir, "/");

						strPtr = strtok(NULL, "/");
					}
				}
				else
				{
					getcwd(currentDir, BUFLEN);

					strcat(currentDir, "/");
					strcat(currentDir, errorDir);

					strcpy(errorDir, currentDir);
				}


				// 옵션 인자로 들어온 디렉토리 경로가 존재하는지 검사
				if(access(errorDir, F_OK) < 0)	// 디렉토리 존재하지 않음
					mkdir(errorDir, 0755);	// 해당 경로에 디렉토리 생성, 권한은 0755
				else{	// 이미 디렉토리 존재함
					rmdirs(errorDir);	// 기존 디렉토리 삭제
					mkdir(errorDir, 0755);	// 해당 경로에 디렉토리 생성, 권한은 0755
				}
				break;
			case 'n':

				if(nOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				nOption = true;

				strcpy(csvName, optarg);

				strcpy(tmp, csvName);
				strcpy(path, csvName);

				// 입력 인자의 확장자 검사
				if(strncmp(&csvName[strlen(csvName) - strlen(".csv")], ".csv", strlen(".csv")) != 0)
				{
					fprintf(stderr, "%s is not csv file\n", csvName);

					return false;
				}

				// 경로 인자에 존재하지 않는 디렉토리가 있을 경우 디렉토리 생성
				strPtr = strrchr(tmp, '/');
				if(strPtr != NULL)
				{
					memset(newFile, 0, BUFLEN);
					
					tmp[strPtr - tmp] = '\0';

					if(tmp[0] == '/')
					{
						strcpy(newFile, "/");
					}

					strPtr = strtok(tmp, "/");
					
					while(strPtr != NULL)
					{
						if(strcmp(newFile, "") == 0)
							strcpy(newFile, strPtr);
						else
							strcat(newFile, strPtr);
						
						if(access(newFile, F_OK) < 0)
						{
							mkdir(newFile, 0755);
						}

						strcat(newFile, "/");

						strPtr = strtok(NULL, "/");
					}
				}

				break;
			case 's':

				if(sOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				sOption = true;

				strcpy(category, optarg);

				// s 옵션 관련 인자 검사
				if(strcmp(category, "1") == 0 || strcmp(category, "-1") == 0)
				{
					fprintf(stderr, "%s is not <CATEGORY>\n", category);

					return false;
				}

				if(strcmp(category, "stdid") != 0 && strcmp(category, "score") != 0)
				{
					fprintf(stderr, "%s is not <CATEGORY>\n", category);

					return false;
				}

				if(optind >= argc)
				{
					fprintf(stderr, "Need sorting type <1|-1>\n");

					return false;
				}

				sortType = atoi(argv[optind]);

				if(sortType != 1 && sortType != -1)
				{
					fprintf(stderr, "Your sorting type is not <1|-1>\n");

					return false;
				}

				if(strcmp(category, "stdid") == 0 && sortType == 1)
				{
					// 학번 기준 오름차순 정렬

					Sort_List = sort_id_ascending;
				}
				else if(strcmp(category, "stdid") == 0 && sortType == -1)
				{
					// 학번 기준 내림차순 정렬

					Sort_List = sort_id_descending;
				}
				else if(strcmp(category, "score") == 0 && sortType == 1)
				{
					// 점수 기준 오름차순 정렬

					Sort_List = sort_score_ascending;
				}
				else if(strcmp(category, "score") == 0 && sortType == -1)
				{
					// 점수 기준 내림차순 정렬

					Sort_List = sort_score_descending;
				}

				break;
			case 't':	// -t 옵션 입력된 경우

				if(tOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				tOption = true;		// -t 옵션 플래그 활성화
				i = optind;
				j = 0;

				while(i < argc && argv[i][0] != '-'){

					if(j == ARGNUM)
					{
						printf("Maximum Number of Argument Exceeded.  :: %s", argv[i]);
					}
					else if(j > ARGNUM)
					{
						printf(" %s", argv[i]);
					}
					else
					{
						strcpy(tmp, argv[i]);

						for(int k = 0; k < strlen(tmp); k++)
						{
							if(tmp[k] == '.')
							{
								tmp[k] = '\0';

								break;
							}
						}

						strcpy(threadFiles[j], tmp);
					}
					i++; 
					j++;
				}

				if(j > ARGNUM)
				{
					printf("\n");
				}

				threadFileCnt = j;

				break;
			case 'm':

				if(mOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				mOption = true;
				break;
			
			case 'c':

				if(cOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				cOption = true;
				i = optind;
				j = 0;

				// 가변인자 검사
				while(i < argc && argv[i][0] != '-'){
					if(j == ARGNUM)
					{
						printf("Maximum Number of Argument Exceeded. :: %s", argv[i]);
					}
					else if(j > ARGNUM)
					{
						printf(" %s", argv[i]);
					}
					else
					{
						if(cpArgFlag == true)
						{
							fprintf(stderr, "[STUDENTIDS ...] is already entered\n");

							return false;
						}

						if(cpArgFlag == false)
						{
							strcpy(iIDs[j], argv[i]);
						}
					}
					
					i++;
					j++;
				}

				if(j > ARGNUM)
				{
					printf("\n");
				}

				if(strcmp(iIDs[0], "") != 0)
				{
					cpArgFlag = true;
				}

				if(studentParams < 1)
				{
					studentParams = j;
				}

				break;

			case 'p':

				if(pOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}
				
				pOption = true;
				i = optind;
				j = 0;

				// 가변인자 검사
				while(i < argc && argv[i][0] != '-'){
					if(j == ARGNUM)
					{
						printf("Maximum Number of Argument Exceeded. :: %s", argv[i]);
					}
					else if(j > ARGNUM)
					{
						printf(" %s", argv[i]);
					}
					else
					{
						if(cpArgFlag == true)
						{
							fprintf(stderr, "[STUDENTIDS ...] is already entered\n");

							return false;
						}

						if(cpArgFlag == false)
						{
							strcpy(iIDs[j], argv[i]);
						}
					}

					i++;
					j++;
				}

				if(j > ARGNUM)
				{
					printf("\n");
				}

				if(strcmp(iIDs[0], "") != 0)
				{
					cpArgFlag = true;
				}

				if(studentParams < 1)
				{
					studentParams = j;
				}

				break;
			
			case '1':
				break;
			
			case '?':
				printf("Unkown option %c\n", optopt);
				return false;
		}
	}

	return true;
}

void do_cOption(FILE *fp, char (*ids)[FILELEN])
{
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j;
	char first, exist;

	double score;
	int num;

	// get qnames
	i = 0;
	fscanf(fp, "%s\n", tmp);
	strcpy(qname[i++], strtok(tmp, ","));
	
	while((p = strtok(NULL, ",")) != NULL)
		strcpy(qname[i++], p);

	// print result
	i = 0;
	while(i++ <= ARGNUM - 1)
	{
		exist = 0;
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%s\n", tmp);

		while(fscanf(fp, "%s\n", tmp) != EOF){
			id = strtok(tmp, ",");

			if(strcmp(ids[0], "") == 0)
			{
				exist = 1;
				j = 0;
				first = 0;
				while((p = strtok(NULL, ",")) != NULL){
					if(!strcmp(qname[j], "sum"))
					{
						printf("%s's score : %s", id, p);

						score += atof(p);
						num++;
					}
					j++;
				}
				printf("\n");
			}
			else
			{
				if(!strcmp(ids[i - 1], id)){
					exist = 1;
					j = 0;
					first = 0;
					while((p = strtok(NULL, ",")) != NULL){
						if(!strcmp(qname[j], "sum"))
							printf("%s's score : %s", id, p);
						j++;
					}
					printf("\n");
				}
			}
		}

		if(strcmp(ids[0], "") == 0)
		{
			break;
		}
	}
}

void do_pOption_not_s(char *stdId, char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j;
	char first, exist;

	WrongNode *travelNode;
	char saved_path[BUFLEN];


	getcwd(saved_path, BUFLEN);

	chdir(ansDir);

	if((fp = fopen("./score.csv", "r")) == NULL){
		fprintf(stderr, "score.csv file doesn't exist\n");
		return;
	}

	chdir(saved_path);

	// get qnames
	i = 0;
	fscanf(fp, "%s\n", tmp);
	strcpy(qname[i++], strtok(tmp, ","));
	
	while((p = strtok(NULL, ",")) != NULL)
		strcpy(qname[i++], p);

	
	if(WrongList == NULL)
	{
		WrongList = malloc(sizeof(WrongNode));
	}

	// print result
	i = 0;
	while(i++ <= ARGNUM - 1)
	{
		exist = 0;
		fseek(fp, 0, SEEK_SET);
		fscanf(fp, "%s\n", tmp);

		while(fscanf(fp, "%s\n", tmp) != EOF){
			id = strtok(tmp, ",");
			
			if(strcmp(ids[0], "") == 0)
			{
				if(strcmp(id, stdId) == 0)
				{
					exist = 1;
					j = 0;
					first = 0;
					while((p = strtok(NULL, ",")) != NULL){
						if(atof(p) == 0)
						{
							// 틀린 문제 링크드리스트 노드 추가

							if(!first){
								first = 1;

								travelNode = WrongList;

								memset(travelNode->wrongProblem, 0, FILELEN);
								strcpy(travelNode->wrongProblem, "wrong problem : ");
							}
							if(strcmp(qname[j], "sum"))
							{
								travelNode->nextNode = malloc(sizeof(WrongNode));
								travelNode = travelNode->nextNode;

								memset(travelNode->wrongProblem, 0, FILELEN);
								strcpy(travelNode->wrongProblem, qname[j]);
							}
						}
						j++;
					}
				}
			}
			else if(strcmp(id, stdId) == 0)
			{
				exist = 1;
				j = 0;
				first = 0;
				while((p = strtok(NULL, ",")) != NULL){
					if(atof(p) == 0)
					{
						// 틀린 문제 링크드리스트 노드 추가

						if(!first){
							first = 1;

							travelNode = WrongList;

							memset(travelNode->wrongProblem, 0, FILELEN);
							strcpy(travelNode->wrongProblem, "wrong problem : ");
						}
						if(strcmp(qname[j], "sum"))
						{
							travelNode->nextNode = malloc(sizeof(WrongNode));
							travelNode = travelNode->nextNode;

							memset(travelNode->wrongProblem, 0, FILELEN);
							strcpy(travelNode->wrongProblem, qname[j]);
						}
					}
					j++;
				}
			}
		}

		if(strcmp(ids[0], "") == 0)
		{
			break;
		}

		if(exist == 1)
		{
			break;
		}
	}

	fclose(fp);
}

void do_pOption_with_s(char *stdId, char (*ids)[FILELEN])
{
	FILE *fp;
	char tmp[BUFLEN];
	char qname[QNUM][FILELEN];
	char *p, *id;
	int i, j;
	char first, exist;

	SortNode *studentNode;
	WrongNode *travelNode;
	char saved_path[BUFLEN];


	getcwd(saved_path, BUFLEN);

	chdir(ansDir);

	if((fp = fopen("./score.csv", "r")) == NULL){
		fprintf(stderr, "score.csv file doesn't exist\n");
		return;
	}

	chdir(saved_path);

	// get qnames
	i = 0;
	fscanf(fp, "%s\n", tmp);
	strcpy(qname[i++], strtok(tmp, ","));
	
	while((p = strtok(NULL, ",")) != NULL)
		strcpy(qname[i++], p);

	
	if(WrongList == NULL)
	{
		WrongList = malloc(sizeof(WrongNode));
	}

	memset(tmp, 0, BUFLEN);

	studentNode = SortList;

	// 채점 결과 정렬 링크드리스트 순회
	while(studentNode != NULL)
	{
		id = studentNode->studentId;
			
		if(strcmp(ids[0], "") == 0)
		{
			if(strcmp(id, stdId) == 0)
			{
				exist = 1;
				j = 0;
				first = 0;

				strcpy(tmp, studentNode->studentResult);

				p = strtok(tmp, ",");

				while(1)
				{
					if(p == NULL)
					{
						break;
					}

					if(atof(p) == 0)
					{
						// 틀린 문제 링크드리스트 노드 추가

						if(!first)
						{
							first = 1;

							travelNode = WrongList;

							memset(travelNode->wrongProblem, 0, FILELEN);
							strcpy(travelNode->wrongProblem, "wrong problem : ");
						}
						if(strcmp(qname[j], "sum"))
						{
							travelNode->nextNode = malloc(sizeof(WrongNode));
							travelNode = travelNode->nextNode;

							memset(travelNode->wrongProblem, 0, FILELEN);
							strcpy(travelNode->wrongProblem, qname[j]);
						}
					}
					j++;

					p = strtok(NULL, ",");
				}
			}
		}
		else if(strcmp(id, stdId) == 0)
		{
			exist = 1;
			j = 0;
			first = 0;

			strcpy(tmp, studentNode->studentResult);

			p = strtok(tmp, ",");

			while(1)
			{
				if(p == NULL)
				{
					break;
				}
				
				if(atof(p) == 0)
				{
					// 틀린 문제 링크드리스트 노드 추가

					if(!first)
					{
						first = 1;

						travelNode = WrongList;

						memset(travelNode->wrongProblem, 0, FILELEN);
						strcpy(travelNode->wrongProblem, "wrong problem : ");
					}
					if(strcmp(qname[j], "sum"))
					{
						travelNode->nextNode = malloc(sizeof(WrongNode));
						travelNode = travelNode->nextNode;

						memset(travelNode->wrongProblem, 0, FILELEN);
						strcpy(travelNode->wrongProblem, qname[j]);
					}
				}
				j++;

				p = strtok(NULL, ",");
			}
		}

		studentNode = studentNode->nextNode;
	}


	fclose(fp);
}

int do_mOption(char *ansDir)
{
	double newScore;
	char modiName[FILELEN];
	char filename[FILELEN];
	char *ptr;
	int i;

	int mFlag;

	ptr = malloc(sizeof(char) * FILELEN);

	while(1)
	{
		// 배점을 수정할 문제 번호 입력
		printf("Input question's number to modify >> ");
		scanf("%s", modiName);

		// no 입력시 배점 수정 안함
		if(strcmp(modiName, "no") == 0)
			break;
		
		mFlag = false;
		
		// 점수 테이블 순회
		for(i=0; i < scoreTableCnt; i++)
		{
			strcpy(ptr, score_table[i].qname);
			ptr = strtok(ptr, ".");
			if(!strcmp(ptr, modiName)){
				printf("Current score : %.2f\n", score_table[i].score);
				printf("New score : ");
				scanf("%lf", &newScore);
				getchar();
				score_table[i].score = newScore;

				mFlag = true;

				break;
			}
		}

		if(mFlag == false)
		{
			fprintf(stderr, "%s doesn't exist in ScoreTable\n", modiName);

			break;
		}
	}

	free(ptr);

	if(mFlag == false)
	{
		return false;
	}

	write_scoreTable(filename);

	return true;
}

void do_sOption()
{
	SortNode *travelNode;
	int nodeCnt;


	nodeCnt = 0;
	travelNode = SortList;
	while(travelNode != NULL)
	{
		nodeCnt++;

		travelNode = travelNode->nextNode;
	}

	// 링크드리스트 정렬 수행
	Sort_List(nodeCnt);

	return;
}

void sort_id_ascending(int nodeCnt)
{
	SortNode *headNode;
	SortNode *travelNode;

	int i;
	int j;


	if(nodeCnt < 1)
	{
		return;
	}

	headNode = SortList;

	// 학번 기준으로 오름차순 정렬 수행
	for(i = 0; i < nodeCnt - 1; i++)
	{
		travelNode = headNode;

		for(j = 0; j < nodeCnt - 1 - i; j++)
		{
			if(strcmp(travelNode->studentId, travelNode->nextNode->studentId) > 0)
			{
				if(travelNode->previousNode != NULL)
				{
					travelNode->previousNode->nextNode = travelNode->nextNode;
				}

				travelNode->nextNode->previousNode = travelNode->previousNode;

				travelNode->previousNode = travelNode->nextNode;

				if(travelNode->nextNode->nextNode != NULL)
				{
					travelNode->nextNode->nextNode->previousNode = travelNode;
				}

				travelNode->nextNode = travelNode->nextNode->nextNode;

				travelNode->previousNode->nextNode = travelNode;
			}
			else
			{
				travelNode = travelNode->nextNode;
			}
		}

		while(travelNode->previousNode != NULL)
		{
			travelNode = travelNode->previousNode;
		}

		headNode = travelNode;
	}

	SortList = headNode;
}

void sort_id_descending(int nodeCnt)
{
	SortNode *headNode;
	SortNode *travelNode;

	int i;
	int j;


	if(nodeCnt < 1)
	{
		return;
	}

	headNode = SortList;

	// 학번 기준으로 내림차순 정렬 수행
	for(i = 0; i < nodeCnt - 1; i++)
	{
		travelNode = headNode;

		for(j = 0; j < nodeCnt - 1 - i; j++)
		{
			if(strcmp(travelNode->studentId, travelNode->nextNode->studentId) < 0)
			{
				if(travelNode->previousNode != NULL)
				{
					travelNode->previousNode->nextNode = travelNode->nextNode;
				}

				travelNode->nextNode->previousNode = travelNode->previousNode;

				travelNode->previousNode = travelNode->nextNode;

				if(travelNode->nextNode->nextNode != NULL)
				{
					travelNode->nextNode->nextNode->previousNode = travelNode;
				}

				travelNode->nextNode = travelNode->nextNode->nextNode;

				travelNode->previousNode->nextNode = travelNode;
			}
			else
			{
				travelNode = travelNode->nextNode;
			}
		}

		while(travelNode->previousNode != NULL)
		{
			travelNode = travelNode->previousNode;
		}

		headNode = travelNode;
	}

	SortList = headNode;
}

void sort_score_ascending(int nodeCnt)
{
	SortNode *headNode;
	SortNode *travelNode;

	int i;
	int j;


	if(nodeCnt < 1)
	{
		return;
	}

	headNode = SortList;

	// 점수 기준으로 오름차순 정렬 수행
	for(i = 0; i < nodeCnt - 1; i++)
	{
		travelNode = headNode;

		for(j = 0; j < nodeCnt - 1 - i; j++)
		{
			if(travelNode->nextNode != NULL && strcmp(travelNode->studentScore, travelNode->nextNode->studentScore) > 0)
			{
				if(travelNode->previousNode != NULL)
				{
					travelNode->previousNode->nextNode = travelNode->nextNode;
				}

				travelNode->nextNode->previousNode = travelNode->previousNode;

				travelNode->previousNode = travelNode->nextNode;

				if(travelNode->nextNode->nextNode != NULL)
				{
					travelNode->nextNode->nextNode->previousNode = travelNode;
				}

				travelNode->nextNode = travelNode->nextNode->nextNode;

				travelNode->previousNode->nextNode = travelNode;
			}
			else
			{
				travelNode = travelNode->nextNode;
			}
		}

		while(travelNode->previousNode != NULL)
		{
			travelNode = travelNode->previousNode;
		}

		headNode = travelNode;
	}

	SortList = headNode;
}

void sort_score_descending(int nodeCnt)
{
	SortNode *headNode;
	SortNode *travelNode;

	int i;
	int j;


	if(nodeCnt < 1)
	{
		return;
	}

	headNode = SortList;

	// 점수 기준으로 내림차순 정렬 수행
	for(i = 0; i < nodeCnt - 1; i++)
	{
		travelNode = headNode;

		for(j = 0; j < nodeCnt - 1 - i; j++)
		{
			if(travelNode->nextNode != NULL && strcmp(travelNode->studentScore, travelNode->nextNode->studentScore) < 0)
			{
				if(travelNode->previousNode != NULL)
				{
					travelNode->previousNode->nextNode = travelNode->nextNode;
				}

				travelNode->nextNode->previousNode = travelNode->previousNode;

				travelNode->previousNode = travelNode->nextNode;

				if(travelNode->nextNode->nextNode != NULL)
				{
					travelNode->nextNode->nextNode->previousNode = travelNode;
				}

				travelNode->nextNode = travelNode->nextNode->nextNode;

				travelNode->previousNode->nextNode = travelNode;
			}
			else
			{
				travelNode = travelNode->nextNode;
			}
		}

		while(travelNode->previousNode != NULL)
		{
			travelNode = travelNode->previousNode;
		}

		headNode = travelNode;
	}

	SortList = headNode;
}

void Save_Sorted_Score(int fd)
{
	char tmp[MAX_PATH];
	SortNode *travelNode;

	
	travelNode = SortList;

	// 정렬된 링크드리스트 내용으로 채점 결과 파일에 저장
	while(travelNode != NULL)
	{
		snprintf(tmp, MAX_PATH, "%s,%s%s\n", travelNode->studentId, travelNode->studentResult, travelNode->studentScore);
		
		write(fd, tmp, strlen(tmp));
		
		travelNode = travelNode->nextNode;
	}
	
	
	return;
}

int is_exist(char (*src)[FILELEN], char *target)
{
	int i = 0;

	while(1)
	{
		if(i >= ARGNUM)
			return false;
		else if(!strcmp(src[i], ""))
			return false;
		else if(!strcmp(src[i++], target))
			return true;
	}
	return false;
}

void set_scoreTable(char *ansDir)
{
	char filename[FILELEN];

	snprintf(filename, FILELEN, "%s/%s", ansDir, "score_table.csv");	// 점수 테이블 파일 경로 생성

	// check exist
	// 점수 테이블 파일이 존재하는지 검사
	if(access(filename, F_OK) == 0)
		// 이미 존재하면 기존 파일 읽기
		read_scoreTable(filename);
	else
	{
		// 존재하지 않으면 스코어 테이블 및 파일 생성
		make_scoreTable(ansDir);
		write_scoreTable(filename);
	}
}

void read_scoreTable(char *path)
{
	FILE *fp;
	char qname[FILELEN];
	char score[BUFLEN];
	int idx = 0;

	// 점수 테이블 파일을 읽기 전용으로 오픈
	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	// 파일의 끝까지 순회
	while(fscanf(fp, "%[^,],%s\n", qname, score) != EOF){
		strcpy(score_table[idx].qname, qname);
		score_table[idx++].score = atof(score);
	}

	scoreTableCnt = idx;

	fclose(fp);
}

void make_scoreTable(char *ansDir)
{
	int type, num;
	double score, bscore, pscore;
	struct dirent *dirp, *c_dirp;
	DIR *dp, *c_dp;
	char *tmp;
	int idx = 0;
	int i;

	num = get_create_type();	// 점수 테이블 생성 타입 획득

	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);
		printf("Input value of program question : ");
		scanf("%lf", &pscore);
	}

	// 정답 디렉토리 오픈
	if((dp = opendir(ansDir)) == NULL){
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}

	// 정답 디렉토리 내 하위 디렉토리 읽기
	while((dirp = readdir(dp)) != NULL){
		// . 디렉토리와 .. 디렉토리 건너뛰기
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		// 파일의 타입(확장자) 검사
		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		// 스코어 구조체에 각 문제의 정답 파일명 저장
		strcpy(score_table[idx].qname, dirp->d_name);

		idx++;
	}

	closedir(dp);			// 정답 디렉토리 닫기
	sort_scoreTable(idx);	// 점수 테이블 정렬

	// 점수 테이블 전체 순회
	for(i = 0; i < idx; i++)
	{
		// 정답 파일의 타입 획득
		type = get_file_type(score_table[i].qname);

		// 점수 테이블 파일의 타입에 따라 배점 수정 또는 입력
		if(num == 1)
		{
			// 텍스트 파일, 소스코드 파일에 따라 배점 수정
			if(type == TEXTFILE)
				score = bscore;
			else if(type == CFILE)
				score = pscore;
		}
		else if(num == 2)
		{
			printf("Input of %s: ", score_table[i].qname);
			scanf("%lf", &score);
		}

		score_table[i].score = score;	// 스코어 구조체에 배점 저장
	}

	scoreTableCnt = idx;
}

void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = scoreTableCnt;

	// 점수 테이블 파일 생성
	if((fd = creat(filename, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	// 스코어 테이블 구조체 순회
	for(i = 0; i < num; i++)
	{
		// 배점이 0점이면 끝냄
		if(score_table[i].score == 0)
			break;

		// 스코어 구조체의 내용을 점수 테이블 파일에 쓰기
		snprintf(tmp, BUFLEN, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp));
	}

	// 점수 테이블 파일 닫기
	close(fd);
}


void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	// 학생 답안 디렉토리 오픈
	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	// 학생 답안 디렉토리의 하위 디렉토리 즉, 학번 디렉토리 읽기
	while((dirp = readdir(dp)) != NULL){
		// . 디렉토리와 .. 디렉토리는 건너뛴다.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		// 학번 디렉토리 경로 생성
		snprintf(tmp, BUFLEN, "%s/%s", stuDir, dirp->d_name);

		// 학번 디렉토리에 대한 stat 구조체 획득
		stat(tmp, &statbuf);

		// 학번 디렉토리 경로가 실제로 디렉토리가 맞는지 검사
		if(S_ISDIR(statbuf.st_mode))
			// 학번 테이블에 학번 추가
			strcpy(id_table[num++], dirp->d_name);
		else
			// 디렉토리가 아니면 건너뛴다.
			continue;
	}
	closedir(dp);	// 디렉토리 닫기

	StudentTotal = num;

	// 학번 테이블 정렬
	sort_idTable(num);
}

void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	// 학번 테이블 순회
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			// 학번 테이블 삽입 정렬로 정렬
			if(strcmp(id_table[j], id_table[j+1]) > 0){
				strcpy(tmp, id_table[j]);
				strcpy(id_table[j], id_table[j+1]);
				strcpy(id_table[j+1], tmp);
			}
		}
	}
}

void sort_scoreTable(int size)
{
	int i, j;
	struct ssu_scoreTable tmp;
	int num1_1, num1_2;
	int num2_1, num2_2;

	// 점수 테이블 순회
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){
			// 점수 테이블 삽입 정렬로 정렬

			// 현재 문제 파일명과 다음 문제 파일명에서 특수기호(-, .)와 확장자를 제외한 정수형의 문제 번호를 획득
			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);

			// 현재 문제 번호와 다음 문제 번호를 비교
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){
				// 두자리수의 현재 문제 번호가 다음 문제 번호보다 크거나 같으면 서로 교환
				memcpy(&tmp, &score_table[j], sizeof(score_table[0]));
				memcpy(&score_table[j], &score_table[j+1], sizeof(score_table[0]));
				memcpy(&score_table[j+1], &tmp, sizeof(score_table[0]));
			}
		}
	}
}

void get_qname_number(char *qname, int *num1, int *num2)
{
	char *p;
	char dup[FILELEN];

	// 문제 파일명 복사 후 '-'과 '.' 으로 분리 및 문자를 정수형으로 변환
	strncpy(dup, qname, strlen(qname));
	*num1 = atoi(strtok(dup, "-."));
	
	// '-'과 '.'으로 분리 후 분리된 문자가 없으면 0, 있으면 문자를 정수형으로 변환
	p = strtok(NULL, "-.");
	if(p == NULL)
		*num2 = 0;
	else
		*num2 = atoi(p);
}

int get_create_type()
{
	int num;

	while(1)
	{
		// 점수 테이블 타입 선택
		printf("score_table.csv file doesn't exist in \"%s\"!\n", ansDir);
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		// 올바르지 않은 입력
		if(num != 1 && num != 2)
			printf("not correct number!\n");
		else
			break;
	}

	return num;
}

void score_students()
{
	double score = 0;
	int num;
	int fd = -1;
	char tmp[BUFLEN];
	int size = sizeof(id_table) / sizeof(id_table[0]);

	FILE *fp = NULL;
	char scorePath[BUFLEN];
	char *strPtr;

	double (*score_student)(int fd, char *id, int idx);

	int argFlag = 0;


	// 채점 결과 파일 생성
	if(nOption == true)
	{
		if((fd = creat(csvName, 0666)) < 0)
		{
			fprintf(stderr, "creat error for %s", csvName);
			return;
		}
		write_first_row(fd);	// 점수 테이블 csv 파일에 column 작성
	}
	else
	{
		memset(scorePath, 0, BUFLEN);
		strcpy(scorePath, ansDir);
		strcat(scorePath, "/score.csv");

		if((fd = creat(scorePath, 0666)) < 0)
		{
			fprintf(stderr, "creat error for score.csv");
			return;
		}
		write_first_row(fd);	// 점수 테이블 csv 파일에 column 작성
	}

	if(sOption == true)
	{
		score_student = score_student_with_s;
		
		do_pOption = do_pOption_with_s;
	}
	else
	{
		score_student = score_student_not_s;

		do_pOption = do_pOption_not_s;
	}


	if(WrongList == NULL)
	{
		WrongList = malloc(sizeof(WrongNode) * size);
		if(WrongList == NULL)
		{
			fprintf(stderr, "fail to generate WrongList\n");
			
			return;
		}

		WrongListCnt = size;
	}


	// 학번 테이블 순회
	for(num = 0; num < size; num++)
	{
		// 학번 테이블 비었으면 종료
		if(!strcmp(id_table[num], ""))
			break;

		if(sOption == false)
		{
			// 각 학번을 "학번, " 형식으로 작성
			snprintf(tmp, FILELEN, "%s,", id_table[num]);
			write(fd, tmp, strlen(tmp));
		}

		memset((WrongList + num)->wrongProblem, 0, FILELEN);
		strcpy((WrongList + num)->wrongProblem, "wrong problem : ");

		// 학생별 채점 점수 누적
		if(cOption == true || pOption == true)
		{
			if(strcmp(iIDs[0], "") == 0)
			{
				score += score_student(fd, id_table[num], num);
			}
			else
			{
				argFlag = 0;

				for(int i = 0; i < studentParams; i++)
				{
					if(strcmp(iIDs[i], id_table[num]) == 0)
					{
						argFlag = 1;
					}
				}

				if(argFlag == 1)
				{
					score += score_student(fd, id_table[num], num);
				}
				else
				{
					score_student(fd, id_table[num], num);
				}
			}
		}
		else
		{
			score_student(fd, id_table[num], num);
		}
	}

	Delete_WrongList();

	if(sOption == true)
	{
		do_sOption();

		Save_Sorted_Score(fd);
	}

	Delete_SortList();

	if(cOption == true)
	{
		if(strcmp(iIDs[0], "") == 0)
		{
			studentParams = num;
		}

		printf("Total average : %.2f\n", score / studentParams);	// 평균 점수 출력
	}

	if(fd > 0)
	{
		close(fd);

		fd = -1;
	}

	if(fp != NULL)
	{
		fclose(fp);

		fp = NULL;
	}
}

double score_student_with_s(int fd, char *id, int idx)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[MAX_PATH];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	int cState;
	int pState;
	int cpState;

	char id_buf[10];

	SortNode *travelNode;
	WrongNode *wrongTravelNode;


	if(SortList == NULL)
	{
		SortList = malloc(sizeof(SortNode));
		travelNode = SortList;
	}
	else
	{
		travelNode = SortList;

		while(travelNode->nextNode != NULL)
		{
			travelNode = travelNode->nextNode;
		}

		travelNode->nextNode = malloc(sizeof(SortNode));

		travelNode->nextNode->previousNode = travelNode;
		travelNode = travelNode->nextNode;
	}

	memset(travelNode->studentId, 0, 10);
	memset(travelNode->studentResult, 0, BUFLEN);
	memset(travelNode->studentScore, 0, FILELEN);


	memset(tmp, 0, MAX_PATH);
	memset(id_buf, 0, 10);
	snprintf(id_buf, 10, "%s", id);
	strcpy(travelNode->studentId, id_buf);


	wrongTravelNode = (WrongList + idx);


	for(i = 0; i < size ; i++)
	{
		if(score_table[i].score == 0)
			break;

		snprintf(tmp, MAX_PATH, "%s/%s/%s", stuDir, id, score_table[i].qname);		// 학생이 제출한 각 문제의 답안 파일 경로 생성

		// 학생 제출 답안 파일 존재 여부 검사
		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			// 파일 파입이 텍스트 파일이나 소스코드 파일이 아니면 건너뛴다
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			// 텍스트 파일이면 빈칸채우기 문제이므로 score_blank 실행
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			
			// 소스코드 파일이면 프로그램 작성 문제이므로 score_program 실행
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}

		
		if(result == false)
		{
			if(strlen(travelNode->studentResult) > 0)
			{
				strcat(travelNode->studentResult, "0,");
			}
			else
			{
				strcpy(travelNode->studentResult, "0,");
			}

			wrongTravelNode->nextNode = malloc(sizeof(WrongNode));
			wrongTravelNode = wrongTravelNode->nextNode;

			memset(wrongTravelNode->wrongProblem, 0, FILELEN);
			strcpy(wrongTravelNode->wrongProblem, score_table[i].qname);
			wrongTravelNode->score = score_table[i].score;
		}
		else
		{
			if(result == true)
			{
				score += score_table[i].score;
				snprintf(tmp, MAX_PATH, "%.2f,", score_table[i].score);
			}
			else if(result < 0)
			{
				score = score + score_table[i].score + result;
				snprintf(tmp, MAX_PATH, "%.2f,", score_table[i].score + result);
			}

			if(strlen(travelNode->studentResult) > 0)
			{
				strcat(travelNode->studentResult, tmp);
			}
			else
			{
				strcpy(travelNode->studentResult, tmp);
			}
		}
	}

	if(pOption == true && cOption == false)
	{
		pState = 0;
		for(int i = 0; i < studentParams; i++)
		{
			if(strcmp(iIDs[i], id) == 0)
			{
				pState = 1;

				break;
			}
		}

		if(strcmp(iIDs[0], "") == 0 || pState == 1)
		{
			printf("%s is finished.. ", id);

			Print_WrongList(idx);
		}
		else
		{
			printf("%s is finished..\n", id);
		}
	}
	else if(cOption == true && pOption == false)
	{
		cState = 0;
		for(int i = 0; i < studentParams; i++)
		{
			if(strcmp(iIDs[i], id) == 0)
			{
				cState = 1;

				break;
			}
		}

		if(strcmp(iIDs[0], "") == 0 || cState == 1)
		{
			printf("%s is finished.. score : %.2f\n", id, score);
		}
		else
		{
			printf("%s is finished..\n", id);
		}
	}
	else if(cOption == true && pOption == true)
	{
		cpState = 0;
		for(int i = 0; i < studentParams; i++)
		{
			if(strcmp(iIDs[i], id) == 0)
			{
				cpState = 1;

				break;
			}
		}

		if(strcmp(iIDs[0], "") == 0 || cpState == 1)
		{
			printf("%s is finished.. score : %.2f, ", id, score);

			Print_WrongList(idx);
		}
		else
		{
			printf("%s is finished..\n", id);
		}
	}
	else
	{
		printf("%s is finished..\n", id);
	}

	snprintf(tmp, MAX_PATH, "%.2f", score);
	strcpy(travelNode->studentScore, tmp);

	return score;
}

double score_student_not_s(int fd, char *id, int idx)
{
	int type;
	double result;
	double score = 0;
	int i;
	char tmp[MAX_PATH];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	int cState;
	int pState;
	int cpState;

	WrongNode *wrongTravelNode;

	wrongTravelNode = (WrongList + idx);

	for(i = 0; i < size ; i++)
	{
		if(score_table[i].score == 0)
			break;

		snprintf(tmp, MAX_PATH, "%s/%s/%s", stuDir, id, score_table[i].qname);		// 학생이 제출한 각 문제의 답안 파일 경로 생성

		// 학생 제출 답안 파일 존재 여부 검사
		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			// 파일 파입이 텍스트 파일이나 소스코드 파일이 아니면 건너뛴다
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			// 텍스트 파일이면 빈칸채우기 문제이므로 score_black 실행
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			
			// 소스코드 파일이면 프로그램 작성 문제이므로 score_program 실행
			else if(type == CFILE)
				result = score_program(id, score_table[i].qname);
		}

		
		if(result == false)
		{
			write(fd, "0,", 2);

			wrongTravelNode->nextNode = malloc(sizeof(WrongNode));
			wrongTravelNode = wrongTravelNode->nextNode;

			memset(wrongTravelNode->wrongProblem, 0, FILELEN);
			strcpy(wrongTravelNode->wrongProblem, score_table[i].qname);
			wrongTravelNode->score = score_table[i].score;
		}
		else{
			if(result == true){
				score += score_table[i].score;
				snprintf(tmp, MAX_PATH, "%.2f,", score_table[i].score);
			}
			else if(result < 0){
				score = score + score_table[i].score + result;
				snprintf(tmp, MAX_PATH, "%.2f,", score_table[i].score + result);
			}
			write(fd, tmp, strlen(tmp));
		}
	}

	if(pOption == true && cOption == false)
	{
		pState = 0;
		for(int i = 0; i < studentParams; i++)
		{
			if(strcmp(iIDs[i], id) == 0)
			{
				pState = 1;

				break;
			}
		}

		if(strcmp(iIDs[0], "") == 0 || pState == 1)
		{
			printf("%s is finished.. ", id);

			Print_WrongList(idx);
		}
		else
		{
			printf("%s is finished..\n", id);
		}
	}
	else if(cOption == true && pOption == false)
	{
		cState = 0;
		for(int i = 0; i < studentParams; i++)
		{
			if(strcmp(iIDs[i], id) == 0)
			{
				cState = 1;

				break;
			}
		}

		if(strcmp(iIDs[0], "") == 0 || cState == 1)
		{
			printf("%s is finished.. score : %.2f\n", id, score);
		}
		else
		{
			printf("%s is finished..\n", id);
		}
	}
	else if(cOption == true && pOption == true)
	{
		cpState = 0;
		for(int i = 0; i < studentParams; i++)
		{
			if(strcmp(iIDs[i], id) == 0)
			{
				cpState = 1;

				break;
			}
		}

		if(strcmp(iIDs[0], "") == 0 || cpState == 1)
		{
			printf("%s is finished.. score : %.2f, ", id, score);

			Print_WrongList(idx);
		}
		else
		{
			printf("%s is finished..\n", id);
		}
	}
	else
	{
		printf("%s is finished..\n", id);
	}

	snprintf(tmp, MAX_PATH, "%.2f\n", score);
	write(fd, tmp, strlen(tmp));

	return score;
}

void write_first_row(int fd)
{
	int i;
	char tmp[BUFLEN];
	int size = sizeof(score_table) / sizeof(score_table[0]);

	write(fd, ",", 1);	// 파일의 첫 행, 가장 처음에 ,를 쓴다

	for(i = 0; i < size; i++){
		// 배점이 0점이면 끝냄
		if(score_table[i].score == 0)
			break;
		
		// 스코어 구조체에서 각 문제 파일명을 가져와서 점수 테이블 파일에 "문제 파일명, " 형식으로 작성
		snprintf(tmp, BUFLEN, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);	// 점수 테이블 파일 가장 마지막에 "sum" 문자열 작성
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	// 답을 저장할 result 메모리 초기화
	memset(result, 0, BUFLEN);

	// 오픈된 파일을 1바이트씩 모두 읽기
	while(read(fd, &c, 1) > 0)
	{
		// 콜론(:) 만나면 종료
		if(c == ':')
			break;
		
		// 읽은 문자를 result에 저장
		result[idx++] = c;
	}

	// 파일을 모두 읽어서 마지막에 개행문자가 들어갔으면 널문자로 변경
	if(result[strlen(result) - 1] == '\n')
		result[strlen(result) - 1] = '\0';

	return result;
}

int score_blank(char *id, char *filename)
{
	char tokens[TOKEN_CNT][MINLEN];
	node *std_root = NULL, *ans_root = NULL;
	int idx, start;
	char tmp[MAX_PATH];
	char s_answer[BUFLEN], a_answer[BUFLEN];
	char qname[FILELEN];
	int fd_std, fd_ans;
	int result = true;
	int has_semicolon = false;

	// 문제 파일명 메모리 초기화
	memset(qname, 0, sizeof(qname));

	// filename에서 확장자를 제외한 파일명을 qname으로 복사
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	// 학생의 문제 답안 파일 경로 생성
	snprintf(tmp, MAX_PATH, "%s/%s/%s", stuDir, id, filename);

	// 학생의 문제 답안 파일을 읽기 전용으로 오픈
	fd_std = open(tmp, O_RDONLY);

	// 학생이 제출한 답을 읽은 후에 s_answer로 복사
	strcpy(s_answer, get_answer(fd_std, s_answer));

	// 제출 답안이 비었으면 예외 처리
	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	// 괄호( '(', ')' ) 존재 여부 검사
	// 학생이 제출한 답에 (와 )의 개수가 서로 다르게 존재하면 예외 처리
	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	// 학생 답안의 앞부분부터 공백으로 채워진 부분이 있으면 공백을 지움
	strcpy(s_answer, ltrim(rtrim(s_answer)));

	// 학생 답안의 가장 마지막에 세미콜론이 존재하는지 검사
	if(s_answer[strlen(s_answer) - 1] == ';'){
		// 플래그 활성화
		has_semicolon = true;
		// 세미콜론을 널문자로 변경
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	// 학생 답안 문자열을 토큰으로 분리
	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	// 분리된 토큰으로 트리 생성
	std_root = make_tree(std_root, tokens, &idx, 0);

	// 정답 파일 오픈
	snprintf(tmp, MAX_PATH, "%s/%s", ansDir, filename);
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		ans_root = NULL;
		result = true;

		// tokens 전체 초기화
		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		// 정답을 읽은 후에 a_answer로 복사
		strcpy(a_answer, get_answer(fd_ans, a_answer));

		// a_answer가 비었으면 종료
		if(!strcmp(a_answer, ""))
			break;

		// 앞부분과 뒷부분의 공백을 모두 제거
		strcpy(a_answer, ltrim(rtrim(a_answer)));

		// 학생 답안에 세미콜론이 존재하지 않았던 경우
		if(has_semicolon == false){
			// 정답의 가장 마지막이 세미콜론이면 건너뛴다
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		// 학생 답안에 세미콜론이 존재했던 경우
		else if(has_semicolon == true)
		{
			// 정답의 가장 마지막이 세미콜론이 아니라면 건너뛴다
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			
			// 정답의 가장 마지막이 세미콜론이면 세미콜론을 널문자로 변경
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		// 정답 문자열을 토큰으로 분리
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		// 분리된 토큰으로 트리 생성
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		// 학생 답안 트리와 정답 트리를 비교
		compare_tree(std_root, ans_root, &result);

		// 답안 비교가 성공하면 파일을 닫고, 트리 노드의 메모리 반환
		if(result == true){
			close(fd_std);
			close(fd_ans);

			if(std_root != NULL)
				free_node(std_root);
			if(ans_root != NULL)
				free_node(ans_root);
			return true;

		}
	}
	
	// 파일 닫기
	close(fd_std);
	close(fd_ans);

	// 트리 노드의 메모리 반환
	if(std_root != NULL)
		free_node(std_root);
	if(ans_root != NULL)
		free_node(ans_root);

	return false;
}

double score_program(char *id, char *filename)
{
	double compile;
	int result;

	// 프로그램 컴파일 수행
	compile = compile_program(id, filename);

	// 컴파일 결과가 실패이면 false 리턴
	if(compile == ERROR || compile == false)
		return false;
	
	// 해당 프로그램 실행
	result = execute_program(id, filename);

	// 실행 결과가 실패이면 false 리턴
	if(!result)
		return false;

	// 컴파일 결과가 음수이면 해당 값 리턴
	if(compile < 0)
		return compile;

	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);


	// t 옵션이 활성화 상태이면서 문제 번호가 입력되지 않은 경우 true 리턴
	if(tOption == true && strcmp(threadFiles[0], "") == 0)
	{
		return true;
	}

	// 입력된 문제 번호들에 대해서 true 리턴
	for(i = 0; i < size; i++){
		if(!strcmp(threadFiles[i], qname))
			return true;
	}
	return false;
}

double compile_program(char *id, char *filename)
{
	int fd;
	char tmp_f[MAX_PATH], tmp_e[MAX_PATH];
	char command[MAX_PATH];
	char qname[FILELEN];
	int isthread;
	off_t size;
	double result;

	char *tmp_e_ptr;
	char *tmp_f_ptr;


	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));
	
	// thread 옵션을 적용할지 검사
	isthread = is_thread(qname);

	// 정답 문제 파일명 경로와 해당 문제의 실행파일명 경로 생성
	snprintf(tmp_f, MAX_PATH, "%s/%s", ansDir, filename);
	snprintf(tmp_e, MAX_PATH, "%s/%s.exe", ansDir, qname);

	tmp_e_ptr = tmp_e;
	tmp_f_ptr = tmp_f;

	// t 옵션이 활성화 상태이면 gcc 컴파일 옵션에 lpthread 추가
	if(tOption && isthread)
		// sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s -lpthread", tmp_e_ptr, tmp_f_ptr);
	
	// t 옵션이 비활성화 상태이면 gcc 컴파일 옵션에 lpthread를 추가하지 않는다
	else
		// sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s", tmp_e_ptr, tmp_f_ptr);

	// 해당 문제에 대한 error 내용 기록 텍스트 파일 생성
	snprintf(tmp_e, MAX_PATH, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666);

	// error 내용 기록 파일과 표준에러로 redirection 수행
	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	if(size > 0)
		return false;

	// 학생 답안 파일명 경로와 해당 문제의 실행파일명 경로 생성
	snprintf(tmp_f, MAX_PATH, "%s/%s/%s", stuDir, id, filename);
	snprintf(tmp_e, MAX_PATH, "%s/%s/%s.stdexe", stuDir, id, qname);

	// t 옵션이 활성화 상태이면 gcc 컴파일 옵션에 lpthread 추가
	if(tOption && isthread)
		// sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s -lpthread", tmp_e_ptr, tmp_f_ptr);
	
	// t 옵션이 비활성화 상태이면 gcc 컴파일 옵션에 lpthread를 추가하지 않는다
	else
		// sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s", tmp_e_ptr, tmp_f_ptr);

	// 해당 문제에 대한 error 내용 기록 텍스트 파일 생성
	snprintf(tmp_f, MAX_PATH, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	// error 내용 기록 파일과 표준에러로 redirection 수행
	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);

	if(size > 0){

		// e 옵션이 활성화 상태인 경우
		if(eOption)
		{
			// error 내용 기록 파일 저장 디렉토리 경로를 생성
			snprintf(tmp_e, MAX_PATH, "%s/%s", errorDir, id);

			// 해당 디렉토리가 존재하지 않으면 디렉토리를 새로 생성
			if(access(tmp_e, F_OK) < 0)
				mkdir(tmp_e, 0755);

			// error 내용 기록 파일 저장 경로를 생성
			snprintf(tmp_e, MAX_PATH, "%s/%s/%s_error.txt", errorDir, id, qname);

			// 위에서 생성된 학생 답안에 대한 error 내용 기록 파일 이름을 error 파일을 저장할 경로 이름으로 변경
			rename(tmp_f, tmp_e);

			// warning 검사
			result = check_error_warning(tmp_e);
		}
		else{
			
			// warning 검사 후 파일 삭제
			result = check_error_warning(tmp_f);
			unlink(tmp_f);
		}

		return result;
	}

	unlink(tmp_f);
	return true;
}

double check_error_warning(char *filename)
{
	FILE *fp;
	char tmp[BUFLEN];
	double warning = 0;

	// 입력으로 들어온 파일을 읽기 전용으로 오픈
	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	// 오픈한 파일에서 내용을 읽어온다
	while(fscanf(fp, "%s", tmp) > 0){

		// 읽어온 내용이 error: 인 경우에는 ERROR 리턴
		if(!strcmp(tmp, "error:"))
			return ERROR;
		
		// 읽어온 내용이 WARNING 인 경우에는 누적한다
		else if(!strcmp(tmp, "warning:"))
			warning += WARNING;
	}

	return warning;
}

int execute_program(char *id, char *filename)
{
	char std_fname[MAX_PATH], ans_fname[MAX_PATH];
	char tmp[MAX_PATH];
	char qname[FILELEN];
	time_t start, end;
	pid_t pid;
	int fd;

	memset(qname, 0, sizeof(qname));
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	// 정답의 실행 결과 파일 경로 생성 후 해당 파일 생성
	snprintf(ans_fname, MAX_PATH, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);

	// 정답 실행 파일 경로 생성 후 정답 실행 결과 파일과 표준출력으로 redirection 수행
	snprintf(tmp, MAX_PATH, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);
	close(fd);

	// 학생 답안의 실행 결과 파일 경로 생성 후 해당 파일 생성
	snprintf(std_fname, MAX_PATH, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);

	// 학생 답안 실행 파일을 백그라운드 실행하기 위한 문자열 생성
	snprintf(tmp, MAX_PATH, "%s/%s/%s.stdexe &", stuDir, id, qname);

	// 시작 시간 얻기
	start = time(NULL);

	// 학생 답안 실행 결과 파일과 표준출력으로 redirection 수행
	redirection(tmp, fd, STDOUT);
	
	// 재첨하는 해당 문제의 실행 파일명 생성
	snprintf(tmp, MAX_PATH, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){

		// 종료 시간 얻기
		end = time(NULL);

		// 실행 제한 시간과 비교하여 초과 시 강제 종료
		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	// 학생의 답안 파일 실행결과와 정답 파일 실행결과를 비교한다
	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	// background.txt 파일 오픈
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	// command에 ps 명령어와 grep 명령어 저장
	sprintf(command, "ps | grep %s", name);

	// 위에서 오픈한 파일과 표준출력을 redirection하고 command를 실행
	redirection(command, fd, STDOUT);

	// 오픈한 파일 오프셋을 가장 처음으로 돌려 놓고, 내용을 읽어 온다
	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	// 읽어온 내용이 없다면 위에서 오픈한 background.txt 파일 삭제하고, 0 리턴
	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	// 읽어온 내용을 공백으로 자른 후에 형변환
	pid = atoi(strtok(tmp, " "));
	close(fd);

	// background.txt 파일 삭제하고, 위에서 공백 기준으로 자른 pid 값을 리턴
	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	// 해당 파일 읽기 전용으로 오픈
	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		// 오픈한 파일에서 공백이 아닌 문자를 찾는다
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}

		// 오픈한 파일에서 공백이 아닌 문자를 찾는다
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		// 2개의 파일 모두 끝까지 읽었으면 반복문 종료
		if(len1 == 0 && len2 == 0)
			break;

		// 각각의 파일에서 읽은 문자들이 알파벳 대문자이면 소문자로 변경한다
		to_lower_case(&c1);
		to_lower_case(&c2);

		// 2개의 파일에서 각각 읽어온 문자가 서로 같지 않으면
		// 오픈한 파일 닫고, false 리턴
		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}

	// 2개의 파일의 내용이 모두 일치하기 때문에 오픈한 파일 닫고, true 리턴
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	// 파일 디스크립터를 복사
	saved = dup(old);
	dup2(new, old);

	// 새롭게 지정된 파일 디스크립터로 command 실행
	system(command);

	// 기존의 파일 디스크립터로 다시 돌려 놓는다
	dup2(saved, old);
	close(saved);
}

int get_file_type(char *filename)
{
	char *extension = strrchr(filename, '.');	// . 기준으로 파일명과 확장자 분리

	// 텍스트 파일
	if(!strcmp(extension, ".txt"))
		return TEXTFILE;
	// 소스코드 파일
	else if (!strcmp(extension, ".c"))
		return CFILE;
	// 텍스트 파일과 소스코드 파일이 아닌 경우 에러
	else
		return -1;
}

void rmdirs(const char *path)
{
	struct dirent *dirp;
	struct stat statbuf;
	DIR *dp;
	// char tmp[50];
	char tmp[MAX_PATH];
	
	// path에 해당하는 디렉토리 오픈
	if((dp = opendir(path)) == NULL)
		return;

	// 위에서 오픈한 디렉토리 순회하면서 읽기
	while((dirp = readdir(dp)) != NULL)
	{
		// .과 ..은 넘어간다
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		// 하위 파일 또는 디렉토리의 경로 생성
		snprintf(tmp, MAX_PATH, "%s/%s", path, dirp->d_name);

		// stat 구조체 얻기
		if(lstat(tmp, &statbuf) == -1)
			continue;

		// 하위 디렉토리이면 재귀호출하여 하위 디렉토리 진입
		if(S_ISDIR(statbuf.st_mode))
			rmdirs(tmp);
		
		// 하위 파일이면 삭제
		else
			unlink(tmp);
	}

	// 오픈한 디렉토리 닫기
	closedir(dp);

	// path에 해당하는 디렉토리 삭제
	rmdir(path);
}

void to_lower_case(char *c)
{
	// 알파벳 대문자를 소문자로 변경
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

void print_usage()
{
	// 프로그램 usage 출력
	printf("Usage : ssu_score <STD_DIR> <ANS_DIR> [OPTION]\n");
	printf("Option :\n");
	printf(" -n <CSVFILENAME>\n");
	printf(" -m\n");
	printf(" -c [STUDENTIDS ...]\n");
	printf(" -p [STUDENTIDS ...]\n");
	printf(" -t [QNAMES ...]\n");
	printf(" -s <CATEGORY> <1|-1>\n");
	printf(" -e <DIRNAME>\n");
	printf(" -h\n");
}

void Print_WrongList(int idx)
{
	WrongNode *travelNode;

	int head;

	char problemName[BUFLEN];
	double problemScore;

	
	if(WrongList == NULL)
	{
		fprintf(stderr, "WrongList is empty\n");

		return;
	}

	travelNode = (WrongList + idx);
	head = 0;

	// 학생별 틀린 문제 순회하면서 틀린 문제 번호 출력
	while(travelNode != NULL)
	{
		if(head == 0)
		{
			head = 1;

			printf("%s", travelNode->wrongProblem);
		}
		else
		{
			strcpy(problemName, travelNode->wrongProblem);

			problemScore = travelNode->score;

			for(int i = strlen(problemName); i >= 0; i--)
			{
				if(problemName[i] == '.')
				{
					problemName[i] = '\0';

					break;
				}
			}

			if(problemScore - (int)problemScore == 0)
			{
				printf("%s(%d)", problemName, (int)problemScore);
			}
			else
			{
				printf("%s(%.1f)", problemName, problemScore);
			}

			if(travelNode->nextNode != NULL)
			{
				printf(", ");
			}
		}
		
		travelNode = travelNode->nextNode;
	}

	printf("\n");

	
	return;
}

void Delete_WrongList()
{
	WrongNode *travelNode;
	WrongNode *deleteNode;


	if(WrongList == NULL)
	{
		return;
	}

	// 학생별 틀린 문제 리스트 순회하면서 메모리 반환
	for(int i = 0; i < WrongListCnt; i++)
	{
		if(i == StudentTotal)
		{
			break;
		}

		if((WrongList + i) == NULL)
		{
			break;
			//continue;
		}

		if((WrongList + i)->nextNode == NULL)
		{
			break;
			//continue;
		}

		travelNode = (WrongList + i)->nextNode;

		while(travelNode != NULL)
		{
			deleteNode = travelNode;
			travelNode = travelNode->nextNode;
			
			free(deleteNode);

			deleteNode = NULL;
		}
	}

	if(WrongList != NULL)
	{
		free(WrongList);

		WrongList = NULL;
	}

	return;
}

void Delete_SortList()
{
	SortNode *travelNode;
	SortNode *deleteNode;


	if(SortList == NULL)
	{
		return;
	}

	travelNode = SortList->nextNode;

	// 학생 채점 결과 정렬 리스트 순회하면서 메모리 반환
	while(travelNode->nextNode != NULL)
	{
		if(travelNode->previousNode != NULL)
		{
			free(travelNode->previousNode);
		}

		travelNode = travelNode->nextNode;
	}

	free(travelNode->previousNode);
	free(travelNode);

	SortList = NULL;

	return;
}