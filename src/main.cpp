// C Standard Libraries
#include <cstdio>

// GSL
#include <gsl/gsl_matrix.h>

// Project
#include "rocket.hpp"
#include "demorocket.hpp"

int main(int argc, char** argv) {
  // check for arguments
  bool use_spreadsheet = false;
  if(argc > 1) {
    if(strcmp(argv[1], "help") == 0) {
      printf("Specify 'spreadsheet' to switch output to an excel-compatible format.\n");
      return 0;
    } else if (strcmp(argv[1], "spreadsheet") == 0) {
      use_spreadsheet = true;
      printf("time sx sy sz lmx lmy lmz amx amy amz mass\n");
    } else {
      printf("Argument '%s' not recognized. Try 'help'\n", argv[1]);
      return 1;
    }
  }


  Rocket rocket(0.01);
  rocket.print();
  int ret = demoRocket(rocket, use_spreadsheet, &argc, argv);
  if(ret != 0) {
    printf("An error occured.\n");
  } else {
    rocket.print();
  }

  return ret;
}
