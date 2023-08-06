#ifndef MAIN_H_
#define MAIN_H_

#ifndef true
	#define true 1
#endif
#ifndef false
	#define false 0
#endif
#ifndef STDOUT
	#define STDOUT 1
#endif
#ifndef STDERR
	#define STDERR 2
#endif
#ifndef TEXTFILE
	#define TEXTFILE 3
#endif
#ifndef CFILE
	#define CFILE 4
#endif
#ifndef OVER
	#define OVER 5
#endif
#ifndef WARNING
	#define WARNING -0.1
#endif
#ifndef ERROR
	#define ERROR 0
#endif

#define MAX_PATH 4096
#define FILELEN 128
#define BUFLEN 1024
#define SNUM 100
#define QNUM 100
#define ARGNUM 5

// 점수 테이블 구조체
struct ssu_scoreTable{
	char qname[FILELEN];	// 정답 디렉토리 내 각 문제의 정답 파일명
	double score;			// 해당 문제의 배점
};

void ssu_score(int argc, char *argv[]);
int check_option(int argc, char *argv[]);
void print_usage();

void score_students();
double score_student_with_s(int fd, char *id, int idx);
double score_student_not_s(int fd, char *id, int idx);
void write_first_row(int fd);

char *get_answer(int fd, char *result);
int score_blank(char *id, char *filename);
double score_program(char *id, char *filename);
double compile_program(char *id, char *filename);
int execute_program(char *id, char *filname);
pid_t inBackground(char *name);
double check_error_warning(char *filename);
int compare_resultfile(char *file1, char *file2);

void do_cOption(FILE *fp, char (*ids)[FILELEN]);
void do_pOption_with_s(char *stdId, char (*ids)[FILELEN]);
void do_pOption_not_s(char *stdId, char (*ids)[FILELEN]);
int do_mOption();
void do_nOption();
void do_sOption();
int is_exist(char (*src)[FILELEN], char *target);

void sort_id_ascending();
void sort_id_descending();
void sort_score_ascending();
void sort_score_descending();
void Save_Sorted_Score(int fd);

int is_thread(char *qname);
void redirection(char *command, int newfd, int oldfd);
int get_file_type(char *filename);
void rmdirs(const char *path);
void to_lower_case(char *c);

void set_scoreTable(char *ansDir);
void read_scoreTable(char *path);
void make_scoreTable(char *ansDir);
void write_scoreTable(char *filename);
void set_idTable(char *stuDir);
int get_create_type();

void sort_idTable(int size);
void sort_scoreTable(int size);
void get_qname_number(char *qname, int *num1, int *num2);

void Print_WrongList();

void Delete_WrongList();
void Delete_SortList();

#endif
