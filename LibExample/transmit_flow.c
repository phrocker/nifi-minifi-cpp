#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include "capi/nanofi.h"

int is_dir(const char *path) {
  struct stat stat_struct;
  if (stat(path, &stat_struct) != 0)
    return 0;
  return S_ISDIR(stat_struct.st_mode);
}

void transfer_file_or_directory(nifi_instance *instance, char *file_or_dir) {
  int size = 1;

  if (is_dir(file_or_dir)) {
    DIR *d;

    struct dirent *dir;
    d = opendir(file_or_dir);
    if (d) {
      while ((dir = readdir(d)) != NULL) {
        if (!memcmp(dir->d_name,".",1) )
          continue;
        char *file_path = malloc(strlen(file_or_dir) + strlen(dir->d_name) + 2);
        sprintf(file_path,"%s/%s",file_or_dir,dir->d_name);
        transfer_file_or_directory(instance,file_path);
        free(file_path);
      }
      closedir(d);
    }
    printf("%s is a directory", file_or_dir);
  } else {
    printf("Transferring %s\n",file_or_dir);

    flow_file_record *record = create_flowfile(file_or_dir);

    add_attribute(record, "addedattribute", "1", 2);

    transmit_flowfile(record, instance);

    free_flowfile(record);
  }
}

/**
 * This is an example of the C API that transmits a flow file to a remote instance.
 */
int main(int argc, char **argv) {

  if (argc < 4) {
    printf("Error: must run ./transmit_flow <instance> <remote port> <file or directory>\n");
    exit(1);
  }

  char *instance_str = argv[1];
  char *portStr = argv[2];
  char *file = argv[3];

  nifi_port port;

  port.pord_id = portStr;

  nifi_instance *instance = create_instance(instance_str, &port);

  // initializing will make the transmission slightly more efficient.
  //initialize_instance(instance);
  transfer_file_or_directory(instance,file);

  free_instance(instance);
}
