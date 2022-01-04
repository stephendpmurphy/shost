//
// Created by noeel on 03-12-21.
//

#ifndef MPSSE_CLI_PROTOCOL_H
#define MPSSE_CLI_PROTOCOL_H

#include "shost.h"

class Protocol{
public:
    const char* name;

    Protocol(const char* name);

    void read(shost_xfer_t* xfer);
    void write(shost_xfer_t* xfer);
    void read_write(shost_xfer_t* xfer);

protected:
    struct mpsse_context* mpsse;
private:
    void virtual _write(shost_xfer_t* xfer) = 0;
    void virtual _read(shost_xfer_t* xfer) = 0;
};


#endif //MPSSE_CLI_PROTOCOL_H
