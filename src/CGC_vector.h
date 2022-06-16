#ifndef __CGC_VECTOR__
#define __CGC_VECTOR__

/**
 * File : CGC_vector.h
 * -------------------
 * 
 * This vector allows you to store any number of elements of any desired
 * type and it appropriate for a wide variety of storage problems. The vector
 * imposes no upper bound on the number of elements. It deals with its own
 * memory management. You need to specify (in bytes) the size of the elements
 * that will be stored in the vector when it is created, so that it can refer
 * to elements via (void *) pointers.
 */


/**
 * Type : CGC_vector_cmp_fn
 * ------------------------
 * This is a pointer to a client-supplied function which
 * the vector uses to sort or search for elements. The comparator
 * takes two (const void *) pointers and returns an int.
 * The comparator should indicate the order of the elements using
 * the same convention as the strcmp library function.
 * 
 *		If elem_addr1 is less than elem_addr2, return a negative number.
 * 		If elem_addr1 is greater then elem_addr2, return a positive number.
 * 		If the two elements are equal, return 0.
 */
typedef int (*CGC_vector_cmp_fn)(const void *elem_addr1, const void *elem_addr2);


/**
 * Type : CGC_vector_map_fn
 * ------------------------
 * This function can be used to map over the elements in the vector.
 * A map function is called with a pointer to the element and a client
 * data pointer (for any additional data that you might need) passed in from the original caller.
 */
typedef void (*CGC_vector_map_fn)(void *elem_addr, void *aux_data);


/**
 * Type : CGC_vector_free_fn
 * -------------------------
 * This function defines the space of functions that can be used
 * as the clean-up function for each element as it is deleted
 * from the vector or when the entire vector is destroyed.
 */
typedef (*CGC_vector_free_fn)(void *elem_addr);


/**
 * Type : CGC_vector 
 * -----------------
 * The concrete representation of the vector.
 * Even though everything is exposed, you should
 * respect the privacy of the representation and
 * initialise, dispose of, and interact with a
 * vector using the functions defined in this file.
 */
typedef struct {
	void *elems;
	int elem_size;
	unsigned long long log_size;
	unsigned long long alloc_size;
	CGC_vector_free_fn;
} CGC_vector;



/**
 * Function : CGC_vector_init
 * --------------------------
 * Constructs a raw or previously destryed vector to be
 * the empty vector.
 *
 * The elem_size parameter specifies the number of bytes that
 * a single element of the vector should take up. For example,
 * if you want to store elements of type char *, you would pass
 * sizeof(char*) as parameter.
 *
 *
 * The CGC_vector_free_fn is the function that will be called on
 * an element when it is deleted or removed. Also, it will be used
 * when the entire vector is being freed ( with CGC_vector_clear or
 * CGC_vector_dispose). This function allows you to do any cleanup
 * required for the element. Pass NULL if the elements don't require
 * any special handling.
 *
 * The initial_allocation parameter specifies the initial allocated
 * length of the vector, as well as the dynamic reallocation increment
 * for when the vector needs to grow. If you pass 0 for this parameter,
 * the vector will use the default value of 4.
 *
 * --------------------------------------------------------------------
 *
 * An assert is raised if:
 *					- elem_size <= 0
 *					- initial_allocation < 0
 */
void CGC_vector_init(CGC_vector *vector, int elem_size, 
					 CGC_vector_free_fn free_fn, int initial_allocation); 













