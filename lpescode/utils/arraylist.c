/*
 * arraylist.c
 *
 *  Created on: Dec 17, 2018
 *      Author: Adam Nuhaily
 */


#include "arraylist.h"


/*
 * This function initializes a new list. We assume that the array list has been
 * declared on the stack and is of size CONTACTS_MAX.
 */
int8_t ArrayList_Init(Contacts_t* al)
{
    uint8_t i = 0;

    for(i = 0; i < CONTACTS_MAX; i++)
    {
        al[i].phone_number = 0;
        al[i].msg_count = 0;
        al[i].last_batt = 0;
    }

    return 0;
}


/*
 * This function pushes an element to the end of the list.
 * index contains a pointer to the last element of the list, and is overwritten
 * with the new last element after a successful write.
 * Returns -1 if the list was full or al is unallocated.
 */
int8_t ArrayList_Push(Contacts_t* al, Contacts_t new, uint8_t* index)
{
    if(al == NULL || (*index + 1 > CONTACTS_MAX) )
    {
        return -1;
    }

    *index++;
    al[*index] = new;

    return 0;
}

/*
 * Search for an element in the array by phone number
 * Resulting index is stored in rtn
 * Return 0 if not found
 * Return -1 if al is not initialized or rtn is null
 * Return 1 if element found
 */
int8_t ArrayList_Search(Contacts_t* al, uint64_t num_search, uint8_t* rtn)
{
    uint8_t i = 0;

    if(al == NULL || rtn == NULL)
    {
        return -1;
    }

    for(i = 0; i < CONTACTS_MAX; i++)
    {
        // If element found, store in return container and return success
        if(al[i].phone_number == num_search)
        {
            *rtn = i;
            return 1;
        }
    }

    // Element not found, clear return container
    *rtn = 0;

    return 0;
}
