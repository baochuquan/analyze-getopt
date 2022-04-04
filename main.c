//
//  main.cpp
//  test
//
//  Created by baochuquan on 2022/3/20.
//

#include <stdio.h>
#include <string.h>
#include <getopt.h>

static void permute(char *const *argv, int dest, int src)
{
    char **av = (char **)argv;
    char *tmp = av[src];
    int i;
    // src > dest
    for (i = src; i > dest; i--)
        av[i] = av[i-1];
    av[dest] = tmp;
}

int main(int argc, const char * argv[]) {
    // insert code here...

    char *option[] = {
        "getopt",
        "--options",
        "hxab:c::",
        "--longoptions",
        "help,debug,a-long,b-long:,c-long::",
        "-n",
        "--",
        "\'Error\'",
        "--a-long",
        "--b-long",
        "arg1",
        "--c-long"
    };
    int count = 11;
    char *optstring = "+ao:l:n:qQs:TuhV";
    static const struct option longopts[] = {
            {"options", required_argument, NULL, 'o'},
            {"longoptions", required_argument, NULL, 'l'},
            {"quiet", no_argument, NULL, 'q'},
            {"quiet-output", no_argument, NULL, 'Q'},
            {"shell", required_argument, NULL, 's'},
            {"test", no_argument, NULL, 'T'},
            {"unquoted", no_argument, NULL, 'u'},
            {"help", no_argument, NULL, 'h'},
            {"alternative", no_argument, NULL, 'a'},
            {"name", required_argument, NULL, 'n'},
            {"version", no_argument, NULL, 'V'},
            {NULL, 0, NULL, 0}
        };
    int num = getopt_long(count, option, optstring, longopts, NULL);
    printf("HHHH: %d", num);
//    permute(option, 6, 3);
//    for (int i = 0; i < 11; i++) {
//        printf("%d: ", i);
//        printf("%s\n", option[i]);
//    }
    return 0;
}

//int
//main (argc, argv)
//     int argc;
//     char **argv;
//{
//  int c;
//  int digit_optind = 0;
//
//  while (1)
//    {
//      int this_option_optind = optind ? optind : 1;
//
//      c = getopt (argc, argv, "abc:d:0123456789");
//      if (c == -1)
//        break;
//
//      switch (c)
//        {
//        case '0':
//        case '1':
//        case '2':
//        case '3':
//        case '4':
//        case '5':
//        case '6':
//        case '7':
//        case '8':
//        case '9':
//          if (digit_optind != 0 && digit_optind != this_option_optind)
//            printf ("digits occur in two different argv-elements.\n");
//          digit_optind = this_option_optind;
//          printf ("option %c\n", c);
//          break;
//
//        case 'a':
//          printf ("option a\n");
//          break;
//
//        case 'b':
//          printf ("option b\n");
//          break;
//
//        case 'c':
//          printf ("option c with value `%s'\n", optarg);
//          break;
//
//        case '?':
//          break;
//
//        default:
//          printf ("?? getopt returned character code 0%o ??\n", c);
//        }
//    }
//
//  if (optind < argc)
//    {
//      printf ("non-option ARGV-elements: ");
//      while (optind < argc)
//        printf ("%s ", argv[optind++]);
//      printf ("\n");
//    }
//}
