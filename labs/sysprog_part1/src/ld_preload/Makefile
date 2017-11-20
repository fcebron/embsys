SONAME = libhook.so
GCC = gcc
TOP = ../../..

all : hook

hook:
	@echo "Creation of a shared library with hook.c"
	$(GCC) -c -fPIC hook.c -o hook.o -I $(TOP)/gps/include -L $(TOP)/gps/include/
	$(GCC) -shared -Wl,-soname,$(SONAME) -o $(SONAME) hook.o -I $(TOP)/gps/include -L $(TOP)/gps/lib/
	mv $(SONAME) ../../../gps/lib 
