/* OS X Launcher for SC.  Based on SDLMain.m from the SDL project. */
#import "launcher.h"

static int gReturnCode;
static int gArgc;
static char **gArgv;
static BOOL gFinderLaunch;
static BOOL gCalledAppMainline = FALSE;

/* name of the application */
@implementation SCApplication
- (void) terminate:(id) sender
{
	/* Post a SDL_QUIT event */
	SDL_Event event;
	event.type = SDL_QUIT;
	SDL_PushEvent(&event);
}

- (void) sendEvent:(NSEvent *) anEvent
{
	if ((NSKeyDown == [anEvent type] || NSKeyUp == [anEvent type])) {
		/* only handle events that involve the command key
		   all others are passed to SDL only. */
		if ([anEvent modifierFlags] & NSCommandKeyMask)
			[super sendEvent: anEvent];
	} else
		[super sendEvent: anEvent];
}
@end

/* The main class of the application, the application's delegate */
@implementation SCMain

/* Set the working directory to the .app's parent directory */
- (void) setupWorkingDirectory:(BOOL) shouldChdir
{
	NSString *resourcePath = [[NSBundle mainBundle] resourcePath];
	[[NSFileManager defaultManager] changeCurrentDirectoryPath:resourcePath];
}

static void 
setApplicationMenu(void)
{
	NSMenu *appleMenu;
	NSMenuItem *menuItem;
	NSString *title;

	appleMenu = [[NSMenu alloc] initWithTitle:@""];

	/* About entry */
	title = @"About SandboxCraft";
	[appleMenu addItemWithTitle:title action:@selector(orderFrontStandardAboutPanel:)
	 keyEquivalent:@""];
	[appleMenu addItem:[NSMenuItem separatorItem]];

	/* Window management */
	title = @"Hide SandboxCraft";
	[appleMenu addItemWithTitle:title action:@selector(hide:) keyEquivalent:@"h"];
	menuItem = (NSMenuItem *)[appleMenu addItemWithTitle:@"Hide Others"
		action:@selector(hideOtherApplications:) keyEquivalent:@"h"];
	[menuItem setKeyEquivalentModifierMask:(NSAlternateKeyMask|NSCommandKeyMask)];
	[appleMenu addItemWithTitle:@"Show All" action:@selector(unhideAllApplications:)
	 keyEquivalent:@""];

	[appleMenu addItem:[NSMenuItem separatorItem]];
	title = @"Quit SandboxCraft";
	[appleMenu addItemWithTitle: title action:@selector(terminate:) keyEquivalent:@"q"];

	/* Put menu into the menubar */
	menuItem = [[NSMenuItem alloc] initWithTitle:@"" action:nil keyEquivalent:@""];
	[menuItem setSubmenu:appleMenu];
	[[NSApp mainMenu] addItem:menuItem];

	/* Tell the application object that this is now the application menu */
	[NSApp setAppleMenu:appleMenu];

	/* Finally give up our references to the objects */
	[appleMenu release];
	[menuItem release];
}

/* Create a window menu */
static void 
setupWindowMenu(void)
{
	NSMenu *windowMenu;
	NSMenuItem *windowMenuItem;
	NSMenuItem *menuItem;

	windowMenu = [[NSMenu alloc] initWithTitle:@"Window"];

	/* "Minimize" item */
	menuItem = [[NSMenuItem alloc] initWithTitle: @"Minimize"
		action: @selector(performMiniaturize: ) keyEquivalent:@"m"];
	[windowMenu addItem:menuItem];
	[menuItem release];

	/* Put menu into the menubar */
	windowMenuItem =[[NSMenuItem alloc] initWithTitle:@"Window"
		action:nil keyEquivalent:@""];
	[windowMenuItem setSubmenu:windowMenu];
	[[NSApp mainMenu] addItem:windowMenuItem];

	/* Tell the application object that this is now the window menu */
	[NSApp setWindowsMenu:windowMenu];

	/* Finally give up our references to the objects */
	[windowMenu release];
	[windowMenuItem release];
}

/* Replacement for NSApplicationMain */
static void 
customApplicationMain(int argc, char **argv)
{
	NSAutoreleasePool *pool = [[NSAutoreleasePool alloc] init];
	SCMain *m;

	/* Ensure the application object is initialised */
	[SCApplication sharedApplication];
	CPSProcessSerNum PSN;

	/* Tell the dock about us */
	if (!CPSGetCurrentProcess(&PSN))
		if (!CPSEnableForegroundOperation(&PSN, 0x03, 0x3C, 0x2C, 0x1103))
			if (!CPSSetFrontProcess(&PSN))
				[SCApplication sharedApplication];

	/* Set up the menubar */
	[NSApp setMainMenu:[[NSMenu alloc] init]];
	setApplicationMenu();
	setupWindowMenu();

	/* Create SCMain and make it the app delegate */
	m = [[SCMain alloc] init];
	[NSApp setDelegate:m];

	/* Start the main event loop */
	[NSApp run];

	[m release];
	[pool release];
}

/* Called when the internal event loop has just started running */
-(void) applicationDidFinishLaunching:(NSNotification *) note
{
	/* Set the working directory to the .app's parent directory */
	[self setupWorkingDirectory:gFinderLaunch];

	/* Hand over some events to Cocoa */
	setenv("SDL_ENABLEAPPEVENTS", "1", 1);

	/* Hand off to main application code */
	gCalledAppMainline = TRUE;
	gReturnCode = client_main(gArgc, gArgv);

	/* we're done, stop the application. */
	[NSApp stop: self];
}
@end

/* Main entry point to executable */
int 
main(int argc, char **argv)
{
	char *fakeArgv[2];

	/* unless other specified, return success */
	gReturnCode = 0;

	/* launched from finder? */
	if (argc >= 2 && strncmp(argv[1], "-psn", 4) == 0) {
		fakeArgv[0] = argv[0];
		fakeArgv[1] = NULL;
		gArgv = fakeArgv;
		gArgc = 1;
		gFinderLaunch = YES;

	/* does not seem like */
	} else {
		gArgc = argc;
		gArgv = argv;
		gFinderLaunch = NO;
	}

	customApplicationMain(argc, argv);

	return gReturnCode;
}
