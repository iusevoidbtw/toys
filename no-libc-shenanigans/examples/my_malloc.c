#include <mem.h>
#include <syscall.h>
#include <types.h>

/*
 * ===========================================================================
 * the malloc implementation
 */

/* config options */
#ifdef __BIGGEST_ALIGNMENT__
#define ALIGN __BIGGEST_ALIGNMENT__
#else
#define ALIGN 16 /* should be enough */
#endif

/* types */
struct blockinfo {
	struct blockinfo *next;
	size_t size;
	int free;
	/* gcc adds 4 bytes of padding here without packed */
} __attribute__((__packed__));

/* function declarations and macros */
#define BRK_FAILED(addr) ((void *)syscall(SYS_brk, addr) < (void *)(addr))

#if ALIGN > 4
#define ALIGNPAD (ALIGN - 4)
#define BLOCKINFO(ptr) ((struct blockinfo *)((uint8_t *)(ptr) - ALIGNPAD) - 1)
#else
#define BLOCKINFO(ptr) ((struct blockinfo *)(ptr) - 1)
#endif

#ifdef DEBUG
#define DEBUG_LOG(s) syscall(SYS_write, 2, s, sizeof(s) - 1)
#define BREAKPOINT() asm("int3; nop")
static void print_ptr(void *ptr);
static void print_size(size_t n);
#endif

/* global variables */
static struct blockinfo *base = NULL, *last = NULL;

/* functions */
static struct blockinfo *
find_free(size_t size)
{
	struct blockinfo *curr = base;
	while (curr) {
#ifdef DEBUG
		DEBUG_LOG("base->next = ");
		print_ptr(base->next);
		DEBUG_LOG(" curr_next = ");
		print_ptr(curr->next);
		DEBUG_LOG("\n");
#endif
#ifdef DEBUG
		if (curr) {
			DEBUG_LOG("found block at ");
			print_ptr(curr);
			DEBUG_LOG(" size = ");
			print_size(curr->size);
			if (curr->free) DEBUG_LOG(" free = 1\n");
			else DEBUG_LOG(" free = 0\n");
		}
#endif
		if (curr && curr->free && curr->size >= size) break;
		curr = curr->next;
	}
	if (!(curr && curr->free && curr->size >= size)) return NULL;
	return curr;
}

static struct blockinfo *
alloc_block(size_t size)
{
	struct blockinfo *block = (struct blockinfo *)syscall(SYS_brk, NULL);
	if (BRK_FAILED((uint8_t *)block + size + sizeof(struct blockinfo) + ALIGNPAD)) return NULL;
#ifdef DEBUG
	if (last) {
		last->next = block;
		DEBUG_LOG("setting ");
		print_ptr(&last->next);
		DEBUG_LOG(" to ");
		print_ptr(block);
		DEBUG_LOG("; &base->next = ");
		print_ptr(&base->next);
		DEBUG_LOG("\n");
	}
#else
	if (last) last->next = block;
#endif
	block->size = size;
	block->next = NULL;
	block->free = 0;
	return block;
}

static void *
my_malloc(size_t size)
{
	struct blockinfo *block;
	if (size == 0) return NULL;
	if (!base) {
		block = alloc_block(size);
		if (!block) return NULL;
		base = last = block;
	} else {
		if (!(block = find_free(size))) {
			block = alloc_block(size);
			if (!block) return NULL;
			last = block;
		} else {
			block->free = 0;
		}
	}
#ifdef ALIGNPAD
#ifdef DEBUG
	void *ptr = (uint8_t *)(block + 1) + ALIGNPAD;
	void *bp = BLOCKINFO(ptr);
	if (bp != block) {
		DEBUG_LOG("ERROR: blockinfo not accessed properly after alignment:\n");
		DEBUG_LOG("actual blockinfo = ");
		print_ptr(block);
		DEBUG_LOG("\nrecovered blockinfo pointer = ");
		print_ptr(bp);
		DEBUG_LOG("\nmore debug info:\n\nblock + 1 = ");
		print_ptr(block + 1);
		DEBUG_LOG("\n(uint8_t *)(block + 1) + ALIGNPAD = ");
		print_ptr((uint8_t *)(block + 1) + ALIGNPAD);
		DEBUG_LOG("\n(uint8_t *)(ptr) - ALIGNPAD = ");
		print_ptr((uint8_t *)(ptr) - ALIGNPAD);
		DEBUG_LOG("\n(struct blockinfo *)((uint8_t *)(ptr) - ALIGNPAD) - 1 = ");
		print_ptr((struct blockinfo *)((uint8_t *)(ptr) - ALIGNPAD) - 1);
		DEBUG_LOG("\n");
		BREAKPOINT();
	} else {
		DEBUG_LOG("blockinfo accessed properly after alignment\n");
	}
#endif
	return (uint8_t *)(block + 1) + ALIGNPAD;
#else
	return block + 1;
#endif
}

static void
my_free(void *ptr)
{
	if (!ptr) return;
	BLOCKINFO(ptr)->free = 1;
}

static void *
my_calloc(size_t nmemb, size_t size)
{
	size_t total;
	uint8_t *p;
	if (__builtin_mul_overflow(nmemb, size, &total)) return NULL;
	if (!(p = my_malloc(total))) return NULL;
	return memset(p, 0, total);
}

static void *
my_realloc(void *ptr, size_t size)
{
	size_t oldsz;
	void *newp;
	if (!size) return NULL;
	if (!ptr) return my_malloc(size);
	if ((oldsz = BLOCKINFO(ptr)->size) >= size) return ptr;
	if (!(newp = my_malloc(size))) return NULL;
	memcpy(newp, ptr, oldsz);
	my_free(ptr);
	return newp;
}

/*
 * ===========================================================================
 * quick example
 */
#define PRINT(s) syscall(SYS_write, 1, s, sizeof(s) - 1)
#define STRINGIZE_HELPER(n) #n
#define STRINGIZE(n) STRINGIZE_HELPER(n)

static void
print_ptr(void *ptr)
{
	if (!ptr) PRINT("NULL");
	else {
		/* assume that pointers are 64 bits wide or less */
		char buf[18] __attribute__((__nonstring__));

		uintptr_t p = (uintptr_t)ptr;
		size_t i = 0, j;
		buf[i++] = '0';
		buf[i++] = 'x';
		for (j = 2 * sizeof(p) - 1; j >= 0; --j) {
			if (i < sizeof(buf)) buf[i++] = "0123456789abcdef"[(p >> (4 * j)) & 0xF];
			else break;
		}
		syscall(SYS_write, 1, buf, i);
	}
}

static void
print_size(size_t n)
{
	/* assume that size_t is 64 bits wide or less */
	char buf[20] __attribute__((__nonstring__));
	size_t i = 19;
	do {
		if (i >= 0) buf[i--] = '0' + (n % 10);
	} while (n /= 10);
	if (i < 19) syscall(SYS_write, 1, buf + i + 1, sizeof(buf) - i - 1);
}

int
main(int argc, char *argv[])
{
	char *p, *p2;
	PRINT("calling malloc(16), aligning memory to " STRINGIZE(ALIGN) " bytes\n");
	p = my_malloc(16);
	PRINT("malloc returned ");
	print_ptr(p);
	PRINT("\ncopying string 'abcdefghijklmno' into returned buffer\n");
	memcpy(p, "abcdefghijklmno", 16);
	PRINT("contents of buffer: ");
	syscall(SYS_write, 1, p, 15);
	PRINT("\ncalling free\n\n");
	my_free(p);
	
	PRINT("calling malloc(4)\n");
	p = my_malloc(4);
	PRINT("malloc returned ");
	print_ptr(p);
	PRINT("\ncalling free\n\n");
	my_free(p);
	
	PRINT("calling malloc(32)\n");
	p = my_malloc(32);
	PRINT("malloc returned ");
	print_ptr(p);
	PRINT("\ncalling free\n\n");
	my_free(p);


	PRINT("calling calloc(8, 4)\n");
	p = my_calloc(8, 4);
	PRINT("calloc returned ");
	print_ptr(p);
	PRINT("\ncalling free\n\n");
	my_free(p);

	PRINT("calling malloc(512)\n");
	p = p2 = my_malloc(512);
	PRINT("malloc returned ");
	print_ptr(p);
	PRINT("\ncalling realloc(p, 1024)\n");
	if ((p = my_realloc(p, 1024)) == NULL) my_free(p2);
	PRINT("realloc returned ");
	print_ptr(p);
	PRINT("\ncalling free\n\n");
	my_free(p);
	return 0;
}
