
### MAKEMAKE STARTS HERE #######################################################


### Created by makemake.pl on Sun Nov 30 18:56:07 2025 #########################


### GLOBAL TARGETS #############################################################

default: mm_update all

re: mm_update rebuild

li: mm_update link

all: mm_update libdrlog.a drlog-test 

clean: mm_update clean-libdrlog.a clean-drlog-test 

rebuild: mm_update rebuild-libdrlog.a rebuild-drlog-test 

link: mm_update link-libdrlog.a link-drlog-test 

### GLOBAL (AND USER) DEFS #####################################################


AR = ar rv
CC = gcc
LD = gcc
MKDIR = mkdir -p
RANLIB = ranlib
RMDIR = rm -rf
RMFILE = rm -f
SRC = *.c


### TARGET 1: libdrlog.a #######################################################

CC_1       = gcc
LD_1       = gcc
AR_1       = ar rv
RANLIB_1   = ranlib
CCFLAGS_1  =   
LDFLAGS_1  =  
DEPFLAGS_1 = 
ARFLAGS_1  = 
TARGET_1   = libdrlog.a

### SOURCES FOR TARGET 1: libdrlog.a ###########################################

SRC_1= \
     dr_log.c \

#### OBJECTS FOR TARGET 1: libdrlog.a ##########################################

OBJ_1= \
     .OBJ.libdrlog.a/dr_log.o \

### TARGET DEFINITION FOR TARGET 1: libdrlog.a #################################

.OBJ.libdrlog.a: 
	$(MKDIR) .OBJ.libdrlog.a

libdrlog.a:   .OBJ.libdrlog.a $(OBJ_1)
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)

clean-libdrlog.a: 
	$(RMFILE) $(TARGET_1)
	$(RMDIR) .OBJ.libdrlog.a

rebuild-libdrlog.a:
	$(MAKE) clean-libdrlog.a
	$(MAKE)       libdrlog.a

re-libdrlog.a: rebuild-libdrlog.a

link-libdrlog.a: .OBJ.libdrlog.a $(OBJ_1)
	$(RMFILE) libdrlog.a
	$(AR_1) $(ARFLAGS_1) $(TARGET_1) $(OBJ_1)
	$(RANLIB_1) $(TARGET_1)


### TARGET OBJECTS FOR TARGET 1: libdrlog.a ####################################

.OBJ.libdrlog.a/dr_log.o: dr_log.c  dr_log.c dr_log.h
	$(CC_1) $(CFLAGS_1) $(CCFLAGS_1) -c dr_log.c             -o .OBJ.libdrlog.a/dr_log.o


### TARGET 2: drlog-test #######################################################

CC_2       = gcc
LD_2       = gcc
AR_2       = ar rv
RANLIB_2   = ranlib
CCFLAGS_2  = -DDR_LOG_TEST
LDFLAGS_2  =  
DEPFLAGS_2 = 
ARFLAGS_2  = 
TARGET_2   = drlog-test

### SOURCES FOR TARGET 2: drlog-test ###########################################

SRC_2= \
     dr_log.c \

#### OBJECTS FOR TARGET 2: drlog-test ##########################################

OBJ_2= \
     .OBJ.drlog-test/dr_log.o \

### TARGET DEFINITION FOR TARGET 2: drlog-test #################################

.OBJ.drlog-test: 
	$(MKDIR) .OBJ.drlog-test

drlog-test:   .OBJ.drlog-test $(OBJ_2)
	$(LD_2) $(OBJ_2) $(LDFLAGS_2) -o $(TARGET_2)

clean-drlog-test: 
	$(RMFILE) $(TARGET_2)
	$(RMDIR) .OBJ.drlog-test

rebuild-drlog-test:
	$(MAKE) clean-drlog-test
	$(MAKE)       drlog-test

re-drlog-test: rebuild-drlog-test

link-drlog-test: .OBJ.drlog-test $(OBJ_2)
	$(RMFILE) drlog-test
	$(LD_2) $(OBJ_2) $(LDFLAGS_2) -o $(TARGET_2)


### TARGET OBJECTS FOR TARGET 2: drlog-test ####################################

.OBJ.drlog-test/dr_log.o: dr_log.c  dr_log.c dr_log.h
	$(CC_2) $(CFLAGS_2) $(CCFLAGS_2) -c dr_log.c             -o .OBJ.drlog-test/dr_log.o


mm_update:
	


### MAKEMAKE ENDS HERE #########################################################

