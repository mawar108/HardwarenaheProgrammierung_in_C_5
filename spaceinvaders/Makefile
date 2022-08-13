# Sie dürfen Tests in dieser Datei hinzufügen, aber nicht entfernen.
CFLAGS_FSANITIZE := -g -std=c99 -Wall -Wextra -Werror -Wmissing-prototypes -fsanitize=address -fsanitize=undefined
CFLAGS_VALGRIND  := -g -std=c99 -Wall -Wextra -Werror -Wmissing-prototypes
CFLAGS_KEYBOARD  := -g -Wall -Wextra -Werror -Wmissing-prototypes

SOURCES := spaceinvaders.c
HEADERS := spaceinvaders.h

run: argtests codestyle checksums misc questions mytests

keyboard.o: keyboard.c keyboard.h
	gcc $(CFLAGS_KEYBOARD) -c keyboard.c

spaceinvaders_fsanitize: $(SOURCES) $(HEADERS) keyboard.o
	gcc $(CFLAGS_FSANITIZE) spaceinvaders.c keyboard.o -lrt -o spaceinvaders_fsanitize

spaceinvaders_valgrind: $(SOURCES) $(HEADERS) keyboard.o
	gcc $(CFLAGS_VALGRIND) $(SOURCES) keyboard.o -lrt -o spaceinvaders_valgrind

argtests_fsanitize: spaceinvaders_fsanitize
	tests/argtests_fsanitize.sh

argtests_valgrind: spaceinvaders_valgrind
	tests/argtests_valgrind.sh

argtests: argtests_fsanitize argtests_valgrind

questions: Fragen.txt
	tests/questions.py

codestyle:
	tests/codestyle.py

misc:
	tests/misc.py

checksums:
	tests/check_checksums.sh

clean:
	rm -f *.o spaceinvaders_fsanitize spaceinvaders_valgrind

mytests:
	tests/mytest.sh

mytests2:
	tests/mytest2.sh	
