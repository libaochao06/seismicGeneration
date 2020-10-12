######### Makefile COMMON ########
#c 还是 c++
EXTENSION = cpp
#gcc 还是 g++
CC = g++
#生成的可执行文件名
EXE = seismicGeneration
#源文件路径 空格 可以继续添如 src src1 src2
SUBDIR = src
#.o文件存放路径
OBJ = obj
#.d文件存放路径
DEP = ./dep
 
#预处理选项（要包含的.h文件的路径）
CPPFLAGS += -I ./headers
#链接选项
#LDFLAGS +=
#编译器的选项
#CFLAGS += -g -Wall
 
#后面的内容都不需要修改
CXX_SOURCES = $(foreach dir,$(SUBDIR), $(wildcard $(dir)/*.$(EXTENSION)))
CXX_OBJECTS = $(patsubst  %.$(EXTENSION), $(OBJ)/%.o, $(notdir $(CXX_SOURCES)))
DEP_FILES  = $(patsubst  %.$(EXTENSION), $(DEP)/%.d, $(notdir $(CXX_SOURCES)))
 
$(EXE): $(CXX_OBJECTS)
	$(CC) $(CXX_OBJECTS) -o $(EXE)
 
$(OBJ)/%.o: $(SUBDIR)/%.$(EXTENSION)
	$(CC) -c $(CPPFLAGS) $< -o $@
 
#$(DEP)/%.d: $(SUBDIR)/%.$(EXTENSION)
#	$(CC) $(CPPFLAGS) -MM $< | sed -e 1's,^,$(OBJ)/,' > $@
 
ifneq "$(MAKECMDGOALS)" "clean"
-include $(DEP_FILES)
endif
 
.PHONY:clean
clean:
	-rm  -f  $(CXX_OBJECTS)  $(DEP_FILES)  $(EXE)