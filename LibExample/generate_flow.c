#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "capi/nanofi.h"


/**
 * This is an example of the C API that transmits a flow file to a remote instance.
 */
int main(int argc, char **argv) {

  if (argc < 3) {
    printf("Error: must run ./generate_flow <instance> <remote port> \n");
    exit(1);
  }

  char *instance_str = argv[1];
  char *portStr = argv[2];

  nifi_port port;

  port.pord_id = portStr;

  nifi_instance *instance = create_instance(instance_str, &port);

  flow *new_flow = create_flow(instance,"GenerateFlowFile");

  flow_file_record *record = get_next_flow_file(instance, new_flow );

  if (record == 0){
    printf("Could not create flow file");
    exit(1);
  }

  transmit_flowfile(record,instance);

  free_flowfile(record);

  // initializing will make the transmission slightly more efficient.
  //initialize_instance(instance);
  //transfer_file_or_directory(instance,file);

  free_flow(new_flow);

  free_instance(instance);
}
