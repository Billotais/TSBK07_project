# set this variable to the director in which you saved the common files
commondir = ../common/

all : clean main run

main : main.c $(commondir)simplefont.c $(commondir)GL_utilities.c $(commondir)CallMeAL.c $(commondir)VectorUtils3.c $(commondir)loadobj.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c
	gcc -Wall -o main -I$(commondir) -I../common/Linux -DGL_GLEXT_PROTOTYPES main.c $(commondir)simplefont.c $(commondir)GL_utilities.c $(commondir)CallMeAL.c $(commondir)loadobj.c $(commondir)VectorUtils3.c $(commondir)LoadTGA.c $(commondir)Linux/MicroGlut.c utils.c -lXt -lX11 -lGL -lm -lopenal -g
run:
	./main
clean :
	rm -f main
