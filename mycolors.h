#ifndef _MYCOLORS_H_
#define _MYCOLORS_H_

#ifndef CRESET

#define CRESET   "\e[0m"
#define CRED     "\e[31m"
#define CGREEN   "\e[32m"
#define CYELLOW  "\e[33m"
#define CBLUE    "\e[34m"
#define CMAGENTA "\e[35m"
#define CCYAN    "\e[36m"
#define RED(X)     CRED X CRESET
#define GREEN(X)   CGREEN X CRESET
#define YELLOW(X)  CYELLOW X CRESET
#define BLUE(X)    CBLUE X CRESET
#define MAGENTA(X) CMAGENTA X CRESET
#define CYAN(X)    CCYAN X CRESET

#endif

#endif
