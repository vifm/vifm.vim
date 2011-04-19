#ifndef __KEYS_ENG_H__
#define __KEYS_ENG_H__

enum
{
	NO_COUNT_GIVEN = -1,
	NO_REG_GIVEN = -1,
};

enum
{
	MF_USES_REGS = 1,
	MF_USES_COUNT = 2,
	MF_USES_INPUT = 4,
};

enum
{
	KEYS_UNKNOWN = -1024,
	KEYS_WAIT = -2048,
	KEYS_WAIT_SHORT = -4096,
};

#define IS_KEYS_RET_CODE(c) \
		((c) == KEYS_UNKNOWN || (c) == KEYS_WAIT || (c) == KEYS_WAIT_SHORT)

enum FOLLOWED_BY
{
	FOLLOWED_BY_NONE,
	FOLLOWED_BY_SELECTOR,
	FOLLOWED_BY_MULTIKEY,
};

enum KEYS_TYPE
{
	BUILDIN_WAIT_POINT, /* infinite wait of next key press */
	BUILDIN_KEYS,
	BUILDIN_CMD,
	USER_CMD,
};

enum KEYS_SELECTOR
{
	KS_NOT_A_SELECTOR,
	KS_SELECTOR_AND_CMD,
	KS_ONLY_SELECTOR,
};

struct key_info
{
	int count; /* repeat count, maybe equal NO_COUNT_GIVEN */
	int reg;   /* number of selected register */
	int multi; /* multi key */
};

struct keys_info
{
	int selector; /* selector passed */
	int count;    /* count of selected items */
	int *indexes; /* item indexes */
};

typedef void (*keys_handler)(struct key_info key_info,
		struct keys_info *keys_info);

struct key_t
{
	enum KEYS_TYPE type;
	enum FOLLOWED_BY followed;   /* what type of key should we wait for */
	enum KEYS_SELECTOR selector; /* can this key be used as a selector? */
	union
	{
		keys_handler handler;
		const char *cmd;
	}data;
};

typedef int (*default_handler)(const char* keys);

/*
 * mode and mode_flags can't be NULL
 * mode_flags is a sequence of ints (combinations of MF_*)
 * handler can be NULL
 */
void init_keys(int modes_count, int* key_mode, int* key_mode_flags,
		default_handler handler);
int execute_keys(const char *keys);
struct key_t* add_keys(const char *keys, int mode);
int add_user_keys(const char *keys, const char *cmd, int mode);

#endif

/* vim: set tabstop=2 softtabstop=2 shiftwidth=2 noexpandtab : */
