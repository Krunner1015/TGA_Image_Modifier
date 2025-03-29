#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include <algorithm>
using namespace std;

//color data is in BGR not RGB!!!

struct Header {
    char idLength;
    char colorMapType;
    char dataTypeCode;
    short colorMapOrigin;
    short colorMapLength;
    char colorMapDepth;
    short xOrigin;
    short yOrigin;
    short width;
    short height;
    char bitsPerPixel;
    char imageDescription;
};

class TGA {
public:
    string path;
    Header header;
    vector<char> data;

    TGA(): header() {}

    void read(const string &path) {
        ifstream file(path, ios::binary);
        if (!file.is_open()) {
            cerr << "Error opening file " << path << endl;
            exit(1);
        }

        file.read(&header.idLength, sizeof(header.idLength));
        file.read(&header.colorMapType, sizeof(header.colorMapType));
        file.read(&header.dataTypeCode, sizeof(header.dataTypeCode));
        file.read(reinterpret_cast<char*>(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
        file.read(reinterpret_cast<char*>(&header.colorMapLength), sizeof(header.colorMapLength));
        file.read(&header.colorMapDepth, sizeof(header.colorMapDepth));
        file.read(reinterpret_cast<char*>(&header.xOrigin), sizeof(header.xOrigin));
        file.read(reinterpret_cast<char*>(&header.yOrigin), sizeof(header.yOrigin));
        file.read(reinterpret_cast<char*>(&header.width), sizeof(header.width));
        file.read(reinterpret_cast<char*>(&header.height), sizeof(header.height));
        file.read(&header.bitsPerPixel, sizeof(header.bitsPerPixel));
        file.read(&header.imageDescription, sizeof(header.imageDescription));

        data.resize(header.width * header.height * (header.bitsPerPixel / 8));
        file.read(data.data(), data.size());

        file.close();
    }

    void write(const string &path) {
        ofstream file(path, ios::binary);
        if (!file.is_open()) {
            cerr << "Error opening file " << path << endl;
            exit(1);
        }

        file.write(&header.idLength, sizeof(header.idLength));
        file.write(&header.colorMapType, sizeof(header.colorMapType));
        file.write(&header.dataTypeCode, sizeof(header.dataTypeCode));
        file.write(reinterpret_cast<char*>(&header.colorMapOrigin), sizeof(header.colorMapOrigin));
        file.write(reinterpret_cast<char*>(&header.colorMapLength), sizeof(header.colorMapLength));
        file.write(&header.colorMapDepth, sizeof(header.colorMapDepth));
        file.write(reinterpret_cast<char*>(&header.xOrigin), sizeof(header.xOrigin));
        file.write(reinterpret_cast<char*>(&header.yOrigin), sizeof(header.yOrigin));
        file.write(reinterpret_cast<char*>(&header.width), sizeof(header.width));
        file.write(reinterpret_cast<char*>(&header.height), sizeof(header.height));
        file.write(&header.bitsPerPixel, sizeof(header.bitsPerPixel));
        file.write(&header.imageDescription, sizeof(header.imageDescription));

        file.write(data.data(), data.size());

        file.close();
    }

    TGA Multiply(const TGA &other) const {
        TGA result = *this;
        for (int i = 0; i < data.size(); i++) {
            unsigned char P1 = static_cast<unsigned char>(data[i]);
            unsigned char P2 = static_cast<unsigned char>(other.data[i]);

            float NP1 = P1 / 255.0f;
            float NP2 = P2 / 255.0f;

            int val = static_cast<int>(NP1 * NP2 * 255 + 0.5f);
            if (val < 0) {
                val = 0;
            } else if (val > 255) {
                val = 255;
            }

            result.data[i] = static_cast<unsigned char>(val);
        }
        return result;
    }

    TGA Screen(const TGA &other) const {
        TGA result = *this;
        for (int i = 0; i < data.size(); i++) {
            unsigned char P1 = static_cast<unsigned char>(data[i]);
            unsigned char P2 = static_cast<unsigned char>(other.data[i]);

            float NP1 = P1 / 255.0f;
            float NP2 = P2 / 255.0f;

            int val = static_cast<int>((1-((1-NP1)*(1-NP2))) * 255 + 0.5f);
            if (val < 0) {
                val = 0;
            } else if (val > 255) {
                val = 255;
            }

            result.data[i] = static_cast<unsigned char>(val);
        }
        return result;
    }

    TGA Subtract(const TGA &other) const {
        TGA result = *this;
        for (int i = 0; i < data.size(); i++) {
            unsigned char P1 = static_cast<unsigned char>(data[i]);
            unsigned char P2 = static_cast<unsigned char>(other.data[i]);

            int val = static_cast<int>(P1) - static_cast<int>(P2);
            if (val < 0) {
                val = 0;
            } else if (val > 255) {
                val = 255;
            }

            result.data[i] = static_cast<unsigned char>(val);
        }
        return result;
    }

    TGA Addition(const TGA &other) const {
        TGA result = *this;
        for (int i = 0; i < data.size(); i++) {
            unsigned char P1 = static_cast<unsigned char>(data[i]);
            unsigned char P2 = static_cast<unsigned char>(other.data[i]);

            int val = static_cast<int>(P1) + static_cast<int>(P2);
            if (val < 0) {
                val = 0;
            } else if (val > 255) {
                val = 255;
            }

            result.data[i] = static_cast<unsigned char>(val);
        }
        return result;
    }

    TGA Overlay(const TGA &other) const {
        TGA result = *this;
        for (int i = 0; i < data.size(); i++) {
            unsigned char P1 = static_cast<unsigned char>(data[i]);
            unsigned char P2 = static_cast<unsigned char>(other.data[i]);

            float NP1 = P1 / 255.0f;
            float NP2 = P2 / 255.0f;
            if (NP2 <= 0.5) {
                int val = static_cast<int>(2*NP1*NP2 * 255 + 0.5f);
                if (val < 0) {
                    val = 0;
                } else if (val > 255) {
                    val = 255;
                }

                result.data[i] = static_cast<unsigned char>(val);
            } else {
                int val = static_cast<int>((1-(2*(1-NP1)*(1-NP2))) * 255 + 0.5f);
                if (val < 0) {
                    val = 0;
                } else if (val > 255) {
                    val = 255;
                }

                result.data[i] = static_cast<unsigned char>(val);
            }
        }
        return result;
    }
};

int main(int argc, char *argv[]) {
    vector<string> args(argv + 1, argv + argc);
    vector<string> validfiles = {
        "car.tga", "circles.tga", "layer1.tga", "layer2.tga",
        "layer_blue.tga", "layer_green.tga", "layer_red.tga",
        "pattern1.tga", "pattern2.tga", "text.tga", "text2.tga"};
    string tracking = "";

    cout << "You inputted " << args.size() << " arguments." << endl;
    for (int i = 0; i < args.size(); i++) {
        cout << i + 1 << ": " << args[i] << endl;
    }

    if (argc <= 1 || args[0] == "--help") { //checks for no input or help input
        cout << "Project 2: Image Processing, Spring 2025" << endl << endl;
        cout << "Usage:" << endl;
        cout << "   ./project2.out [output] [firstImage] [method] [...]" << endl;
    } else if (args[0] == "Tasks") {
        TGA car, circles, layer1, layer2, layer_blue, layer_green, layer_red, pattern1, pattern2, text, text2;

        car.read("input/car.tga");
        circles.read("input/circles.tga");
        layer1.read("input/layer1.tga");
        layer2.read("input/layer2.tga");
        layer_blue.read("input/layer_blue.tga");
        layer_green.read("input/layer_green.tga");
        layer_red.read("input/layer_red.tga");
        pattern1.read("input/pattern1.tga");
        pattern2.read("input/pattern2.tga");
        text.read("input/text.tga");
        text2.read("input/text2.tga");

        TGA output1 = layer1.Multiply(pattern1);
        output1.write("output/part1.tga");

        TGA output2 = car.Subtract(layer2);
        output2.write("output/part2.tga");

        TGA output3 = layer1.Multiply(pattern2).Screen(text);
        output3.write("output/part3.tga");

        TGA output4 = layer2.Multiply(circles).Subtract(pattern2);
        output4.write("output/part4.tga");

        TGA output5 = layer1.Overlay(pattern1);
        output5.write("output/part5.tga");

        TGA output6 = car;
        for (int i = 0; i < output6.data.size(); i += 3) {
            unsigned char P1 = static_cast<unsigned char>(output6.data[i+1]);

            int val = static_cast<int>(P1) + 200;
            if (val < 0) {
                val = 0;
            } else if (val > 255) {
                val = 255;
            }

            output6.data[i+1] = static_cast<unsigned char>(val);
        }
        output6.write("output/part6.tga");

        TGA output7 = car;
        for (int i = 0; i < output7.data.size(); i += 3) {
            unsigned char P1 = static_cast<unsigned char>(output7.data[i+2]);

            int valb = 0;
            int valr = static_cast<int>(P1) * 4;
            if (valr < 0) {
                valr = 0;
            } else if (valr > 255) {
                valr = 255;
            }

            output7.data[i] = static_cast<unsigned char>(valb);
            output7.data[i+2] = static_cast<unsigned char>(valr);
        }
        output7.write("output/part7.tga");

        TGA output8_b = car;
        TGA output8_g = car;
        TGA output8_r = car;
        for (int i = 0; i < car.data.size(); i += 3) {
            unsigned char P1 = static_cast<unsigned char>(car.data[i]);
            unsigned char P2 = static_cast<unsigned char>(car.data[i+1]);
            unsigned char P3 = static_cast<unsigned char>(car.data[i+2]);

            output8_b.data[i] = P1;
            output8_b.data[i+1] = P1;
            output8_b.data[i+2] = P1;
            output8_g.data[i] = P2;
            output8_g.data[i+1] = P2;
            output8_g.data[i+2] = P2;
            output8_r.data[i] = P3;
            output8_r.data[i+1] = P3;
            output8_r.data[i+2] = P3;
        }
        output8_b.write("output/part8_b.tga");
        output8_g.write("output/part8_g.tga");
        output8_r.write("output/part8_r.tga");

        TGA output9 = layer_blue;
        for (int i = 0; i < layer_blue.data.size(); i += 3) {
            output9.data[i] = layer_blue.data[i];
            output9.data[i+1] = layer_green.data[i+1];
            output9.data[i+2] = layer_red.data[i+2];
        }
        output9.write("output/part9.tga");

        TGA output10 = text2;
        int pixelCount = text2.data.size() / 3;
        for (int i = 0; i < pixelCount; i++) {
            int j = (pixelCount - i - 1) * 3;
            output10.data[i * 3] = text2.data[j];
            output10.data[i * 3 + 1] = text2.data[j+1];
            output10.data[i * 3 + 2] = text2.data[j+2];
        }
        output10.write("output/part10.tga");

        cout << "Tasks complete!" << endl;

    } else if (args[0].size() < 4 || args[0].substr(args[0].size() - 4) != ".tga") {
        //checks first argument for a correct output file type
        cout << "Invald file name." << endl;

    } else if (argc <= 3 || args[1].size() < 4 || args[1].substr(args[1].size() - 4) != ".tga") {
        //checks second argument for a correct input file type
        cout << "Invald file name." << endl;

    } else if (find(validfiles.begin(), validfiles.end(), args[1]) == validfiles.end()) {
        //checks that the second inputted file actually exists as an output file
        cout << "File does not exist." << endl;

    } else {
        string output = "C:/Users/kaide/CLionProjects/Project_2/output/" + args[0];
        string input = "C:/Users/kaide/CLionProjects/Project_2/input/" + args[1];
        if (args[2] == "multiply") {
            if (args.size() >= 4) {
                if (args[3].size() >= 4 && args[3].substr(args[3].size() - 4) == ".tga") {
                    if (find(validfiles.begin(), validfiles.end(), args[3]) != validfiles.end()) {
                        string input2 = "C:/Users/kaide/CLionProjects/Project_2/input/" + args[3];
                        TGA mult1, mult2;
                        mult1.read(input);
                        mult2.read(input2);
                        mult1.Multiply(mult2).write(output);

                        cout << "Multiplied " << args[1] << " and " << args[3] << endl;
                    } else {
                        cout << "Invalid argument, file does not exist." << endl;
                    }
                } else {
                    cout << "Invalid argument, invalid file name." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "subtract") {
            if (args.size() >= 4) {
                if (args[3].size() >= 4 && args[3].substr(args[3].size() - 4) == ".tga") {
                    if (find(validfiles.begin(), validfiles.end(), args[3]) == validfiles.end()) {
                        cout << "Multiplying " << args[1] << endl;
                    } else {
                        cout << "Invalid argument, file does not exist." << endl;
                    }
                } else {
                    cout << "Invalid argument, invalid file name." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "overlay") {
            if (args[3].data() && args[4].data()) {
                if (args[3].size() >= 4 && args[4].size() >= 4 && args[3].substr(args[3].size() - 4) == ".tga" && args[4].substr(args[4].size() - 4) == ".tga") {
                    if ((find(validfiles.begin(), validfiles.end(), args[3]) == validfiles.end()) && (find(validfiles.begin(), validfiles.end(), args[4]) == validfiles.end())) {
                        cout << "Multiplying " << args[1] << endl;
                    } else {
                        cout << "Invalid argument, file does not exist." << endl;
                    }
                } else {
                    cout << "Invalid argument, invalid file name." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "screen") {
            if (args.size() >= 4) {
                if (args[3].size() >= 4 && args[3].substr(args[3].size() - 4) == ".tga") {
                    if (find(validfiles.begin(), validfiles.end(), args[3]) == validfiles.end()) {
                        cout << "Multiplying " << args[1] << endl;
                    } else {
                        cout << "Invalid argument, file does not exist." << endl;
                    }
                } else {
                    cout << "Invalid argument, invalid file name." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "combine") {
            if (args.size() >= 4) {
                if (args[3].size() >= 4 && args[3].substr(args[3].size() - 4) == ".tga") {
                    if (find(validfiles.begin(), validfiles.end(), args[3]) == validfiles.end()) {
                        cout << "Multiplying " << args[1] << endl;
                    } else {
                        cout << "Invalid argument, file does not exist." << endl;
                    }
                } else {
                    cout << "Invalid argument, invalid file name." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "flip") {
            cout << "Flipping " << args[1] << endl;
        } else if (args[2] == "onlyred") {
            cout << "Only red " << args[1] << endl;
        } else if (args[2] == "onlygreen") {
            cout << "Only green " << args[1] << endl;
        } else if (args[2] == "onlyblue") {
            cout << "Only blue " << args[1] << endl;
        } else if (args[2] == "addred") {
            cout << "addred" << endl;
            if (args.size() >= 4) {
                try {
                    int num = atoi(args[3].data());
                    cout << "Int: " << num << endl;
                } catch (const invalid_argument &e) {
                    cout << "Invalid argument, expected a number." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "addgreen") {
            if (args.size() >= 4) {
                try {
                    int num = atoi(args[3].data());
                    cout << "Int: " << num << endl;
                } catch (const invalid_argument &e) {
                    cout << "Invalid argument, expected a number." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "addblue") {
            if (args.size() >= 4) {
                try {
                    int num = atoi(args[3].data());
                    cout << "Int: " << num << endl;
                } catch (const invalid_argument &e) {
                    cout << "Invalid argument, expected a number." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "scalered") {
            if (args.size() >= 4) {
                try {
                    int num = atoi(args[3].data());
                    if (num < 0) {
                        cout << "Invalid argument, expected a number." << endl;
                    } else {
                        cout << "Int: " << num << endl;
                    }
                } catch (const invalid_argument &e) {
                    cout << "Invalid argument, expected a number." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "scalegreen") {
            if (args.size() >= 4) {
                try {
                    int num = atoi(args[3].data());
                    if (num < 0) {
                        cout << "Invalid argument, expected a number." << endl;
                    } else {
                        cout << "Int: " << num << endl;
                    }
                } catch (const invalid_argument &e) {
                    cout << "Invalid argument, expected a number." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else if (args[2] == "scaleblue") {
            if (args.size() >= 4) {
                try {
                    int num = atoi(args[3].data());
                    if (num < 0) {
                        cout << "Invalid argument, expected a number." << endl;
                    } else {
                        cout << "Int: " << num << endl;
                    }
                } catch (const invalid_argument &e) {
                    cout << "Invalid argument, expected a number." << endl;
                }
            } else {
                cout << "Missing argument." << endl;
            }
        } else {
            cout << "Invalid method name." << endl;
        }
    }

    return 0;
}