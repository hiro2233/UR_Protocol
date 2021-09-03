#include "UR_RingBuffer.h"

#if defined(SHAL_CORE_APM2) \
    || defined(SHAL_CORE_MEGA02) \
    || defined(SHAL_CORE_APM328) \
    || defined(SHAL_CORE_APM32U4)

#include <AP_HAL_URUS/AP_HAL_URUS.h>
extern const AP_HAL::HAL& hal;
#define DEBUG(s, ...) hal.console->printf_PS(s, ## __VA_ARGS__)
#else
#define PSTR(s) s
#define DEBUG(s, ...) printf(s, ## __VA_ARGS__)
#endif

#if TEST_RINGBUF == ENABLED
void test_ringbuffer()
{
    // buffer structure
    rbuf_t buffer;
    buf_item_t b_item;

    // init buffer
    ringbuf_init(&buffer);

    // pulling from an empty buffer will not work
    buf_item_t elem = ringbuf_get(&buffer);
    DEBUG(PSTR("buf %s\n"), RBUF_EMPTY ==  (int)elem.time ? "empty" : "not empty");

    // fill up the buffer
    for (int i = 0; i < RBUF_SIZE; ++i) {
        b_item.time = i + 1;
        ringbuf_put(&buffer, b_item);
    }

    // show buffer
    ringbuf_print(&buffer);
    buf_item_t item_tmp = ringbuf_peek(&buffer);
    DEBUG(PSTR("1st out: %u\n"), item_tmp.time);

    // buffer is full, trying to insert will not work
    b_item.time = 42;
    ringbuf_put(&buffer, b_item);

    // however if we delete some from the buffer
    ringbuf_get(&buffer);

    // we can insert again
    b_item.time = 42;
    ringbuf_put(&buffer, b_item);
    ringbuf_print(&buffer);

    // we can reset buffer counts or clear the contents completely
    ringbuf_flush(&buffer, RBUF_CLEAR);
}
#endif // TEST_RINGBUF

void ringbuf_init(rbuf_t* _this)
{
    // clear the _thisfer and init the values
    // and sets head = tail in one go
    memset( _this, 0, sizeof(*_this) );
}

bool ringbuf_empty(rbuf_t* _this)
{
    // test if the queue is empty
    // 0 returns true
    // nonzero false
    return (0 == _this->count);
}

bool ringbuf_full(rbuf_t* _this)
{
    // full when no of elements exceed the max size
    return (_this->count >= RBUF_SIZE);
}

buf_item_t ringbuf_get(rbuf_t* _this)
{
    buf_item_t item;
    if (_this->count > 0) {
        // get item element
        item        = _this->buf[_this->tail];
        // advance the tail
        _this->tail = ringbuf_adv(_this->tail, RBUF_SIZE);
        // reduce the total count
        --_this->count;
    } else {
        // the queue is empty
        item.time = (uint32_t)RBUF_EMPTY;
    }

    return item;
}

void ringbuf_put(rbuf_t* _this, const buf_item_t item)
{
    if (_this->count < RBUF_SIZE) {
        // set the item at head position
        _this->buf[_this->head] = item;
        // advance the head
        _this->head = ringbuf_adv(_this->head, RBUF_SIZE);
        // increase the total count
        ++_this->count;
    }
}

buf_item_t ringbuf_peek(rbuf_t* _this)
{
    buf_item_t e_item;

    e_item.time = 0;

    if (_this->count != 0) {
        return _this->buf[_this->tail];
    }
    return e_item;
}

void ringbuf_print(rbuf_t* _this)
{
    for (int i = 0; i < RBUF_SIZE; i++) {
        DEBUG(PSTR("%2d:%09u\n"), i, _this->buf[i].time);
    }

    DEBUG(PSTR("\n"));
}

void ringbuf_flush(rbuf_t* _this, rbuf_opt_e clear)
{
    _this->count = 0;
    _this->head  = 0;
    _this->tail  = 0;

    // optionally clear
    if (RBUF_CLEAR == clear) {
        memset(_this->buf, 0, sizeof(_this->buf));
    }
}

unsigned int ringbuf_adv (const unsigned int value, const unsigned int max_val)
{
    static unsigned int index;
    index = value + 1;
    if (index >= max_val) {
        index = 0;
    }
    return index;
}
