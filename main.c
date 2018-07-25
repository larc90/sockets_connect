#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include "stdtypes.h"
#include "socket_connect.h"

using namespace std;


/* Function:     main */
/** @brief       Configures a client or server (depending of arguments given)
 **              for reception or sending of a image.
 **
 ** @param[in]   argc - Number of given parameter in command line.
 ** @param[in]   argv[1] - server/client.
 ** @param[in]   argv[2] - <Port number>.
 ** @param[in]   argv[3] - TBD. For now localhost is used for testing
 ** @param[out]  None
 ** @return      -1 Error. Otherwise, all good.
 **/
int main (int argc, char const *argv[]) {
//  FILE* image;
  Connection_Utilities Conn_Utils;
  s8 Buffer[30];
  const s8* Ip_Addr_Server = SERVER_ADDR; // Used for testing
  u16 Port_Num;
  boolean Action = CLIENT_CONF;
  boolean Status = TRUE;

  // Check arguments given in command line
  if (argc < 3) {
    cout << "  ERROR: Bad number of arguments given. \n";
    exit (EXIT_FAILURE);
  }

  if (argv[1] == "server") {
      Action = SERVER_CONF;
  }

  Port_Num = atoi (argv[2]);
//    image = fopen(argv[1], rb);

  // TODO: Add argv[3] processing

  // Create local socket
  if (Conn_Utils.Create_Socket() == FALSE) {
    exit (EXIT_FAILURE);
  }

  // Configure Server
  Conn_Utils.Config_Server (Port_Num, Ip_Addr_Server);

  // Run code as a server
  if (Action == SERVER_CONF) {

    // Bind the server to the IP address and port number specified
    if (Conn_Utils.bind_server () == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Look for some client connection
    if (Conn_Utils.listen_client (QUEUE_CONNECTIONS) == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Accept connection from client 
    if (Conn_Utils.accept_client () == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Receive image from client
    if (Conn_Utils.receive_img (Buffer, 30) < 1) {
      exit (EXIT_FAILURE);
    }

    cout << Buffer << '\n';
    
  } else {  // Run code as a client

    // Initiates a connection with server socket
    if (Conn_Utils.connect_server () == FALSE) {
      exit (EXIT_FAILURE);
    }

    // Send new image to server
    if (Conn_Utils.send_img ((s8*) "HELLO WORLD", 12) == FALSE) {
      exit (EXIT_FAILURE);
    }

  }
  
  // Close opened sockets
  if (Conn_Utils.Close_Socket () == FALSE) {
    exit (EXIT_FAILURE);
  }
}
