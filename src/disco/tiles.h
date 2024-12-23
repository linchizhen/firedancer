#ifndef HEADER_fd_src_app_fdctl_run_tiles_h
#define HEADER_fd_src_app_fdctl_run_tiles_h

#include "stem/fd_stem.h"
#include "shred/fd_shredder.h"
#include "../ballet/shred/fd_shred.h"
#include "../ballet/pack/fd_pack.h"
#include "topo/fd_topo.h"

#include <linux/filter.h>

/* fd_shred34 is a collection of up to 34 shreds batched in a way that's
   convenient for use in a dcache and for access from Rust. The limit of
   34 comes so that sizeof( fd_shred34_t ) < USHORT_MAX. */

struct __attribute__((aligned(FD_CHUNK_ALIGN))) fd_shred34 {
  ulong shred_cnt;

  /* est_txn_cnt: An estimate of the number of transactions contained in this
     shred34_t.  The true value might not be a whole number, but this is
     helpful for diagnostic purposes. */
  ulong est_txn_cnt;
  ulong stride;
  ulong offset;
  ulong shred_sz; /* The size of each shred */
  /* For i in [0, shred_cnt), shred i's payload spans bytes
     [i*stride+offset, i*stride+offset+shred_sz ), counting from the
     start of the struct, not this point. */
  union {
    fd_shred_t shred;
    uchar      buffer[ FD_SHRED_MAX_SZ ];
  } pkts[ 34 ];
};
typedef struct fd_shred34 fd_shred34_t;

struct fd_became_leader {
  /* Start and end time of the slot in nanoseconds (from
     fd_log_wallclock()). */
  long   slot_start_ns;
  long   slot_end_ns;

  /* An opaque pointer to a Rust Arc<Bank> object, which should only
     be used with fd_ext_* functions to execute transactions or drop
     the bank.  The ownership is complicated, but basically any bank
     tile that receives this frag has a strong refcnt to the bank and
     should release it when done, other tiles should ignore and never
     use the bank. */
  void const * bank;

  /* The maximum number of microblocks that pack is allowed to put
     into the block. This allows PoH to accurately track and make sure
     microblocks do not need to be dropped. */
  ulong max_microblocks_in_slot;

  /* The number of ticks (effectively empty microblocks) that the PoH
     tile will put in the block.  This is used to adjust some pack
     limits. */
  ulong ticks_per_slot;

  /* The number of ticks that the PoH tile has skipped, but needs to
     publish to show peers they were skipped correctly.  This is used
     to adjust some pack limits. */
  ulong total_skipped_ticks;
};
typedef struct fd_became_leader fd_became_leader_t;

struct fd_rooted_bank {
  void * bank;
  ulong  slot;
};

typedef struct fd_rooted_bank fd_rooted_bank_t;

struct fd_completed_bank {
   ulong slot;
   uchar hash[32];
};

typedef struct fd_completed_bank fd_completed_bank_t;

struct fd_microblock_trailer {
  /* The hash of the transactions in the microblock, ready to be
     mixed into PoH. */
  uchar hash[ 32UL ];
};
typedef struct fd_microblock_trailer fd_microblock_trailer_t;

struct fd_done_packing {
   ulong microblocks_in_slot;
};
typedef struct fd_done_packing fd_done_packing_t;

struct fd_microblock_bank_trailer {
  /* An opaque pointer to the bank to use when executing and committing
     transactions.  The lifetime of the bank is owned by the PoH tile,
     which guarantees it is valid while pack or bank tiles might be
     using it. */
  void const * bank;

  /* The sequentially increasing index of the microblock, across all
     banks.  This is used by PoH to ensure microblocks get committed
     in the same order they are executed. */
  ulong microblock_idx;
};
typedef struct fd_microblock_bank_trailer fd_microblock_bank_trailer_t;

typedef struct __attribute__((packed)) {
  ulong  tick_duration_ns;
  ulong  hashcnt_per_tick;
  ulong  ticks_per_slot;
  ulong  tick_height;
  uchar  last_entry_hash[32];
} fd_poh_init_msg_t;

#endif /* HEADER_fd_src_app_fdctl_run_tiles_h */
