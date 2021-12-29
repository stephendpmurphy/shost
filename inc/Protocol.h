//
// Created by noeel on 03-12-21.
//

#ifndef MPSSE_CLI_PROTOCOL_H
#define MPSSE_CLI_PROTOCOL_H

#include "cli.h"

class Protocol{
public:
    const char* name;

    Protocol(const char* name);

    void read(arg_t* arg);
    void write(arg_t* arg);
    void read_write(arg_t* arg);

protected:
    struct mpsse_context* mpsse;
private:
    void virtual _write(arg_t* arg) = 0;
    void virtual _read(arg_t* arg) = 0;
};


#endif //MPSSE_CLI_PROTOCOL_H
