#ifndef _IMC_SC_LAUNCHER_H_
#define _IMC_SC_LAUNCHER_H_

#include <SDL.h>
#import <Cocoa/Cocoa.h>
#import <sys/param.h>
#import <unistd.h>

/* we have our own main here. */
#ifdef main
#  undef main
#endif

/* Portions of CPS.h */
typedef struct CPSProcessSerNum_ {
	UInt32 lo;
	UInt32 hi;
} CPSProcessSerNum;

extern OSErr CPSGetCurrentProcess(CPSProcessSerNum *psn);
extern OSErr CPSEnableForegroundOperation(CPSProcessSerNum *psn,
	UInt32 _arg2, UInt32 _arg3, UInt32 _arg4, UInt32 _arg5);
extern OSErr CPSSetFrontProcess(CPSProcessSerNum *psn);

/* this is the function we call after startup */
int client_main(int, char **);

/* the SDLMain thingy */
@interface SCMain : NSObject
@end

@interface SCApplication : NSApplication
@end

/* For some reaon, Apple removed setAppleMenu from the headers in 10.4,
 but the method still is there and works. To avoid warnings, we declare
 it ourselves here. */
@interface NSApplication(SCNSApplicationAdditions)
- (void) setAppleMenu:(NSMenu *) menu;
@end

#endif
