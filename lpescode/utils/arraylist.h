/*
 * arraylist.h
 *
 *  Created on: Dec 17, 2018
 *      Author: Adam Nuhaily
 */

#ifndef ARRAYLIST_H_
#define ARRAYLIST_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gps_decoder.h"    // for gps_raw_t

#define CONTACTS_MAX    255

typedef struct Contacts
{
    gps_raw_t gps_data;
    uint64_t phone_number;
    uint32_t msg_count;
    uint32_t last_batt;
} Contacts_t;


/*
 * This function initializes a new list. We assume that the array list has been
 * declared on the stack and is of size CONTACTS_MAX.
 */
int8_t ArrayList_Init(Contacts_t* al);

/*
 * This function pushes an element to the end of the list.
 * index contains a pointer to the last element of the list, and is overwritten
 * with the new last element after a successful write.
 * Returns -1 if the list was full or al is unallocated.
 */
int8_t ArrayList_Push(Contacts_t* al, Contacts_t new, uint8_t* index);

/*
 * Search for an element in the array by phone number
 * Resulting index is stored in rtn
 * Return 0 if not found
 * Return -1 if al is not initialized or rtn is null
 * Return 1 if element found
 */
int8_t ArrayList_Search(Contacts_t* al, uint64_t num_search, uint8_t* rtn);

#endif /* ARRAYLIST_H_ */
