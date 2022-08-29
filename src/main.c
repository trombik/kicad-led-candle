/*
 * Copyright (c) 2022 Tomoyuki Sakurai <y@trombik.org>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <Arduino.h>
#include <avr/io.h>
#include <util/delay.h>

#if defined(USING_PINK_NOISE_FLOAT)

/* rand() is defined in stdlib.h */
#include <stdlib.h>
#endif

/* due to size limit in flash memory, use small PRNG instead of rand() in
 * avr-libc.
 */
#include "tiny_random.h"

/* the initial seed for tiny_random() */
#define PRNG_SEED 0xDEAD

/* time to delay */
#define DELAY_MS (70)

/* generates pink noise, aka 1/f noise. returns 0..1000. uses floating point
 * math. too big for attiny13a.
 *
 * use it when flash memory size permits.
 */
#if defined(USING_PINK_NOISE_FLOAT)
static int
pink_noise(void)
{
	static int value = 0.1;
	if (value < 0.5) {
		value += value * value * 2;
	} else {
		value -= (1.0 - value) * (1.0 - value) * 2;
	}

	/* as the value tends to stay both ends of the range, use random */
	if (value < 0.1 || value > 0.9) {
		value = rand() / RAND_MAX;
	}
	/* multiply by 1000 for compatibility in main() */
	return value * 1000;
}
#elif defined(USING_WHITE_NOISE)
static int
white_noise(void)
{
	return tiny_random() / (0xffff / 1000) + 1;
}
#else

/* integer version of pink_noise(). does not use floating point math. */
static int
pink_noise(void)
{
	static int value = 100;
	if (value < 500) {
		value += (value * value / 1000) * 2;
	} else {
		value -= ((1000 - value) * (1000 - value) / 1000) * 2;
	}

	if (value < 100 || value > 900) {
		value = tiny_random() / ((0xffff / 1000) + 1);
	}
	return value;
}
#endif

int duty;

int
main(void)
{

	/* configure PB0 and PB1 as output */
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);

	/* set initial state to off */
	digitalWrite(0, LOW);
	digitalWrite(1, LOW);

	/* initialize PRNG */
	tiny_random_init(PRNG_SEED);

	for (;;) {
#if defined(USING_WHITE_NOISE)
		duty = white_noise();
#else
		duty = pink_noise();
#endif
		analogWrite(0, duty);

		/* another (optional) LED for reversed phase */
		analogWrite(1, 0xff - duty);
		_delay_ms(DELAY_MS);
	}
}
