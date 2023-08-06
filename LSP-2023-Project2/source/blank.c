#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
#include "blank.h"

// 데이터 타입 정의
char datatype[DATATYPE_SIZE][MINLEN] = {"int", "char", "double", "float", "long"
			, "short", "ushort", "FILE", "DIR","pid"
			,"key_t", "ssize_t", "mode_t", "ino_t", "dev_t"
			, "nlink_t", "uid_t", "gid_t", "time_t", "blksize_t"
			, "blkcnt_t", "pid_t", "pthread_mutex_t", "pthread_cond_t", "pthread_t"
			, "void", "size_t", "unsigned", "sigset_t", "sigjmp_buf"
			, "rlim_t", "jmp_buf", "sig_atomic_t", "clock_t", "struct"};


// 연산자 기호 정의
operator_precedence operators[OPERATOR_CNT] = {
	{"(", 0}, {")", 0}
	,{"->", 1}	
	,{"*", 4}	,{"/", 3}	,{"%", 2}	
	,{"+", 6}	,{"-", 5}	
	,{"<", 7}	,{"<=", 7}	,{">", 7}	,{">=", 7}
	,{"==", 8}	,{"!=", 8}
	,{"&", 9}
	,{"^", 10}
	,{"|", 11}
	,{"&&", 12}
	,{"||", 13}
	,{"=", 14}	,{"+=", 14}	,{"-=", 14}	,{"&=", 14}	,{"|=", 14}
};

void compare_tree(node *root1,  node *root2, int *result)
{
	node *tmp;
	int cnt1, cnt2;

	// 비교할 2개의 트리 중에서 1개라도 없으면 종료
	if(root1 == NULL || root2 == NULL){
		*result = false;
		return;
	}

	// root1이 대소 비교 연산자인 경우
	if(!strcmp(root1->name, "<") || !strcmp(root1->name, ">") || !strcmp(root1->name, "<=") || !strcmp(root1->name, ">=")){

		// 2개의 트리가 같지 않은 경우
		if(strcmp(root1->name, root2->name) != 0){

			// root2도 대소 비교 연산자인 경우

			if(!strncmp(root2->name, "<", 1))
				strncpy(root2->name, ">", 1);

			else if(!strncmp(root2->name, ">", 1))
				strncpy(root2->name, "<", 1);

			else if(!strncmp(root2->name, "<=", 2))
				strncpy(root2->name, ">=", 2);

			else if(!strncmp(root2->name, ">=", 2))
				strncpy(root2->name, "<=", 2);

			// 자식 노드 변경
			root2 = change_sibling(root2);
		}
	}

	// root1, root가 서로 같지 않으면 false 리턴
	if(strcmp(root1->name, root2->name) != 0){
		*result = false;
		return;
	}

	// 둘 중에서 하나라도 자식 노드가 없으면 false 리턴
	if((root1->child_head != NULL && root2->child_head == NULL)
		|| (root1->child_head == NULL && root2->child_head != NULL)){
		*result = false;
		return;
	}

	// root1의 자식 노드가 존재하는 경우
	else if(root1->child_head != NULL){

		// 각각의 자식 노드 개수가 서로 같지 않으면 false 리턴
		if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
			*result = false;
			return;
		}

		// root1이 == 연산자나 != 연산자인 경우
		if(!strcmp(root1->name, "==") || !strcmp(root1->name, "!="))
		{
			// 각각의 자식 노드끼리 비교한다
			compare_tree(root1->child_head, root2->child_head, result);

			// 비교 결과가 false인 경우
			if(*result == false)
			{
				// root2의 자식 노드 변경 후에 다시 비교한다
				*result = true;
				root2 = change_sibling(root2);
				compare_tree(root1->child_head, root2->child_head, result);
			}
		}

		// root1이 +, *, |, &, ||,, && 연산자 중 한개에 해당하는 경우
		else if(!strcmp(root1->name, "+") || !strcmp(root1->name, "*")
				|| !strcmp(root1->name, "|") || !strcmp(root1->name, "&")
				|| !strcmp(root1->name, "||") || !strcmp(root1->name, "&&"))
		{
			// 각각의 자식 노드 개수가 서로 같지 않으면 false 리턴
			if(get_sibling_cnt(root1->child_head) != get_sibling_cnt(root2->child_head)){
				*result = false;
				return;
			}

			// root2의 자식 노드를 가져온다
			tmp = root2->child_head;

			// 가장 처음의 노드로 이동
			while(tmp->prev != NULL)
				tmp = tmp->prev;

			while(tmp != NULL)
			{
				// root1의 자식 노드가 tmp 노드를 비교
				compare_tree(root1->child_head, tmp, result);
			
				if(*result == true)
					break;
				
				// 비교 결과가 false이면 다시 tmp 노드 기준으로 바로 다음 노드로 이동
				else{
					if(tmp->next != NULL)
						*result = true;
					tmp = tmp->next;
				}
			}
		}

		// 그 외의 문자인 경우
		else{

			// roo1과 roo2의 자식 노드끼리 비교
			compare_tree(root1->child_head, root2->child_head, result);
		}
	}	


	// root1의 다음 노드가 존재하는 경우
	if(root1->next != NULL){

		// 각각의 자식 노드 개수가 서로 같지 않으면 false 리턴
		if(get_sibling_cnt(root1) != get_sibling_cnt(root2)){
			*result = false;
			return;
		}

		// 자식 노드 개수가 서로 같은 경우
		if(*result == true)
		{
			// roo1의 연산자 문자를 가져온다
			tmp = get_operator(root1);
	
			// 가져온 문자가 +, *, |, &, ||, && 중 한개인 경우
			if(!strcmp(tmp->name, "+") || !strcmp(tmp->name, "*")
					|| !strcmp(tmp->name, "|") || !strcmp(tmp->name, "&")
					|| !strcmp(tmp->name, "||") || !strcmp(tmp->name, "&&"))
			{	
				// root2 기준으로 변경
				tmp = root2;
	
				// tmp 기준으로 가장 처음 노드로 이동
				while(tmp->prev != NULL)
					tmp = tmp->prev;

				while(tmp != NULL)
				{
					// root1의 다음 노드들과 tmp 노드를 비교
					compare_tree(root1->next, tmp, result);

					if(*result == true)
						break;
					
					// 비교 결과가 false이면 tmp 기준으로 바로 다음 노드로 이동
					else{
						if(tmp->next != NULL)
							*result = true;
						tmp = tmp->next;
					}
				}
			}

			// 그 외의 문자인 경우
			else
				// root1과 root2의 다음 노드끼리 비교
				compare_tree(root1->next, root2->next, result);
		}
	}
}

int make_tokens(char *str, char tokens[TOKEN_CNT][MINLEN])
{
	char *start, *end;
	char tmp[BUFLEN];
	char str2[BUFLEN];
	char *op = "(),;><=!|&^/+-*\""; 
	int row = 0;
	int i;
 	int isPointer;
	int lcount, rcount;
	int p_str;
	
	// str : 학생 답안
	// tokens : ssu_score에서 넘겨준 2차원 char 배열

	// tokens 메모리 초기화
	clear_tokens(tokens);

	// start는 학생 답안을 가리킴
	start = str;
	
	// 학생 답안에서 gcc 또는 데이터 타입 포함 여부 검사
	if(is_typeStatement(str) == 0) 
		return false;	
	
	while(1)
	{
		// 학생 답안에서 사전에 정의된 연산자들이 포함되어 있는지 검사
		if((end = strpbrk(start, op)) == NULL)
			// 정의된 연산자들이 검출되지 않으면 종료
			break;

		// 학생 답안 문자열의 현재 참조 포인터가 검출된 연산자와 같은 경우
		if(start == end){

			// 학생 답안의 2바이트와 "--" 문자열, "++" 문자열을 비교
			if(!strncmp(start, "--", 2) || !strncmp(start, "++", 2)){
				// 학생 답안의 4바이트와 "++++" 문자열, "----" 문자열을 비교
				if(!strncmp(start, "++++", 4)||!strncmp(start,"----",4))
					// "++++" 또는 "----"가 있으면 false 리턴
					return false;

				// ex) ++a
				// 학생 답안에서 2바이트의 연산자 기호 뒤에 위치한 한 글자에 대해 character 검사
				// 검사하기 전에 연산자 기호와 뒤에 위치한 글자 사이에 공백이 있으면 ltrim으로 공백 제거
				if(is_character(*ltrim(start + 2))){
					// tokens에 대해 character 검사
					if(row > 0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]))
						return false; //ex) ++a++

					// 검출된 연산자 기호 이후에 다른 연산자가 더 존재하는지 검사
					end = strpbrk(start + 2, op);
					
					// 연산자가 추가로 더 검출되지 않은 경우
					if(end == NULL)
						// end를 str의 가장 마지막 위치로 지정
						end = &str[strlen(str)];
					
					// start 즉, 학생 답안 문자열이 검출한 연산자를 만날 때까지 반복
					while(start < end) {
						// start의 이전 문자가 공백이면서 tokens에 대한 is_character가 참일 때 false 리턴
						if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
							return false;
						
						// start의 현재 문자가 공백이 아닐 때
						else if(*start != ' ')
							// tokens에 start의 문자 1바이트를 결합
							strncat(tokens[row], start, 1);
						
						// start 인덱스 증가
						start++;	
					}
				}
				// ex) a++
				// 2바이트 연산자 기호 뒤에 위치한 글자
				else if(row>0 && is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					if(strstr(tokens[row - 1], "++") != NULL || strstr(tokens[row - 1], "--") != NULL)	
						return false;

					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row - 1], tmp);
					start += 2;
					row--;
				}
				else{
					memset(tmp, 0, sizeof(tmp));
					strncpy(tmp, start, 2);
					strcat(tokens[row], tmp);
					start += 2;
				}
			}

			// 학생 답안이 아래의 연산자 기호들로 시작할 경우
			else if(!strncmp(start, "==", 2) || !strncmp(start, "!=", 2) || !strncmp(start, "<=", 2)
				|| !strncmp(start, ">=", 2) || !strncmp(start, "||", 2) || !strncmp(start, "&&", 2) 
				|| !strncmp(start, "&=", 2) || !strncmp(start, "^=", 2) || !strncmp(start, "!=", 2) 
				|| !strncmp(start, "|=", 2) || !strncmp(start, "+=", 2)	|| !strncmp(start, "-=", 2) 
				|| !strncmp(start, "*=", 2) || !strncmp(start, "/=", 2)){

				// tokens의 현재 행(row)에 2바이트의 해당 연산자 기호 복사, 학생 답안 문자열은 연산자 기호 뒤로 인덱스 이동
				strncpy(tokens[row], start, 2);
				start += 2;
			}
			
			// 학생 답안이 -> 기호로 시작할 경우
			else if(!strncmp(start, "->", 2))
			{
				// 검출된 -> 기호 뒤에 정의된 연산자(op) 기호가 존재하는지 검사
				end = strpbrk(start + 2, op);

				// 연산자가 추가로 검출되지 않은 경우
				if(end == NULL)
					// end는 학생 답안 문자열의 가장 마지막을 가리킴
					end = &str[strlen(str)];

				// 학생 답안 문자열을 끝까지 순회
				while(start < end){
					// 공백 문자가 아닌 경우
					if(*start != ' ')
						// tokens의 이전 행(row)에 해당 문자 결합
						strncat(tokens[row - 1], start, 1);
					// 다음 문자로 인덱스 증가
					start++;
				}

				// 행 번호 감소 시킴
				row--;
			}

			// 검출된 연산자가 &일 때
			else if(*end == '&')
			{
				// ex) &a (address)
				// tokens에 데이터를 처음 넣는 경우 또는 이전 tokens에 연산자(op)들이 있는 경우
				if(row == 0 || (strpbrk(tokens[row - 1], op) != NULL)){
					// & 연산자 뒤에 다른 연산자 기호가 더 있는지 검사
					end = strpbrk(start + 1, op);
					// 다른 연산자 기호가 없는 경우
					if(end == NULL)
						// end는 학생 답안 문자열의 끝을 가리킴
						end = &str[strlen(str)];
					
					// 현재 tokens에 & 연산자를 결합하고 학생 답안 문자열은 다음 문자로 인덱스 증가
					strncat(tokens[row], start, 1);
					start++;

					// 학생 답안 문자열의 끝까지 순회
					while(start < end){
						// 학생 답안 문자열의 현재 문자 바로 앞이 공백문자이면서 현재 tokens의 마지막 문자가 & 기호가 아닌 경우 false 리턴
						if(*(start - 1) == ' ' && tokens[row][strlen(tokens[row]) - 1] != '&')
							return false;
						
						// 학생 답안 문자열의 현잼 문자가 공백이 아닐 경우
						else if(*start != ' ')
							// 1바이트의 해당 문자를 현재 tokens에 결합
							strncat(tokens[row], start, 1);
						
						// 학생 답안 문자열의 인덱스 증가
						start++;
					}
				}
				// ex) a & b (bit)
				// tokens에 다른 연산자(op)들이 아닌 다른 문자가 있는 경우
				else{
					// 현재 tokens에 & 연산자를 결합하고 학생 답안 문자열은 다음 문자로 인덱스 증가
					strncpy(tokens[row], start, 1);
					start += 1;
				}
				
			}

			// 검출된 연산자가 *일 때
		  	else if(*end == '*')
			{
				isPointer=0;

				// tokens에 데이터를 처음 넣는 경우가 아닌 경우
				// 즉, * 연산자 앞에 다른 문자가 존재하는 경우
				if(row > 0)
				{
					//ex) char** (pointer)
					// 사전에 정의된 데이터타입 개수만큼 순회
					for(i = 0; i < DATATYPE_SIZE; i++) {
						// tokens에 이미 들어가 있는 데이터가 정의된 데이터타입 중에서 한 가지인지 검사
						if(strstr(tokens[row - 1], datatype[i]) != NULL){
							// 정의된 데이터타입 중 한 가지가 tokens에 이미 들어가 있다면
							// 이전 tokens 즉, 해당 데이터 타입 뒤에 * 연산자를 결합
							strcat(tokens[row - 1], "*");

							// 학생 답안 문자열을 * 연산자 뒤로 인덱스 증가
							start += 1;	
							// 플래그 변경
							isPointer = 1;
							break;
						}
					}

					// tokens에 데이터타입이 존재할 경우 continue
					if(isPointer == 1)
						continue;
					
					// 학생 답안 문자열의 다음 문자가 널문자가 아닌 경우
					if(*(start+1) !=0)
						// end는 학생 답안에서 다음 문자를 가리팀
						end = start + 1;

					// ex) a * **b (multiply then pointer)
					// 2개 이상의 토근이 있고 2개 이전의 토큰이 * 연산자이면서 이전 토큰이 * 연산자만으로 이루어져 있는 경우
					if(row>1 && !strcmp(tokens[row - 2], "*") && (all_star(tokens[row - 1]) == 1)){
						// 이전 tokens 뒤에 학생 답안 문자열에서 * 연산자 다음의 문자를 결합
						strncat(tokens[row - 1], start, end - start);

						// tokens 위치인 row는 직전에 결합했던 tokens의 위치로 변경
						row--;
					}
					
					// ex) a*b(multiply)
					// 이전 토큰이 is_character의 범위 안에 포함될 경우
					// 즉, 숫자 또는 알파벳 문자일 경우
					else if(is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1]) == 1){
						// 검출된 * 연산자 뒤에 위치한 문자를 이전 토큰에 결합
						strncat(tokens[row], start, end - start);   
					}

					// ex) ,*b (pointer)
					// 이전 토근에 연산자 기호(op)가 존재하는지 검사
					else if(strpbrk(tokens[row - 1], op) != NULL){
						// op가 있으면 현재 토큰에 * 연산자 뒤의 문자를 결합	
						strncat(tokens[row] , start, end - start); 
							
					}
					else
						// * 연산자 뒤의 문자를 현재 토큰에 결합
						strncat(tokens[row], start, end - start);

					// 학생 답안 문자열의 인덱스를 토큰과 마지막으로 결합한 문자의 다음 위치로 이동시킴
					start += (end - start);
				}

				// 토큰에 데이터를 처음 넣는 경우
			 	else if(row == 0)
				{
					// * 연산자 뒤에 다른 연산자 기호(op)가 포함되어 있는지 검사
					if((end = strpbrk(start + 1, op)) == NULL){
						// op가 검출되지 않았으면 현재 토큰에 학생 답안 문자열로부터 1바이트의 문자를 결합
						strncat(tokens[row], start, 1);

						// 학생 답안 문자열은 다음 문자로 인덱스 증가
						start += 1;
					}

					// * 연산자 뒤에 다른 op가 추가로 더 검출된 경우
					else{
						// 검출된 op까지 학생 답안 문자열 순회
						while(start < end){
							// * 연산자 이전 문자가 공백이면서 현재 토큰이 숫자 또는 알파벳 문자로 끝나는 경우 false 리턴
							if(*(start - 1) == ' ' && is_character(tokens[row][strlen(tokens[row]) - 1]))
								return false;
							
							// 학생 답안 문자열의 문자가 공백이 아닌 경우
							else if(*start != ' ')
								// 현재 토큰에 해당 문자 결합
								strncat(tokens[row], start, 1);
							// 학생 답안 문자열 인덱스 증가
							start++;
						}

						// 추가된 현재 토큰이 모두 * 연산자로 이루어져 있으면 토큰 위치를 이전 토큰으로 이동시킴
						if(all_star(tokens[row]))
							row--;
						
					}
				}
			}

			// 검출된 연산자가 (일 때
			else if(*end == '(')
			{
				lcount = 0;	// ( 연산자가 연속으로 존재할 때 (의 총 개수 ( ex. (((a+b))) 인 경우, lcount는 3 )
				rcount = 0;

				// 이미 추가된 토큰이 존재하고 이전 토큰이 & 또는 * 연산자인 경우
				if(row>0 && (strcmp(tokens[row - 1],"&") == 0 || strcmp(tokens[row - 1], "*") == 0)){
					// 검출된 ( 연산자 이후에 ( 연산자가 더 이상 검출되지 않을때까지 반복
					while(*(end + lcount + 1) == '(')
						lcount++;
					// start는 최초에 검출된 ( 연산자 이후에 추가로 더 검출된 ( 연산자들 중에서 가장 마지막 위치의 ( 연산자 위치를 가리킴
					start += lcount;

					// end는 start 문자열에서 가장 처음으로 검출되는 ) 연산자의 위치를 가리킴
					end = strpbrk(start + 1, ")");

					// ) 연산자가 검출되지 않았을 경우에는 false 리턴
					// ( 연산자는 존재하지만 ) 연산자는 존재하지 않는다는 의미임
					if(end == NULL)
						return false;
					
					// ) 연산자가 검출된 경우
					else{
						// 검출된 ) 연산자 이후에 ) 연산자가 더 이상 검출되지 않을때까지 반복
						while(*(end + rcount +1) == ')')
							rcount++;
						
						// end는 최초에 검출된 ) 연산자 이후에 추가로 더 검출된 ) 연산자들 중에서 가장 마지막 위치의 ) 연산자 위치를 가리킴
						end += rcount;

						// ( 연산자의 개수와 ) 연산자의 개수가 같지 않을 경우에는 false 리턴
						if(lcount != rcount)
							return false;

						// 2개 이상의 토큰이 존재하고 2개 이전의 토큰이 숫자와 알파벳 문자가 아니거나, 추가된 토근이 1개인 경우
						if( (row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1])) || row == 1){
							// 이전 토큰에 가장 마지막으로 검출된 ( 연산자 뒤에 위치한 문자를 결합
							strncat(tokens[row - 1], start + 1, end - start - rcount - 1);

							// 토큰의 위치를 이전 토큰의 위치로 이동시킴
							row--;

							// start는 가장 마지막으로 검출된 ) 연산자 뒤에 위치한 문자로 이동시킴
							start = end + 1;
						}
						// 추가된 토큰이 없거나, 2개 이전의 토큰이 숫자 또는 알파벳 문자인 경우
						else{
							// 현재 토큰에 가장 마지막 ( 연산자 뒤에 위치한 문자를 결합
							strncat(tokens[row], start, 1);

							// start는 해당 문자의 다음 문자를 가리킴
							start += 1;
						}
					}
						
				}
				// 추가된 토큰이 없거나 이전 토큰이 & 또는 * 연산자가 아닌 경우
				else{
					// 현재 토큰에 검출된 ( 연산자를 결합
					strncat(tokens[row], start, 1);

					// start는 ( 연산자의 다음 문자를 가리킴
					start += 1;
				}

			}

			// 검출된 연산자가 \"일 때
			else if(*end == '\"') 
			{
				// 학생 답안 문자열에서 \" 기호가 더 존재하는지 검사
				end = strpbrk(start + 1, "\"");
				
				// \"가 검출되지 않았으면 false 리턴
				if(end == NULL)
					return false;

				// \"가 검출되엇을 경우
				else{
					// 현재 토큰에 \"을 결합
					strncat(tokens[row], start, end - start + 1);

					// start는 \"의 다음 문자를 가리킴
					start = end + 1;
				}

			}

			// 그 외의 문자인 경우
			else{
				// ex) a++ ++ +b
				// ++ 기호가 중간에 한번 더 존재하면 false 리턴
				if(row > 0 && !strcmp(tokens[row - 1], "++"))
					return false;

				// ex) a-- -- -b
				// -- 기호가 중간에 한번 더 존재하면 false 리턴
				if(row > 0 && !strcmp(tokens[row - 1], "--"))
					return false;
	
				// 현재 토큰에 start를 연결
				strncat(tokens[row], start, 1);
				start += 1;
				
				// ex) -a or a, -b
				// -, + 기호나 --, ++ 기호인 경우
				if(!strcmp(tokens[row], "-") || !strcmp(tokens[row], "+") || !strcmp(tokens[row], "--") || !strcmp(tokens[row], "++")){


					// ex) -a or -a+b
					if(row == 0)
						row--;

					// ex) a+b = -c
					// 숫자나 알파벳 문자 포함 여부 검사
					else if(!is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])){
					
						// ++ 기호와 -- 기호가 둘 다 아닌 경우
						if(strstr(tokens[row - 1], "++") == NULL && strstr(tokens[row - 1], "--") == NULL)
							row--;
					}
				}
			}
		}

		// 학생 답안 문자열이 검출된 연산자로 시작하지 않을 경우
		else{
			// 이전 토큰이 * 연산자로만 이루어져 있고, 2개 이상의 토큰이 존재하면서 2개 이전의 토큰이 숫자나 알파벳 문자가 아닌 경우
			if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))
				// 토큰 위치를 이전 토큰으로 이동시킴
				row--;

			// 이전 토큰이 * 연산자로만 이루어져 있고 토큰이 1개만 존재하는 경우
			if(all_star(tokens[row - 1]) && row == 1)
				// 토큰 위치를 이전 토큰으로 이동시킴
				row--;
			
			// start의 현재 시작 위치부터 op가 검출된 위치 사이의 길이만큼 순회
			for(i = 0; i < end - start; i++){
				// start로부터 i번째 위치에 있는 문자가 '.'일 때
				if(i > 0 && *(start + i) == '.'){
					// 현재 토큰에 '.'을 결합
					strncat(tokens[row], start + i, 1);

					// '.' 문자 뒤에 공백이 있으면서 '.' 문자가 start부터 op 검출 위치 사이에 있을 동안 반복
					while( *(start + i +1) == ' ' && i< end - start )
						// i 인덱스를 증가시켜 start로부터 공백이 아닌 다른 문자를 찾음
						i++;
				}
				// start로부터 i번째 위치에 있는 문자가 공백문자일 때
				else if(start[i] == ' '){
					// 공백 외의 다른 문자를 찾을 때까지 반복
					while(start[i] == ' ')
						i++;
					break;
				}
				else
					// '.' 문자와 공백문자가 아닌 그 외의 문자일 때는 현재 토큰에 해당 문자를 결합
					strncat(tokens[row], start + i, 1);
			}

			// start의 첫번째 문자가 공백문자이면 start는 그 다음 문자를 가리킴
			if(start[0] == ' '){
				start += i;
				continue;
			}

			// start의 인덱스를 증가시켜 그 다음 문자를 가리킴
			start += i;
		}
		
		// 현재 토큰에 들어가 있는 문자열의 앞부분과 뒷부분에서 공백을 모두 제거하여 현재 토큰에 다시 복사
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));

		// 토큰이 1개 이상 있고, 현재 토큰의 마지막 문자가 숫자 또는 알파벳 문자이면서
		// 이전 토큰이 gcc 또는 데이터타입 중 한가지 이거나
		// 이전 토큰의 마지막 문자가 숫자 또는 알파벳 문자이거나
		// 이전 토큰의 마지막 문자가 '.'일 때
		 if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
				&& (is_typeStatement(tokens[row - 1]) == 2 
					|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
					|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.' ) ){

			// 토큰이 2개 이상 있으면서 2개 이전의 토큰이 ( 연산자일 때
			if(row > 1 && strcmp(tokens[row - 2],"(") == 0)
			{
				// 이전 토큰이 struct 혹은 unsigned가 아닌 경우에는 false 리턴
				if(strcmp(tokens[row - 1], "struct") != 0 && strcmp(tokens[row - 1],"unsigned") != 0)
					return false;
			}

			// 토큰이 2개개 있으면서 현재 토큰의 마지막 문자가 숫자 또는 알파벳 문자일 때
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				// 첫번째 토큰이 extern 혹은 unsigned, gcc 또는 데이터타입 중 한가지가 아닌 경우에는 false 리턴
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}

			// 토큰이 2개 이상 있으면서 이전 토큰이 gcc 또는 데이터타입 중 한가지일 때
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				// 2개 이전의 토큰이 unsigned 혹은 extern이 아닌 경우에는 false 리턴
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
			
		}

		// 토큰이 1개만 존재하면서 해당 토큰이 gcc인 경우
		if((row == 0 && !strcmp(tokens[row], "gcc")) ){
			// 토큰을 모두 비움
			clear_tokens(tokens);

			// 첫번째 토큰에 학생 답안 문자열을 복사
			strcpy(tokens[0], str);
			return 1;
		} 

		// 토큰 위치 증가
		// 즉, 토큰 개수 증가
		row++;
	}

	// 이전 토큰이 * 연산자로만 이루어져 있으면서 토큰이 2개 이상 있고, 2개 이전의 토큰의 마지막 문자가 숫자 또는 알파벳 문자가 아닌 경우
	if(all_star(tokens[row - 1]) && row > 1 && !is_character(tokens[row - 2][strlen(tokens[row - 2]) - 1]))
		// 토큰 위치를 이전 토큰으로 이동시킴
		row--;
	
	// 이전 토큰이 * 연산자로만 이루어져 있으면서 토큰이 2개만 존재하는 경우
	if(all_star(tokens[row - 1]) && row == 1)
		// 토큰 위치를 이전 토큰으로 이동시킴
		row--;

	// start의 전체 길이만큼 반복
	for(i = 0; i < strlen(start); i++)   
	{
		// start에서 공백문자를 찾은 경우
		if(start[i] == ' ')  
		{
			// 처음 공백문자를 찾은 위치에서부터 공백 이외의 다른 문자를 찾을 때까지 반복
			while(start[i] == ' ')
				i++;
			
			// start의 첫번째 문자가 공백문자인 경우
			if(start[0]==' ') {
				// 첫번째 공백문자 뒤에 존재하는 공백 이외의 다른 문자의 위치로 start를 이동시킴
				start += i;
				i = 0;
			}

			// start의 첫번째 문자가 공백문자가 아닌 경우
			else
				// 토큰 개수 증가
				row++;
			
			i--;
		}

		// start에서 공백문자 이외의 문자를 찾은 경우
		else
		{
			// 현재 토큰에 해당 문자를 결합
			strncat(tokens[row], start + i, 1);

			// start의 i번째 문자가 '.' 이면서 '.'이 start의 마지막 문자 이전에 위치한 경우
			if( start[i] == '.' && i<strlen(start)){
				// start의 i번째 문자 뒤에 공백문자 이외의 문자가 있거나 start의 마지막 문자를 만나기 전까지 반복
				while(start[i + 1] == ' ' && i < strlen(start))
					i++;

			}
		}

		// 현재 토큰에 들어가 있는 문자열의 앞부분과 뒷부분의 공백을 모두 제거
		strcpy(tokens[row], ltrim(rtrim(tokens[row])));

		// 토큰이 1개 이상 존재하고, 현재 토큰이 lpthread 이면서 이전 토큰이 '-' 문자인 경우
		if(!strcmp(tokens[row], "lpthread") && row > 0 && !strcmp(tokens[row - 1], "-")){
			// 이전 토큰에 현재 토큰인 lpthread을 결합하고 현재 토큰은 메모리 초기화
			strcat(tokens[row - 1], tokens[row]);
			memset(tokens[row], 0, sizeof(tokens[row]));

			// 토큰 개수 감소
			row--;
		}

		// 토큰이 1개 이상 존재하고 현재 토큰의 마지막 문자가 숫자 또는 알파벳 문자이면서
		// 이전 토큰이 gcc 또는 데이터타입 중 한가지 이거나
		// 이전 토큰의 마지막 문자가 숫자 또는 알파벳 문자이거나
		// 이전 토큰의 마지막 문자가 '.'일 때
		else if(row > 0 && is_character(tokens[row][strlen(tokens[row]) - 1]) 
			&& (is_typeStatement(tokens[row - 1]) == 2 
			|| is_character(tokens[row - 1][strlen(tokens[row - 1]) - 1])
			|| tokens[row - 1][strlen(tokens[row - 1]) - 1] == '.') ){

			// 토큰이 2개 이상 존재하면서 2개 이전의 토큰이 ( 인 경우
			if(row > 1 && strcmp(tokens[row-2],"(") == 0)
			{
				// 이전 토큰이 struct 또는 unsigned가 아니라면 false 리턴
				if(strcmp(tokens[row-1], "struct") != 0 && strcmp(tokens[row-1], "unsigned") != 0)
					return false;
			}

			// 토큰이 2개만 존재하면서 현재 토큰의 마지막 문자가 숫자 또는 알파벳 문자인 경우
			else if(row == 1 && is_character(tokens[row][strlen(tokens[row]) - 1])) {
				// 첫번째 토큰이 extern 혹은 unsigned, gcc 또는 데이터타입 중 한가지가 아닌 경우에는 false 리턴
				if(strcmp(tokens[0], "extern") != 0 && strcmp(tokens[0], "unsigned") != 0 && is_typeStatement(tokens[0]) != 2)	
					return false;
			}

			// 토큰이 2개 이상 있으면서 이전 토큰이 gcc 또는 데이터타입 중 한가지일 때
			else if(row > 1 && is_typeStatement(tokens[row - 1]) == 2){
				// 2개 이전의 토큰이 unsigned 또는 extern이 아닌 경우에는 false 리턴
				if(strcmp(tokens[row - 2], "unsigned") != 0 && strcmp(tokens[row - 2], "extern") != 0)
					return false;
			}
		}
	}

	// 토큰이 1개 이상 존재하는 경우
	if(row > 0)
	{

		// ex) #include <sys/types.h>
		// 첫번째 토큰이 #include 이거나 include 이거나 struct 인 경우
		if(strcmp(tokens[0], "#include") == 0 || strcmp(tokens[0], "include") == 0 || strcmp(tokens[0], "struct") == 0){
			// 모든 토큰 메모리 초기화
			clear_tokens(tokens);

			// 학생 답안 문자열에서 연속된 공백 문자는 1개의 공백만 남기도록 변환한 후에 첫번째 토큰에 복사
			strcpy(tokens[0], remove_extraspace(str)); 
		}
	}

	// 첫번째 토큰이 gcc 또는 데이터타입 중 한가지이거나 extern을 포함하는 경우
	if(is_typeStatement(tokens[0]) == 2 || strstr(tokens[0], "extern") != NULL){
		// 토큰 최대 개수만큼 반복
		for(i = 1; i < TOKEN_CNT; i++){
			// 토큰이 비었으면 종료
			if(strcmp(tokens[i],"") == 0)  
				break;		       

			// 현재 순회 중인 토큰이 최대 토큰 개수보다 작은 경우
			if(i != TOKEN_CNT -1 )
				// 첫번째 토큰의 가장 뒤에 공백을 추가
				strcat(tokens[0], " ");
			
			// 첫번재 토큰의 가장 뒤에 현재 토큰을 결합
			strcat(tokens[0], tokens[i]);

			// 현재 토큰의 메모리 초기화
			memset(tokens[i], 0, sizeof(tokens[i]));
		}
	}

	//change ( ' char ' )' a  ->  (char)a
	// 데이터 타입 캐스팅 형식 토큰을 찾음
	while((p_str = find_typeSpecifier(tokens)) != -1){
		// p_str 뒤에 있는 토큰을 앞으로 복사, 결합하여 토큰을 모으고 해당 토큰들은 메모리 초기화
		if(!reset_tokens(p_str, tokens))
			return false;
	}

	//change sizeof ' ( ' record ' ) '-> sizeof(record)
	// struct 선언 형식 토큰을 찾음
	while((p_str = find_typeSpecifier2(tokens)) != -1){
		// p_str 뒤에 있는 토큰을 앞으로 복사, 결합하여 토큰을 모으고 해당 토큰들은 메모리 초기화
		if(!reset_tokens(p_str, tokens))
			return false;
	}

	return true;
}

node *make_tree(node *root, char (*tokens)[MINLEN], int *idx, int parentheses)
{
	node *cur = root;
	node *new;
	node *saved_operator;
	node *operator;
	int fstart;
	int i;

	while(1)
	{
		// idx번째 토큰이 비었으면 종료
		if(strcmp(tokens[*idx], "") == 0)
			break;
		
		// idx번째 토큰이 ) 이면 최상위 노드를 리턴
		if(!strcmp(tokens[*idx], ")"))
			return get_root(cur);

		// idx번째 토큰이 , 이면 최상위 노드를 리턴
		else if(!strcmp(tokens[*idx], ","))
			return get_root(cur);

		// idx번째 토큰이 ( 인 경우
		else if(!strcmp(tokens[*idx], "("))
		{
			// function()
			// 현재 idx 이전 토큰이 정의된 연산자와 , 이 아닐 때
			if(*idx > 0 && !is_operator(tokens[*idx - 1]) && strcmp(tokens[*idx - 1], ",") != 0){
				// 플래그 변경
				fstart = true;

				while(1)
				{
					// 토큰 인덱스 증가
					*idx += 1;

					// 토큰이 ) 이면 종료
					if(!strcmp(tokens[*idx], ")"))
						break;
					
					// 현재 토큰 기준으로 하위 트리 생성(재귀 호출)
					new = make_tree(NULL, tokens, idx, parentheses + 1);
					
					// 생성된 트리가 null이 아닐 때
					if(new != NULL){
						// while문 첫번째 순회일 때
						if(fstart == true){
							// 자식 노드들의 시작 노드는 새로 생성된 트리를 시작점으로 함
							cur->child_head = new;

							// 새로 생성된 하위 트리의 부모 노드는 이전에 생성된 트리로 함
							new->parent = cur;
	
							fstart = false;
						}

						// while문 두번째 이상 순회일 때
						else{
							// 다음 노드는 새로 생성된 트리를 시작점으로 함
							cur->next = new;

							// 양방향 트리 구조로 만든다
							new->prev = cur;
						}

						// 현재 노드를 새로 생성된 노드로 바꾼다
						cur = new;
					}

					// 현재 토큰이 ) 이면 종료
					if(!strcmp(tokens[*idx], ")"))
						break;
				}
			}

			// 토큰이 연산자 기호이거나 , 일 때
			// 혹은 첫번째 토큰일 때
			else{
				// 토큰 인덱스 증가
				*idx += 1;
	
				// 현재 토큰 기준으로 하위 트리 생성(재귀 호출)
				new = make_tree(NULL, tokens, idx, parentheses + 1);

				// 현재 노드가 없으면 새로 생성된 트리로 바꾼다.
				if(cur == NULL)
					cur = new;

				// 생성된 트리의 상위 노드와 현재 노드가 같은 연산자인 경우
				else if(!strcmp(new->name, cur->name)){
					// 새로 생성된 트리의 상위 노드가 |, ||, &, && 중 한가지로 시작하는 경우
					if(!strcmp(new->name, "|") || !strcmp(new->name, "||") 
						|| !strcmp(new->name, "&") || !strcmp(new->name, "&&"))
					{
						// 현재 트리에서의 가장 마지막 자식 노드를 얻는다
						cur = get_last_child(cur);

						// 새로 생성된 트리의 자식 노드가 존재할 경우
						if(new->child_head != NULL){
							// 자식 시작 노드의 주변 노드 관계를 정의함
							new = new->child_head;

							new->parent->child_head = NULL;
							new->parent = NULL;
							new->prev = cur;
							cur->next = new;
						}
					}
					
					// 새로 생성된 트리의 상위 노드가 +, * 중 한가지로 시작하는 경우
					else if(!strcmp(new->name, "+") || !strcmp(new->name, "*"))
					{
						i = 0;

						while(1)
						{
							// 다음 노드가 비었으면 종료
							if(!strcmp(tokens[*idx + i], ""))
								break;

							// 다음 노드가 ) 가 아닌 연산자 기호이면 종료
							if(is_operator(tokens[*idx + i]) && strcmp(tokens[*idx + i], ")") != 0)
								break;

							// 따라서, i는 토큰에서 연산자 기호를 찾거나 마지막 토큰까지 증가함
							i++;
						}
						
						// 다음 토큰이 + 또는 * 보다 우선 순위가 높은 경우
						if(get_precedence(tokens[*idx + i]) < get_precedence(new->name))
						{
							// 가장 마지막 자식 노드의 다음 노드를 생성된 트리의 상위 노드로 지정
							cur = get_last_child(cur);
							cur->next = new;
							new->prev = cur;
							cur = new;
						}

						// 다음 토큰이 + 또는 * 보다 우선 순위가 낮은 경우
						else
						{
							// 가장 마지막 자식 노드를 가져온다
							cur = get_last_child(cur);

							// 생성된 트리에 자식 노드가 존재하는 경우
							if(new->child_head != NULL){
								// 생성된 트리의 자식 노드를 마지막 자식 노드 다음으로 지정
								new = new->child_head;

								new->parent->child_head = NULL;
								new->parent = NULL;
								new->prev = cur;
								cur->next = new;
							}
						}
					}

					// 새로 생성된 트리의 상위 노드가 그 외의 연산자로 시작하는 경우
					else{
						// 가장 마지막 자식 노드의 다음 노드를 생성된 트리의 상위 노드로 지정
						cur = get_last_child(cur);
						cur->next = new;
						new->prev = cur;
						cur = new;
					}
				}
	
				// 연산자가 서로 다른 경우
				else
				{
					// 마지막 자식 노드의 다음 노드를 생성된 트리의 상위 노드로 지정
					cur = get_last_child(cur);

					cur->next = new;
					new->prev = cur;
	
					cur = new;
				}
			}
		}

		// idx번째 토큰이 정의된 연산자들 중 한가지인 경우
		else if(is_operator(tokens[*idx]))
		{
			// 현재 토큰이 ||, &&, |, &, +, * 중에서 한가지인 경우
			if(!strcmp(tokens[*idx], "||") || !strcmp(tokens[*idx], "&&")
					|| !strcmp(tokens[*idx], "|") || !strcmp(tokens[*idx], "&") 
					|| !strcmp(tokens[*idx], "+") || !strcmp(tokens[*idx], "*"))
			{
				// 현재 노드가 정의된 연산자이면서 idx번째 토큰과 같은 연산자인 경우
				if(is_operator(cur->name) == true && !strcmp(cur->name, tokens[*idx]))
					// 연산자 기호를 현재 노드의 연산자 기호로 한다
					operator = cur;
				
				// 정의된 연산자가 아니거나 서로 다른 연산자인 경우
				else
				{
					// idx번째 토큰을 기준으로 새로운 노드를 생성
					new = create_node(tokens[*idx], parentheses);

					// 새로 생성된 노드보다 높은 우선 순위 연산자를 찾음
					operator = get_most_high_precedence_node(cur, new);

					// 상위 노드와 이전 노드가 없는 경우
					if(operator->parent == NULL && operator->prev == NULL){

						// 연산 우선 순위 비교
						if(get_precedence(operator->name) < get_precedence(new->name)){
							// 새로 생성된 노드가 더 낮은 순위이면 현재 operator를 기준으로 추가
							cur = insert_node(operator, new);
						}

						// 새로 생성된 노드가 더 높은 순위인 경우
						else if(get_precedence(operator->name) > get_precedence(new->name))
						{
							// 현재 operator에 자식 노드가 존재하는 경우
							if(operator->child_head != NULL){
								// operator의 마지막 자식 노드를 찾음
								operator = get_last_child(operator);

								// 변경된 operator를 기준으로 새로 생성한 노드를 추가
								cur = insert_node(operator, new);
							}
						}

						// 연산 순위가 동일한 경우
						else
						{
							// operator를 현재 노드로 변경
							operator = cur;
	
							while(1)
							{
								// operator가 정의된 연산자이면서 idx번째 토큰과 동일하면 종료
								if(is_operator(operator->name) == true && !strcmp(operator->name, tokens[*idx]))
									break;
								
								// operator의 이전 노드가 존재하면 operator를 이전 노드로 변경
								if(operator->prev != NULL)
									operator = operator->prev;
								
								// operator의 이전 노드가 없으면 종료
								else
									break;
							}

							// operator와 idx번째 토큰이 동일하지 않으면 operator를 상위 노드로 변경
							if(strcmp(operator->name, tokens[*idx]) != 0)
								operator = operator->parent;

							// operator가 존재하는 경우
							if(operator != NULL){
								// operator와 idx번째 토큰이 동일하면 현재 노드를 operator로 변경
								if(!strcmp(operator->name, tokens[*idx]))
									cur = operator;
							}
						}
					}

					// 상위 노드 또는 이전 노드가 존재하는 경우
					else
						// operator를 기준으로 새로 생성한 노드를 추가
						cur = insert_node(operator, new);
				}

			}

			// 현재 토큰이 그 외의 연산자인 경우
			else
			{
				// idx번째 토큰으로 노드 생성
				new = create_node(tokens[*idx], parentheses);

				// 현재 노드가 없으면 생성한 노드를 가리킴
				if(cur == NULL)
					cur = new;

				// 현재 노드가 존재하는 경우
				else
				{
					// 새로 생성한 노드보다 높은 우선 순위 연산자를 찾음
					operator = get_most_high_precedence_node(cur, new);

					// operator의 parentheses가 더 높으면 현재 operator를 기준으로 노드 추가
					if(operator->parentheses > new->parentheses)
						cur = insert_node(operator, new);

					// operator의 상위 노드와 이전 노드가 모두 없는 경우
					else if(operator->parent == NULL && operator->prev ==  NULL){
						// operator의 연산 순위가 더 낮은 경우
						if(get_precedence(operator->name) > get_precedence(new->name))
						{
							// operator의 자식 노드가 존재하는 경우
							if(operator->child_head != NULL){
								// operator를 마지막 자식 노드로 변경
								operator = get_last_child(operator);

								// 마지막 자식 노드로 변경된 operator를 기준으로 노드 추가
								cur = insert_node(operator, new);
							}
						}

						// 새로 생성한 노드의 연산 순위가 더 높거나 같은 경우
						else
							// 현재 operator를 기준으로 노드 추가
							cur = insert_node(operator, new);
					}

					// 상위 노드 또는 이전 노드가 존재하는 경우
					else
						// 현재 operator를 기준으로 노드 추가
						cur = insert_node(operator, new);
				}
			}
		}

		// 그 외의 문자인 경우
		else
		{
			// idx번째 토큰으로 노드 생성
			new = create_node(tokens[*idx], parentheses);

			// 현재 노드가 없으면 생성한 노드를 가리킴
			if(cur == NULL)
				cur = new;
			
			// 현재 노드는 존재하지만 자식 노드가 없는 경우
			else if(cur->child_head == NULL){
				// 자식 시작 노드를 새로 생성한 노드로 지정
				// 따라서, 새로 생성한 노드의 상위 노드는 현재 노드로 지정됨
				cur->child_head = new;
				new->parent = cur;

				cur = new;
			}

			// 현재 노드의 자식 노드가 존재하는 경우
			else{
				// 마지막 자식 노드를 가져온다
				cur = get_last_child(cur);

				// 마지막 자식 노드의 다음 노드로 새로 생성한 노드를 지정
				cur->next = new;
				new->prev = cur;

				cur = new;
			}
		}

		// 토큰 인덱스 증가
		*idx += 1;
	}

	// 현재 노드의 최상위 노드를 리턴
	return get_root(cur);
}

node *change_sibling(node *parent)
{
	node *tmp;
	
	// 자식 노드를 바꾼다

	tmp = parent->child_head;

	parent->child_head = parent->child_head->next;
	parent->child_head->parent = parent;
	parent->child_head->prev = NULL;

	parent->child_head->next = tmp;
	parent->child_head->next->prev = parent->child_head;
	parent->child_head->next->next = NULL;
	parent->child_head->next->parent = NULL;		

	return parent;
}

node *create_node(char *name, int parentheses)
{
	node *new;

	// 새로운 노드 생성하여 반환

	new = (node *)malloc(sizeof(node));
	new->name = (char *)malloc(sizeof(char) * (strlen(name) + 1));
	strcpy(new->name, name);

	new->parentheses = parentheses;
	new->parent = NULL;
	new->child_head = NULL;
	new->prev = NULL;
	new->next = NULL;

	return new;
}

int get_precedence(char *op)
{
	int i;

	// 사전에 정의된 연산자 개수만큼 반복
	for(i = 2; i < OPERATOR_CNT; i++){
		// op가 정의된 연산자 기호이면 리턴해준다
		if(!strcmp(operators[i].operator, op))
			return operators[i].precedence;
	}
	return false;
}

int is_operator(char *op)
{
	int i;

	// 사전에 정의된 연산자 개수만큼 반복
	for(i = 0; i < OPERATOR_CNT; i++)
	{
		// operator가 없으면 종료
		if(operators[i].operator == NULL)
			break;
		
		// 입력으로 들어온 연산자가 정의된 연산자 중에서 일치하는 것이 존재하면 true 리턴
		if(!strcmp(operators[i].operator, op)){
			return true;
		}
	}

	// 정의된 연산자와 일치하는 연산자가 아닌 경우에는 false 리턴
	return false;
}

void print(node *cur)
{
	// 노드 내용 출력

	if(cur->child_head != NULL){
		print(cur->child_head);
		printf("\n");
	}

	if(cur->next != NULL){
		print(cur->next);
		printf("\t");
	}
	printf("%s", cur->name);
}

node *get_operator(node *cur)
{
	// 연산자 기호 문자를 가지고 있는 노드 반환

	if(cur == NULL)
		return cur;

	if(cur->prev != NULL)
		while(cur->prev != NULL)
			cur = cur->prev;

	return cur->parent;
}

node *get_root(node *cur)
{
	// 입력된 노드가 null이면 해당 노드 리턴
	if(cur == NULL)
		return cur;

	// 해당 노드의 prev 노드가 null을 만나기 전까지 반복
	while(cur->prev != NULL)
		cur = cur->prev;

	// 해당 노드의 parent 노드가 null을 만나기 전까지 반복
	// 해당 노드의 parent 노드를 입력으로 함수 재귀 호출
	if(cur->parent != NULL)
		cur = get_root(cur->parent);

	// 결과적으로 최상위 노드를 리턴
	return cur;
}

node *get_high_precedence_node(node *cur, node *new)
{
	// 현재 노드가 정의된 연산자 기호인 경우
	if(is_operator(cur->name))
		// 현재 노드의 연산자가 새로 생성된 노드의 연산자보다 우선 순위가 높으면 현재 노드를 리턴
		if(get_precedence(cur->name) < get_precedence(new->name))
			return cur;

	// 이전 노드가 존재하는 경우
	if(cur->prev != NULL){
		// 가장 첫번째 노드까지 반복
		while(cur->prev != NULL){
			cur = cur->prev;
			
			// 이전 노드와 새로 생성된 노드의 연산자 비교를 위해 재귀 호출
			return get_high_precedence_node(cur, new);
		}

		// 상위 노드가 존재하는 경우
		if(cur->parent != NULL)
			// 상위 노드에 대해서도 새로 생성된 노드와 연산자 비교를 위해 재귀 호출
			return get_high_precedence_node(cur->parent, new);
	}

	// if(cur->parent == NULL)
	//	return cur
	
	return cur;
}

node *get_most_high_precedence_node(node *cur, node *new)
{
	// operator는 새로 생성된 노드의 연산자보다 우선 순위가 더 높은 노드를 가리킴
	node *operator = get_high_precedence_node(cur, new);
	node *saved_operator = operator;

	while(1)
	{
		// 상위 노드가 없으면 종료
		if(saved_operator->parent == NULL)
			break;

		// 이전 노드가 있으면 이전 노드를 시작으로 하여 높은 우선 순위의 노드를 찾음
		if(saved_operator->prev != NULL)
			operator = get_high_precedence_node(saved_operator->prev, new);

		// 상위 노드가 있으면 상위 노드를 시작으로 하여 높은 우선 순위의 노드를 찾음
		else if(saved_operator->parent != NULL)
			operator = get_high_precedence_node(saved_operator->parent, new);

		// save_operator는 새로 생성된 노드의 연산자보다 높은 우선 순위의 연산자를 가리킴
		saved_operator = operator;
	}

	// 결과적으로 새로 생성된 노드보다 높은 우선 순위 연산자를 리턴
	return saved_operator;
}

node *insert_node(node *old, node *new)
{
	// 기존 노드의 이전 노드가 존재하는 경우
	if(old->prev != NULL){
		// 해당 이전 노드의 다음 노드로 새로 생성한 노드를 추가
		new->prev = old->prev;
		old->prev->next = new;
		old->prev = NULL;
	}

	// 기존 노드를 새로 생성한 노드의 자식 노드로 변경
	new->child_head = old;
	old->parent = new;

	return new;
}

node *get_last_child(node *cur)
{
	// 자식 노드의 시작 노드가 있는 경우
	if(cur->child_head != NULL)
		// 자식 시작노드를 가리킴
		cur = cur->child_head;

	// 자식 노드의 마지막 노드까지 반복
	while(cur->next != NULL)
		cur = cur->next;

	// 결과적으로 가장 마지막 자식 노드를 리턴
	return cur;
}

int get_sibling_cnt(node *cur)
{
	int i = 0;

	// 현재 노드 기준으로 자식 노드의 개수를 얻는다

	while(cur->prev != NULL)
		cur = cur->prev;

	while(cur->next != NULL){
		cur = cur->next;
		i++;
	}

	return i;
}

void free_node(node *cur)
{
	// 트리의 자식 노드 포함 모든 노드의 메모리 반환

	if(cur->child_head != NULL)
		free_node(cur->child_head);

	if(cur->next != NULL)
		free_node(cur->next);

	if(cur != NULL){
		cur->prev = NULL;
		cur->next = NULL;
		cur->parent = NULL;
		cur->child_head = NULL;
		free(cur);
	}
}


int is_character(char c)
{
	// 인자로 들어온 char 변수가 숫자 범위 또는 알파벳 범위에 포함된다면 1 리턴, 그렇지 않으면 0 리턴
	return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int is_typeStatement(char *str)
{ 
	char *start;
	char str2[BUFLEN] = {0}; 
	char tmp[BUFLEN] = {0}; 
	char tmp2[BUFLEN] = {0}; 
	int i;	 
	
	// str : 학생 답안

	start = str;

	// str2에 str 즉, 학생 답안을 복
	strncpy(str2,str,strlen(str));

	// 문자열에서 공백을 모두 제거
	remove_space(str2);

	// 학생 답안에서 공백이 아닌 부분을 찾을때까지 반복
	while(start[0] == ' ')
		start += 1;

	// 학생 답안에 gcc가 포함되어 있는지 검사
	if(strstr(str2, "gcc") != NULL)
	{
		// gcc가 포함되어 있으면 tmp2에 "gcc"의 문자열 길이만큼 start에서 복사
		strncpy(tmp2, start, strlen("gcc"));
		
		// tmp2가 gcc와 같지 않다면 학생 답안의 가장 첫부분이 gcc가 아니라는 의미로
		// 이는 gcc 컴파일 구문에 맞지 않기 때문에 0 리턴
		if(strcmp(tmp2,"gcc") != 0)
			return 0;
		else
			// gcc가 맞다면 2 리턴
			return 2;
	}


	// 이하 로직은 gcc로 시작하는 컴파일 답안이 아닌 경우에 실행

	for(i = 0; i < DATATYPE_SIZE; i++)
	{
		// 정의된 datatype과 일치하는 문자열이 학생 답안에 존재하는지 검사
		if(strstr(str2,datatype[i]) != NULL)
		{
			// 데이터 타입이 포함되어 있으면 tmp에 해당 데이터 타입의 문자열 길이만큼 str2에서 복사
			strncpy(tmp, str2, strlen(datatype[i]));

			// 데이터 타입이 포함되어 있으면 tmp2에 해당 데이터 타입의 문자열 길이만큼 start에서 복사
			strncpy(tmp2, start, strlen(datatype[i]));
			
			// tmp가 해당 데이터 타입 문자열과 동일한지 검사
			if(strcmp(tmp, datatype[i]) == 0)
				// tmp와 tmp2가 동일한지 검사
				if(strcmp(tmp, tmp2) != 0)
					// 동일하지 않다면 0 리턴
					return 0;  
				else
					// 동일하면 2 리턴
					return 2;
		}

	}

	// gcc 또는 데이터 타입이 검출되지 않았으면 1 리턴
	return 1;

}

int find_typeSpecifier(char tokens[TOKEN_CNT][MINLEN]) 
{
	int i, j;

	// 최대 토큰 개수만큼 반복
	for(i = 0; i < TOKEN_CNT; i++)
	{
		// 데이터타입 종류만큼 반복
		for(j = 0; j < DATATYPE_SIZE; j++)
		{
			// 두번째 이후의 토큰에 데이터타입 중 한가지가 존재하는 경우
			if(strstr(tokens[i], datatype[j]) != NULL && i > 0)
			{
				// 이전 토큰이 ( 이면서 다음 토큰은 ) 이고,
				// ) 다음 토큰의 첫번째 문자가 &, *, ), (, -, + 중 한가지인 경우에는 현재 토큰의 인덱스를 리턴
				if(!strcmp(tokens[i - 1], "(") && !strcmp(tokens[i + 1], ")") 
						&& (tokens[i + 2][0] == '&' || tokens[i + 2][0] == '*' 
							|| tokens[i + 2][0] == ')' || tokens[i + 2][0] == '(' 
							|| tokens[i + 2][0] == '-' || tokens[i + 2][0] == '+' 
							|| is_character(tokens[i + 2][0])))
					return i;
			}
		}
	}

	// 토큰이 ()안에 데이터타입이 존재하고() 뒤에 &, *, ), (, -, + 중 한가지가 있는 경우가 아니면 -1 리턴
	return -1;
}

int find_typeSpecifier2(char tokens[TOKEN_CNT][MINLEN]) 
{
    int i, j;

	// 최대 토큰 개수만큼 반복
    for(i = 0; i < TOKEN_CNT; i++)
    {
		// 데이터타입 종류만큼 반복
        for(j = 0; j < DATATYPE_SIZE; j++)
        {
			// 현재 토큰이 struct이고, 다음 토큰의 가장 마지막 문자가 숫자 또는 알파벳 문자인 경우에는 현재 토큰의 인덱스를 리턴
            if(!strcmp(tokens[i], "struct") && (i+1) <= TOKEN_CNT && is_character(tokens[i + 1][strlen(tokens[i + 1]) - 1]))  
                    return i;
        }
    }

	// 토큰이 struct 선언 형식이 아닌 경우에는 -1 리턴
    return -1;
}

int all_star(char *str)
{
	int i;
	int length= strlen(str);
	
	// 입력된 문자열이 비었을 경우 0 리턴
 	if(length == 0)	
		return 0;
	
	// 입력된 문자열의 길이만큼 순회
	for(i = 0; i < length; i++)
		// 입력된 문자열에서 * 연산자 이외의 다른 문자가 포함되어 있으면 0 리턴
		if(str[i] != '*')
			return 0;
	
	// 입력된 문자열이 * 연산자만으로 이루어져 있으면 1 리턴
	return 1;

}

int all_character(char *str)
{
	int i;

	// 입력으로 들어온 문자열 전체에 대해 숫자와 알파벳 문자 포함 여부를 검사
	for(i = 0; i < strlen(str); i++)
		if(is_character(str[i]))
			return 1;
	return 0;
	
}

int reset_tokens(int start, char tokens[TOKEN_CNT][MINLEN]) 
{
	int i;
	int j = start - 1;
	int lcount = 0, rcount = 0;
	int sub_lcount = 0, sub_rcount = 0;

	// start는 reset_tokens를 적용할 토큰의 인덱스
	
	// 정상적인 토큰 인덱스 범위인 경우
	if(start > -1){
		// 토큰이 struct일 때
		if(!strcmp(tokens[start], "struct")) {
			// struct 뒤에 공백 추가
			strcat(tokens[start], " ");

			// 공백 뒤에 다음 토큰 결합
			strcat(tokens[start], tokens[start+1]);	     

			// struct 다음 토큰부터 tokens 끝까지 순회
			for(i = start + 1; i < TOKEN_CNT - 1; i++){
				// 현재 토큰을 다음 토큰으로 덮어쓰기
				strcpy(tokens[i], tokens[i + 1]);

				// 다음 토큰 메모리 초기화
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

		// 토큰이 unsigned 이면서 다음 토큰이 )이 아닐 때
		else if(!strcmp(tokens[start], "unsigned") && strcmp(tokens[start+1], ")") != 0) {
			// unsigned 뒤에 공백 추가
			strcat(tokens[start], " ");

			// 공백 뒤에 다음 토큰을 결합
			strcat(tokens[start], tokens[start + 1]);

			// 그 다음 토큰도 결합
			strcat(tokens[start], tokens[start + 2]);

			// unsigned 다음 토큰부터 tokens 끝까지 순회
			for(i = start + 1; i < TOKEN_CNT - 1; i++){
				// 현재 토큰을 다음 토큰으로 덮어쓰기
				strcpy(tokens[i], tokens[i + 1]);

				// 다음 토큰 메모리 초기화
				memset(tokens[i + 1], 0, sizeof(tokens[0]));
			}
		}

		// j는 struct 혹은 데이터타입 종류 다음 토큰을 가리킴
		j = start + 1;

		// 토큰이 )이 아닌 다른 문자를 찾기 전까지 반복
		while(!strcmp(tokens[j], ")")){
				// 찾은 ) 개수만큼 rcount 증가
				rcount ++;

				// 토큰 인덱스가 최대 토큰 개수까지 도달했으면 종료
				if(j==TOKEN_CNT)
						break;
				
				// 토큰 인덱스 증가
				j++;
		}
	
		// j는 struct 혹은 데이터타입 종류 이전 토큰을 가리킴
		j = start - 1;

		// 토큰이 ( 이 아닌 다른 문자를 찾기 전까지 반복
		while(!strcmp(tokens[j], "(")){
					// 찾은 ( 개수만큼 lcount 증가
        	        lcount ++;

					// 토큰 인덱스가 첫번째 토큰까지 도달했으면 종료
                	if(j == 0)
                        	break;
					
					// 토큰 인덱스 감소
               		j--;
		}

		// 토큰 인덱스가 첫번째 토큰이 아니면서 현재 토큰의 마지막 문자가 숫자 또는 알파벳 문자이거나
		// 토큰 인덱스가 첫번째 토큰을 가리키고 있을 경우
		if( (j!=0 && is_character(tokens[j][strlen(tokens[j])-1]) ) || j==0)
			// ( 개수를 ) 개수로 변경
			lcount = rcount;

		// ( 개수와 ) 개수가 같지 않을 경우에는 false 리턴
		if(lcount != rcount )
			return false;

		// start에서 가장 앞에 위치한 ( 의 위치가 첫번째 문자 이후에 있으면서
		// 가장 앞에 위치한 ( 의 바로 앞 토큰이 sizeof인 경우에는 true 리턴
		if( (start - lcount) >0 && !strcmp(tokens[start - lcount - 1], "sizeof")){
			return true; 
		}
		
		// start 위치의 토큰이 unsigned 또는 struct 이면서 다음 토큰이 ) 인 경우
		else if((!strcmp(tokens[start], "unsigned") || !strcmp(tokens[start], "struct")) && strcmp(tokens[start+1], ")")) {
			// 가장 앞에 위치한 ( 의 토큰에 start 위치의 토큰을 결합
			strcat(tokens[start - lcount], tokens[start]);

			// 다음 토큰을 그 뒤에 결합
			strcat(tokens[start - lcount], tokens[start + 1]);

			// 가장 앞에 위치한 ( 의 바로 다음 토큰에 가장 마지막에 위치한 ) 의 토큰을 결합
			strcpy(tokens[start - lcount + 1], tokens[start + rcount]);

			// 가장 앞에 위치한 ( 의 다음 토큰 위치부터 ( 와 )의 개수를 제외한 토큰 개수만큼 반복
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount; i++) {
				// 현재 토큰으로부터 가까운 )의 다음 토큰을 복사
				strcpy(tokens[i], tokens[i + lcount + rcount]);

				// 복사한 토큰 메모리 초기화
				memset(tokens[i + lcount + rcount], 0, sizeof(tokens[0]));
			}


		}

		// start 위치의 토큰이 unsigned, struct 둘 다 아니거나 다음 토큰이 ) 가 아닌 경우
 		else{
			// start 기준으로 2개 뒤에 있는 토큰 첫번째 문자가 ( 인 경우
			if(tokens[start + 2][0] == '('){
				// j는 2개 뒤에 있는 토큰을 가리킴
				j = start + 2;

				// ( 가 아닌 토큰을 만나기 전까지 반복
				while(!strcmp(tokens[j], "(")){
					// 안쪽에 있는 ( 의 개수만큼 증가
					sub_lcount++;
					j++;
				}

				// 가장 내부에 있는 ( 다음에 있는 문자 뒤에 토큰이 ) 인 경우
				if(!strcmp(tokens[j + 1],")")){
					// j는 ) 토큰을 가리킴
					j = j + 1;

					// ) 가 아닌 토큰을 만나기 전까지 반복
					while(!strcmp(tokens[j], ")")){
						// 안쪽에 있는 ) 의 개수만큼 증가
						sub_rcount++;
						j++;
					}
				}
				// ) 가 없으면 false 리턴
				else
					return false;

				// 내부에 있는 ( 의 개수와 ) 의 개수가 같지 않으면 false 리턴
				if(sub_lcount != sub_rcount)
					return false;
				
				// start + 2의 토큰에 가장 안족 ( 의 다음 토큰을 복사
				strcpy(tokens[start + 2], tokens[start + 2 + sub_lcount]);

				// start + 3부터 tokens 끝까지 순회
				for(int i = start + 3; i<TOKEN_CNT; i++)
					// 토큰들의 메모리 초기화
					memset(tokens[i], 0, sizeof(tokens[0]));

			}

			// start - lcount의 토큰에 start 토큰 결합
			strcat(tokens[start - lcount], tokens[start]);
			// 그 뒤에 start + 1 토큰을 결합
			strcat(tokens[start - lcount], tokens[start + 1]);
			// 그 뒤에 start + rcound + 1 토큰을 결합
			strcat(tokens[start - lcount], tokens[start + rcount + 1]);

			// 가장 외곽의 ( 부터 ( 와 ) 개수만큼 제외한 토큰 개수만큼 반복
			for(int i = start - lcount + 1; i < TOKEN_CNT - lcount -rcount -1; i++) {
				// ( 다음에 모든 괄호 개수만큼 뒤에 있는 토큰을 복사
				strcpy(tokens[i], tokens[i + lcount + rcount +1]);

				// 복사한 토큰의 메모리 초기화
				memset(tokens[i + lcount + rcount + 1], 0, sizeof(tokens[0]));

			}
		}
	}
	return true;
}

void clear_tokens(char tokens[TOKEN_CNT][MINLEN])
{
	int i;

	// 인자로 들어온 배열 메모리 초기화
	for(i = 0; i < TOKEN_CNT; i++)
		memset(tokens[i], 0, sizeof(tokens[i]));
}

char *rtrim(char *_str)
{
	char tmp[BUFLEN];
	char *end;

	// 인자로 들어온 문자열을 tmp에 복사한 후에 end는 tmp의 가장 마지막 문자를 가리킴
	strcpy(tmp, _str);
	end = tmp + strlen(tmp) - 1;

	// end가 _str의 시작 위치와 만나지 않고 end가 공백문자일 동안 반복
	while(end != _str && isspace(*end))
		// end의 인덱스를 감소시켜서 가장 마지막 문자부터 첫번째 문자로 이동함
		--end;

	// end에서 찾은 공백이 아닌 문자 뒤에 널문자를 추가함
	*(end + 1) = '\0';
	
	// 결과적으로 문자열의 뒷부분부터 공백으로 채워진 부분을 모두 지운것이 됨
	_str = tmp;
	return _str;
}

char *ltrim(char *_str)
{
	char *start = _str;

	// 인자로 들어온 문자열을 가장 처음부터 참조하여 널문자가 아니고, 공백일 동안 반복
	while(*start != '\0' && isspace(*start))
		// 문자열 인덱스 증가
		++start;
	
	// 결과적으로 문자열의 앞부분부터 공백으로 채워진 부분을 모두 지운것이 됨
	_str = start;
	return _str;
}

char* remove_extraspace(char *str)
{
	int i;
	char *str2 = (char*)malloc(sizeof(char) * BUFLEN);
	char *start, *end;
	char temp[BUFLEN] = "";
	int position;

	char *spaceStr = " ";

	// 입력 문자열에서 "include<"가 존재하는지 검사
	if(strstr(str,"include<")!=NULL){
		// "include<"가 존재할 때
		// start는 입력 문자열을 가리킴
		start = str;

		// end는 입력 문자열 str에서 '<'가 있는 위치를 가리킴
		end = strpbrk(str, "<");

		// position은 start로부터 '<'가 있는 위치까지의 길이를 나타냄
		// 즉, start부터 '<' 사이의 문자열의 길이
		position = end - start;
		
		// str에서 '<' 까지의 문자들을 temp에 결합
		strncat(temp, str, position);

		// temp에 공백문자 1개 결합
		strncat(temp, spaceStr, 1);

		// str에서 '<' 부터 그 뒤의 문자들을 temp에 결합 
		strncat(temp, str + position, strlen(str) - position + 1);

		// 입력 문자열 str이 변환된 문자열 temp를 가리킴
		str = temp;		
	}
	
	// str 전체 길이만큼 순회
	for(i = 0; i < strlen(str); i++)
	{
		// str의 i번재 문자가 공백문자인 경우
		if(str[i] ==' ')
		{
			// str의 첫번째 문자가 공백문자인 경우
			if(i == 0 && str[0] ==' ')
				// str의 첫번째 문자에서부터 공백이 아닌 다른 문자를 찾을때까지 반복
				while(str[i + 1] == ' ')
					i++;

			// str의 첫번재 문자가 아닌 그 뒤의 문자들 중에서 공백문자를 찾은 경우	
			else{
				// 찾은 공백문자 바로 앞에 위치한 문자가 공백문자가 아닌 경우
				if(i > 0 && str[i - 1] != ' ')
					// str2의 가장 뒤에 str에서 찾은 공백문자를 추가
					str2[strlen(str2)] = str[i];
				
				// 찾은 공백문자 바로 앞에 위치한 문자도 공백문자인 경우
				// str의 i번째 문자 이후부터 공백이 아닌 다른 문자를 찾을때까지 반복
				while(str[i + 1] == ' ')
					i++;
			}
		}
		// str의 i번째 문자가 공백문자가 아닌 경우
		else
			// str2의 가장 뒤에 str에서 찾은 공백이 아닌 문자를 추가
			str2[strlen(str2)] = str[i];
	}

	// 결과적으로 다수의 공백이 연속으로 존재하면 1개의 공백만 남겨두고 나머지 공백은 제거
	return str2;
}



void remove_space(char *str)
{
	char* i = str;
	char* j = str;
	
	// j 즉, str의 가장 끝까지 반복
	while(*j != 0)
	{
		// i가 j의 현재 인덱스를 참조
		*i = *j++;

		// i가 공백문자인지 검사
		if(*i != ' ')
			// 공백이 아니면 i 인덱스 증가
			i++;
	}
	// 결과적으로 j의 끝까지 순회하면서 i에 대입하는데 공백이 아니면 그에 대응되는
	// i의 인덱스를 증가시키고, 공백이면 증가시키지 않는다
	// 따라서, 문자열 str에서 문자들 사이의 공백은 모두 제거됨

	// i의 마지막 인덱스를 널문자로 변경하여 문자열로 지정함
	*i = 0;
}

int check_brackets(char *str)
{
	char *start = str;
	int lcount = 0, rcount = 0;
	
	while(1){
		// 인자로 들어온 문자열에서 "()" 문자열의 문자들 즉, ( 또는 )가 있는지 찾는다
		if((start = strpbrk(start, "()")) != NULL){
			// 찾은 문자가 (이면 left count 증가
			if(*(start) == '(')
				lcount++;
			// 찾은 문자가 )이면 right count 증가
			else
				rcount++;

			// 문자열 인덱스 증가
			start += 1; 		
		}
		else
			// 더이상 (또는 )가 존재하지 않으면 종료
			break;
	}

	// 찾은 ( 개수와 )개수가 같지 않으면 0 리턴
	if(lcount != rcount)
		return 0;
	// 개수가 서로 같으면 1 리턴
	else
		return 1;
}

int get_token_cnt(char tokens[TOKEN_CNT][MINLEN])
{
	int i;
	
	// 토큰 개수를 확인해서 반환한다
	for(i = 0; i < TOKEN_CNT; i++)
		if(!strcmp(tokens[i], ""))
			break;

	return i;
}
