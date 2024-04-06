#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>

bool full_file_text = true;

char* GetFilenameFromConsole(int argc, char** argv) {
    char* filename = nullptr;
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' && argv[i - 1][0] != '-') {
            filename = argv[i];
        }
    }

    return filename;
}

int GetLinesFromConsole (int argc, char** argv) {
    int lines_num = 1;
    for (int i = 1; i < argc - 1; i++) {
        if (!strcmp(argv[i], "-l")) {
            full_file_text = false;
            if (argv[i + 1][0] == '-' || argv[i + 1] == nullptr) {
                return EXIT_FAILURE;
            } else {
                int res = std::atoi(argv[i + 1]);
                lines_num = res;
            }
        } else if (!strncmp("--lines=", argv[i], 8)) {
            full_file_text = false;
            int res = std::atoi(argv[i] + 8);
            lines_num = res;
        }
    }

    if (lines_num <= 0) {
        std::cerr << "User error";
        return EXIT_FAILURE;
    }

    return lines_num;
}

bool GetTailFromConsole (int argc, char** argv) {
    bool get_tail = false;
    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-t") || !strcmp(argv[i], "--tail")) {
            get_tail = true;
        }
    }

    return get_tail;
}

char GetDelimiterFromConsole (int argc, char** argv) {
    for (int i = 1; i < argc - 1; i++) {
        unsigned int length1 = std::strlen(argv[i]);
        unsigned int length2 = std::strlen(argv[i + 1]);
        if (!strcmp("-d", argv[i])) {
            if (argv[i + 1] == nullptr || argv[i + 1][0] == '-' && isalpha(argv[i + 1][1]) || argv[i + 1][1] == '-') {
                std::cerr << "User error";
                return EXIT_FAILURE;
            } else if (length2 == 1) {
                return argv[i + 1][0];
            } else if (length2 == 2) {
                return argv[i + 1][1];
            }
        } else if (!strncmp("--delimiter=", argv[i], 12)) {
            if (length1 == 13) {
                return argv[i][12];
            } else if (length1 == 14) {
                return argv[i][13];
            }
        }
    }

    return 'n';
}

int main(int argc, char** argv) {
    setlocale(LC_ALL, "RU");
    char* filename = GetFilenameFromConsole(argc, argv);
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cout << "Error: programme can't open file" << '\n';
    } else {
        int lines_num;
        bool tail;
        char delimiter[3] = {'\\', 'n', '\0'};
        tail = GetTailFromConsole(argc, argv);
        lines_num = GetLinesFromConsole(argc, argv);
        delimiter[1] = GetDelimiterFromConsole(argc, argv);
        char current_delimiter;
        if (delimiter[0] == '\\') {
            switch (delimiter[1]) {
                case 'n':
                    current_delimiter = '\n';
                    break;
                case 't':
                    current_delimiter = '\t';
                    break;
                case 'r':
                    current_delimiter = '\r';
                    break;
                case 'v':
                    current_delimiter = '\v';
                    break;
                case 'b':
                    current_delimiter = '\b';
                    break;
                case '\\':
                    current_delimiter = '\\';
                    break;
                case '\'':
                    current_delimiter = '\'';
                    break;
                case '\"':
                    current_delimiter = '\"';
                    break;
                case '0':
                    current_delimiter = '\0';
                    break;
                case '?':
                    current_delimiter = '\?';
                    break;
                default:
                    current_delimiter = delimiter[1];
                    break;
            }
        } else {
            return EXIT_FAILURE;
        }
        char current_symbol;
        file.get(current_symbol);
        if (full_file_text) {
            while (!file.eof()) {
                std::cout << current_symbol;
                file.get(current_symbol);
            }
            file.close();
        } else if (!tail) {
            while (!file.eof()) {
                if (!lines_num) {
                    file.close();
                    break;
                }
                std::cout << current_symbol;
                file.get(current_symbol);
                if (current_symbol == current_delimiter) {
                    lines_num--;
                }
            }
            file.close();
        } else {
            file.seekg(-1, std::ios::end);
            int counter = 0;
            while (file.seekg(-1, std::ios::cur)) {
                if (file.peek() == current_delimiter) {
                    counter++;
                }
                if (counter == lines_num || (lines_num - 1 == counter && !file.tellg())) {
                    file.get();
                    std::cout << file.rdbuf();
                    break;
                }
            }
            file.close();
        }
    }
    file.close();

    return 0;
}
