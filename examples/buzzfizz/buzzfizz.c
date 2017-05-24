/*
 * Copyright (C) 2017 Red Rocket Computing, LLC
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/.
 *
 * buzzfizz.c
 *
 * Created on: Apr 19, 2017
 *     Author: Stephen Street (stephen@redrocketcomputing.com)
 */

#include <stdio.h>
#include <stdbool.h>

#define array_size(x) (sizeof(x)/sizeof(x[0]))

/* Indexes of Fibonacci Primes from https://oeis.org/A001605, includes proven and probable primes */
static const unsigned long long prime_idxs[] = {
	3, 4, 5, 7, 11, 13, 17, 23, 29, 43, 47, 83, 131, 137, 359, 431, 433, 449, 509, 569,
	571, 2971, 4723, 5387, 9311, 9677, 14431, 25561, 30757, 35999, 37511, 50833, 81839,
	104911, 130021, 148091, 201107, 397379, 433781, 590041, 593689, 604711, 931517,
	1049897, 1285607, 1636007, 1803059, 1968721, 2904353
};

static inline int __attribute__((always_inline)) uaddll_overflow(uint64_t op1, uint64_t op2, uint64_t *result)
{
	uint64_t sum;
	uint32_t overflow;

	asm volatile(
		"adds	%Q[sum], %Q[op1], %Q[op2]			\n\t" /* Add the lower 32 bits, set the carry bit */
		"adcs	%R[sum], %R[op1], %R[op2]			\n\t" /* Add the upper 32 bits with carry, set the carry for overflow */
		"itte	cc									\n\t" /* Setup of if/then/else assembly block */
		"strdcc	%Q[sum], %R[sum], %[addr]			\n\t" /* Store the 64 bit addition if carry not set */
		"movcc	%[overflow], #0						\n\t" /* Clear overflow flag if carry not set */
		"movcs	%[overflow], #1						\n\t" /* Set the overflow flag if carry is set */
		: [sum] "=&r" (sum), [addr] "=m" (*result), [overflow] "=r" (overflow) : [op1] "r" (op1), [op2] "r" (op2), [result] "r" (result) : "cc"
	);

	return overflow;
}

static bool buzzfizz(unsigned long long n)
{
	unsigned long long f = 0;
	unsigned long long u1 = 0;
	unsigned long long u2 = 1;
	int prime_idx = 0;

	/* The seeds are neither prime nor multiples of 3, 5 or 15, just print */
	printf("%lu: %llu \n", 0L, u1);
	printf("%lu: %llu\n", 1L, u2);

	/* Generate and test all Fibonacci numbers less then or equal to n */
	for (unsigned long i = 2; i <= n; ++i) {

		/* Generate the next F(n) using the GCC assigned addition builtin to detect overflow */
		if (uaddll_overflow(u1, u2, &f))
			return false;

		/* We are using the prime and probable prime Fibonacci number index from https://oeis.org/A001605
		 * to elimate the prime test by checking just the current index again the known prime index */
		if (prime_idx < array_size(prime_idxs) && i == prime_idxs[prime_idx]) {

			/* Found a prime */
			printf("%lu: BuzzFizz\n", i);

			/* Make sure we move to the next index */
			++prime_idx;

		} else {

			/* Use modulo arithmetic to check for multiples of 3 and 5. Since numbers divisible 3 AND 5 must
			 * be also divisible by 15, we encode the divisibility:
			 * 0 - Not divisible by 3, 5 or 15
			 * 3 - Divisible by 3
			 * 5 - Divisible by 5
			 * 8 - Divisible by 3, 5 and 15 */
			switch ((f % 3 == 0 ? 3 : 0) + (f % 5 == 0 ? 5 : 0)) {
				case 3:
					printf("%lu: Buzz\n", i);
					break;
				case 5:
					printf("%lu: Fizz\n", i);
					break;
				case 8:
					printf("%lu: FizzBuzz\n", i);
					break;
				default:
					printf("%lu: %llu\n", i, f);
					break;
			}
		}

		/* Update the previous sequence values */
		u1 = u2;
		u2 = f;
	}

	/* No overflow detected */
	return true;
}

int main(int argc, char **argv)
{
	if (!buzzfizz(200)) {
		printf("buzzfizz overflowed\n");
		return 1;
	}

	return 0;
}



