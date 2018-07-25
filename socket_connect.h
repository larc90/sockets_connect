
#ifndef SOCKET_CONN_H
#define SOCKET_CONN_H

/********* Macros definition **********/
#define SERVER_CONF (0U) /**< Receive an image **/
#define CLIENT_CONF (1U) /**< Send an image **/

#define SERVER_ADDR "127.0.0.1" /**< Local host IP address (Only for testing) **/

#define QUEUE_CONNECTIONS (3) /**< Max queue of pending connections to be stored in server side **/

#define MAX_CHUNK_IMG (1024U) /**< Maximun size of image chunks to be transfered **/
#define ID_STR_SIZE   (3)     /**< Size of identifiers **/

#define NEW_IMG_AVAILABLE_ID  "NI" /**< Identifier for request of sending a image from client **/
#define START_IMG_TX_ID       "SI" /**< Identifier for starting image transference **/
#define INVALID_PETITION_ID   "IP" /**< Identifier for invalid petition of client **/
#define TERMINATE_COM_ID      "TC" /**< Identifier for finishing image transferece **/


/********* Global class definition **********/
class Connection_Utilities {
    struct sockaddr_in socket_server_addr;
    s32 socket_local_desc;
    s32 socket_remote_desc;
    s32 socket_server_addrlen;
  public:
    boolean Create_Socket ();
    void Config_Server (u16, const s8*);
    boolean bind_server ();
    boolean listen_client (s32);
    boolean accept_client ();
    boolean connect_server ();
    s32 receive_img (s8*, s32);
    boolean send_img (s8*, s32);
    boolean Close_Socket ();
};
#endif // SOCKET_CONN_H