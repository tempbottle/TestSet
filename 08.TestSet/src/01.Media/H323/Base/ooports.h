/*
 * Copyright (C) 2004 by Objective Systems, Inc.
 *
 * This software is furnished under an open source license and may be 
 * used and copied only in accordance with the terms of this license. 
 * The text of the license may generally be found in the root 
 * directory of this installation in the LICENSE.txt file.  It 
 * can also be viewed online at the following URL:
 *
 *   http://www.obj-sys.com/open/license.html
 *
 * Any redistributions of this file including modified versions must 
 * maintain this copyright notice.
 *
 *****************************************************************************/

/**
 * @file ooports.h 
 * This file contains functions to manage ports used by the stack. 
 */

#ifndef _OOPORTS_H_
#define _OOPORTS_H_

#include "ootypes.h"

#define OOTCP 1
#define OOUDP 2
#define OORTP 3


#ifdef __cplusplus
extern "C" {
#endif

#ifndef EXTERN
#ifdef _WIN32
#define EXTERN __declspec(dllexport)
#else
#define EXTERN
#endif /* _WIN32 */
#endif /* EXTERN */

/**
 * Sets the range of ports that can be potentially used for TCP 
 * connections.
 *
 * @param context   Pointer to application context.
 * @param start     Starting port number for the range.
 * @param max       Ending port number for the range
 *
 * @return          Completion status of operation: 0 (OO_OK) = success,
 *                  negative return value is error.
 */
EXTERN int ooSetTCPPorts(struct ooAppContext *context, int start, int max);

/**
 *  Sets the range of ports that can be potentially used for UDP
 *  transport
 *
 * @param context   Pointer to application context.
 * @param start     Starting port number for the range.
 * @param max       Ending port number for the range
 *
 * @return          Completion status of operation: 0 (OO_OK) = success,
 *                  negative return value is error.
 */

EXTERN int ooSetUDPPorts(struct ooAppContext *context, int start, int max);

/**
 * Sets the range of ports that can be potentially used for RTP
 * RTCP transport.
 *
 * @param context   Pointer to application context.
 * @param start     Starting port number for the range.
 * @param max       Ending port number for the range
 *
 * @return           Completion status of operation: 0 (OO_OK) = success,
 *                   negative return value is error.
 */

EXTERN int ooSetRTPPorts(struct ooAppContext *context, int start, int max);

/**
 * Get the next port of type TCP/UDP/RTP from the corresponding range.
 * When max value for the range is reached, it starts again from the 
 * first port number of the range.
 *
 * @param context    Application Context
 * @param type       Type of the port to be retrieved(OOTCP/OOUDP/OORTP).
 *
 * @return           The next port number for the specified type is returned.
 */
EXTERN int ooGetNextPort(struct ooAppContext *context, int type);

/**
 * Bind socket to a port within the port range specified by the
 * application at the startup.
 *
 * @param context   Pointer to application context.
 * @param type      Type of the port required for the socket.
 * @param socket    The socket to be bound.
 *
 * @return          In case of success returns the port number to which
 *                  socket is bound and in case of failure just returns
 *                  a negative value.
*/
EXTERN int ooBindPort(struct ooAppContext *context, int type, 
                        OOSOCKET socket);

#ifdef __cplusplus
}
#endif

#endif
