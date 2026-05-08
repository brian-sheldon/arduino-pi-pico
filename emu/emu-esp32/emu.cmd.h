
typedef void (*cmd_func_t)(void);

typedef struct {
  const char *name;
  cmd_func_t func;
  const char *params;
  const char *desc;
} cmd_entry_t;

