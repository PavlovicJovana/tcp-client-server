#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>

void error(char *msg) {
  fprintf(stderr, "%s\n", msg);
  exit(EXIT_FAILURE);
}

double uniform_distribution(double min, double max) {
    double range = max - min; 
    double div = RAND_MAX / range;
    return min + rand() / div;
}

int main() {
  int number_of_picks = 5;   //for example
  int frequency = 100;   //for example
  double pick = 0.0;
  char output[9] = "";
  char line[150] = "";
  
  //taking port from properties file
  FILE *file;
  if (file = fopen("../Server-in-Java/config.properties", "r")) {
    while(!feof(file)) {
      fgets(line, 150, file);
      memcpy(line, &line[7], 4);
    }
  } else {
    error("fopen() failed");
  }
  fclose(file);

  //create a socket
  int client;
  if((client = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    error("socket() failed");

  //specify an address for the socket
  struct sockaddr_in server_address;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(atoi(line));
  server_address.sin_addr.s_addr = INADDR_ANY;

  //check for error with the connection
  if(connect(client, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
    error("connect() failed");
  printf("The client has connected to the server\n\n");

  //send number of pics to the server
  if(send(client, (void*) &number_of_picks, sizeof(number_of_picks), 0) < 0)
    error("send() failed");

  //send frequency to the server
  snprintf(output, 9, "%d", frequency);
  strcat(output, "\n");
  if(send(client, output, sizeof(output), 0) < 0)
    error("send() failed");

  //send picks to the server
  printf("Data generation begins\n");
  for(int i = 1; i <= number_of_picks; i++) {
    pick = (double) i/frequency + uniform_distribution((double) -1/(10*frequency), (double) 1/(10*frequency));
    printf("%f\n", pick);
    snprintf(output, 9, "%f", pick);
    strcat(output, "\n");
    
    if(send(client, output, sizeof(output), 0) < 0)
      error("send() failed");
  }
  printf("Data generation ends\n\n"); 

  //close the socket
  if(close(client) < 0)
    error("close() failed");

  return 0;
}
