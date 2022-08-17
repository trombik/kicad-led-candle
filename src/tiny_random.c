/**
 * Copyright (c) 2017, Łukasz Marcin Podkalicki <lpodkalicki@gmail.com>
 * Lightweight library of 16 bit random number generator based on LFSR.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its contributors
 * may be used to endorse or promote products derived from this software without
 * specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <avr/eeprom.h>
#include "tiny_random.h"

static uint16_t random_number = 0;

static uint16_t
lfsr16_next(uint16_t n)
{

	return (n >> 0x01U) ^ (-(n & 0x01U) & 0xB400U);
}


void
tiny_random_init(uint16_t seed)
{
#ifdef USE_RANDOM_SEED
	random_number = lfsr16_next(eeprom_read_word((uint16_t *)RANDOM_SEED_ADDRESS) ^ seed);
        eeprom_write_word((uint16_t *)0, random_number);
#else
	random_number = seed;
#endif	/* !USE_RANDOM_SEED */
}


uint16_t
tiny_random(void)
{

	return (random_number = lfsr16_next(random_number));
}

