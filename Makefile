OBJ = NetWork.o Test.o
PROG = test
NAME = NetWork.cpp Test.cpp

all: $(PROG)

$(PROG):$(OBJ)
	g++ -o $(PROG) $(OBJ) -lsfml-network -lsfml-system -lsfml-graphics -lsfml-window -lpthread -Wall 

$(OBJ):
	g++ -c $(NAME)

install_SFML:
	sudo apt-get install libsfml-dev	

clear: 
	$(RM) $(OBJ)
	$(RM) $(PROG)

