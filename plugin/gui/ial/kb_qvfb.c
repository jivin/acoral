#include <stdio.h>
#include <fcntl.h>
#include <sys/select.h>
#include "../include/common.h"
#include "../include/keyboard_pro.h"
#include "../include/kb_ial.h"
#define QT_VFB_KEYBOARD_PIPE "/tmp/.qtvfb_keyboard-0" // pipe管道文件[luther.gliethttp]

#define SCANCODE_ESCAPE 1

#define SCANCODE_1 2
#define SCANCODE_2 3
#define SCANCODE_3 4
#define SCANCODE_4 5
#define SCANCODE_5 6
#define SCANCODE_6 7
#define SCANCODE_7 8
#define SCANCODE_8 9
#define SCANCODE_9 10
#define SCANCODE_0 11

#define SCANCODE_MINUS 12
#define SCANCODE_EQUAL 13

#define SCANCODE_BACKSPACE 14
#define SCANCODE_TAB 15

#define SCANCODE_Q 16
#define SCANCODE_W 17
#define SCANCODE_E 18
#define SCANCODE_R 19
#define SCANCODE_T 20
#define SCANCODE_Y 21
#define SCANCODE_U 22
#define SCANCODE_I 23
#define SCANCODE_O 24
#define SCANCODE_P 25
#define SCANCODE_BRACKET_LEFT 26
#define SCANCODE_BRACKET_RIGHT 27

#define SCANCODE_ENTER 28

#define SCANCODE_LEFTCONTROL 29

#define SCANCODE_A 30
#define SCANCODE_S 31
#define SCANCODE_D 32
#define SCANCODE_F 33
#define SCANCODE_G 34
#define SCANCODE_H 35
#define SCANCODE_J 36
#define SCANCODE_K 37
#define SCANCODE_L 38
#define SCANCODE_SEMICOLON 39
#define SCANCODE_APOSTROPHE 40
#define SCANCODE_GRAVE 41

#define SCANCODE_LEFTSHIFT 42
#define SCANCODE_BACKSLASH 43

#define SCANCODE_Z 44
#define SCANCODE_X 45
#define SCANCODE_C 46
#define SCANCODE_V 47
#define SCANCODE_B 48
#define SCANCODE_N 49
#define SCANCODE_M 50
#define SCANCODE_COMMA 51
#define SCANCODE_PERIOD 52
#define SCANCODE_SLASH 53

#define SCANCODE_RIGHTSHIFT 54
#define SCANCODE_KEYPADMULTIPLY 55

#define SCANCODE_LEFTALT 56
#define SCANCODE_SPACE 57
#define SCANCODE_CAPSLOCK 58

#define SCANCODE_F1 59
#define SCANCODE_F2 60
#define SCANCODE_F3 61
#define SCANCODE_F4 62
#define SCANCODE_F5 63
#define SCANCODE_F6 64
#define SCANCODE_F7 65
#define SCANCODE_F8 66
#define SCANCODE_F9 67
#define SCANCODE_F10 68

#define SCANCODE_NUMLOCK 69
#define SCANCODE_SCROLLLOCK 70

#define SCANCODE_KEYPAD7 71
#define SCANCODE_CURSORUPLEFT 71
#define SCANCODE_KEYPAD8 72
#define SCANCODE_CURSORUP 72
#define SCANCODE_KEYPAD9 73
#define SCANCODE_CURSORUPRIGHT 73
#define SCANCODE_KEYPADMINUS 74
#define SCANCODE_KEYPAD4 75
#define SCANCODE_CURSORLEFT 75
#define SCANCODE_KEYPAD5 76
#define SCANCODE_KEYPAD6 77
#define SCANCODE_CURSORRIGHT 77
#define SCANCODE_KEYPADPLUS 78
#define SCANCODE_KEYPAD1 79
#define SCANCODE_CURSORDOWNLEFT 79
#define SCANCODE_KEYPAD2 80
#define SCANCODE_CURSORDOWN 80
#define SCANCODE_KEYPAD3 81
#define SCANCODE_CURSORDOWNRIGHT 81
#define SCANCODE_KEYPAD0 82
#define SCANCODE_KEYPADPERIOD 83

#define SCANCODE_LESS 86

#define SCANCODE_F11 87
#define SCANCODE_F12 88

#define SCANCODE_KEYPADENTER 96
#define SCANCODE_RIGHTCONTROL 97
#define SCANCODE_CONTROL 97
#define SCANCODE_KEYPADDIVIDE 98
#define SCANCODE_PRINTSCREEN 99
#define SCANCODE_RIGHTALT 100
#define SCANCODE_BREAK 101 /* Beware: is 119 */
#define SCANCODE_BREAK_ALTERNATIVE 119 /* on some keyboards! */

#define SCANCODE_HOME 102
#define SCANCODE_CURSORBLOCKUP 103 /* Cursor key block */
#define SCANCODE_PAGEUP 104
#define SCANCODE_CURSORBLOCKLEFT 105 /* Cursor key block */
#define SCANCODE_CURSORBLOCKRIGHT 106 /* Cursor key block */
#define SCANCODE_END 107
#define SCANCODE_CURSORBLOCKDOWN 108 /* Cursor key block */
#define SCANCODE_PAGEDOWN 109
#define SCANCODE_INSERT 110
#define SCANCODE_REMOVE 111

#define SCANCODE_PAUSE 119

#define SCANCODE_POWER 120
#define SCANCODE_SLEEP 121
#define SCANCODE_WAKEUP 122

#define SCANCODE_LEFTWIN 125
#define SCANCODE_RIGHTWIN 126
#define SCANCODE_MENU 127

#define SCANCODE_LEFTBUTTON 0x1000
#define SCANCODE_RIGHTBUTTON 0x2000
#define SCANCODE_MIDDLBUTTON 0x4000

#define NOBUTTON 0x0000
#define LEFTBUTTON 0x0001
#define RIGHTBUTTON 0x0002
#define MIDBUTTON 0x0004
#define MOUSEBUTTONMASK 0x00FF



#define SHIFTBUTTON 0x0100
#define CONTROLBUTTON 0x0200
#define ALTBUTTON 0x0400
#define METABUTTON 0x0800
#define KEYBUTTONMASK 0x0FFF
#define KEYPAD 0x4000

struct QVFbKeyData
{
    #define NR_KEYS 128
    unsigned int unicode;
    unsigned int modifiers;
    unsigned char press;
    unsigned char repeat;
};

static int 
BDKB_Open(void);

static void
BDKB_Close(void);

static int 
BDKB_Read(
	BYTE* btScanCode,
	BYTE* btPressed
);

static int kbd_fd = -1;
static fd_set rfds;
static struct QVFbKeyData kbd_data;
static unsigned char kbd_state [NR_KEYS];
static unsigned char keycode_scancode [256];

KBDEVICE kbdev_Qvfb = {
        BDKB_Open,
        BDKB_Close,
        BDKB_Read
};

// 本代码源自libminigui-1.6.10/src/ial/qvfb.c

static void init_code_map (void)
{
    keycode_scancode [0x00] = SCANCODE_ESCAPE;

    keycode_scancode [0x29] = SCANCODE_0;
    keycode_scancode [0x21] = SCANCODE_1;
    keycode_scancode [0x40] = SCANCODE_2;
    keycode_scancode [0x23] = SCANCODE_3;
    keycode_scancode [0x24] = SCANCODE_4;
    keycode_scancode [0x25] = SCANCODE_5;
    keycode_scancode [0x5E] = SCANCODE_6;
    keycode_scancode [0x26] = SCANCODE_7;
    keycode_scancode [0x2A] = SCANCODE_8;
    keycode_scancode [0x28] = SCANCODE_9;

    keycode_scancode [0x30] = SCANCODE_0;
    keycode_scancode [0x31] = SCANCODE_1;
    keycode_scancode [0x32] = SCANCODE_2;
    keycode_scancode [0x33] = SCANCODE_3;
    keycode_scancode [0x34] = SCANCODE_4;
    keycode_scancode [0x35] = SCANCODE_5;
    keycode_scancode [0x36] = SCANCODE_6;
    keycode_scancode [0x37] = SCANCODE_7;
    keycode_scancode [0x38] = SCANCODE_8;
    keycode_scancode [0x39] = SCANCODE_9;
    keycode_scancode [0x2D] = SCANCODE_MINUS;
    keycode_scancode [0x5F] = SCANCODE_MINUS;
    keycode_scancode [0x3D] = SCANCODE_EQUAL;
    keycode_scancode [0x2B] = SCANCODE_EQUAL;
    keycode_scancode [0x03] = SCANCODE_BACKSPACE;
    keycode_scancode [0x01] = SCANCODE_TAB;
    keycode_scancode [0x51] = SCANCODE_Q;
    keycode_scancode [0x57] = SCANCODE_W;
    keycode_scancode [0x45] = SCANCODE_E;
    keycode_scancode [0x52] = SCANCODE_R;
    keycode_scancode [0x54] = SCANCODE_T;
    keycode_scancode [0x59] = SCANCODE_Y;
    keycode_scancode [0x55] = SCANCODE_U;
    keycode_scancode [0x49] = SCANCODE_I;
    keycode_scancode [0x4F] = SCANCODE_O;
    keycode_scancode [0x50] = SCANCODE_P;
    keycode_scancode [0x5B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x7B] = SCANCODE_BRACKET_LEFT;
    keycode_scancode [0x5D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x7D] = SCANCODE_BRACKET_RIGHT;
    keycode_scancode [0x04] = SCANCODE_ENTER;
    keycode_scancode [0x41] = SCANCODE_A;
    keycode_scancode [0x53] = SCANCODE_S;
    keycode_scancode [0x44] = SCANCODE_D;
    keycode_scancode [0x46] = SCANCODE_F;
    keycode_scancode [0x47] = SCANCODE_G;
    keycode_scancode [0x48] = SCANCODE_H;
    keycode_scancode [0x4A] = SCANCODE_J;
    keycode_scancode [0x4B] = SCANCODE_K;
    keycode_scancode [0x4C] = SCANCODE_L;

    keycode_scancode [0x3A] = SCANCODE_SEMICOLON;
    keycode_scancode [0x3B] = SCANCODE_SEMICOLON;

    keycode_scancode [0x27] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x22] = SCANCODE_APOSTROPHE;
    keycode_scancode [0x60] = SCANCODE_GRAVE;
    keycode_scancode [0x7E] = SCANCODE_GRAVE;

    keycode_scancode [0x20] = SCANCODE_SPACE;

    keycode_scancode [0x5C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x7C] = SCANCODE_BACKSLASH;
    keycode_scancode [0x5A] = SCANCODE_Z;
    keycode_scancode [0x58] = SCANCODE_X;
    keycode_scancode [0x43] = SCANCODE_C;
    keycode_scancode [0x56] = SCANCODE_V;
    keycode_scancode [0x42] = SCANCODE_B;
    keycode_scancode [0x4E] = SCANCODE_N;
    keycode_scancode [0x4D] = SCANCODE_M;
    keycode_scancode [0x2C] = SCANCODE_COMMA;
    keycode_scancode [0x3C] = SCANCODE_COMMA;
    keycode_scancode [0x2E] = SCANCODE_PERIOD;
    keycode_scancode [0x3E] = SCANCODE_PERIOD;
    keycode_scancode [0x2F] = SCANCODE_SLASH;
    keycode_scancode [0x3F] = SCANCODE_SLASH;

    keycode_scancode [0x09] = SCANCODE_PRINTSCREEN;
    keycode_scancode [0x08] = SCANCODE_BREAK;
    keycode_scancode [0x06] = SCANCODE_INSERT;
    keycode_scancode [0x07] = SCANCODE_REMOVE;
    keycode_scancode [0x10] = SCANCODE_HOME;
    keycode_scancode [0x11] = SCANCODE_END;
    keycode_scancode [0x16] = SCANCODE_PAGEUP;
    keycode_scancode [0x17] = SCANCODE_PAGEDOWN;
    keycode_scancode [0x13] = SCANCODE_CURSORBLOCKUP;
    keycode_scancode [0x12] = SCANCODE_CURSORBLOCKLEFT;
    keycode_scancode [0x14] = SCANCODE_CURSORBLOCKRIGHT;
    keycode_scancode [0x15] = SCANCODE_CURSORBLOCKDOWN;
}

static unsigned char keycode_to_scancode (unsigned char keycode, char asscii)
{
    if(!asscii)
    {
        switch (keycode) {
            case 0x30 ... 0x39:
                return SCANCODE_F1 + keycode - 0x30;
            case 0x3B:
                return SCANCODE_F12;
            case 0x20:
                return SCANCODE_LEFTSHIFT;
            case 0x21:
                return SCANCODE_LEFTCONTROL;
            case 0x23:
                return SCANCODE_LEFTALT;
            case 0x24:
                return SCANCODE_CAPSLOCK;
            case 0x25:
                return SCANCODE_NUMLOCK;
            case 0x26:
                return SCANCODE_SCROLLLOCK;
            case 0x3A:
                return SCANCODE_F11;
        }
    }

    return keycode_scancode [keycode];
}

static unsigned char nr_changed_keys = 0;

static int keyboard_update (void)
{
    return nr_changed_keys;
}

static int read_key (void)
{
    static unsigned char last;
    struct QVFbKeyData l_kbd_data;
    int ret;
    unsigned char scancode;

    ret = read (kbd_fd, &l_kbd_data, sizeof (struct QVFbKeyData));

    if (ret == sizeof (struct QVFbKeyData)) {
        kbd_data = l_kbd_data;
		printf("Bd Ok\n");
    }
    else
        return 0;

    if (kbd_data.repeat) {
        return 0;
    }

    if (kbd_data.unicode == 0 && !kbd_data.press) {
        kbd_state [last] = 0;
    }
    else {
        scancode = keycode_to_scancode ((kbd_data.unicode >> 16) & 0x00FF,
                    kbd_data.unicode & 0x00FF);
        kbd_state [scancode] = kbd_data.press ? 1 : 0;
        last = scancode;
    }

    nr_changed_keys = last + 1;
    return 1;
}

static const char* keyboard_getstate (void)
{
    return (char*)kbd_state;
}


static int 
BDKB_Open(void)
{
    /* open keyboard pipe */
    if ((kbd_fd = open (QT_VFB_KEYBOARD_PIPE, O_RDONLY)) < 0) {
        fprintf (stderr, "QVFB IAL engine: can not open keyboard pipe.\n");
        return -1;
    }
    init_code_map();
}

static void 
BDKB_Close(void)
{
	close(kbd_fd);
}


static int 
BDKB_Read(
	BYTE* btScanCode,
	BYTE* btPressed
)
{
		acoral_delay_self(50);
      if (read_key() ) {
			printf("in Bb\n");
			const char *kbd = keyboard_getstate();
			int key_updated = keyboard_update() - 1;
			*btScanCode = key_updated;
			*btPressed =kbd[key_updated];
			return 1;
    }
	else{
		printf("read keyboard error\n");
		return 0;
	}
	return 1;
}
