#ifndef __CFS_RDMA_BUFFER_H__
#define __CFS_RDMA_BUFFER_H__

#include <linux/list.h>
#include <linux/printk.h>
#include <linux/types.h>
#include <linux/mutex.h>
#include <linux/wait.h>
#include <rdma/ib_verbs.h>
#include <rdma/rdma_cm.h>
#include <rdma/ib_cm.h>

#define BUFFER_512B_SIZE 512
#define BUFFER_4K_SIZE 4096
#define BUFFER_128K_SIZE 128 * 1024
#define BUFFER_DEFAULT_NUM 4096

#define BUFFER_1M_SIZE 1024 * 1024
#define BUFFER_1M_NUM 256

#define BUFFER_LEVEL_NUM 4

#define DEFAULT_RDMA_PORT 17360

enum cfs_rdma_event_state {
	EVENT_STATE_INIT = 0,
	EVENT_STATE_ADDRESSRESOLVED = 1,
	EVENT_STATE_ROUTERESOLVED = 2,
	EVENT_STATE_OTHER = 3
};

struct cfs_node {
	char *pBuff;
	u64 dma_addr;
	bool used;
	size_t size;
    struct list_head list;
    struct list_head all_list;
    bool is_tmp;
    unsigned long recv_jiffies;
};

struct cfs_rdma_buffer {
    struct list_head lru;
    struct mutex lock;
    size_t size;
};

struct cfs_rdma_buffer_pool {
	struct rdma_cm_id *cm_id;
    wait_queue_head_t event_wait_queue;
	struct cfs_rdma_buffer buffer[BUFFER_LEVEL_NUM];
    struct list_head all_list;
    struct mutex all_lock;
    enum cfs_rdma_event_state state;
};

int cfs_rdma_buffer_new(void);
void cfs_rdma_buffer_release(void);
int cfs_rdma_buffer_get(struct cfs_node **item, size_t size);
void cfs_rdma_buffer_put(struct cfs_node *item);

#define ibv_print_error(fmt, ...) printk("%s:%d[%s] ERROR: "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#define ibv_print_info(fmt, ...) printk("%s:%d[%s] INFO: "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#ifdef DEBUG
#define ibv_print_debug(fmt, ...) printk("%s:%d[%s] DEBUG: "fmt, __FILE__, __LINE__, __FUNCTION__, ##__VA_ARGS__)
#else
#define ibv_print_debug(fmt, ...)
#endif

#endif