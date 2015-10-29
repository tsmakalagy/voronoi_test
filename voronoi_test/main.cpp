
#include <iostream>
#include <iomanip>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <voronoi.h>

using namespace std;
using namespace cv;

inline int CLI_help(int argc, char** argv) {
  printf("Usage: %s <command> <implementation_name> <files>\n", argv[0]);
  printf(" * command: [ thin | video | video_bright | video_comparer | benchmark ]\n");
  printf("   If command =  video_comparer or benchmark, no implementation must be specified.\n");
  printf(" * implementation_name: [%s]\n",
         VoronoiThinner::all_implementations_as_string().c_str());
  printf("\nExamples:\n");
  printf("  %s video           morph            horse.png\n", argv[0]);
  printf("  %s thin            zhang_suen_fast  *.png\n", argv[0]);
  printf("  %s video_comparer                   *.png\n", argv[0]);
  return -1;
}

enum {THIN, VIDEO, VIDEO_BRIGHT, VIDEO_COMPARER, BENCHMARK};
int CLI(int argc, char** argv) {
  //  for (int argi = 0; argi < argc; ++argi)
  //    printf("argv[%i]:'%s'\n", argi, argv[argi]);
  if (argc < 3) // [exename] + 2 args
    return CLI_help(argc, argv);
  // detect order
  int order = -1;
  string order_str (argv[1]);
  if (order_str == "thin")
    order = THIN;
  else {
    printf("Unknown order '%s'\n", order_str.c_str());
    return CLI_help(argc, argv);
  }
  // check implementation
  string implementation_name (argv[2]);
  int first_file_idx = 2;
  if (order != VIDEO_COMPARER && order != BENCHMARK) {
    first_file_idx = 3;
    if (!VoronoiThinner::is_implementation_valid(implementation_name)) {
      printf("Unknown implementation '%s'\n", implementation_name.c_str());
      return CLI_help(argc, argv);
    }
  }
  if (argc < first_file_idx + 1) // [exename] [order] [file(s)]
    return CLI_help(argc, argv);

  VoronoiThinner thinner;
  // load files
  vector<Mat1b> files;
  for (int argi = first_file_idx; argi < argc; ++argi) {
    Mat1b file = imread(argv[argi], CV_LOAD_IMAGE_GRAYSCALE);
    if (file.empty())
      printf("Could not load file '%s'\n", argv[argi]);
    else
      files.push_back(file);
  } // end loop argi

  // process
  if (order == THIN) {
    for (unsigned int file_idx = 0; file_idx < files.size(); ++file_idx) {
      //Timer timer;
      bool ok = thinner.thin(files[file_idx], implementation_name, true);
      if (!ok) {
        printf("Failed thinning with implementation '%s'\n", implementation_name.c_str());
        continue;
      }
      //timer.printTime(implementation_name.c_str());
      // write file
      ostringstream out; out << "out_" << file_idx << ".png";
      imwrite(out.str(), thinner.get_skeleton());
      printf("Written file '%s'\n", out.str().c_str());
      // show res
      imshow("query", files[file_idx]);
      imshow(implementation_name, thinner.get_skeleton());
      waitKey(0);
    } // end loop file_idx
  } // end if (order == THIN)

  return 0;
} // end CLI()

////////////////////////////////////////////////////////////////////////////////

int main(int argc, char** argv) {
  return CLI(argc, argv);
}