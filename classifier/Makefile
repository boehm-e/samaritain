EXEC = samaritain
#INCLUDE+= -I/opt/mvIMPACT_Acquire/
#FLAGS = -Wno-write-strings
FLAGS+= -O2 -Wall -W -fPIC -D_REENTRANT -D_GNU_SOURCE  -DNDEBUG -fvisibility=hidden
LIB= libs/darknet/libdarknet.a -lm
LIB+= `pkg-config --libs opencv`
LIB+=  -lpthread
COMMON=-DOPENCV
# GPU
COMMON+= -DGPU -I/usr/local/cuda/include/
LIB+= -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -lcurand

COMMON+= -DCUDNN
LIB+= -lcudnn
LIB+= -lstdc++
LIB+= -shared-libgcc


all:
		g++ -c src/main.cpp  $(COMMON) $(FLAGS) $(INCLUDE)
		g++ -o $(EXEC) samaritain.o $(LIB) $(COMMON)

run:
		make
		./samaritain


clean:
		rm *.o
		rm $(EXEC)

darknet:
		$(MAKE) -C libs/darknet
