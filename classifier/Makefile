EXEC = samaritain
#INCLUDE+= -I/opt/mvIMPACT_Acquire/
FLAGS = -Wno-write-strings -Wno-unused-parameter -Wno-unused-but-set-variable
FLAGS+= -O2 -Wall -W -fPIC -D_REENTRANT -D_GNU_SOURCE  -DNDEBUG -fvisibility=hidden

LIB= -lm
LIB+= `pkg-config --libs opencv`
LIB+= `pkg-config --libs --cflags dlib-1`
LIB+=  -llapack
LIB+=  libraries/darknet/libdarknet.a
LIB+= -L/usr/local/cuda/lib64 -lcuda -lcudart -lcublas -lcurand
LIB+= -lcudnn
LIB+= -lstdc++
LIB+= -shared-libgcc
LIB+=  -lboost_system -lboost_date_time -lboost_thread
LIB+=  -lpthread

COMMON=-DOPENCV
COMMON+= -DGPU -I/usr/local/cuda/include/
COMMON+= -DCUDNN



all:
		g++ -c src/main.cpp src/samaritain.cpp src/facerec.cpp  $(COMMON) $(FLAGS) $(INCLUDE)
		g++ -o $(EXEC) main.o samaritain.o facerec.o $(LIB) $(COMMON)

run:
		make
		./samaritain


clean:
		rm *.o
		rm $(EXEC)

darknet:
		$(MAKE) -C libraries/darknet
