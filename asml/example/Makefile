.POSIX:

cc      = cc
output  = example
sources = example.c
headers = ../asml.h

wflags  = -std=c99 -Werror -pedantic -Wall -Wextra -Wdeclaration-after-statement -Wshadow -Wpointer-arith -Wcast-align -Wwrite-strings -Wmissing-prototypes -Wmissing-declarations -Wredundant-decls -Wnested-externs -Winline -Wconversion -Wstrict-prototypes -Wdeprecated
cflags  = -g -O3 $(wflags)

xflags  = -lX11 -DASML_BACKEND_X11
wlflags = -lwayland-client -lxkbcommon -DASML_BACKEND_WAYLAND
wlprtcl = xdg-shell-protocol.c ../xdg-shell-client-protocol.h

x11: $(sources) $(headers)
	$(cc) $(xflags) $(cflags) -o $(output)-x11 $(sources)

wayland: $(sources) $(headers) $(wlprtcl)
	$(cc) $(wlflags) $(cflags) -o $(output)-wayland xdg-shell-protocol.c $(sources)
xdg-shell-protocol.c: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner private-code \
		< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		> xdg-shell-protocol.c
../xdg-shell-client-protocol.h: /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml
	wayland-scanner client-header \
		< /usr/share/wayland-protocols/stable/xdg-shell/xdg-shell.xml \
		> ../xdg-shell-client-protocol.h

clean:
	rm -f $(output)-x11 $(output)-wayland $(wlprtcl) *.o
