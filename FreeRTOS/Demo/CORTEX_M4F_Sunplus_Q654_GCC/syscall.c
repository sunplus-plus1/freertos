/*
 * FreeRTOS V202104.00
 * Copyright (C) 2020 Amazon.com, Inc. or its affiliates.  All Rights Reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of
 * this software and associated documentation files (the "Software"), to deal in
 * the Software without restriction, including without limitation the rights to
 * use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of
 * the Software, and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR
 * COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER
 * IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * https://www.FreeRTOS.org
 * https://github.com/FreeRTOS
 *
 */
#ifdef __cplusplus
extern "C" {
#endif

#include <sys/types.h>
#include <platform.h>

extern unsigned long _heap_bottom;
extern unsigned long _heap_top;
extern unsigned long g_ulBase;

static void *heap_end = 0;

/**
 * @brief initializes the UART emulated hardware
 */
void uart_init()
{
    // TODO: uart initial
}

/**
 * @brief not used anywhere in the code
 * @todo  implement if necessary
 *
 */
int _fstat(int file)
{
    return 0;
}

/**
 * @brief not used anywhere in the code
 * @todo  implement if necessary
 *
 */
int _read(int file, char *buf, int len)
{
     return -1;
}

/**
 * @brief  Write bytes to the UART channel to be displayed on the command line
 *         with qemu
 * @param [in] file  ignored
 * @param [in] buf   buffer to send
 * @param [in] len   length of the buffer
 * @returns the number of bytes written
 */
int _write(int file, char *buf, int len)
{
    int todo;

    for (todo = 0; todo < len; todo++){
        UART_putc(*buf++);
    }
    return len;
}

/**
 * @brief function called by malloc and friends to reserve memory on the heap
 * @param [in] incr the amount of bytes to increase or decrease
 * @returns the previous top of the heap
 * @note uses a global variable <b>heap_end</b> to keep track of the previous top
 */
void* _sbrk(int incr)
{
    char *prev_heap_end;

    if (heap_end == 0)
    {
        heap_end = (void*) &_heap_bottom;
    }

    prev_heap_end = heap_end;

    if ((heap_end + incr) > (void*)&_heap_top)
    {
        return (void*)-1;
    }

    heap_end += incr;

    return prev_heap_end;
}

#ifdef __cplusplus
}
#endif
