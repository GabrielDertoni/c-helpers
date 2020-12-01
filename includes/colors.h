#ifndef _COLORS_H

#define ANSI(n) "\033["#n"m"
#define RESET ANSI(0)

#define BOLD ANSI(1)
#define UNDERLINE ANSI(4)

#define CLEAR "\033[1K"

#define BLACK   ANSI(30)
#define RED     ANSI(31)
#define GREEN   ANSI(32)
#define YELLOW  ANSI(33)
#define BLUE    ANSI(34)
#define MAGENTA ANSI(35)
#define CYAN    ANSI(36)
#define WHITE   ANSI(37)

#define BG_BLACK   ANSI(40)
#define BG_RED     ANSI(41)
#define BG_GREEN   ANSI(42)
#define BG_YELLOW  ANSI(43)
#define BG_BLUE    ANSI(44)
#define BG_MAGENTA ANSI(45)
#define BG_CYAN    ANSI(46)
#define BG_WHITE   ANSI(47)

#endif
