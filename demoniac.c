/*

		     Demoniac v0.666a -- DAEMON'iac Advisor
		     ======================================

Coded by Stas (Mail: stanis@linuxmail.org; URL: http://sysd.org/);
(C)opyLeft by SysD Destructive Labs, 1997-2004


DESCRIPTION:

	Produces sound (in this specific case, the beginning of Iron Maiden song
"Fear Of The Dark") on internal PC Speaker hardware. Compiles and works on:

 * DOS (DJGPP, Turbo C)
 * Windows 9x/NT/2K/XP (Borland C, Microsoft Visual C, MinGW)
 * Linux (gcc)
 * FreeBSD (gcc)

(see compiling instructions below)

Can be useful to provide audible alert without acessing soundcard. Requires
superuser powers except for Windows NT/2K/XP (note that superuser is default
mode for DOS & Windows 9x ;)
This code is also supposed to be portable to *any* OS that runs on i386
architecture. Other architectures are not supported and neither will be!


COMPILING:

 * Compile on Linux & FreeBSD with:
	gcc -O2 -o demoniac demoniac.c; strip demoniac

 * Compile Windows generic with:
	gcc -O2 -o demoniac.exe demoniac.c
	strip demoniac.exe
		or
	cl demoniac.c
		or
	bcc32 demoniac.c

 * Compile Windows 9x-specific (for debugging) with:
	gcc -D_WIN9X -O2 -o demoniac.exe demoniac.c
		or
	cl -D_WIN9X demoniac.c
		or
	bcc32 -D_WIN9X demoniac.c

 * Compile Windows NT/2K/XP-specific (for debugging) with:
	gcc -D_WINNT -O2 -o demoniac.exe demoniac.c
		or
	cl -D_WINNT demoniac.c
		or
	bcc32 -D_WINNT demoniac.c

 * Compile on DOS under Turbo C:
	tcc -O demoniac.c

 * Compile on DOS under DJGPP:
	gcc -O2 -o demoniac.exe demoniac.c
	strip demoniac


	Note: you might try -DNOFEAR compiler flag if you want more simple music.
	(this WAS the original music played by demoniac 0.1b ;)


USAGE:

	Reprogram sound in the way you want and paste into your own source :)


BUGS:

	* Sound timing is different on different circunstances.
	* Sound finalization isn't properly handled on DOS/Win9x.


THANKS:

	* Dr. Checks, for music conversion
	* Iron Maiden, for music itself :)
	* Linus Torvalds, for /usr/src/linux/include/asm-i386
	* Peter Norton, for PC speaker controlling algorithm
	* Riku Saikkonen, for his IO-Port-Programming-HOWTO
	* The Snowblind Alliance, for Rio utility v1.07
	* t0p, for idea of daemon beeper

*/


#ifdef _WIN32

	#define WIN32_LEAN_AND_MEAN
	#include <stdio.h>
	#include <stdlib.h>
	#include <windows.h>

	#if defined(_MSC_VER)

		#pragma comment(linker,"/ENTRY:main")
		#pragma comment(linker,"/MERGE:.rdata=.data")
		#pragma comment(linker,"/MERGE:.text=.data")

		#pragma comment(linker,"/NODEFAULTLIB:libc.lib")
		#pragma comment(linker,"/NODEFAULTLIB:libcmt.lib")
		#pragma comment(lib,"msvcrt.lib")
		#pragma comment(lib,"kernel32.lib")

		#if (_MSC_VER < 1300)
	
			#pragma comment(linker,"/IGNORE:4078")
			#pragma comment(linker,"/OPT:NOWIN98")

		#endif

	#elif defined(__BORLANDC__)

		typedef int (*toutp) (unsigned short port, int databyte);
		typedef int (*tinp) (unsigned short port);

		toutp _outp	= NULL;
		tinp _inp	= NULL;

		int ImportMSFunc(void)
		{
			HINSTANCE hMsvcrt;

			hMsvcrt = LoadLibrary("msvcrt.dll");
			if (hMsvcrt == NULL)
				return 0;

			_outp = (toutp) GetProcAddress(hMsvcrt, "_outp");
			if (_outp == NULL)
				return 0;

			_inp = (tinp) GetProcAddress(hMsvcrt, "_inp");
			if (_inp == NULL)
				return 0;

			return 1;
		}

	#endif

	#define OUTPORT(p, v) _outp((uword) (p), v)
	#define INPORT(p) _inp(p)
	#define SLEEP(t) Sleep(t)

#elif defined(__TURBOC__)

	#include <dos.h>

	#define _DOS

	#define SLEEP(t) delay(t)

#elif defined(__DJGPP__)

	#include <pc.h>

	#define _DOS

	#undef nosound

	void nosound(void)
	{
		sound(0);
		return;
	}

	#define SLEEP(t) delay(t/2)

#endif


#if defined (_WIN9X)

	#define DoSound(freq, time) HWDoSound(freq, time)

#elif defined(_WINNT)

	#define INTERN

	#define DoSound(freq, time) Beep(freq, time)

#elif defined (_WIN32)

	#define API_OR_HW

#elif defined (_DOS)

	#define INTERN
	#define NEEDSTOP

	void DoSound(unsigned int freq, unsigned int time)
	{
		sound(freq);
		SLEEP(time);
		nosound();

		return;
	}

#elif defined(__linux__)

	#include <asm/io.h>
	#include <signal.h>
	#include <stdio.h>

	#define NEEDINIT
	#define NEEDSTOP
	#define IS_UNIX

	#define OUTPORT(p, v) outb(v, p)
	#define INPORT(p) inb(p)
	#define SLEEP(t) usleep(t*1000) // microsseconds => milliseconds

	#define DoSound(freq, time) HWDoSound(freq, time)

#elif defined(__FreeBSD__)

	#include <fcntl.h>
	#include <machine/cpufunc.h>
	#include <signal.h>
	#include <stdio.h>

	#define NEEDINIT
	#define NEEDSTOP
	#define IS_UNIX

	#define OUTPORT(p, v) outb(p, v)
	#define INPORT(p) inb(p)
	#define SLEEP(t) usleep(t*1000) // microsseconds => milliseconds

	#define DoSound(freq, time) HWDoSound(freq, time)

#else

	#error Unsupported platform!

#endif


#ifndef INTERN

	#ifdef uword
		#undef uword
	#endif
	typedef unsigned short uword;

	#ifdef uchar
		#undef uchar
	#endif
	typedef unsigned char uchar;

	#ifndef LOBYTE
		#define LOBYTE(w) ((uchar) (w))
	#endif

	#ifndef HIBYTE
		#define HIBYTE(w) ((uchar) (((uword) (w) >> 8) & 0xFF))
	#endif

	void HWSound(unsigned int freq)
	{
		uword counter = 1193280 / freq;		/* cycle counter */

		OUTPORT(0x43, 0xB6);			/* prepare timer */
		OUTPORT(0x42, LOBYTE(counter));		/* send low byte */ 
		OUTPORT(0x42, HIBYTE(counter));		/* send high byte */
		OUTPORT(0x61, INPORT(0x61) | 0x03);	/* turn speaker ON */

		return;
	}

	void HWNoSound(void)
	{
		OUTPORT(0x61, INPORT(0x61) & 0xFC);	/* turn speaker OFF */

		return;
	}

	void HWDoSound(unsigned int freq, unsigned int time)
	{
		HWSound(freq);
		SLEEP(time);
		HWNoSound();

		return;
	}

#endif


#ifdef API_OR_HW

	void DoSound(unsigned int freq, unsigned int time)
	{
		static OSVERSIONINFO osver;

		if (osver.dwOSVersionInfoSize != sizeof(OSVERSIONINFO))
		{
			osver.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
			GetVersionEx(&osver);
		}

		if (osver.dwPlatformId == VER_PLATFORM_WIN32_NT)
			Beep(freq, time);
		else
			HWDoSound(freq, time);

		return;
	}

#endif


#ifdef NEEDINIT

	#if defined(__linux__)

	int InitSound(void)
	{
		if (	ioperm(0x42, 2, 1) ||
			ioperm(0x61, 1, 1) ||
			ioperm(0x80, 1, 1)	)	/* can we do that? */
			return 0;			/* no; see errno() */
		else
			return 1;			/* yeah, I've done it!!! */
	}

	#elif defined(__FreeBSD__)

	int InitSound(void)
	{
		if (open("/dev/io", O_RDONLY) == -1)
			return 0;
		else
			return 1;
	}

	#else

		#error I dunno how to init sound on this platform!

	#endif

#endif


#ifdef NEEDSTOP

	#if defined(IS_UNIX)

		static void shutup(int signal)
		{
			HWNoSound();
			exit(1);
		}

	#endif

	void StopHandle(void)
	{
		#if defined(_DOS)

			atexit(nosound);

		#elif defined(IS_UNIX)

			signal(SIGTERM, shutup);
			signal(SIGINT, shutup);
			signal(SIGHUP, shutup);

		#else

			#error I dunno how to handle finalization on this platform!

		#endif

		return;
	}

#endif


#define LEN 150
#define MLEN 1
#define PlayNote(freq, time) { DoSound(freq, time); SLEEP(MLEN); }


int main(void)
{
	#ifdef NEEDINIT
		if (!InitSound())
		{
			fprintf(stderr, "must be root!!!\n");
			return -1;
		}
	#endif

	#if defined(_WIN32) && defined(__BORLANDC__)
		if (!ImportMSFunc())
		{
			fprintf(stderr, "can't import from MSVC runtime!!!\n");
			return -1;
		}
	#endif

	#ifdef NEEDSTOP
		StopHandle();
	#endif

	
	#ifndef NOFEAR

		PlayNote(587, LEN * 2);		// D5
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5
		PlayNote(698, LEN);		// F5
		PlayNote(698, LEN);		// F5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5

		PlayNote(523, LEN * 2);		// C5
		PlayNote(392, LEN);		// G4
		PlayNote(392, LEN);		// G4
		PlayNote(523, LEN);		// C5
		PlayNote(523, LEN);		// C5
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(523, LEN);		// C5
		PlayNote(523, LEN);		// C5
		PlayNote(659, LEN);		// E5
		PlayNote(523, LEN);		// C5

		PlayNote(587, LEN * 2);		// D5
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5
		PlayNote(698, LEN);		// F5
		PlayNote(698, LEN);		// F5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5

		PlayNote(523, LEN * 2);		// C5
		PlayNote(392, LEN);		// G4
		PlayNote(392, LEN);		// G4
		PlayNote(523, LEN);		// C5
		PlayNote(523, LEN);		// C5
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(659, LEN);		// E5
		PlayNote(659, LEN);		// E5
		PlayNote(587, LEN);		// D5
		PlayNote(587, LEN);		// D5
		PlayNote(523, LEN);		// C5
		PlayNote(523, LEN);		// C5
		PlayNote(659, LEN);		// E5
		PlayNote(523, LEN);		// C5


		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(330, LEN);		// E4
		PlayNote(330, LEN);		// E4
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4
		PlayNote(523, LEN);		// C5
		PlayNote(523, LEN);		// C5
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4

		PlayNote(392, LEN);		// G4
		PlayNote(392, LEN);		// G4
		PlayNote(294, LEN);		// D4
		PlayNote(294, LEN);		// D4
		PlayNote(392, LEN);		// G4
		PlayNote(392, LEN);		// G4
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(392, LEN);		// G4
		PlayNote(392, LEN);		// G4
		PlayNote(494, LEN);		// B4
		PlayNote(392, LEN);		// G4


		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(330, LEN);		// E4
		PlayNote(330, LEN);		// E4
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4
		PlayNote(523, LEN);		// C5
		PlayNote(523, LEN);		// C5
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4
		PlayNote(440, LEN);		// A4
		PlayNote(440, LEN);		// A4
		PlayNote(494, LEN);		// B4
		PlayNote(494, LEN);		// B4

		PlayNote(349, LEN);		// F4
		PlayNote(349, LEN);		// F4
		PlayNote(262, LEN);		// C4
		PlayNote(262, LEN);		// C4
		PlayNote(349, LEN);		// F4
		PlayNote(349, LEN);		// F4
		PlayNote(392, LEN);		// G4
		PlayNote(392, LEN);		// G4
		PlayNote(440, LEN + (LEN / 8));	// A4
		PlayNote(440, LEN + (LEN / 7));	// A4
		PlayNote(392, LEN + (LEN / 6));	// G4
		PlayNote(392, LEN + (LEN / 5));	// G4
		PlayNote(349, LEN + (LEN / 4));	// F4
		PlayNote(349, LEN + (LEN / 3));	// F4
		PlayNote(440, LEN + (LEN / 2));	// A4
		PlayNote(349, LEN * 4);		// F4

	#else

		PlayNote(466, LEN);		// A#4
		PlayNote(622, LEN);		// D#5
		PlayNote(784, LEN);		// G5
		PlayNote(932, LEN * 2);		// A#5
		PlayNote(784, LEN);		// G5
		PlayNote(932, LEN * 3);		// A#5

	#endif


	return 0;
}
