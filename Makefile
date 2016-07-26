CCOMPILE = gcc
CPPCOMPILE = g++
COMPILEOPTION = -c -g -fPIC -D__USERWLOCK -D__TEMP_COPY_ -DDEBUG #-D_FILE_OFFSET_BITS=64 #-D__TEST_VERSION
INCLUDEDIR = 
LINK = g++
LIBDIRS = 
COMMONOBJS = 
SHAREDLIB = -lpthread 
SHARE_SO=
APPENDLIB = 
SRCS	=	$(wildcard *.cpp)
OBJS	=	$(patsubst %.cpp, %.o, $(SRCS))   
TARGET = snake  
 
default:$(TARGET)

$(TARGET): $(OBJS)
	$(CPPCOMPILE) -g -o  $@ $^ $(LIBDIRS) $(SHAREDLIB)
.cpp.o: $(COMMONOBJS)
	$(CPPCOMPILE) -c -o $*.o $(COMPILEOPTION) $(INCLUDEDIR)  $*.cpp
	
.PHONY:clean
clean: 
	rm -f $(OBJS) $(TARGET)
	
	


	



	
	
