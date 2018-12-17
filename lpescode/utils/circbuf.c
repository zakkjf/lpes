/*
 * circbuf.c
 *
 *  Created on: Dec 17, 2018
 *      Author: Adam Nuhaily
 */


/****************************************************************************
 * Copyright (C) 2017 by Adam Nuhaily                                       *
 ****************************************************************************/

/**
 * @file circbuf.c
 * @author Adam Nuhaily
 * @date 27 Oct 2017
 * @brief Source file containing circular buffer implementation for Project 2
 *
 * Source file containing circular buffer driver and utility functions for use
 * with project 2 KL25Z UART buffer. Buffer functions include safeguards and
 * status reporting using CB_status enum. See circbuf.h or README.md for
 * explanation of CB_status enum.
 *
 */

#include "circbuf.h"


/**
 * @brief If CB not full, adds a single byte, updates head pointer and count
 *
 * @param *buffer Previously-initialized circular buffer
 * @param add_data Data to add to buffer at current head
 * @return CB_status CB_SUCCESS if succeed, CB_FULL if fail (buffer full)
 *
 */
CB_status CB_buffer_add_item(CB_t *buffer, char add_data[])
{
  if(!buffer->data)  // if cb has not yet been allocated, return error
    return CB_NULL_ERROR;

  // check buffer full status, if full then return specific error
  if(CB_is_full(buffer) == CB_FULL) {// printf("CB is full, can't add.\n");
    return CB_FULL;
  }

  else {
    //*((buffer->data)+buffer->head) = add_data; // insert item at head

    strcpy( &(buffer->data[buffer->head * 16]), add_data);

    CB_advance_head(buffer);
    // update count (done here as dedicated sub function was not working on
    //  KL25Z for some reason)
    int8_t tmpcount = buffer->head - buffer->tail;
    if (tmpcount < 0)
    {
        tmpcount += buffer->size;
    }
    buffer->count=tmpcount;

    // Now do index updates
    //CB_advance_head(buffer);  // increment head first
  }

  return CB_SUCCESS;
}


/**
 * @brief If CB not empty, reads a single byte, updates tail pointer and count
 *
 * @param *buffer Previously-initialized circular buffer
 * @param *rm_data Pointer to memory location to store read byte
 * @return CB_status CB_SUCCESS if succeed, CB_EMPTY if fail (buffer empty)
 *
 */
CB_status CB_buffer_remove_item(CB_t *buffer, char *rm_data)
{
  if(!buffer->data)  // if cb has not yet been allocated, return error
    return CB_NULL_ERROR;

  // check buffer empty status, if empty then return specific error
  if(CB_is_empty(buffer) == CB_EMPTY)
    return CB_EMPTY;

  //*rm_data = *((buffer->data)+buffer->tail); // read item at tail and store
  strcpy(rm_data, &(buffer->data[buffer->tail * 16]) );

    // update count (done here as dedicated sub function was not working on
    //  KL25Z for some reason)
  int8_t tmpcount = buffer->head - buffer->tail;
  if (tmpcount < 0)
  {
    tmpcount += buffer->size;
  }
  buffer->count=tmpcount-1;

  CB_advance_tail(buffer);

  return CB_SUCCESS;
}


/**
 * @brief Return full/not full status of CB
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_FULL if full, CB_SUCCESS if buffer not full
 *
 */
CB_status CB_is_full(CB_t *buffer)
{
  int8_t tmpcount = buffer->head - buffer->tail;
  if (tmpcount < 0)
  {
      tmpcount += buffer->size;
  }

  if(!buffer->data)  // if cb has not yet been allocated, return error
  {
    return CB_NULL_ERROR;
  }

    // update count (done here as dedicated sub function was not working on
    //  KL25Z for some reason)
  if(tmpcount == buffer->size-1)
  {
      return CB_FULL;             // return specific status
  }

  else
  {
    return CB_SUCCESS;
  }

  return CB_NULL_ERROR;
}


/**
 * @brief Return empty/not empty status of CB
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_EMPTY if empty, CB_SUCCESS if buffer not empty
 *
 */
CB_status CB_is_empty(CB_t *buffer)
{
  if(!buffer->data)  // if cb has not yet been allocated, return error
    return CB_NULL_ERROR;

 // refresh count before proceeding
  if(buffer->head == buffer->tail)
  {
    return CB_EMPTY;        // return specific status
  }

  else
  {
    return CB_SUCCESS;
  }

}


/**
 * @brief "Peek behind" the write pointer by peek_pos elements
 *
 * @param *buffer Previously-initialized circular buffer
 * @param peek_pos Number of positions behind the head to peek
 * @param *peek_data Pointer at which to store peeked data
 * @return CB_status CB_EMPTY if empty, CB_SUCCESS if buffer not empty
 *
 */
CB_status CB_peek(CB_t *buffer, uint16_t peek_pos, char *peek_data)
{
  if(!buffer->data)  // if cb has not yet been allocated, return error
  {
    return CB_NULL_ERROR;
  }

  if (peek_pos > buffer->count) // if we're peeking too far back, return error
  {
    return CB_NULL_ERROR; // change this to a unique error later
  }

  // adjust peek_pos to be relative to first CB address for easier use
  // also adjust peek_pos to wrap around the buffer if necessary
  if((buffer->head)-peek_pos >= (buffer->size))
  {
    peek_pos = (buffer->head) - peek_pos + buffer->size;
  }
  else // if no wrap, adjust peek_pos to be relative to first index of data
  {
      peek_pos = (buffer->head)-peek_pos;
  }

  //*peek_data = *((buffer->data)+peek_pos);  // fetch data at desired position
  strcpy(peek_data, buffer->data[peek_pos]);

 return CB_SUCCESS;
}

/**
 * @brief Initialize a circular buffer object on the heap
 *
 * @param *buffer Circular buffer to initialize
 * @param buf_size Number of positions behind the head to peek
 * @return CB_status CB_SUCCESS if successful, CB_NULL_ERROR else
 *
 */
CB_status CB_init(CB_t *buffer, char* data, uint16_t buf_size)
{
  // allocate circular buffer data on the heap and initialize all cb fields
  //buffer->data = (unsigned char *)malloc(sizeof(CB_t)*buf_size*buf_count);

    buffer->data = data;
    buffer->count = 0;
    buffer->size = buf_size;
    buffer->head = 0;
    buffer->tail = 0;

    if(!(buffer->data))  // if data was not allocated, return error
    {
        return CB_NULL_ERROR;
    }
    else
    {
        return CB_SUCCESS;
    }

    return CB_NULL_ERROR;
}

/**
 * @brief Free an allocated circular buffer from memory
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_SUCCESS if successful, CB_NULL_ERROR else
 *
 */
CB_status CB_destroy(CB_t *buffer)
{

  free(buffer->data);  // use free() to unallocate the heap memory
  buffer->data = NULL; // clear the pointer to the CB
  buffer->count = 0;   // reset all the cb fields
  buffer->size = 0;
  buffer->head = 0;
  buffer->tail = 0;

  if(!(buffer->data))  // determine if pointer was cleared, return success if so
  {
    return CB_SUCCESS;
  }

  return CB_NULL_ERROR;
}


/**
 * @brief Advance head pointer including wrap around handling
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_SUCCESS if successful, CB_FULL if full
 *
 */
CB_status CB_advance_head(CB_t *buffer)
{
  if(CB_is_full(buffer) == CB_FULL) // if full, do not advance
  {
    return CB_FULL;
  }

  else
  {
    if(++(buffer->head) == buffer->size) // wrap-around if necessary
    {
      buffer->head = 0;
    }
  }
  return CB_SUCCESS;
}


/**
 * @brief Advance tail pointer including wrap around handling
 *
 * @param *buffer Previously-initialized circular buffer
 * @return CB_status CB_SUCCESS if successful, CB_EMPTY if empty
 *
 */
CB_status CB_advance_tail(CB_t *buffer)
{
  if(CB_is_empty(buffer) == CB_EMPTY) // if buffer empty, do not advance
  {
    return CB_EMPTY;
  }

  else
  {
    if(++(buffer->tail) == buffer->size) // advance tail and determine
    {                                    // wrap-around
      buffer->tail = 0;
    }
  }
  return CB_SUCCESS;
}


