#pragma once

typedef unsigned short block_index_t;

#define CHUNK_WIDTH 32
#define CHUNK_HEIGHT 32
#define CHUNK_VOLUME (CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT)

typedef struct {
  unsigned int id_meta;
} Block;

typedef struct {
} _SubChunk;

typedef struct {
  _SubChunk* subchunks[1];
} BlockSlab;

typedef struct {
  Block blocks[CHUNK_VOLUME];
} Chunk;

#define SLAB_OFFSET SLAB_VOLUME

#define abs_offset_to_slab_index(o) ((o) / SLAB_VOLUME)
#define abs_offset_to_slab_offset(o) ((o) % SLAB_VOLUME)

#define slab_offset_to_subchunk_index(o) ((o) / SUBCHUNK_VOLUME)
#define slab_offset_to_subchunk_offset(o) ((o) % SUBCHUNK_VOLUME) 

 #define abs_offset_to_subchunk_index(o) (slab_offset_to_subchunk_index(abs_offset_to_slab_offset(o))
#define abs_offset_to_subchunk_offset(o) (slab_offset_to_subchunk_offset(abs_offset_to_slab_offset(o))

#define abs_x_to_subchunk_index(x) ((x) / SUBCHUNK_WIDTH)
#define abs_y_to_subchunk_index(y) (0) // y does not contribute to subchunk indices yet
#define abs_z_to_subchunk_index(z) (((z) / SUBCHUNK_WIDTH) * 2)

#define abs_x_to_slab_index(x) (0)
#define abs_y_to_slab_index(y) ((y) / SUBCHUNK_HEIGHT)
#define abs_z_to_slab_index(x) (0)

//#define abs_x_to_offset(x) ((x) % COL_WIDTH)
#define abs_x_to_offset(x) (abs_x_to_subchunk_index(x) * SUBCHUNK_VOLUME + (x % SUBCHUNK_WIDTH))

//#define abs_y_to_offset(y) ((y) * COL_WIDTH2)
#define abs_y_to_offset(y) (abs_y_to_slab_index(y) * SLAB_VOLUME + (y % SLAB_HEIGHT) * SUBCHUNK_WIDTH2)

//#define abs_z_to_offset(z) ((z % COL_WIDTH) * COL_WIDTH)
#define abs_z_to_offset(z) (abs_z_to_subchunk_index(z) * SUBCHUNK_VOLUME + (z % SUBCHUNK_WIDTH) * SUBCHUNK_WIDTH)
/*
static inline block_index_t slab_offset_to_x(block_index_t o) {
block_index_t subchunk_index = slab_offset_to_subchunk_index(o);
  return (subchunk_index % 2) * 8 + (o % SUBCHUNK_WIDTH);
}

static inline block_index_t slab_offset_to_y(block_index_t o) {
  block_index_t subchunk_offset = slab_offset_to_subchunk_offset(o);
  return subchunk_offset / SUBCHUNK_WIDTH2;
}

static inline block_index_t slab_offset_to_z(block_index_t o) {
  block_index_t subchunk_index = slab_offset_to_subchunk_index(o);
  return (subchunk_index / 2) * 8 + (o / SUBCHUNK_WIDTH) % SUBCHUNK_WIDTH;
}*/