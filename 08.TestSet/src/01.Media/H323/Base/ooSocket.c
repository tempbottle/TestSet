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

#include "ooSocket.h"

#if defined(_WIN32_WCE)
static int inited = 0;
#define SEND_FLAGS     0
#define SHUTDOWN_FLAGS 0
#elif defined (_WIN32)/*
static LPFN_SEND send;
static LPFN_SOCKET socket;
static LPFN_SETSOCKOPT setsockopt;
static LPFN_BIND bind;
static LPFN_HTONL htonl;
static LPFN_HTONS htons;
static LPFN_CONNECT connect;
static LPFN_INET_ADDR inet_addr;
static LPFN_LISTEN listen;
static LPFN_ACCEPT accept;
static LPFN_NTOHL ntohl;
static LPFN_NTOHS ntohs;
static LPFN_RECV recv;
static LPFN_SHUTDOWN shutdown;


static LPFN_SENDTO sendto;
static LPFN_INET_NTOA inet_ntoa;
static LPFN_RECVFROM recvfrom;
static LPFN_SELECT select;
static LPFN_GETHOSTNAME gethostname;
static LPFN_GETHOSTBYNAME gethostbyname;
static LPFN_WSAGETLASTERROR  WSAGetLastError;
static LPFN_WSACLEANUP WSACleanup;
static LPFN_CLOSESOCKET closesocket;*/
static HMODULE ws32 = 0;
#define SEND_FLAGS     0
#define SHUTDOWN_FLAGS SD_BOTH
#else
#define SEND_FLAGS     0
#define SHUTDOWN_FLAGS SHUT_RDWR
#define closesocket close
#endif

int ooSocketsInit ()
{
#if defined(_WIN32_WCE)
   WORD wVersionRequested;
   WSADATA wsaData;
   int err;

   if (inited) return ASN_OK; 

   wVersionRequested = MAKEWORD( 1, 1 );
    
   err = WSAStartup (wVersionRequested, &wsaData);
   if ( err != 0 ) {
      /* Tell the user that we could not find a usable */
      /* WinSock DLL.   */
      return ASN_E_NOTINIT;
   }
   inited = 1;

///////////////////////////#elif defined (_WIN32)
   LPFN_WSASTARTUP wsaStartup = NULL;
   WSADATA wsaData;

   if (ws32 != 0) return ASN_OK;

//   ws32 = LoadLibrary ("WSOCK32.DLL");
  ws32 = LoadLibrary ("WS2_32.DLL");
   if (ws32 == NULL) return ASN_E_NOTINIT;
   
   wsaStartup = (LPFN_WSASTARTUP) GetProcAddress (ws32, "WSAStartup");
   if (wsaStartup == NULL) return ASN_E_NOTINIT;
   
   send = (LPFN_SEND) GetProcAddress (ws32, "send");
   if (send == NULL) return ASN_E_NOTINIT;
   
   socket = (LPFN_SOCKET) GetProcAddress (ws32, "socket");
   if (socket == NULL) return ASN_E_NOTINIT;
   
   setsockopt = (LPFN_SETSOCKOPT) GetProcAddress (ws32, "setsockopt");
   if (setsockopt == NULL) return ASN_E_NOTINIT;
   
   bind = (LPFN_BIND) GetProcAddress (ws32, "bind");
   if (bind == NULL) return ASN_E_NOTINIT;
   
   htonl = (LPFN_HTONL) GetProcAddress (ws32, "htonl");
   if (htonl == NULL) return ASN_E_NOTINIT;
   
   htons = (LPFN_HTONS) GetProcAddress (ws32, "htons");
   if (htons == NULL) return ASN_E_NOTINIT;
   
   connect = (LPFN_CONNECT) GetProcAddress (ws32, "connect");
   if (connect == NULL) return ASN_E_NOTINIT;
   
   listen = (LPFN_LISTEN) GetProcAddress (ws32, "listen");
   if (listen == NULL) return ASN_E_NOTINIT;
   
   accept = (LPFN_ACCEPT) GetProcAddress (ws32, "accept");
   if (accept == NULL) return ASN_E_NOTINIT;
   
   inet_addr = (LPFN_INET_ADDR) GetProcAddress (ws32, "inet_addr");
   if (inet_addr == NULL) return ASN_E_NOTINIT;
   
   ntohl = (LPFN_NTOHL) GetProcAddress (ws32, "ntohl");
   if (ntohl == NULL) return ASN_E_NOTINIT;
   
   ntohs = (LPFN_NTOHS) GetProcAddress (ws32, "ntohs");
   if (ntohs == NULL) return ASN_E_NOTINIT;
   
   recv = (LPFN_RECV) GetProcAddress (ws32, "recv");
   if (recv == NULL) return ASN_E_NOTINIT;
   
   shutdown = (LPFN_SHUTDOWN) GetProcAddress (ws32, "shutdown");
   if (shutdown == NULL) return ASN_E_NOTINIT;
   
   closesocket = (LPFN_CLOSESOCKET) GetProcAddress (ws32, "closesocket");
   if (closesocket == NULL) return ASN_E_NOTINIT;
   
   sendto = (LPFN_SENDTO) GetProcAddress (ws32, "sendto");
   if (sendto == NULL) return ASN_E_NOTINIT;

   inet_ntoa = (LPFN_INET_NTOA) GetProcAddress (ws32, "inet_ntoa");
   if (inet_ntoa == NULL) return ASN_E_NOTINIT;

   recvfrom = (LPFN_RECVFROM) GetProcAddress (ws32, "recvfrom");
   if (recvfrom == NULL) return ASN_E_NOTINIT;

   select = (LPFN_SELECT) GetProcAddress (ws32, "select");
   if (select == NULL) return ASN_E_NOTINIT;

   gethostname = (LPFN_GETHOSTNAME) GetProcAddress (ws32, "gethostname");
   if (gethostname == NULL) return ASN_E_NOTINIT;

   gethostbyname = (LPFN_GETHOSTBYNAME) GetProcAddress (ws32, "gethostbyname");
   if (gethostbyname == NULL) return ASN_E_NOTINIT;
   
   WSAGetLastError = (LPFN_WSAGETLASTERROR) GetProcAddress (ws32, 
                                                           "WSAGetLastError");
   if (WSAGetLastError == NULL) return ASN_E_NOTINIT;

   WSACleanup = (LPFN_WSACLEANUP) GetProcAddress (ws32, "WSACleanup");
   if (WSACleanup == NULL) return ASN_E_NOTINIT;
   
      
   if (wsaStartup (MAKEWORD(1, 1), &wsaData) == -1) return ASN_E_NOTINIT;
 #endif
   return ASN_OK;
}

#if defined (_WIN32) || \
defined(_HP_UX) || defined(__hpux) || defined(_HPUX_SOURCE)
typedef int OOSOCKLEN;
#else
typedef size_t OOSOCKLEN;
#endif

int ooSocketCreate (OOSOCKET* psocket) 
{
   int on;
   struct linger linger;
   OOSOCKET sock = socket (AF_INET,
                             SOCK_STREAM,
                             0);
  
   if (sock == OOSOCKET_INVALID)
   {
		int iEC = WSAGetLastError();
	   return ASN_E_INVSOCKET;
   }

   on = 1;
   if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, 
                   (const char* ) &on, sizeof (on)) == -1)
      return ASN_E_INVSOCKET;
   linger.l_onoff = 1;
   linger.l_linger = 0;
   if (setsockopt (sock, SOL_SOCKET, SO_LINGER, 
                   (const char* ) &linger, sizeof (linger)) == -1)
      return ASN_E_INVSOCKET;
   *psocket = sock;
   return ASN_OK;
}

int ooSocketCreateUDP (OOSOCKET* psocket) 
{
   int on;
   struct linger linger;

   OOSOCKET sock = socket (AF_INET,
                             SOCK_DGRAM,
                             0);

   if (sock == OOSOCKET_INVALID) return ASN_E_INVSOCKET;

   on = 1;
   if (setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, 
                   (const char* ) &on, sizeof (on)) == -1)
      return ASN_E_INVSOCKET;
   linger.l_onoff = 1;
   linger.l_linger = 0;
   /*if (setsockopt (sock, SOL_SOCKET, SO_LINGER, 
                 (const char* ) &linger, sizeof (linger)) == -1)
      return ASN_E_INVSOCKET;
   */
   *psocket = sock;
   return ASN_OK;
}

int ooSocketClose (OOSOCKET socket)
{
   shutdown (socket, SHUTDOWN_FLAGS);
   if (closesocket (socket) == -1)
      return ASN_E_INVSOCKET;
   return ASN_OK;
}

int ooSocketBind (OOSOCKET socket, OOIPADDR addr, int port) 
{
   struct sockaddr_in m_addr;

   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;

   memset (&m_addr, 0, sizeof (m_addr));
   m_addr.sin_family = AF_INET;
   m_addr.sin_addr.s_addr = (addr == 0) ? INADDR_ANY : htonl (addr);
   m_addr.sin_port = htons ((unsigned short)port);

   if (bind (socket, (struct sockaddr *) (void*) &m_addr,
                     sizeof (m_addr)) == -1)
      return ASN_E_INVSOCKET;

   return ASN_OK;
}

int ooSocketListen (OOSOCKET socket, int maxConnection) 
{
   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;

   if (listen (socket, maxConnection) == -1)
      return ASN_E_INVSOCKET;

   return ASN_OK;
}

int ooSocketAccept (OOSOCKET socket, OOSOCKET *pNewSocket, 
                    OOIPADDR* destAddr, int* destPort) 
{
   struct sockaddr_in m_addr;
   OOSOCKLEN addr_length = sizeof (m_addr);

   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;
   if (pNewSocket == 0) return ASN_E_INVPARAM;

   *pNewSocket = accept (socket, (struct sockaddr *) (void*) &m_addr, 
                         &addr_length);
   if (*pNewSocket <= 0) return ASN_E_INVSOCKET;

   if (destAddr != 0) 
      *destAddr = ntohl (m_addr.sin_addr.s_addr);
   if (destPort != 0)
      *destPort = ntohs (m_addr.sin_port);

   return ASN_OK;
}

int ooSocketConnect (OOSOCKET socket, const char* host, int port) 
{
	struct sockaddr_in m_addr;

	if (socket == OOSOCKET_INVALID)
	{
		   return ASN_E_INVSOCKET;
	}

	memset (&m_addr, 0, sizeof (m_addr));

	m_addr.sin_family = AF_INET;
	m_addr.sin_port = htons ((unsigned short)port);
	m_addr.sin_addr.s_addr = inet_addr (host);

	if (connect (socket, (struct sockaddr *) (void*) &m_addr, 
				sizeof (m_addr)) == -1)
	{
		int nEC = WSAGetLastError();
		return ASN_E_INVSOCKET;
	}
	return ASN_OK;
}
/*
// **Need to add check whether complete data was sent by checking the return
// **value of send and if complete data is not sent then add mechanism to 
// **send remaining bytes. This will make ooSocketSend call atomic.
*/
int ooSocketSend (OOSOCKET socket, const ASN1OCTET* pdata, ASN1UINT size)
{
   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;
   
   if (send (socket, (const char*) pdata, size, SEND_FLAGS) == -1)
      return ASN_E_INVSOCKET;
   return ASN_OK;
}

int ooSocketSendTo(OOSOCKET socket, const ASN1OCTET* pdata, ASN1UINT size,
                     const char* host, int port)
{
   struct sockaddr_in m_addr;
   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;
   
   memset (&m_addr, 0, sizeof (m_addr));

   m_addr.sin_family = AF_INET;
   m_addr.sin_port = htons ((unsigned short)port);
   m_addr.sin_addr.s_addr = inet_addr (host);
   if (sendto (socket, (const char*) pdata, size, SEND_FLAGS, 
                                    (const struct sockaddr*)&m_addr, 
                                    sizeof(m_addr)) == -1)
      return ASN_E_INVSOCKET;
   return ASN_OK;
}

int ooSocketRecv (OOSOCKET socket, ASN1OCTET* pbuf, ASN1UINT bufsize)
{
   int len;
   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;

   if ((len = recv (socket, (char*) pbuf, bufsize, 0)) == -1)
      return ASN_E_INVSOCKET;
   return len;
}

int ooSocketRecvFrom (OOSOCKET socket, ASN1OCTET* pbuf, ASN1UINT bufsize,
                        char * remotehost, int * remoteport)
{
   struct sockaddr_in m_addr;
   int len, addrlen;

   if (socket == OOSOCKET_INVALID) return ASN_E_INVSOCKET;
   addrlen = sizeof(struct sockaddr_in);
   
   memset (&m_addr, 0, sizeof (m_addr));
      
   if ((len = recvfrom (socket, (char*) pbuf, bufsize, 0, 
                        (struct sockaddr*)&m_addr, &addrlen)) == -1)
   {
		int nWSAECode = WSAGetLastError();
	   return ASN_E_INVSOCKET;
	  }

   if(remoteport)
      *remoteport = ntohs(m_addr.sin_port);
   if(remotehost)
      strcpy(remotehost, inet_ntoa(m_addr.sin_addr));
   return len;
}

int ooSocketSelect(int nfds, fd_set *readfds, fd_set *writefds, 
                     fd_set *exceptfds, struct timeval * timeout)
{
   int ret;   
#if defined (_WIN32)
  ret = select(nfds, readfds, writefds, exceptfds, 
             (const struct timeval *) timeout);
#else
   ret = select(nfds, readfds, writefds, exceptfds, timeout);
#endif
   if(ret == SOCKET_ERROR)
   {
	   int iEC = WSAGetLastError();
   }
   return ret;
}

int ooGetLocalIPAddress(char * pIPAddrs)
{
   int ret;
   struct hostent *phost;
   struct in_addr addr;
   char hostname[20];

   if(pIPAddrs == NULL)
      return -1; /* Need to find suitable return value */
   ret = gethostname(hostname, 20);
   if(ret == 0)
   {
      phost = gethostbyname(hostname);
      if(phost == NULL)
         return -1; /* Need to define a return value if made part of rtsrc */
      memcpy(&addr, phost->h_addr_list[0], sizeof(struct in_addr));
      strcpy(pIPAddrs, inet_ntoa(addr));
   }
   return ASN_OK;
}

int ooSocketStrToAddr (const char* pIPAddrStr, OOIPADDR* pIPAddr) 
{
   int b1, b2, b3, b4;
   int rv = sscanf (pIPAddrStr, "%d.%d.%d.%d", &b1, &b2, &b3, &b4);
   if (rv != 4 ||
      (b1 < 0 || b1 > 256) || (b2 < 0 || b2 > 256) ||
      (b3 < 0 || b3 > 256) || (b4 < 0 || b4 > 256))
      return ASN_E_INVPARAM;
   *pIPAddr = ((b1 & 0xFF) << 24) | ((b2 & 0xFF) << 16) | 
              ((b3 & 0xFF) << 8) | (b4 & 0xFF);
   return ASN_OK;
}

int ooSocketAddrToStr (OOIPADDR ipAddr, char* pbuf, int bufsize)
{
   char buf1[5], buf2[5], buf3[5], buf4[5];
   int cnt = 0;

   if (bufsize < 8) 
      return ASN_E_BUFOVFLW;

   cnt += sprintf (buf1, "%d", (ipAddr >> 24) & 0xFF);
   cnt += sprintf (buf2, "%d", (ipAddr >> 16) & 0xFF);
   cnt += sprintf (buf3, "%d", (ipAddr >> 8) & 0xFF);
   cnt += sprintf (buf4, "%d", ipAddr & 0xFF);
   if (bufsize < cnt + 4)
      return ASN_E_BUFOVFLW;
   sprintf (pbuf, "%s.%s.%s.%s", buf1, buf2, buf3, buf4);
   return ASN_OK;
}

int ooSocketsCleanup (void)
{
#ifdef _WIN32
   int ret = WSACleanup();
   if(ret == 0)
      return ASN_OK;
   else
      return ret;
#endif
   return ASN_OK;
}

long ooHTONL(long val)
{
   return htonl(val);
}

short ooHTONS(short val)
{
   return htons(val);
}
