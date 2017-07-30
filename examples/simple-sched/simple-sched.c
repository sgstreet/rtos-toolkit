#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <errno.h>

#include <cmsis.h>
#include <diag/diag.h>
#include <scheduler/simple-sched.h>

struct test_task_descriptor
{
	struct task_descriptor td;
	unsigned long tid;
	uint8_t __attribute__((aligned(8))) stack[2048];
};

static int diag_task(unsigned int tid, void *context)
{
	struct task_descriptor *td = context;
	diag_printf("%s running\n", td->name);
	for (int counter = 0; counter < 10000; ++counter) {
		size_t random_size = (size_t)(rand() % 10) + 1;
		char  random_stack[random_size];
		memset(random_stack, 0xff, random_size);
		unsigned long delay = (unsigned long)(rand() % 20);
		if (delay > 9)
			scheduler_yield();
		else
			scheduler_sleep(delay);
	}

	diag_printf("%s completed\n", td->name);
	return 0;
}

static int diag_task_float(unsigned int tid, void *context)
{
	struct task_descriptor *td = context;
	diag_printf("%s running\n", td->name);
	for (int counter = 0; counter < 10000; ++counter) {
		size_t random_size = (size_t)(rand() % 10) + 1;
		char  random_stack[random_size];
		memset(random_stack, 0xff, random_size);
		float delay = (rand() % 20) * 2.5;
		diag_printf("%s delaying: %f\n", td->name, delay);
		if (delay > 9)
			scheduler_yield();
		else
			scheduler_sleep(delay);
	}

	diag_printf("%s completed\n", td->name);
	return 0;
}

static int main_task(unsigned int tid, void *context)
{
	struct test_task_descriptor descriptors[] =
	{
		{
			.td =
			{
				.name = "diag_task_a",
				.entry_point = diag_task,
				.context = &descriptors[0].td,
				.stack_size = sizeof(descriptors[0].stack),
				.stack = descriptors[0].stack,
			},
		},
		{
			.td =
			{
				.name = "diag_task_b",
				.entry_point = diag_task,
				.context = &descriptors[1].td,
				.stack_size = sizeof(descriptors[1].stack),
				.stack = descriptors[1].stack,
			},
		},
		{
			.td =
			{
				.name = "diag_task_c - float",
				.entry_point = diag_task_float,
				.context = &descriptors[2].td,
				.stack_size = sizeof(descriptors[2].stack),
				.stack = descriptors[2].stack,
			},
		},
		{
			.td =
			{
				.name = "diag_task_d - float",
				.entry_point = diag_task_float,
				.context = &descriptors[3].td,
				.stack_size = sizeof(descriptors[3].stack),
				.stack = descriptors[3].stack,
			},
		},
	};

	/* Launch the tasks */
	diag_printf("creating tasks\n");
	for (int i = 0; i < 4; ++i) {
		if ((descriptors[i].tid = scheduler_create(&descriptors[i].td)) == 0) {
			diag_printf("failed to create task '%s': %d\n", descriptors[i].td.name, errno);
			abort();
		}
	}

	/* Join the tasks */
	for (int i = 0; i < 4; ++i) {
		diag_printf("joining task '%s' with 0x%08lx\n", descriptors[i].td.name, descriptors[i].tid);
		if (scheduler_join(descriptors[i].tid, 0, 0) < 0) {
			diag_printf("failed to join task '%s': %d\n", descriptors[i].td.name, errno);
			abort();
		}
	}

	/* All done */
	diag_printf("all done");
	return 0;
}

static uint8_t __attribute__((aligned(8))) main_task_stack[40 * 1024];
static const struct task_descriptor main_task_descriptor =
{
	.name = "main_task",
	.entry_point = main_task,
	.context = 0,
	.stack_size = sizeof(main_task_stack),
	.stack = main_task_stack,
};


int main(int argc, char* argv[])
{
	struct scheduler scheduler;

	return scheduler_run(&scheduler, 1, &main_task_descriptor);
}
