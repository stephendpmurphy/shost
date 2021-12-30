//
// Created by noeel on 03-12-21.
//

#include <cstdio>
#include <system_error>
#include <cassert>
#include "Protocol.h"

void Protocol::read(xfer_t* xfer) {
    if( xfer->len < 1 ) {
        throw std::invalid_argument("You must provide a buffer to execute a \"read\" transfer.");
    }
    assert(xfer->clk > 0 && "Clock should be atleast 1Hz!");
    assert(xfer->channel >= 0 && "Define a channel!");
    assert(reinterpret_cast<uint64_t>(&xfer->buff) > 0 && "Read buffer should point to a valid address");
    assert(xfer->xferType == XFER_READ && "transfer type should be read!");
    _read(xfer);
}

void Protocol::write(xfer_t* xfer) {
    if( xfer->len < 1 ) {
        throw std::invalid_argument("You must provide a buffer to execute a \"write\" transfer.");
    }
    assert(xfer->clk > 0 && "Clock should be atleast 1Hz!");
    assert(xfer->channel >= 0 && "Define a channel!");
    assert(xfer->len > 0 && "Data size to write should be bigger then 1 byte!");
    assert(xfer->xferType == XFER_WRITE && "transfer type should be write!");
    _write(xfer);
}

void Protocol::read_write(xfer_t* xfer) {
// TODO
}

Protocol::Protocol(const char *name): name(name) {

}
