TARGET_SSU_SCORE 	= ssu_score 

OBJECT_MAIN		= main.o
OBJECT_SCORE		= ssu_score.o
OBJECT_BLANK		= blank.o


all : $(TARGET_SSU_SCORE) clean

$(TARGET_SSU_SCORE) : $(OBJECT_MAIN) $(OBJECT_SCORE) $(OBJECT_BLANK)
	gcc -g $(OBJECT_MAIN) $(OBJECT_SCORE) $(OBJECT_BLANK) -o $(TARGET_SSU_SCORE)

$(OBJECT_MAIN) : source/main.c
	gcc -c source/main.c

$(OBJECT_SCORE) : source/ssu_score.c
	gcc -c source/ssu_score.c

$(OBJECT_BLANK) : source/blank.c
	gcc -c source/blank.c

clean :
	rm $(OBJECT_MAIN)
	rm $(OBJECT_SCORE)
	rm $(OBJECT_BLANK)
