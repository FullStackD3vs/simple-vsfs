all: main

build/main.o: main.c
	      mkdir -p build/
	      gcc -c $< -o $@

build/%.o: functions/%.c functions/%.h
	   mkdir -p build/
	   gcc -c $< -o $@
	         	

main: 	build/main.o build/vsfs.o 
	gcc $^ -o $@ && ./main config.txt

clean: 
	rm -rf build/ main
