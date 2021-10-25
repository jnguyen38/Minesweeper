
CMP = gcc
FUNC = funcproj
MAIN = project
EXEC = runproject

$(EXEC): $(FUNC).o $(MAIN).o
	$(CMP) $(FUNC).o $(MAIN).o gfx2.o -o $(EXEC) -lX11 -lm

$(FUNC).o: $(FUNC).c $(FUNC).h
	$(CMP) -c $(FUNC).c -o $(FUNC).o 

$(MAIN).o: $(MAIN).c $(FUNC).h
	$(CMP) -c $(MAIN).c -o $(MAIN).o 

clean:
	rm $(FUNC).o
	rm $(MAIN).o
	rm $(EXEC)

