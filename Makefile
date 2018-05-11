.PHONY: all

FLAGS=$(shell pkg-config --libs --cflags gio-2.0 gio-unix-2.0 glib-2.0)
all: server client client2
server: server.o minminbus.o
	gcc -o $@ $^ $(FLAGS)
client: client.o minminbus.o
	gcc -o $@ $^ $(FLAGS)
client2: client2.o minminbus.o
	gcc -o $@ $^ $(FLAGS)
server.o: server.c
	gcc -o $@ $^ -c $(FLAGS)
client.o: client.c
	gcc -o $@ $^ -c $(FLAGS)
client2.o: client2.c
	gcc -o $@ $^ -c $(FLAGS)
minminbus.o: minminbus.c
	gcc -o $@ $^ -c $(FLAGS)

clean:
	rm -f *.o server client client2

# gen stub
gen:
	gdbus-codegen --generate-c-code minminbus --c-namespace MinMinBus --interface-prefix com.fatminmin. com.fatminmin.GDBUS.xml
	gdbus-codegen --generate-c-code vinay245bus --c-namespace vinay245Bus --interface-prefix com.vinay245. com.vinay245.GDBUS.xml

# dbus
introspect:
	gdbus introspect -e -d com.fatminmin -o /com/fatminmin/GDBUS
	gdbus introspect -e -d com.vinay245 -o /com/vinay245/GDBUS

call:
	gdbus call -e -d com.fatminmin -o /com/fatminmin/GDBUS -m com.fatminmin.GDBUS.HelloWorld fatminmin
	gdbus call -e -d com.vinay245 -o /com/vinay245/GDBUS -m com.vinay245.GDBUS.HelloClient vinay245