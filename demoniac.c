#include <stdio.h>
#include <unistd.h>
#include <asm/io.h>

#define	SHORT_S	110
#define	LONG_S	SHORT_S * 2

void Sound(unsigned int freq)
{
	unsigned int counter = 1193280 / freq;		/* cycle counter */

	outb(0xB6, 0x43);				/* prepare timer */
	outb(*((unsigned char *) &counter + 0), 0x42);	/* low byte */ 
	outb(*((unsigned char *) &counter + 1), 0x42);	/* high byte */
	outb(inb(0x61) | 0x3, 0x61);			/* turn speaker ON */
}

void NoSound(void)
{
	outb(inb(0x61) & 0xFC, 0x61);			/* turn speaker OFF */
}

void DoSound(unsigned int freq, unsigned int time)
{
	Sound(freq);
	usleep(time * 1000);	// microsseconds => milliseconds
	NoSound();
}

int InitSound(void)
{
	if (ioperm(0x42, 2, 1) || ioperm(0x61, 1, 1) || ioperm(0x80, 1, 1))	/* can we do that? */
		return -1;				/* no; see errno() */
	else
		return 0;				/* yeah, I done it!!! */
}


int main(int argc, char *argv[])
{
	if (InitSound())
	{
		fprintf(stderr, "must be root!!!\n");
		return -1;
	}
	
	DoSound(466, SHORT_S);		// A#4
	DoSound(622, SHORT_S);		// D#5
	DoSound(784, SHORT_S);		// G5
	DoSound(932, LONG_S);		// A#5
	DoSound(784, SHORT_S);		// G5
	DoSound(932, LONG_S + SHORT_S);	// A#5

	return 0;
}
