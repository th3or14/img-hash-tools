#include "key-frames-extractor.hpp"

#include <QCommandLineParser>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);
    QCommandLineParser parser;
    parser.addHelpOption();
    QCommandLineOption input_video_filename_option(
        "i", "Sets input video filename.", "input video filename");
    parser.addOption(input_video_filename_option);
    QCommandLineOption output_directory_option("o", "Sets output directory.",
                                               "output directory");
    parser.addOption(output_directory_option);
    parser.process(app);
    if (!parser.isSet(input_video_filename_option)) {
        std::cout << "Error: input video filename is not set." << "\n";
        parser.showHelp(EXIT_FAILURE);
    }
    if (!parser.isSet(output_directory_option)) {
        std::cout << "Error: output directory is not set." << "\n";
        parser.showHelp(EXIT_FAILURE);
    }
    extract_key_frames(parser.value(input_video_filename_option),
                       parser.value(output_directory_option));
    return EXIT_SUCCESS;
}
