CFLAGS = $$(pkg-config --cflags gtk4)
LDLIBS = $$(pkg-config --libs gtk4)

%.out: %.c
	gcc $(CFLAGS) -o $@  $(basename $@).c $(LDLIBS)
