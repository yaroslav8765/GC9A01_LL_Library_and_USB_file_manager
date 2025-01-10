# GC9A01_LL_library_and_USB_file_manager
This project uses GC9A01 TFT 1.28 display, which connected through SPI interface, STM32F411CEU6 MCU, USB flash drive and five buttons (fifth is on the board) to navigate the menu.
![Структурна схема](https://github.com/user-attachments/assets/eebf99fe-2d5c-45fb-b687-529c37e6aa4d)
MENU LIBRARY FUNCTIONS:
You can find full rewiew of this lib in the following link:*NEED TO ADD LINK ON THE MENU LIB LATER*
In this case, menu shows list of files from USB flash drive. If name of file is too long, you can turn on animation of running text. But it shows only english letters, arabic numbers and popular symbols like +-_=()~.<>?, and so on.

![p1 test](https://github.com/user-attachments/assets/163c0ef5-59e8-4b2e-a07e-19adb5e07d92)

![p2 test](https://github.com/user-attachments/assets/3195bb0b-595a-476c-85ad-a2607def0ebe)

How running text animation looks like:

![p1 anim test](https://github.com/user-attachments/assets/a30331fc-dcc7-4ee5-af06-b3549c68befd)

MENU NAVIGATION:
As shown on schematic, there are UP, DOWN, LEFT, RIGHT and BACK_TO_MENU buttons on the board. By using these buttons you can scroll list of files, open direcroties and other files, if they're supported.
By pressing UP - DOWN buttons you can choose wished file or directory and by pressing RIGHT button you can try open it, and if it's .dir, .txt ot .bmp file - they will be shown on the screen, ontherwise - nothing will happend.
If you want close file - press LEFT button and you will be back to the menu.

FILE VIEW MODE:
