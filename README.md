

A small Windows program for moving taskbar buttons using keyboard shortcuts.
It uses the [7+ Taskbar Tweaking Library](https://ramensoftware.com/7-taskbar-tweaking-library).

The program provides shortcuts to move button groups (buttons belonging to the same app)
and to move single buttons within their groups.

The default shortcuts are as follows:
- `Win+Ctrl+Alt+Down`: Move active button group to the left.
- `Win+Ctrl+Alt+Up`: Move active button group to the right.
- `Win+Ctrl+Alt+Left`: Move active button to the left within its group.
- `Win+Ctrl+Alt+Right`: Move active button to the rigth within its group.

You have to edit the source and recompile if you want to change the shortcuts.
You need to change the keys passed to the `RegisterHotKey` function.
You also have to change the [virtual keys](https://docs.microsoft.com/en-us/windows/win32/inputdev/virtual-key-codes)
used across the code accordingly.

To build, download TTLib.zip from the link above and extract it in the same directory as this repo, then open Makefile, modify it if necessary and run it.

[7+ Taskbar Tweaker](https://ramensoftware.com/7-taskbar-tweaker) (written by the same author the as 7+ Taskbar Tweaking Library)
provides settings to configure shortcuts to move button groups, but there is no
way to move the active button within its group as far as I know.
