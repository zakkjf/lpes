/*
 * @file circbuf.h
 * @author Adam Nuhaily
 * @date 27 Oct 2017
 * @brief Source file containing headers for circular buffer implementation
 *
 * Source file containing headers for circular buffer driver and utility
 * functions for use with project 2 KL25Z UART buffer. Buffer functions
 * include safeguards and status reporting using CB_status enum.
 *
 * CB_status enum:
 * CB_NULL_ERROR -- null pointer errors
 * CB_FULL -- CB is full
 * CB_EMPTY -- CB is empty
 * CB_SUCCESS -- operation success / CB status is nominal (initialized and
 *      not full or empty)
 *
 */

#ifndef UTILS_CIRCBUF_H_
#define UTILS_CIRCBUF_H_

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

// define CB struct
typedef struct {

  char **data;  // allocated memory for buffer elements (on heap)
                        // allocated dynamically
                        // unsigned char is guaranteed to be one byte


  uint16_t head;  // head ptr pts to head or newest item
                  // match item type (unsigned byte?)

  uint16_t tail;  // tail ptr pts to tail or oldest item
                  // match item type (unsigned byte?)

  uint16_t size;  // # items allocated to buffer, set when buffer allocated

  uint16_t count;     // count- number of items in buffer,
                      // tracked on every add/remove                       */


} CB_t;

// CB status enum for status reporting and tracking
typedef enum {
 CB_FULL, // buffer full
 CB_EMPTY, // buffer empty
 CB_SUCCESS, // success / no error
 CB_NULL_ERROR // null error
} CB_status;

/**
 * @brief If CB not full, adds a single byte, updates head pointer and count
 *
 * @param *buffer Previously-initialized circular buffer
 * @param add_data Data to add to buffer at current head
 * @return CB_status CB_SUCCESS if succeed, CB_FULL if fail (buffer full)
 *
 */
CB_status CB_buffer_add_item(CB_t *buffer, char add_data[]);

/**
 * @brief If CB not empty, reads a single byte, updates tail pointer and count
 *
 * @param *buffer Previously-initialized circular buffer
 * @param *rm_data Pointer to memory location to store read byte
 * @return CB_status CB_SUCCESS if succeed, CB_EMPTY if fail (buffer empty)
 *
 */
CB_status CB_buffer_remove_item(CB_t *buffer, char *rm_data);

/**
 * @brief Return full/not full status of CB
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_FULL if full, CB_SUCCESS if buffer not full
 *
 */
CB_status CB_is_full(CB_t *buffer);

/**
 * @brief Return empty/not empty status of CB
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_EMPTY if empty, CB_SUCCESS if buffer not empty
 *
 */
CB_status CB_is_empty(CB_t *buffer);

/**
 * @brief "Peek behind" the write pointer by peek_pos elements
 *
 * @param *buffer Previously-initialized circular buffer
 * @param peek_pos Number of positions behind the head to peek
 * @param *peek_data Pointer at which to store peeked data
 * @return CB_status CB_EMPTY if empty, CB_SUCCESS if buffer not empty
 *
 */
CB_status CB_peek(CB_t *buffer, uint16_t peek_pos, char *peek_data);

/**
 * @brief Initialize a circular buffer object on the heap
 *
 * @param *buffer Circular buffer to initialize
 * @param buf_size Number of positions behind the head to peek
 * @return CB_status CB_SUCCESS if successful, CB_NULL_ERROR else
 *
 */
CB_status CB_init(CB_t *buffer, char* data, uint16_t buf_size);

/**
 * @brief Free an allocated circular buffer from memory
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_SUCCESS if successful, CB_NULL_ERROR else
 *
 */
CB_status CB_destroy(CB_t *buffer);

/**
 * @brief Advance head pointer including wrap around handling
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_SUCCESS if successful, CB_FULL if full
 *
 */
CB_status CB_advance_head(CB_t *buffer);

/**
 * @brief Advance tail pointer including wrap around handling
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_SUCCESS if successful, CB_EMPTY if empty
 *
 */
CB_status CB_advance_tail(CB_t *buffer);

//CB_status CB_update_count(CB_t *buffer);

#endif /* UTILS_CIRCBUF_H_ */
