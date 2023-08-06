TARGET_SSU_BACKUP 	= ssu_backup
TARGET_ADD 			= Add
TARGET_REMOVE 		= Remove
TARGET_RECOVER 		= Recover
TARGET_HELP			= Help

OBJECTS_SSU_BACKUP 	= Main.o
OBJECT_FILE_LIST	= FileListManager.o
OBJECT_HASH_MD5		= md5.o
OBJECT_HASH_SHA1	= sha1.o
OBJECT_BASE_COMMAND	= BaseCommand.o
OBJECTS_ADD 		= Add.o
OBJECT_REMOVE 		= Remove.o
OBJECT_RECOVER 		= Recover.o
OBJECT_HELP			= Help.o

COMMON_OBJECT 		= $(OBJECT_FILE_LIST) $(OBJECT_HASH_MD5) $(OBJECT_HASH_SHA1) $(OBJECT_BASE_COMMAND)

OPENSSL				= -lcrypto

all : $(TARGET_SSU_BACKUP) $(TARGET_ADD) $(TARGET_REMOVE) $(TARGET_RECOVER) $(TARGET_HELP) clean

$(TARGET_SSU_BACKUP) : $(OBJECTS_SSU_BACKUP)
	gcc -g $(OBJECTS_SSU_BACKUP) -o $(TARGET_SSU_BACKUP)

$(OBJECTS_SSU_BACKUP) : Main/Main.c
	gcc -c Main/Main.c

$(TARGET_HELP) : $(OBJECT_HELP)
	gcc -g $(OBJECT_HELP) -o $(TARGET_HELP)

$(OBJECT_HELP) : Command/Help/Help.c
	gcc -c Command/Help/Help.c

$(TARGET_ADD) : $(OBJECTS_ADD) $(COMMON_OBJECT)
	gcc -g $(OBJECTS_ADD) $(COMMON_OBJECT) -o $(TARGET_ADD) $(OPENSSL)

$(OBJECTS_ADD) : Command/Add/Add.c
	gcc -c Command/Add/Add.c

$(TARGET_REMOVE) : $(OBJECT_REMOVE) $(COMMON_OBJECT)
	gcc -g $(OBJECT_REMOVE) $(COMMON_OBJECT) -o $(TARGET_REMOVE) $(OPENSSL)

$(OBJECT_REMOVE) : Command/Remove/Remove.c
	gcc -c Command/Remove/Remove.c

$(TARGET_RECOVER) : $(OBJECT_RECOVER) $(COMMON_OBJECT)
	gcc -g $(OBJECT_RECOVER) $(COMMON_OBJECT) -o $(TARGET_RECOVER) $(OPENSSL)

$(OBJECT_RECOVER) : Command/Recover/Recover.c
	gcc -c Command/Recover/Recover.c

$(OBJECT_FILE_LIST) : FileListManager/FileListManager.c
	gcc -c FileListManager/FileListManager.c

$(OBJECT_HASH_MD5) : Hash/md5/md5.c
	gcc -c Hash/md5/md5.c

$(OBJECT_HASH_SHA1) : Hash/sha1/sha1.c
	gcc -c Hash/sha1/sha1.c

$(OBJECT_BASE_COMMAND) : Command/BaseCommand/BaseCommand.c
	gcc -c Command/BaseCommand/BaseCommand.c

clean :
	rm $(OBJECTS_SSU_BACKUP)
	rm $(OBJECT_FILE_LIST)
	rm $(OBJECT_HASH_MD5)
	rm $(OBJECT_HASH_SHA1)
	rm $(OBJECT_BASE_COMMAND)
	rm $(OBJECTS_ADD)
	rm $(OBJECT_REMOVE)
	rm $(OBJECT_RECOVER)
	rm $(OBJECT_HELP)