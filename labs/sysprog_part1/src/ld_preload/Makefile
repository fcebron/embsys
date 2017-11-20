SONAME = libhook.so
GCC = gcc

all : hook

hook: hook.c
	@echo "Creation of a shared library with hook.c"
	$(GCC) -c -fPIC hook.c -o hook.o
	$(GCC) -shared -Wl,-soname,$(SONAME) -o $(SONAME) hook.o
	mv $(SONAME) ../../../gps/lib 
