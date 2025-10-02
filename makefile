all:
	gcc -oFast engine.c -o engine
debug:
	gcc engine.c -o engine