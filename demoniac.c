#include <stdio.h>
#include <unistd.h>
#include <asm/io.h>

#define LEN 150
#define MLEN 1

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

	usleep(MLEN * 1000);
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
	
	DoSound(587, LEN * 2);	// D5
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5
	DoSound(698, LEN);	// F5
	DoSound(698, LEN);	// F5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5

	DoSound(523, LEN * 2);	// C5
	DoSound(392, LEN);	// G4
	DoSound(392, LEN);	// G4
	DoSound(523, LEN);	// C5
	DoSound(523, LEN);	// C5
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(523, LEN);	// C5
	DoSound(523, LEN);	// C5
	DoSound(659, LEN);	// E5
	DoSound(523, LEN);	// C5

	DoSound(587, LEN * 2);	// D5
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5
	DoSound(698, LEN);	// F5
	DoSound(698, LEN);	// F5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5

	DoSound(523, LEN * 2);	// C5
	DoSound(392, LEN);	// G4
	DoSound(392, LEN);	// G4
	DoSound(523, LEN);	// C5
	DoSound(523, LEN);	// C5
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(659, LEN);	// E5
	DoSound(659, LEN);	// E5
	DoSound(587, LEN);	// D5
	DoSound(587, LEN);	// D5
	DoSound(523, LEN);	// C5
	DoSound(523, LEN);	// C5
	DoSound(659, LEN);	// E5
	DoSound(523, LEN);	// C5


	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(330, LEN);	// E4
	DoSound(330, LEN);	// E4
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4
	DoSound(523, LEN);	// C5
	DoSound(523, LEN);	// C5
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4

	DoSound(392, LEN);	// G4
	DoSound(392, LEN);	// G4
	DoSound(294, LEN);	// D4
	DoSound(294, LEN);	// D4
	DoSound(392, LEN);	// G4
	DoSound(392, LEN);	// G4
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(392, LEN);	// G4
	DoSound(392, LEN);	// G4
	DoSound(494, LEN);	// B4
	DoSound(392, LEN);	// G4


	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(330, LEN);	// E4
	DoSound(330, LEN);	// E4
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4
	DoSound(523, LEN);	// C5
	DoSound(523, LEN);	// C5
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4
	DoSound(440, LEN);	// A4
	DoSound(440, LEN);	// A4
	DoSound(494, LEN);	// B4
	DoSound(494, LEN);	// B4

	DoSound(349, LEN);	// F4
	DoSound(349, LEN);	// F4
	DoSound(262, LEN);	// C4
	DoSound(262, LEN);	// C4
	DoSound(349, LEN);	// F4
	DoSound(349, LEN);	// F4
	DoSound(392, LEN);	// G4
	DoSound(392, LEN);	// G4
	DoSound(440, LEN + (LEN / 8));	// A4
	DoSound(440, LEN + (LEN / 7));	// A4
	DoSound(392, LEN + (LEN / 6));	// G4
	DoSound(392, LEN + (LEN / 5));	// G4
	DoSound(349, LEN + (LEN / 4));	// F4
	DoSound(349, LEN + (LEN / 3));	// F4
	DoSound(440, LEN + (LEN / 2));	// A4
	DoSound(349, LEN * 4);	// F4

	return 0;
}
