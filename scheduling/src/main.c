#include <stdio.h>
#include <string.h>
#include <zephyr/kernel.h>

#define STACK_SIZE 1024
#define SLOT_MS 10
#define SLOTS 150

struct job {
	char name;
	int priority;    /* lower number = higher priority, negative = cooperative */
	int arrival_ms;  /* when the job is ready to run */
	int work_ms;     /* how much CPU time the job needs */
	int deadline_ms; /* used only with CONFIG_SCHED_DEADLINE */
};

/* Change this table in the tasks. */
static struct job jobs[] = {
	/* name, priority, arrival_ms, work_ms */
	{ 'A', 5, 0, 300 },
	{ 'B', 5, 0, 100 },
	{ 'C', 5, 0, 50 },
};

#define JOBS ARRAY_SIZE(jobs)

static char timeline[SLOTS + 1];
static int finished_ms[JOBS];
static int64_t start_ms;
static uint32_t loops_per_ms;

K_THREAD_STACK_ARRAY_DEFINE(stacks, JOBS, STACK_SIZE);
static struct k_thread threads[JOBS];

/* Keeps the CPU busy and writes who is running into the timeline. */
static void burn_cpu(char name, uint32_t loops)
{
	for (volatile uint32_t i = 0; i < loops; i++) {
		int slot = (k_uptime_get() - start_ms) / SLOT_MS;

		if (slot < SLOTS) {
			timeline[slot] = name;
		}
	}
}

/* k_busy_wait() is no good here: its time runs even when the thread is not running. */
static void calibrate(void)
{
	uint32_t rounds = 0;

	start_ms = k_uptime_get();
	while (k_uptime_get() - start_ms < 100) {
		burn_cpu('.', 100);
		rounds++;
	}
	loops_per_ms = rounds;
}

static void job_thread(void *p1, void *p2, void *p3)
{
	struct job *job = p1;
	int index = POINTER_TO_INT(p2);

#ifdef CONFIG_SCHED_DEADLINE
	k_thread_deadline_set(k_current_get(), k_ms_to_cyc_ceil32(job->deadline_ms));
	k_yield();
#endif
	for (int ms = 0; ms < job->work_ms; ms++) {
		burn_cpu(job->name, loops_per_ms);
	}
	finished_ms[index] = k_uptime_get() - start_ms;
}

static void print_results(void)
{
	int last = 0;
	int sum = 0;

	for (int i = 0; i < JOBS; i++) {
		last = MAX(last, finished_ms[i]);
	}
	timeline[MIN(last / SLOT_MS + 1, SLOTS)] = '\0';

	printf("\nOne character = %d ms, '.' = no job was running\n", SLOT_MS);
	printf("0         100       200       300       400       500\n");
	printf("%s\n\n", timeline);
	printf("job  priority  arrival  work  finished  turnaround\n");
	for (int i = 0; i < JOBS; i++) {
		int turnaround = finished_ms[i] - jobs[i].arrival_ms;

		printf("  %c  %8d  %7d  %4d  %8d  %10d\n", jobs[i].name, jobs[i].priority,
		       jobs[i].arrival_ms, jobs[i].work_ms, finished_ms[i], turnaround);
		sum += turnaround;
	}
	printf("average turnaround: %d ms\n", sum / (int)JOBS);
}

int main(void)
{
	calibrate();
	memset(timeline, '.', SLOTS);

	/* All jobs are created first, so that none of them starts before the others exist. */
	k_sched_lock();
	start_ms = k_uptime_get();
	for (int i = 0; i < JOBS; i++) {
		k_thread_create(&threads[i], stacks[i], STACK_SIZE, job_thread, &jobs[i],
				INT_TO_POINTER(i), NULL, jobs[i].priority, 0,
				K_MSEC(jobs[i].arrival_ms));
	}
	k_sched_unlock();

	for (int i = 0; i < JOBS; i++) {
		k_thread_join(&threads[i], K_FOREVER);
	}
	print_results();
	return 0;
}
