# A Makefile to build our 'duplicates' project

PROJECT = duplicates
HEADERS = $(PROJECT).h
OBJ		= duplicates.o directories.o files.o findthings.o strSHA2.o minimize.o

C11		= cc -std=c11
CFLAGS	= -Wall -Werror -pedantic

$(PROJECT) : $(OBJ)
	$(C11) $(CFLAGS) -o $(PROJECT) $(OBJ) -lm

%.o : %.c $(HEADERS)
	$(C11) $(CFLAGS) -c $<

clean:
	rm -f $(PROJECT) $(OBJ)
