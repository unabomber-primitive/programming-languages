/* heap-0.c */

#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#define heap_blocks 16
#define block_capacity 1024

struct bitmap {
  char map[heap_blocks / sizeof(char) / 8];
};

static const char bitmasks[8] = {0x80, 0x40, 0x20, 0x10, 0x8, 0x4, 0x2, 0x1};
/*
bool char_bit(char byte, size_t position) {
  return (byte >> (7 - position)) & 0x1;
}
char char_bit_set(char byte, size_t position) {
  return byte | (0x80 >> position);
}
char char_bit_clear(char byte, size_t position) {
  return byte & ~(0x80 >> position);
}
*/

bool char_bit(char byte, size_t position) {
  return (byte & bitmasks[position])? 1:0;
}
char char_bit_set(char byte, size_t position) {
  return byte | bitmasks[position];
}
char char_bit_clear(char byte, size_t position) {
  return byte & ~ bitmasks[position];
}

  void bitmap_mark(struct bitmap * bm, size_t i, bool value) {
    char* c = bm->map + i / 8;
    if (value)
      *c = char_bit_set(*c, i % 8);
    else
      *c = char_bit_clear(*c, i % 8);
  }

  bool bitmap_get(struct bitmap const* bm, size_t i) {
    const char   byte = bm->map[i / 8];
    const size_t position = i % 8;
    return char_bit(byte, position);
  }

  struct maybe_size {
    size_t value;
    bool   valid;
  };
  struct maybe_size some_size(size_t i) {
    return (struct maybe_size){.valid = true, .value = i};
  }
  struct maybe_size none_size() {

    return (struct maybe_size){.valid = false};
  }
  struct maybe_size array_char_find(const char* arr, size_t count,
                                    bool cond(char)) {
    for (size_t i = 0; i < count; i++) {
      if (cond(arr[i]))
        return some_size(i);
    }
    return none_size();
  }
  static bool not_minus_one(char c) { return c != -1; }

  struct maybe_size bitmap_find_zero(struct bitmap const* bm) {
    struct maybe_size sz = array_char_find(bm->map, heap_blocks, not_minus_one);
    if (!sz.valid)
      return none_size();

    const char byte = bm->map[sz.value];

    for (size_t i = 0; i < 8; i++) {
      if (!(byte & bitmasks[i]))
        return some_size(sz.value * 8 + i);
    }
    return none_size();
  }

  struct heap {
    struct block {
      char contents[block_capacity];
    } blocks[heap_blocks];
    struct bitmap occupied;
  } global_heap = {0};

  struct block_id {
    size_t       value;
    bool         valid;
    struct heap* heap;
  };

  struct block_id block_id_new(size_t value, struct heap * from) {
    return (struct block_id){.valid = true, .value = value, .heap = from};
  }
  struct block_id block_id_invalid() {
    return (struct block_id){.valid = false};
  }

  bool block_id_is_valid(struct block_id bid) {
    return bid.valid && bid.value < heap_blocks;
  }
  /* find block */
  /*

  struct block_id {
    size_t value;
    bool   valid;
    struct heap* heap;
  };
    */
  bool block_is_free(struct block_id bid) {
    return block_id_is_valid(bid) &&
           !bitmap_get(&bid.heap->occupied, bid.value);
  }

  /* allocate */
  /* find a free block, reserve it and return its id */
  struct block_id block_allocate(struct heap * heap) {
    struct maybe_size sz = bitmap_find_zero(&heap->occupied);
    if (!sz.valid)
      return block_id_invalid();
    bitmap_mark(&heap->occupied, sz.value, true);
    return block_id_new(sz.value, heap);
  }

  /* mark block as 'free' */
  void block_free(struct block_id b) {
    if (!b.valid)
      return;
    else
      bitmap_mark(&b.heap->occupied, b.value, false);
  }

  /* printer */
  const char* block_repr(struct block_id b) {
    static const char* const repr[] = {[false] = " .", [true] = " ="};
    if (b.valid)
      return repr[bitmap_get(&b.heap->occupied, b.value)];
    else
      return "x";
  }

  void block_debug_info(struct block_id b, FILE * f) {
    fprintf(f, "%s", block_repr(b));
  }

  void block_foreach_printer(struct heap const* h, size_t count,
                             void printer(struct block_id, FILE * f), FILE* f) {
    for (size_t c = 0; c < count; c++)
      printer(block_id_new(c, h), f);
  }

  void heap_debug_info(struct heap const* h, FILE* f) {
    block_foreach_printer(h, heap_blocks, block_debug_info, f);
    fprintf(f, "\n");
  }
  /*  -------- */

  void test_char_bit(char c) {
    c = char_bit_set(c, 3);
    c = char_bit_clear(c, 4);
    printf("%x\n", c);
  }
  int main() {

    heap_debug_info(&global_heap, stdout);
    block_allocate(&global_heap);
    struct block_id bid = block_allocate(&global_heap);
    block_allocate(&global_heap);

    block_free(bid);
    heap_debug_info(&global_heap, stdout);
    return 0;
  }
