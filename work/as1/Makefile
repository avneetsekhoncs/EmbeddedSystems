CC_C = arm-linux-gnueabihf-gcc
CFLAGS = -Wall -g -std=c99 -D _POSIX_C_SOURCE=200809L -Werror

app:
	$(CC_C) $(CFLAGS) helloWorld.c -o hello
	cp hello $(HOME)/cmpt433/public/myApps/
clean:
	rm hello
	rm $(HOME)/cmpt433/public/myApps/hello
