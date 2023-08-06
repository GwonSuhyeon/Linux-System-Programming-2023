TARGET_SSU_MONITOR = ssu_monitor

OBJECT_MAIN = Main.o
OBJECT_BASE_COMMAND = BaseCommand.o
OBJECT_ADD = Add.o
OBJECT_DELETE = Delete.o
OBJECT_TREE = Tree.o
OBJECT_HELP = Help.o
OBJECT_DAEMON = Daemon.o
OBJECT_MONITOR = Monitor.o
OBJECT_FILELISTMANAGER = FileListManager.o

CC = gcc


all : $(TARGET_SSU_MONITOR) clean

$(TARGET_SSU_MONITOR) : $(OBJECT_MAIN) $(OBJECT_BASE_COMMAND) $(OBJECT_ADD) $(OBJECT_DELETE) $(OBJECT_TREE) $(OBJECT_HELP) $(OBJECT_DAEMON) $(OBJECT_MONITOR) $(OBJECT_FILELISTMANAGER)
	$(CC) -g $(OBJECT_MAIN) $(OBJECT_BASE_COMMAND) $(OBJECT_ADD) $(OBJECT_DELETE) $(OBJECT_TREE) $(OBJECT_HELP) $(OBJECT_DAEMON) $(OBJECT_MONITOR) $(OBJECT_FILELISTMANAGER) -o $(TARGET_SSU_MONITOR)

$(OBJECT_MAIN) : source/Main/Main.c
	$(CC) -c source/Main/Main.c

$(OBJECT_BASE_COMMAND) : source/Command/BaseCommand/BaseCommand.c
	$(CC) -c source/Command/BaseCommand/BaseCommand.c

$(OBJECT_ADD) : source/Command/Add/Add.c
	$(CC) -c source/Command/Add/Add.c

$(OBJECT_DELETE) : source/Command/Delete/Delete.c
	$(CC) -c source/Command/Delete/Delete.c

$(OBJECT_TREE) : source/Command/Tree/Tree.c
	$(CC) -c source/Command/Tree/Tree.c

$(OBJECT_HELP) : source/Command/Help/Help.c
	$(CC) -c source/Command/Help/Help.c

$(OBJECT_DAEMON) : source/Daemon/Daemon.c
	$(CC) -c source/Daemon/Daemon.c

$(OBJECT_MONITOR) : source/Monitor/Monitor.c
	$(CC) -c source/Monitor/Monitor.c

$(OBJECT_FILELISTMANAGER) : source/FileListManager/FileListManager.c
	$(CC) -c source/FileListManager/FileListManager.c

clean :
	rm $(OBJECT_MAIN)
	rm $(OBJECT_BASE_COMMAND)
	rm $(OBJECT_ADD)
	rm $(OBJECT_DELETE)
	rm $(OBJECT_TREE)
	rm $(OBJECT_HELP)
	rm $(OBJECT_DAEMON)
	rm $(OBJECT_MONITOR)
	rm $(OBJECT_FILELISTMANAGER)
