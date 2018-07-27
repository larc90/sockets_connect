#ifndef SOCKET_CONN_H
#define SOCKET_CONN_H

#ifdef __WIN32__
#include <winsock2.h>
#include "wininet.h"
#else
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#endif

/********* Macros definition **********/
#define SERVER_CONF (0U) /**< Receive an image **/
#define CLIENT_CONF (1U) /**< Send an image **/

#define SERVER_ADDR "127.0.0.1" /**< Local host IP address (Only for testing) **/

#define QUEUE_CONNECTIONS (3) /**< Max queue of pending connections to be stored in server side **/

#define MAX_CHUNK_IMG  (1024)  /**< Maximum size of image chunks to be transfered **/
#define ID_STR_SIZE    (3)     /**< Size of identifiers **/
#define IMG_SIZE_BYTES (4)     /**< bytes dedicated to image size **/

#define DEFAULT_CLIENT_IMG_PATH "test_client.jpg" /**< Default path for image to be sent from client side **/
#define DEFAULT_SERVER_IMG_PATH "test_server.jpg" /**< Default path for image to be received in server side **/

#define NEW_IMG_AVAILABLE_ID  "NI" /**< Identifier for request of sending a image from client **/
#define START_IMG_TX_ID       "SI" /**< Identifier for starting image transference **/
#define END_IMG_TX_ID         "EI" /**< Identifier for ending image transference **/
#define INVALID_PETITION_ID   "IP" /**< Identifier for invalid petition of client **/
#define TERMINATE_COM_ID      "TC" /**< Identifier for finishing image transferece **/


/********* Global class definition **********/
class Connection_Utilities {
    struct sockaddr_in socket_server_addr;
    struct sockaddr_in socket_client_addr;
    s32 socket_server_addrlen;
    s32 socket_client_addrlen;
    s32 socket_local_desc;
    s32 socket_remote_desc;
  public:
#ifdef __WIN32__
    boolean Init_WinSock ();
    void End_Winsock ();
#endif
    boolean Create_Socket ();
    void Config_Server (u16, const s8*);
    boolean bind_server ();
    boolean listen_client (s32);
    boolean accept_client ();
    boolean connect_server ();
    boolean receive_img (const s8*);
    boolean send_img (const s8*);
    boolean Close_Socket ();
};
#endif // SOCKET_CONN_H
