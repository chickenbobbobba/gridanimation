#include <algorithm>
#include <codecvt>
#include <iostream>
#include <ostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <thread>
#include <chrono>

struct Pixel {
    bool state;
    long age;
};

class Grid {
public:
    std::vector<std::vector<Pixel>> grid;
    long long step = 0;
    size_t width = 10;
    size_t height = 5;

    Grid(size_t w, size_t h) {
        Pixel temp;
        temp.state = false;
        temp.age = 0;
        width = w;
        height = h;
        grid.resize(width, std::vector<Pixel>(height, temp));
    }

    void print() {
        for (size_t i = 0; i < width - 1; i++) std::cout << "~~";
        std::cout << "~\n";
        for (size_t i = 0; i < height; i++) {
            for (size_t j = 0; j < width; j++) {
                if (grid[j][i].state == 1) std::cout << "# ";
                if (grid[j][i].state == 0) std::cout << ". ";
            }
            std::cout << "\n";
        }
    }

    void printBraille() {
        std::string out;
        out += std::to_string(step);
        for (size_t i = 0; i < width/2 - 1 - std::to_string(step).size(); i++) out.append("-");
        std::cout << "-\n";
        for (size_t i = 0; i < height; i += 4) {
            for (size_t j = 0; j < width; j += 2) {
                /*
                14
                25
                36
                78
                */
                long base = 0;
                
                base = base | grid[j+0][i+0].state << 0;
                base = base | grid[j+0][i+1].state << 1;
                base = base | grid[j+0][i+2].state << 2;
                base = base | grid[j+1][i+0].state << 3;
                base = base | grid[j+1][i+1].state << 4;
                base = base | grid[j+1][i+2].state << 5;
                base = base | grid[j+0][i+3].state << 6;
                base = base | grid[j+1][i+3].state << 7;

                std::vector<std::string> braille = {"⠀","⠁","⠂","⠃","⠄","⠅","⠆","⠇","⠈","⠉","⠊","⠋","⠌","⠍","⠎","⠏","⠐","⠑","⠒","⠓","⠔","⠕","⠖","⠗","⠘","⠙","⠚","⠛","⠜","⠝","⠞","⠟","⠠","⠡","⠢","⠣","⠤","⠥","⠦","⠧","⠨","⠩","⠪","⠫","⠬","⠭","⠮","⠯","⠰","⠱","⠲","⠳","⠴","⠵","⠶","⠷","⠸","⠹","⠺","⠻","⠼","⠽","⠾","⠿","⡀","⡁","⡂","⡃","⡄","⡅","⡆","⡇","⡈","⡉","⡊","⡋","⡌","⡍","⡎","⡏","⡐","⡑","⡒","⡓","⡔","⡕","⡖","⡗","⡘","⡙","⡚","⡛","⡜","⡝","⡞","⡟","⡠","⡡","⡢","⡣","⡤","⡥","⡦","⡧","⡨","⡩","⡪","⡫","⡬","⡭","⡮","⡯","⡰","⡱","⡲","⡳","⡴","⡵","⡶","⡷","⡸","⡹","⡺","⡻","⡼","⡽","⡾","⡿","⢀","⢁","⢂","⢃","⢄","⢅","⢆","⢇","⢈","⢉","⢊","⢋","⢌","⢍","⢎","⢏","⢐","⢑","⢒","⢓","⢔","⢕","⢖","⢗","⢘","⢙","⢚","⢛","⢜","⢝","⢞","⢟","⢠","⢡","⢢","⢣","⢤","⢥","⢦","⢧","⢨","⢩","⢪","⢫","⢬","⢭","⢮","⢯","⢰","⢱","⢲","⢳","⢴","⢵","⢶","⢷","⢸","⢹","⢺","⢻","⢼","⢽","⢾","⢿","⣀","⣁","⣂","⣃","⣄","⣅","⣆","⣇","⣈","⣉","⣊","⣋","⣌","⣍","⣎","⣏","⣐","⣑","⣒","⣓","⣔","⣕","⣖","⣗","⣘","⣙","⣚","⣛","⣜","⣝","⣞","⣟","⣠","⣡","⣢","⣣","⣤","⣥","⣦","⣧","⣨","⣩","⣪","⣫","⣬","⣭","⣮","⣯","⣰","⣱","⣲","⣳","⣴","⣵","⣶","⣷","⣸","⣹","⣺","⣻","⣼","⣽","⣾","⣿"};
                long idx = base & 0xFF;
                out.append(braille[idx]);
            }
            out.append("\n");
        }
        std::cout << "\033[1;1H" << out << std::flush;
    }
};

int main(int, char**){
    srand(time(NULL));

    Grid grid(340 * 2, 90 * 4);
    Grid temp = grid;

    for (size_t j = 0; j < grid.height; j++) {
        for (size_t i = 0; i < grid.width; i++) {
            grid.grid[i][j].age = (rand() % 100);
        }
    }

    for (;;) {
        
        for (size_t j = 0; j < grid.height; j++) {
            for (size_t i = 0; i < grid.width; i++) {
                int neighbors = 0;
                if (grid.grid[(i+1) % grid.width][(j-1) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i+1) % grid.width][(j+0) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i+1) % grid.width][(j+1) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i+0) % grid.width][(j-1) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i+0) % grid.width][(j+1) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i-1) % grid.width][(j-1) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i-1) % grid.width][(j+0) % grid.height].state == 1) neighbors++;
                if (grid.grid[(i-1) % grid.width][(j+1) % grid.height].state == 1) neighbors++;

                if (grid.grid[i][j].age < 0) {
                    temp.grid[i][j].age = grid.grid[i][j].age;
                    temp.grid[i][j].state ^= (rand() % 5 != 0);
                } else if (neighbors < 2 || neighbors > 3) {
                    if (grid.grid[i][j].state != 0) {
                        temp.grid[i][j].state = 0;
                        temp.grid[i][j].age = 0;
                    }
                } else if (neighbors == 3) { 
                    if (grid.grid[i][j].state != 1) {
                        temp.grid[i][j].state = 1;
                        temp.grid[i][j].age = 0;
                    }
                } else {
                    temp.grid[i][j].state = grid.grid[i][j].state;
                }
                temp.grid[i][j].age++;
            }
        }

        for (size_t j = 0; j < grid.height; j++) {
            for (size_t i = 0; i < grid.width; i++) {
                grid.grid[i][j] = temp.grid[i][j];
                if (grid.grid[i][j].age > (rand() % 200) + 100) {
                    grid.grid[i][j].age = -1;
                }

            }
        }
        grid.step++;
        grid.printBraille();
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

}
