#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include "transport.h"

void print_packet_information(Packet* packet)
{
  if(packet == NULL)
  {
    printf("Error receiving packet\n");
  }
  printf("Received packet\n");
  printf("type %d\n", packet->type);
  printf("seq %d\n", packet->seq);
  printf("client %d\n", packet->client);
  switch (packet->type) {
    case CONNECTION_REQUEST:
      printf("Connection Request\n");
      printf("client name: %s\n", packet->connection_request.player_name);
      break;
    case CONNECTION_RESPONSE:
      printf("Connection response\n");
      printf("Response message: %d\n", packet->connection_response.response_message);
      break;
    default:
      printf("inavlid packet type\n");
  }

  printf("Printed all packet information\n");

}
