/*
 * irt_common.c
 *
 *  Created on: Feb 18, 2014
 *      Author: jasondel
 *
 */
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "irt_common.h"
#include "nrf_error.h"

static irt_power_meas_t* 	mp_buf_power_meas;
static uint8_t				m_buf_size;
static uint8_t				m_fifo_index;

uint32_t irt_power_meas_fifo_init(uint8_t size)
{
	m_buf_size = size;

	// Allocate buffer on the heap for events.
	mp_buf_power_meas = (irt_power_meas_t*)calloc(size, sizeof(irt_power_meas_t));

	if (mp_buf_power_meas == 0)
		return NRF_ERROR_NO_MEM;

	return IRT_SUCCESS;
}

void irt_power_meas_fifo_free()
{
	// Free up the heap.
	free(mp_buf_power_meas);
}

/*
 *  Implements a FIFO queue of power events.  This function abstracts interaction
 *  with the queue.  first returns a pointer to first event put in the buffer
 *  next returns a pointer to use for the next write.
 *
 *  The FIFO is used to maintain the index in a buffer holding the events.
 */
uint32_t irt_power_meas_fifo_op(irt_power_meas_t** first, irt_power_meas_t** next)
{
	int8_t idx_write;

	// Determine index to write.
	idx_write = m_fifo_index % m_buf_size;

	// Determine index to read.
	if (m_fifo_index >= m_buf_size)
	{
		int8_t idx_read = ((m_fifo_index - m_buf_size+1) % m_buf_size);

		// Return the pointer to the oldest event in the stack.
		*first = &mp_buf_power_meas[idx_read];
	}

	// Clear the bytes for the next write.
	memset(&mp_buf_power_meas[idx_write], 0, sizeof(irt_power_meas_t));

	// Set pointer of the current event to write.
	*next = &mp_buf_power_meas[idx_write];

	// Increment the index.
	m_fifo_index++;

	return IRT_SUCCESS;
}

