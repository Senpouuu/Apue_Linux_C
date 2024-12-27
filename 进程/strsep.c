#include <stdio.h>
       #include <stdlib.h>
       #include <string.h>

       int
       main(int argc, char *argv[])
       {
           char *token, *subtoken;

           if (argc != 4) {
               fprintf(stderr, "Usage: %s string delim subdelim\n", argv[0]);
               exit(EXIT_FAILURE);
           }

           for (unsigned int j = 1; (token = strsep(&argv[1], argv[2])); j++) {
               printf("%u: %s\n", j, token);

               while ((subtoken = strsep(&token, argv[3])))
                   printf("\t --> %s\n", subtoken);
           }

           exit(EXIT_SUCCESS);
       }

