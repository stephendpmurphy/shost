// #include <stdio.h>
// #include <stdint.h>
// #include <stdlib.h>
// #include <string.h>
// #include <fcntl.h>
// #include <stdio.h>
// #include <sys/stat.h>
// #include <sys/syscall.h>
// #include <sys/types.h>
// #include <unistd.h>
#include <argp.h>
#include <stdlib.h>
#include <string.h>
#include "xfer.h"
#include "util.h"

static int parse_opt (int key, char *arg, struct argp_state *state) {
    xfer_t *xfer_ptr = (xfer_t *)&state->input;

    switch (key) {
 		case 'l':
            util_printMPSSEchannelInfo( util_getMPSSEchannelCount() );
			break;
        case 'i':
            if((strcmp(arg, "SPI") == 0) || (strcmp(arg, "spi") == 0)) {
                xfer_ptr->intf = XFER_INTF_SPI;
            }
            else if((strcmp(arg, "I2C") == 0) || (strcmp(arg, "i2c") == 0)) {
                xfer_ptr->intf = XFER_INTF_I2C;
            }
            else {
                argp_failure(state, 0,0,"Invalid serial interface provided.");
            }
            break;
        case 'c':
            if(arg == NULL) {
                printf("No channel value given. Defaulting to channel 0.\n");
                xfer_ptr->channel = 0;
            }
            else {
                xfer_ptr->channel = atoi(arg);
            }
            break;
        case 'f':
            if( arg == NULL ) {
                printf("No frequency value given. Defaulting to 10kHz.\n");
                xfer_ptr->clk = 10000;
            }
            else {
                xfer_ptr->clk = atoi(arg);
            }
            break;
	}
	return 0;
}

int main(int argc, char *argv[] ) {
    struct argp_option cli_options[] = {
        {"list", 'l', 0, OPTION_ARG_OPTIONAL, "Display information about connected FTDI devices and available MPSSE channels"},
        {"interface", 'i', "SPI || I2C", 0, "Serial interface selection"},
        {"channel", 'c', "NUM", OPTION_ARG_OPTIONAL, "MPSSE Channel # - Available channels can be retrieved with the --list option"},
        {"frequency", 'f', "NUM", OPTION_ARG_OPTIONAL, "Serial communication freqeuncy"},
        {"xfer", 'x', "r || w || rw", OPTION_ARG_OPTIONAL, "Serial transfer type - Read, Write or Read & Write"},
        {"data", 'd', "0x00,0x01,0x02 ...", 0, "Comma delimted data to be written in hex."},
        {0}
    };

    struct argp argp = { cli_options, parse_opt };

	return argp_parse (&argp, argc, argv, 0, 0, 0);
}