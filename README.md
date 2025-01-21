# GC9A01_LL_library_and_USB_file_manager
This project uses GC9A01 TFT 1.28 display, which connected through SPI interface, STM32F411CEU6 MCU, USB flash drive and five buttons (fifth is on the board) to navigate the menu.
![Структурна схема](https://github.com/user-attachments/assets/eebf99fe-2d5c-45fb-b687-529c37e6aa4d)
MENU LIBRARY FUNCTIONS:

You can find full rewiew of this lib in the following link:https://github.com/yaroslav8765/GC9A01-LL-library-menu

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

If you open .txt file, it's just shows text it stores on the screen and you can scroll it by pressing UP - DOWN buttons, there is nothing special.

You can open .bmp file with 16-bit colors. If image smaller, than 240x240,  you can move this image on the screen by using buttons and make it smaller by pressing UP and DOWN buttons at the same time. If you want to cancel change of size - press LEFT and RIGHT buttons at the same time. If image size equal to 240x240 - you need to change it's size, as written above, and then you can also move it and make it smaller or bigger (but not bigger, that it's original size). If size bigger, than 240x240 and image not compressed - you can scroll this image and chage it`s size, if it's compressed size smaller, than 240x240 - than you can move this image on the screen.

Image resizing example:
![tsitski](https://github.com/user-attachments/assets/0c357a6d-bbc3-4144-a177-3299e0fe5b91)

![kirara](https://github.com/user-attachments/assets/cdcdccc3-f362-41b2-9fcf-5d6db5eaedd8)

In case, if USB storage disconected or any error in connecting USB, program shows error message:

![disconection](https://github.com/user-attachments/assets/86de5ec3-86fa-4f03-9633-363cd54471d7)
