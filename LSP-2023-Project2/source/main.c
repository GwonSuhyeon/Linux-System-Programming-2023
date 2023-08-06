#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include "ssu_score.h"

#define SECOND_TO_MICRO 1000000

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t);	// 프로그램 런타임 측정

int main(int argc, char *argv[])
{
	struct timeval begin_t, end_t;
	gettimeofday(&begin_t, NULL);	// micro second 단위로 현재 시각 측정, 함수 호출 이후의 로직 수행 시간을 측정하기 위함

	ssu_score(argc, argv);			// 학생 제출 답안 채점

	gettimeofday(&end_t, NULL);		// micro second 단위로 현재 시각 측정, 함수 호출 직전까지의 로직 수행 시간을 측정하기 위함.
	ssu_runtime(&begin_t, &end_t);	// ssu_score 실행 시간을 출력

	exit(0);
}

void ssu_runtime(struct timeval *begin_t, struct timeval *end_t)
{
	// 종료 시간에서 시작 시간을 빼서 실행 시간을 얻는다
	end_t->tv_sec -= begin_t->tv_sec;

	if(end_t->tv_usec < begin_t->tv_usec){
		end_t->tv_sec--;
		end_t->tv_usec += SECOND_TO_MICRO;
	}

	// 프로그램 실행시간 출력
	end_t->tv_usec -= begin_t->tv_usec;
	printf("Runtime: %ld:%06ld(sec:usec)\n", end_t->tv_sec, end_t->tv_usec);
}
