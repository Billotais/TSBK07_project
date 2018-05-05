# set this variable to the director in which you saved the common files
commondir = ../common/

CC = gcc

OUT = -o main.out

INCL_H = -I. -I$(commondir) -I$(commondir)/Linux

FLAGS = -DGL_GLEXT_PROTOTYPES -lXt -lX11 -lGL -lm -lopenal -g -Wno-unused-function -Wno-unused-variable -Wall

SRC = main.c utils.c 

LIB = $(commondir)simplefont.c $(commondir)GL_utilities.c $(commondir)CallMeAL.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c

all : clean main run

main : $(SRC) $(LIB)
	$(CC) $(OUT) $(INCL_H) $(SRC) $(LIB) $(FLAGS)
run:
	./main.out
clean :
	rm -f main
