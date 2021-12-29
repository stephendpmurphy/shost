//
// Created by noeel on 03-12-21.
//

#include <cstdio>
#include <system_error>
#include <cassert>
#include "Protocol.h"

void Protocol::read(arg_t* arg) {
    if( arg->len < 1 ) {
        throw std::invalid_argument("You must provide a buffer to execute a \"read\" transfer.");
    }
    assert(arg->clk > 0 && "Clock should be atleast 1Hz!");
    assert(arg->channel >= 0 && "Define a channel!");
    assert(reinterpret_cast<uint64_t>(&arg->buff) > 0 && "Read buffer should point to a valid address");
    assert(arg->xferType == XFER_READ && "transfer type should be read!");
    _read(arg);
}

void Protocol::write(arg_t* arg) {
    if( arg->len < 1 ) {
        throw std::invalid_argument("You must provide a buffer to execute a \"write\" transfer.");
    }
    assert(arg->clk > 0 && "Clock should be atleast 1Hz!");
    assert(arg->channel >= 0 && "Define a channel!");
    assert(arg->len > 0 && "Data size to write should be bigger then 1 byte!");
    assert(arg->xferType == XFER_WRITE && "transfer type should be write!");
    _write(arg);
}

void Protocol::read_write(arg_t* arg) {
// TODO
}

Protocol::Protocol(const char *name): name(name) {

}
