CC=clang
TTLIB=TTLIB64.lib

move-taskbar-buttons.exe: move-taskbar-buttons.c
	$(CC) $^ $(TTLIB) -l user32 -o $@
