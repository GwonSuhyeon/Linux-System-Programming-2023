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

struct ssu_scoreTable score_table[QNUM]; // ���� ���̺� ����ü
char id_table[SNUM][10]; // ����� ������ �л����� �й� ���̺�

char stuDir[BUFLEN]; // �л� ��� ���丮 ���
char ansDir[BUFLEN]; // ���� ���丮 ���
char errorDir[BUFLEN]; // ���� �޽��� ��� ���
char csvName[BUFLEN]; // ä�� ��� ���� csv ���ϸ�
char threadFiles[ARGNUM][FILELEN]; // lpthread �ɼ��� ������ ���� ���
char iIDs[ARGNUM][FILELEN]; // �й� �Է� ���� ���
char category[BUFLEN]; // s �ɼ� ���� ���� ����

int sortType = 0; // s �ɼ� ���� ���� Ÿ��

// �ɼ� Ȱ��ȭ ���� �÷���
int nOption = false;
int mOption = false;
int cOption = false;
int pOption = false;
int tOption = false;
int eOption = false;
int sOption = false;

int cpArgFlag = false;

WrongNode *WrongList; // Ʋ�� ���� ���� ��ũ�帮��Ʈ
SortNode *SortList; // ä�� ��� ���� ��ũ�帮��Ʈ

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


	// -h �ɼ� �Է� ���� �˻�
	// -h �ɼ��� �ԷµǾ����� usage ��� �� ����
	for(i = 0; i < argc; i++){
		if(!strcmp(argv[i], "-h")){
			print_usage();
			return;
		}
	}

	// -h �ɼ� �Է� ���� �˻�
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

	// �ʼ� �Է� ���� �˻�
	if(argc < 3)
	{
		fprintf(stderr, "Need <STD_DIR> <ANS_DIR>>\n");

		return;
	}

	memset(saved_path, 0, BUFLEN);

	strcpy(stuDir, argv[1]);
	strcpy(ansDir, argv[2]);

	// ���� �۾� ���丮 ��� ���
	getcwd(saved_path, BUFLEN);


	memset(stdPath, 0, BUFLEN);
	memset(ansPath, 0, BUFLEN);
	

	realpath(stuDir, stdPath);
	realpath(ansDir, ansPath);

	// �л� ��� ���丮 stat ����ü �˻�
	if(stat(stdPath, &statbuf) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", stdPath);

		return;
	}
	
	// ���� ���丮 stat ����ü �˻�
	if(stat(ansPath, &statbuf) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", ansPath);

		return;
	}


	// �ɼ� �˻�
	if(!check_option(argc, argv))
		exit(1);
	

	// �л� ��� ���丮�� �̵�
	if(chdir(stuDir) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", stuDir);
		return;
	}
	getcwd(stuDir, BUFLEN);			// �л� ��� ���丮������ ������ ȹ��

	chdir(saved_path);				// ���α׷� ���� ��ġ���� saved_path�� �̵�
	
	// ���� ���丮�� �̵�
	if(chdir(ansDir) < 0)
	{
		fprintf(stderr, "%s doesn't exist\n", ansDir);
		return;
	}
	getcwd(ansDir, BUFLEN);			// ���� ���丮������ ������ ȹ��

	chdir(saved_path);				// ���α׷� ���� ��ġ���� saved_path�� �̵�

	
	// c, p �ɼ� ����
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


	// m �ɼ� ����
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

	set_scoreTable(ansDir); // ���� ���̺� �غ�
	set_idTable(stuDir); // �й� ���̺� �غ�

	// t �ɼ� ����
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

	// -m �ɼ� �÷��� �˻�
	if(mOption)
	{
		// Ȱ��ȭ ���¸� dp_mOption ����
		if(do_mOption() == false)
		{
			return;
		}
	}

	printf("grading student's test papers..\n");

	// ä�� ����
	score_students();


	memset(resolvedPath, 0, MAX_PATH);
	memset(temp, 0, MAX_PATH);

	// n �ɼ� ���� ��� ����
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

	// getopt�� �ɼ� ���� ����
	while((c = getopt(argc, argv, "he:n:s:thmcp1")) != -1)		// �ɼ��� e, n, s, t, h, m, c, p
	{
		switch(c){
			case 'h':

				print_usage();

				return false;
				
			break;

			case 'e':	// -e �ɼ� �Էµ� ���

				if(eOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				eOption = true;		// -e �ɼ� �÷��� Ȱ��ȭ
				strcpy(errorDir, optarg);	// �ɼ� ���ڸ� ���� �޽��� ��� ��ο� ����

				strcpy(tmp, errorDir);

				strcpy(path, tmp);

				// ��� ���ڿ� �������� �ʴ� ���丮�� ���� ��� ���丮 ����
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


				// �ɼ� ���ڷ� ���� ���丮 ��ΰ� �����ϴ��� �˻�
				if(access(errorDir, F_OK) < 0)	// ���丮 �������� ����
					mkdir(errorDir, 0755);	// �ش� ��ο� ���丮 ����, ������ 0755
				else{	// �̹� ���丮 ������
					rmdirs(errorDir);	// ���� ���丮 ����
					mkdir(errorDir, 0755);	// �ش� ��ο� ���丮 ����, ������ 0755
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

				// �Է� ������ Ȯ���� �˻�
				if(strncmp(&csvName[strlen(csvName) - strlen(".csv")], ".csv", strlen(".csv")) != 0)
				{
					fprintf(stderr, "%s is not csv file\n", csvName);

					return false;
				}

				// ��� ���ڿ� �������� �ʴ� ���丮�� ���� ��� ���丮 ����
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

				// s �ɼ� ���� ���� �˻�
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
					// �й� ���� �������� ����

					Sort_List = sort_id_ascending;
				}
				else if(strcmp(category, "stdid") == 0 && sortType == -1)
				{
					// �й� ���� �������� ����

					Sort_List = sort_id_descending;
				}
				else if(strcmp(category, "score") == 0 && sortType == 1)
				{
					// ���� ���� �������� ����

					Sort_List = sort_score_ascending;
				}
				else if(strcmp(category, "score") == 0 && sortType == -1)
				{
					// ���� ���� �������� ����

					Sort_List = sort_score_descending;
				}

				break;
			case 't':	// -t �ɼ� �Էµ� ���

				if(tOption == true)
				{
					fprintf(stderr, "overlap option\n");

					return false;
				}

				tOption = true;		// -t �ɼ� �÷��� Ȱ��ȭ
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

				// �������� �˻�
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

				// �������� �˻�
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
							// Ʋ�� ���� ��ũ�帮��Ʈ ��� �߰�

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
						// Ʋ�� ���� ��ũ�帮��Ʈ ��� �߰�

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

	// ä�� ��� ���� ��ũ�帮��Ʈ ��ȸ
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
						// Ʋ�� ���� ��ũ�帮��Ʈ ��� �߰�

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
					// Ʋ�� ���� ��ũ�帮��Ʈ ��� �߰�

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
		// ������ ������ ���� ��ȣ �Է�
		printf("Input question's number to modify >> ");
		scanf("%s", modiName);

		// no �Է½� ���� ���� ����
		if(strcmp(modiName, "no") == 0)
			break;
		
		mFlag = false;
		
		// ���� ���̺� ��ȸ
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

	// ��ũ�帮��Ʈ ���� ����
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

	// �й� �������� �������� ���� ����
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

	// �й� �������� �������� ���� ����
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

	// ���� �������� �������� ���� ����
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

	// ���� �������� �������� ���� ����
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

	// ���ĵ� ��ũ�帮��Ʈ �������� ä�� ��� ���Ͽ� ����
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

	snprintf(filename, FILELEN, "%s/%s", ansDir, "score_table.csv");	// ���� ���̺� ���� ��� ����

	// check exist
	// ���� ���̺� ������ �����ϴ��� �˻�
	if(access(filename, F_OK) == 0)
		// �̹� �����ϸ� ���� ���� �б�
		read_scoreTable(filename);
	else
	{
		// �������� ������ ���ھ� ���̺� �� ���� ����
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

	// ���� ���̺� ������ �б� �������� ����
	if((fp = fopen(path, "r")) == NULL){
		fprintf(stderr, "file open error for %s\n", path);
		return ;
	}

	// ������ ������ ��ȸ
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

	num = get_create_type();	// ���� ���̺� ���� Ÿ�� ȹ��

	if(num == 1)
	{
		printf("Input value of blank question : ");
		scanf("%lf", &bscore);
		printf("Input value of program question : ");
		scanf("%lf", &pscore);
	}

	// ���� ���丮 ����
	if((dp = opendir(ansDir)) == NULL){
		fprintf(stderr, "open dir error for %s\n", ansDir);
		return;
	}

	// ���� ���丮 �� ���� ���丮 �б�
	while((dirp = readdir(dp)) != NULL){
		// . ���丮�� .. ���丮 �ǳʶٱ�
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		// ������ Ÿ��(Ȯ����) �˻�
		if((type = get_file_type(dirp->d_name)) < 0)
			continue;

		// ���ھ� ����ü�� �� ������ ���� ���ϸ� ����
		strcpy(score_table[idx].qname, dirp->d_name);

		idx++;
	}

	closedir(dp);			// ���� ���丮 �ݱ�
	sort_scoreTable(idx);	// ���� ���̺� ����

	// ���� ���̺� ��ü ��ȸ
	for(i = 0; i < idx; i++)
	{
		// ���� ������ Ÿ�� ȹ��
		type = get_file_type(score_table[i].qname);

		// ���� ���̺� ������ Ÿ�Կ� ���� ���� ���� �Ǵ� �Է�
		if(num == 1)
		{
			// �ؽ�Ʈ ����, �ҽ��ڵ� ���Ͽ� ���� ���� ����
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

		score_table[i].score = score;	// ���ھ� ����ü�� ���� ����
	}

	scoreTableCnt = idx;
}

void write_scoreTable(char *filename)
{
	int fd;
	char tmp[BUFLEN];
	int i;
	int num = scoreTableCnt;

	// ���� ���̺� ���� ����
	if((fd = creat(filename, 0666)) < 0){
		fprintf(stderr, "creat error for %s\n", filename);
		return;
	}

	// ���ھ� ���̺� ����ü ��ȸ
	for(i = 0; i < num; i++)
	{
		// ������ 0���̸� ����
		if(score_table[i].score == 0)
			break;

		// ���ھ� ����ü�� ������ ���� ���̺� ���Ͽ� ����
		snprintf(tmp, BUFLEN, "%s,%.2f\n", score_table[i].qname, score_table[i].score);
		write(fd, tmp, strlen(tmp));
	}

	// ���� ���̺� ���� �ݱ�
	close(fd);
}


void set_idTable(char *stuDir)
{
	struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	char tmp[BUFLEN];
	int num = 0;

	// �л� ��� ���丮 ����
	if((dp = opendir(stuDir)) == NULL){
		fprintf(stderr, "opendir error for %s\n", stuDir);
		exit(1);
	}

	// �л� ��� ���丮�� ���� ���丮 ��, �й� ���丮 �б�
	while((dirp = readdir(dp)) != NULL){
		// . ���丮�� .. ���丮�� �ǳʶڴ�.
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		// �й� ���丮 ��� ����
		snprintf(tmp, BUFLEN, "%s/%s", stuDir, dirp->d_name);

		// �й� ���丮�� ���� stat ����ü ȹ��
		stat(tmp, &statbuf);

		// �й� ���丮 ��ΰ� ������ ���丮�� �´��� �˻�
		if(S_ISDIR(statbuf.st_mode))
			// �й� ���̺� �й� �߰�
			strcpy(id_table[num++], dirp->d_name);
		else
			// ���丮�� �ƴϸ� �ǳʶڴ�.
			continue;
	}
	closedir(dp);	// ���丮 �ݱ�

	StudentTotal = num;

	// �й� ���̺� ����
	sort_idTable(num);
}

void sort_idTable(int size)
{
	int i, j;
	char tmp[10];

	// �й� ���̺� ��ȸ
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 -i; j++){
			// �й� ���̺� ���� ���ķ� ����
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

	// ���� ���̺� ��ȸ
	for(i = 0; i < size - 1; i++){
		for(j = 0; j < size - 1 - i; j++){
			// ���� ���̺� ���� ���ķ� ����

			// ���� ���� ���ϸ�� ���� ���� ���ϸ��� Ư����ȣ(-, .)�� Ȯ���ڸ� ������ �������� ���� ��ȣ�� ȹ��
			get_qname_number(score_table[j].qname, &num1_1, &num1_2);
			get_qname_number(score_table[j+1].qname, &num2_1, &num2_2);

			// ���� ���� ��ȣ�� ���� ���� ��ȣ�� ��
			if((num1_1 > num2_1) || ((num1_1 == num2_1) && (num1_2 > num2_2))){
				// ���ڸ����� ���� ���� ��ȣ�� ���� ���� ��ȣ���� ũ�ų� ������ ���� ��ȯ
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

	// ���� ���ϸ� ���� �� '-'�� '.' ���� �и� �� ���ڸ� ���������� ��ȯ
	strncpy(dup, qname, strlen(qname));
	*num1 = atoi(strtok(dup, "-."));
	
	// '-'�� '.'���� �и� �� �и��� ���ڰ� ������ 0, ������ ���ڸ� ���������� ��ȯ
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
		// ���� ���̺� Ÿ�� ����
		printf("score_table.csv file doesn't exist in \"%s\"!\n", ansDir);
		printf("1. input blank question and program question's score. ex) 0.5 1\n");
		printf("2. input all question's score. ex) Input value of 1-1: 0.1\n");
		printf("select type >> ");
		scanf("%d", &num);

		// �ùٸ��� ���� �Է�
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


	// ä�� ��� ���� ����
	if(nOption == true)
	{
		if((fd = creat(csvName, 0666)) < 0)
		{
			fprintf(stderr, "creat error for %s", csvName);
			return;
		}
		write_first_row(fd);	// ���� ���̺� csv ���Ͽ� column �ۼ�
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
		write_first_row(fd);	// ���� ���̺� csv ���Ͽ� column �ۼ�
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


	// �й� ���̺� ��ȸ
	for(num = 0; num < size; num++)
	{
		// �й� ���̺� ������� ����
		if(!strcmp(id_table[num], ""))
			break;

		if(sOption == false)
		{
			// �� �й��� "�й�, " �������� �ۼ�
			snprintf(tmp, FILELEN, "%s,", id_table[num]);
			write(fd, tmp, strlen(tmp));
		}

		memset((WrongList + num)->wrongProblem, 0, FILELEN);
		strcpy((WrongList + num)->wrongProblem, "wrong problem : ");

		// �л��� ä�� ���� ����
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

		printf("Total average : %.2f\n", score / studentParams);	// ��� ���� ���
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

		snprintf(tmp, MAX_PATH, "%s/%s/%s", stuDir, id, score_table[i].qname);		// �л��� ������ �� ������ ��� ���� ��� ����

		// �л� ���� ��� ���� ���� ���� �˻�
		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			// ���� ������ �ؽ�Ʈ �����̳� �ҽ��ڵ� ������ �ƴϸ� �ǳʶڴ�
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			// �ؽ�Ʈ �����̸� ��ĭä��� �����̹Ƿ� score_blank ����
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			
			// �ҽ��ڵ� �����̸� ���α׷� �ۼ� �����̹Ƿ� score_program ����
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

		snprintf(tmp, MAX_PATH, "%s/%s/%s", stuDir, id, score_table[i].qname);		// �л��� ������ �� ������ ��� ���� ��� ����

		// �л� ���� ��� ���� ���� ���� �˻�
		if(access(tmp, F_OK) < 0)
			result = false;
		else
		{
			// ���� ������ �ؽ�Ʈ �����̳� �ҽ��ڵ� ������ �ƴϸ� �ǳʶڴ�
			if((type = get_file_type(score_table[i].qname)) < 0)
				continue;
			
			// �ؽ�Ʈ �����̸� ��ĭä��� �����̹Ƿ� score_black ����
			if(type == TEXTFILE)
				result = score_blank(id, score_table[i].qname);
			
			// �ҽ��ڵ� �����̸� ���α׷� �ۼ� �����̹Ƿ� score_program ����
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

	write(fd, ",", 1);	// ������ ù ��, ���� ó���� ,�� ����

	for(i = 0; i < size; i++){
		// ������ 0���̸� ����
		if(score_table[i].score == 0)
			break;
		
		// ���ھ� ����ü���� �� ���� ���ϸ��� �����ͼ� ���� ���̺� ���Ͽ� "���� ���ϸ�, " �������� �ۼ�
		snprintf(tmp, BUFLEN, "%s,", score_table[i].qname);
		write(fd, tmp, strlen(tmp));
	}
	write(fd, "sum\n", 4);	// ���� ���̺� ���� ���� �������� "sum" ���ڿ� �ۼ�
}

char *get_answer(int fd, char *result)
{
	char c;
	int idx = 0;

	// ���� ������ result �޸� �ʱ�ȭ
	memset(result, 0, BUFLEN);

	// ���µ� ������ 1����Ʈ�� ��� �б�
	while(read(fd, &c, 1) > 0)
	{
		// �ݷ�(:) ������ ����
		if(c == ':')
			break;
		
		// ���� ���ڸ� result�� ����
		result[idx++] = c;
	}

	// ������ ��� �о �������� ���๮�ڰ� ������ �ι��ڷ� ����
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

	// ���� ���ϸ� �޸� �ʱ�ȭ
	memset(qname, 0, sizeof(qname));

	// filename���� Ȯ���ڸ� ������ ���ϸ��� qname���� ����
	memcpy(qname, filename, strlen(filename) - strlen(strrchr(filename, '.')));

	// �л��� ���� ��� ���� ��� ����
	snprintf(tmp, MAX_PATH, "%s/%s/%s", stuDir, id, filename);

	// �л��� ���� ��� ������ �б� �������� ����
	fd_std = open(tmp, O_RDONLY);

	// �л��� ������ ���� ���� �Ŀ� s_answer�� ����
	strcpy(s_answer, get_answer(fd_std, s_answer));

	// ���� ����� ������� ���� ó��
	if(!strcmp(s_answer, "")){
		close(fd_std);
		return false;
	}

	// ��ȣ( '(', ')' ) ���� ���� �˻�
	// �л��� ������ �信 (�� )�� ������ ���� �ٸ��� �����ϸ� ���� ó��
	if(!check_brackets(s_answer)){
		close(fd_std);
		return false;
	}

	// �л� ����� �պκк��� �������� ä���� �κ��� ������ ������ ����
	strcpy(s_answer, ltrim(rtrim(s_answer)));

	// �л� ����� ���� �������� �����ݷ��� �����ϴ��� �˻�
	if(s_answer[strlen(s_answer) - 1] == ';'){
		// �÷��� Ȱ��ȭ
		has_semicolon = true;
		// �����ݷ��� �ι��ڷ� ����
		s_answer[strlen(s_answer) - 1] = '\0';
	}

	// �л� ��� ���ڿ��� ��ū���� �и�
	if(!make_tokens(s_answer, tokens)){
		close(fd_std);
		return false;
	}

	idx = 0;
	// �и��� ��ū���� Ʈ�� ����
	std_root = make_tree(std_root, tokens, &idx, 0);

	// ���� ���� ����
	snprintf(tmp, MAX_PATH, "%s/%s", ansDir, filename);
	fd_ans = open(tmp, O_RDONLY);

	while(1)
	{
		ans_root = NULL;
		result = true;

		// tokens ��ü �ʱ�ȭ
		for(idx = 0; idx < TOKEN_CNT; idx++)
			memset(tokens[idx], 0, sizeof(tokens[idx]));

		// ������ ���� �Ŀ� a_answer�� ����
		strcpy(a_answer, get_answer(fd_ans, a_answer));

		// a_answer�� ������� ����
		if(!strcmp(a_answer, ""))
			break;

		// �պκа� �޺κ��� ������ ��� ����
		strcpy(a_answer, ltrim(rtrim(a_answer)));

		// �л� ��ȿ� �����ݷ��� �������� �ʾҴ� ���
		if(has_semicolon == false){
			// ������ ���� �������� �����ݷ��̸� �ǳʶڴ�
			if(a_answer[strlen(a_answer) -1] == ';')
				continue;
		}

		// �л� ��ȿ� �����ݷ��� �����ߴ� ���
		else if(has_semicolon == true)
		{
			// ������ ���� �������� �����ݷ��� �ƴ϶�� �ǳʶڴ�
			if(a_answer[strlen(a_answer) - 1] != ';')
				continue;
			
			// ������ ���� �������� �����ݷ��̸� �����ݷ��� �ι��ڷ� ����
			else
				a_answer[strlen(a_answer) - 1] = '\0';
		}

		// ���� ���ڿ��� ��ū���� �и�
		if(!make_tokens(a_answer, tokens))
			continue;

		idx = 0;
		// �и��� ��ū���� Ʈ�� ����
		ans_root = make_tree(ans_root, tokens, &idx, 0);

		// �л� ��� Ʈ���� ���� Ʈ���� ��
		compare_tree(std_root, ans_root, &result);

		// ��� �񱳰� �����ϸ� ������ �ݰ�, Ʈ�� ����� �޸� ��ȯ
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
	
	// ���� �ݱ�
	close(fd_std);
	close(fd_ans);

	// Ʈ�� ����� �޸� ��ȯ
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

	// ���α׷� ������ ����
	compile = compile_program(id, filename);

	// ������ ����� �����̸� false ����
	if(compile == ERROR || compile == false)
		return false;
	
	// �ش� ���α׷� ����
	result = execute_program(id, filename);

	// ���� ����� �����̸� false ����
	if(!result)
		return false;

	// ������ ����� �����̸� �ش� �� ����
	if(compile < 0)
		return compile;

	return true;
}

int is_thread(char *qname)
{
	int i;
	int size = sizeof(threadFiles) / sizeof(threadFiles[0]);


	// t �ɼ��� Ȱ��ȭ �����̸鼭 ���� ��ȣ�� �Էµ��� ���� ��� true ����
	if(tOption == true && strcmp(threadFiles[0], "") == 0)
	{
		return true;
	}

	// �Էµ� ���� ��ȣ�鿡 ���ؼ� true ����
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
	
	// thread �ɼ��� �������� �˻�
	isthread = is_thread(qname);

	// ���� ���� ���ϸ� ��ο� �ش� ������ �������ϸ� ��� ����
	snprintf(tmp_f, MAX_PATH, "%s/%s", ansDir, filename);
	snprintf(tmp_e, MAX_PATH, "%s/%s.exe", ansDir, qname);

	tmp_e_ptr = tmp_e;
	tmp_f_ptr = tmp_f;

	// t �ɼ��� Ȱ��ȭ �����̸� gcc ������ �ɼǿ� lpthread �߰�
	if(tOption && isthread)
		// sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s -lpthread", tmp_e_ptr, tmp_f_ptr);
	
	// t �ɼ��� ��Ȱ��ȭ �����̸� gcc ������ �ɼǿ� lpthread�� �߰����� �ʴ´�
	else
		// sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s", tmp_e_ptr, tmp_f_ptr);

	// �ش� ������ ���� error ���� ��� �ؽ�Ʈ ���� ����
	snprintf(tmp_e, MAX_PATH, "%s/%s_error.txt", ansDir, qname);
	fd = creat(tmp_e, 0666);

	// error ���� ��� ���ϰ� ǥ�ؿ����� redirection ����
	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);
	unlink(tmp_e);

	if(size > 0)
		return false;

	// �л� ��� ���ϸ� ��ο� �ش� ������ �������ϸ� ��� ����
	snprintf(tmp_f, MAX_PATH, "%s/%s/%s", stuDir, id, filename);
	snprintf(tmp_e, MAX_PATH, "%s/%s/%s.stdexe", stuDir, id, qname);

	// t �ɼ��� Ȱ��ȭ �����̸� gcc ������ �ɼǿ� lpthread �߰�
	if(tOption && isthread)
		// sprintf(command, "gcc -o %s %s -lpthread", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s -lpthread", tmp_e_ptr, tmp_f_ptr);
	
	// t �ɼ��� ��Ȱ��ȭ �����̸� gcc ������ �ɼǿ� lpthread�� �߰����� �ʴ´�
	else
		// sprintf(command, "gcc -o %s %s", tmp_e, tmp_f);
		snprintf(command, MAX_PATH, "gcc -o %s %s", tmp_e_ptr, tmp_f_ptr);

	// �ش� ������ ���� error ���� ��� �ؽ�Ʈ ���� ����
	snprintf(tmp_f, MAX_PATH, "%s/%s/%s_error.txt", stuDir, id, qname);
	fd = creat(tmp_f, 0666);

	// error ���� ��� ���ϰ� ǥ�ؿ����� redirection ����
	redirection(command, fd, STDERR);
	size = lseek(fd, 0, SEEK_END);
	close(fd);

	if(size > 0){

		// e �ɼ��� Ȱ��ȭ ������ ���
		if(eOption)
		{
			// error ���� ��� ���� ���� ���丮 ��θ� ����
			snprintf(tmp_e, MAX_PATH, "%s/%s", errorDir, id);

			// �ش� ���丮�� �������� ������ ���丮�� ���� ����
			if(access(tmp_e, F_OK) < 0)
				mkdir(tmp_e, 0755);

			// error ���� ��� ���� ���� ��θ� ����
			snprintf(tmp_e, MAX_PATH, "%s/%s/%s_error.txt", errorDir, id, qname);

			// ������ ������ �л� ��ȿ� ���� error ���� ��� ���� �̸��� error ������ ������ ��� �̸����� ����
			rename(tmp_f, tmp_e);

			// warning �˻�
			result = check_error_warning(tmp_e);
		}
		else{
			
			// warning �˻� �� ���� ����
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

	// �Է����� ���� ������ �б� �������� ����
	if((fp = fopen(filename, "r")) == NULL){
		fprintf(stderr, "fopen error for %s\n", filename);
		return false;
	}

	// ������ ���Ͽ��� ������ �о�´�
	while(fscanf(fp, "%s", tmp) > 0){

		// �о�� ������ error: �� ��쿡�� ERROR ����
		if(!strcmp(tmp, "error:"))
			return ERROR;
		
		// �о�� ������ WARNING �� ��쿡�� �����Ѵ�
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

	// ������ ���� ��� ���� ��� ���� �� �ش� ���� ����
	snprintf(ans_fname, MAX_PATH, "%s/%s.stdout", ansDir, qname);
	fd = creat(ans_fname, 0666);

	// ���� ���� ���� ��� ���� �� ���� ���� ��� ���ϰ� ǥ��������� redirection ����
	snprintf(tmp, MAX_PATH, "%s/%s.exe", ansDir, qname);
	redirection(tmp, fd, STDOUT);
	close(fd);

	// �л� ����� ���� ��� ���� ��� ���� �� �ش� ���� ����
	snprintf(std_fname, MAX_PATH, "%s/%s/%s.stdout", stuDir, id, qname);
	fd = creat(std_fname, 0666);

	// �л� ��� ���� ������ ��׶��� �����ϱ� ���� ���ڿ� ����
	snprintf(tmp, MAX_PATH, "%s/%s/%s.stdexe &", stuDir, id, qname);

	// ���� �ð� ���
	start = time(NULL);

	// �л� ��� ���� ��� ���ϰ� ǥ��������� redirection ����
	redirection(tmp, fd, STDOUT);
	
	// ��÷�ϴ� �ش� ������ ���� ���ϸ� ����
	snprintf(tmp, MAX_PATH, "%s.stdexe", qname);
	while((pid = inBackground(tmp)) > 0){

		// ���� �ð� ���
		end = time(NULL);

		// ���� ���� �ð��� ���Ͽ� �ʰ� �� ���� ����
		if(difftime(end, start) > OVER){
			kill(pid, SIGKILL);
			close(fd);
			return false;
		}
	}

	close(fd);

	// �л��� ��� ���� �������� ���� ���� �������� ���Ѵ�
	return compare_resultfile(std_fname, ans_fname);
}

pid_t inBackground(char *name)
{
	pid_t pid;
	char command[64];
	char tmp[64];
	int fd;
	off_t size;
	
	// background.txt ���� ����
	memset(tmp, 0, sizeof(tmp));
	fd = open("background.txt", O_RDWR | O_CREAT | O_TRUNC, 0666);

	// command�� ps ��ɾ�� grep ��ɾ� ����
	sprintf(command, "ps | grep %s", name);

	// ������ ������ ���ϰ� ǥ������� redirection�ϰ� command�� ����
	redirection(command, fd, STDOUT);

	// ������ ���� �������� ���� ó������ ���� ����, ������ �о� �´�
	lseek(fd, 0, SEEK_SET);
	read(fd, tmp, sizeof(tmp));

	// �о�� ������ ���ٸ� ������ ������ background.txt ���� �����ϰ�, 0 ����
	if(!strcmp(tmp, "")){
		unlink("background.txt");
		close(fd);
		return 0;
	}

	// �о�� ������ �������� �ڸ� �Ŀ� ����ȯ
	pid = atoi(strtok(tmp, " "));
	close(fd);

	// background.txt ���� �����ϰ�, ������ ���� �������� �ڸ� pid ���� ����
	unlink("background.txt");
	return pid;
}

int compare_resultfile(char *file1, char *file2)
{
	int fd1, fd2;
	char c1, c2;
	int len1, len2;

	// �ش� ���� �б� �������� ����
	fd1 = open(file1, O_RDONLY);
	fd2 = open(file2, O_RDONLY);

	while(1)
	{
		// ������ ���Ͽ��� ������ �ƴ� ���ڸ� ã�´�
		while((len1 = read(fd1, &c1, 1)) > 0){
			if(c1 == ' ') 
				continue;
			else 
				break;
		}

		// ������ ���Ͽ��� ������ �ƴ� ���ڸ� ã�´�
		while((len2 = read(fd2, &c2, 1)) > 0){
			if(c2 == ' ') 
				continue;
			else 
				break;
		}
		
		// 2���� ���� ��� ������ �о����� �ݺ��� ����
		if(len1 == 0 && len2 == 0)
			break;

		// ������ ���Ͽ��� ���� ���ڵ��� ���ĺ� �빮���̸� �ҹ��ڷ� �����Ѵ�
		to_lower_case(&c1);
		to_lower_case(&c2);

		// 2���� ���Ͽ��� ���� �о�� ���ڰ� ���� ���� ������
		// ������ ���� �ݰ�, false ����
		if(c1 != c2){
			close(fd1);
			close(fd2);
			return false;
		}
	}

	// 2���� ������ ������ ��� ��ġ�ϱ� ������ ������ ���� �ݰ�, true ����
	close(fd1);
	close(fd2);
	return true;
}

void redirection(char *command, int new, int old)
{
	int saved;

	// ���� ��ũ���͸� ����
	saved = dup(old);
	dup2(new, old);

	// ���Ӱ� ������ ���� ��ũ���ͷ� command ����
	system(command);

	// ������ ���� ��ũ���ͷ� �ٽ� ���� ���´�
	dup2(saved, old);
	close(saved);
}

int get_file_type(char *filename)
{
	char *extension = strrchr(filename, '.');	// . �������� ���ϸ�� Ȯ���� �и�

	// �ؽ�Ʈ ����
	if(!strcmp(extension, ".txt"))
		return TEXTFILE;
	// �ҽ��ڵ� ����
	else if (!strcmp(extension, ".c"))
		return CFILE;
	// �ؽ�Ʈ ���ϰ� �ҽ��ڵ� ������ �ƴ� ��� ����
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
	
	// path�� �ش��ϴ� ���丮 ����
	if((dp = opendir(path)) == NULL)
		return;

	// ������ ������ ���丮 ��ȸ�ϸ鼭 �б�
	while((dirp = readdir(dp)) != NULL)
	{
		// .�� ..�� �Ѿ��
		if(!strcmp(dirp->d_name, ".") || !strcmp(dirp->d_name, ".."))
			continue;

		// ���� ���� �Ǵ� ���丮�� ��� ����
		snprintf(tmp, MAX_PATH, "%s/%s", path, dirp->d_name);

		// stat ����ü ���
		if(lstat(tmp, &statbuf) == -1)
			continue;

		// ���� ���丮�̸� ���ȣ���Ͽ� ���� ���丮 ����
		if(S_ISDIR(statbuf.st_mode))
			rmdirs(tmp);
		
		// ���� �����̸� ����
		else
			unlink(tmp);
	}

	// ������ ���丮 �ݱ�
	closedir(dp);

	// path�� �ش��ϴ� ���丮 ����
	rmdir(path);
}

void to_lower_case(char *c)
{
	// ���ĺ� �빮�ڸ� �ҹ��ڷ� ����
	if(*c >= 'A' && *c <= 'Z')
		*c = *c + 32;
}

void print_usage()
{
	// ���α׷� usage ���
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

	// �л��� Ʋ�� ���� ��ȸ�ϸ鼭 Ʋ�� ���� ��ȣ ���
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

	// �л��� Ʋ�� ���� ����Ʈ ��ȸ�ϸ鼭 �޸� ��ȯ
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

	// �л� ä�� ��� ���� ����Ʈ ��ȸ�ϸ鼭 �޸� ��ȯ
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