CFLAGS = $$(pkg-config --cflags gtk4)
LDLIBS = $$(pkg-config --libs gtk4)

%.out: %.c
	mkdir -p bin
	gcc $(CFLAGS) -o $@  $(basename $@).c $(LDLIBS)
