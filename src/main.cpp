#include <algorithm>
#include <climits>
#include <cmath>
#include <codecvt>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <set>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <thread>
#include <chrono>
#include <unordered_set>

#ifdef _WIN32
#include <windows.h>
#else
#include <sys/ioctl.h>
#include <unistd.h>
#endif

void getTerminalSize(int& scrwidth, int& scrheight) {
#ifdef _WIN32
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    scrwidth = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    scrheight = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
#else
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    scrwidth = ws.ws_col;
    scrheight = ws.ws_row;
#endif
}

struct Pixel {
    bool state;
    long lastupd;
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
        temp.lastupd = -1;
        width = w;
        height = h;
        grid.resize(width, std::vector<Pixel>(height, temp));
    }

    void resize(size_t w, size_t h) {
        if (w == width && h == height) return;
        // create a new grid with requested dimensions and default pixels
        std::vector<std::vector<Pixel>> newgrid;
        Pixel def{false, -1};
        newgrid.resize(w, std::vector<Pixel>(h, def));

        // copy overlapping region from the old grid
        size_t minw = std::min(w, width);
        size_t minh = std::min(h, height);
        for (size_t x = 0; x < minw; ++x) {
            for (size_t y = 0; y < minh; ++y) {
                newgrid[x][y] = grid[x][y];
            }
        }

        grid.swap(newgrid);
        width = w;
        height = h;
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
        out.append("-\n");
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

std::uint64_t hash ( std::uint64_t z ) {
    z = ( z ^ ( z >> 30 ) ) * std::uint64_t { 0xBF58476D1CE4E5B9 };
    z = ( z ^ ( z >> 27 ) ) * std::uint64_t { 0x94D049BB133111EB };
    return z ^ ( z >> 31 );
}

// Hash for tuple<long, long>
struct Hash {
    std::size_t operator()(const std::tuple<long, long>& t) const {
        return hash(std::get<1>(t) + hash(std::get<0>(t)));
    }
};

// Hash for tuple<long, long, bool, long>
struct Hash1 {
    std::size_t operator()(const std::tuple<long, long, bool, long>& t) const {
        return hash(std::get<1>(t) + hash(std::get<0>(t)));
    }
};

int main(int, char**){
    srand(time(NULL));

    int w;
    int h;
    getTerminalSize(w, h);
    Grid grid(w * 2, h * 4 - 8);
    Grid temp = grid;


    std::unordered_set<std::tuple<long, long>, Hash> updates;
    for (size_t j = 0; j < grid.height; j++) {
        for (size_t i = 0; i < grid.width; i++) { 
            updates.insert({i, j});
        }
    }

    std::unordered_set<std::tuple<long, long, bool, long>, Hash1> changes;
    for (size_t j = 0; j < grid.height; j++) {
        for (size_t i = 0; i < grid.width; i++) { 
            if (rand() % 2 == 0) changes.insert({i, j, 1, 0});
        }
    }

    for (;;) {
        for (auto [i,j,val,lastupd] : changes) {
            grid.grid[i][j] = {val, lastupd};
        }

        changes.clear();
        auto newUpdates = updates;
        updates.clear();

        getTerminalSize(w, h);
        grid.resize(w * 2, h * 4 - 8);
        
        for (auto [i, j] : newUpdates) {
            int neighbors = 0;

            if (grid.grid[(i+1) % grid.width][(j-1) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+1) % grid.width][(j+0) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+1) % grid.width][(j+1) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+0) % grid.width][(j-1) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+0) % grid.width][(j+1) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i-1) % grid.width][(j-1) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i-1) % grid.width][(j+0) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i-1) % grid.width][(j+1) % grid.height].state == 1) neighbors++;

            auto addneighbors = [&](){
                updates.insert({(i+1) % grid.width, (j-1) % grid.height});
                updates.insert({(i+1) % grid.width, (j+0) % grid.height});
                updates.insert({(i+1) % grid.width, (j+1) % grid.height});
                updates.insert({(i+0) % grid.width, (j-1) % grid.height});
                updates.insert({(i+0) % grid.width, (j+0) % grid.height});
                updates.insert({(i+0) % grid.width, (j+1) % grid.height});
                updates.insert({(i-1) % grid.width, (j-1) % grid.height});
                updates.insert({(i-1) % grid.width, (j+0) % grid.height});
                updates.insert({(i-1) % grid.width, (j+1) % grid.height});
            };

            if (grid.step - grid.grid[i][j].lastupd < 0 && false) {
                addneighbors();
                changes.insert({i, j, rand() % 2 != 0, grid.step});
            } else if (neighbors < 2 || neighbors > 3) {
                if (grid.grid[i][j].state == 1) {
                    addneighbors();
                    changes.insert({i, j, 0, grid.step});
                }
            } else if (neighbors == 3) { 
                if (grid.grid[i][j].state == 0) {
                    addneighbors();
                    changes.insert({i, j, 1, grid.step});
                }
            } else {
                double a = 5; // deviation (approx gap between midpoint and half way to 1 or 0. eg a=10, b=100 100 -> 50%, 110 -> 75%, 120 -> 87.5% etc)
                double b = 50; // switch point
                double prob = 1/(1+exp(-(1/a) * (grid.step - grid.grid[i][j].lastupd - b)));
                bool pass = prob > (double)rand()/INT_MAX;
                if (pass) {
                    addneighbors();
                    changes.insert({i, j, !grid.grid[i][j].state, grid.step});
                }
            }
        }

        grid.step++;
        grid.printBraille();
        // std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

}
