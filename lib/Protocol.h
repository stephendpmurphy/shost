//
// Created by noeel on 03-12-21.
//

#ifndef MPSSE_CLI_PROTOCOL_H
#define MPSSE_CLI_PROTOCOL_H

#include "xfer.h"

class Protocol{
public:
    const char* name;

    Protocol(const char* name);

    void read(xfer_t* xfer);
    void write(xfer_t* xfer);
    void read_write(xfer_t* xfer);

protected:
    struct mpsse_context* mpsse;
private:
    void virtual _write(xfer_t* xfer) = 0;
    void virtual _read(xfer_t* xfer) = 0;
};


#endif //MPSSE_CLI_PROTOCOL_H
