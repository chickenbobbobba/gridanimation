#include <iostream>
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
        for (size_t i = 0; i < width-1; i++) std::cout << "--";
        std::cout << "-\n";
        for (size_t i = 0; i < height; i += 4) {
            for (size_t j = 0; j < width; j += 2) {

            }
            std::cout << "\n";
        }
    }
};

int main(int, char**){
    srand(time(NULL));

    Grid grid(100, 20);
    grid.print();
    Grid temp(100, 20);

    for (size_t j = 0; j < grid.height; j++) {
        for (size_t i = 0; i < grid.width; i++) {
            grid.grid[i][j].state = (rand() % 2 == 0);
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
                if (grid.grid[i][j].age > 100) {
                    grid.grid[i][j].age = -1;
                }

            }
        }
        
        grid.printBraille();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));

    }

}
