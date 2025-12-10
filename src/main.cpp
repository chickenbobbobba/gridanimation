#include <algorithm>
#include <array>
#include <bit>
#include <boost/unordered/unordered_flat_map_fwd.hpp>
#include <chrono>
#include <climits>
#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <future>
#include <iostream>
#include <ostream>
#include <string>
#include <string_view>
#include <thread>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <boost/unordered/unordered_flat_set.hpp>

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

    bool resize(size_t w, size_t h) {
        if (w == width && h == height) return 0;
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
        return 1;
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

    void printBraille(char live[], char mult[], long step, std::string& out) {
        std::ios::sync_with_stdio(false);
        std::setvbuf(stdout, nullptr, _IOFBF, BUFSIZ * 10);   

        out.clear();
        out += std::to_string(step);
        out.append("--live:");
        for (int i = 0; i < 9; i++) out += std::to_string((long)live[i]);
        out.append("--born:");
        for (int i = 0; i < 9; i++) out += std::to_string((long)mult[i]);
        auto temp = out.size();

        for (size_t i = temp; i < width/2-1; i++) out.append("-");

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

                double avg =    grid[j+0][i+0].lastupd +
                                grid[j+0][i+1].lastupd +
                                grid[j+0][i+2].lastupd +
                                grid[j+1][i+0].lastupd +
                                grid[j+1][i+1].lastupd +
                                grid[j+1][i+2].lastupd +
                                grid[j+0][i+3].lastupd +
                                grid[j+1][i+3].lastupd;

                avg = std::max(0.0, 8 * step - avg);
                avg /= 8;
                // int grayLevel = 232 + (long)(1.5*log(avg + 1)) % 24;
                int grayLevel = (long)(2*sqrt(avg)) % 256;

                static const std::array<std::string_view, 256> braille = {"⠀","⠁","⠂","⠃","⠄","⠅","⠆","⠇","⠈","⠉","⠊","⠋","⠌","⠍","⠎","⠏","⠐","⠑","⠒","⠓","⠔","⠕","⠖","⠗","⠘","⠙","⠚","⠛","⠜","⠝","⠞","⠟","⠠","⠡","⠢","⠣","⠤","⠥","⠦","⠧","⠨","⠩","⠪","⠫","⠬","⠭","⠮","⠯","⠰","⠱","⠲","⠳","⠴","⠵","⠶","⠷","⠸","⠹","⠺","⠻","⠼","⠽","⠾","⠿","⡀","⡁","⡂","⡃","⡄","⡅","⡆","⡇","⡈","⡉","⡊","⡋","⡌","⡍","⡎","⡏","⡐","⡑","⡒","⡓","⡔","⡕","⡖","⡗","⡘","⡙","⡚","⡛","⡜","⡝","⡞","⡟","⡠","⡡","⡢","⡣","⡤","⡥","⡦","⡧","⡨","⡩","⡪","⡫","⡬","⡭","⡮","⡯","⡰","⡱","⡲","⡳","⡴","⡵","⡶","⡷","⡸","⡹","⡺","⡻","⡼","⡽","⡾","⡿","⢀","⢁","⢂","⢃","⢄","⢅","⢆","⢇","⢈","⢉","⢊","⢋","⢌","⢍","⢎","⢏","⢐","⢑","⢒","⢓","⢔","⢕","⢖","⢗","⢘","⢙","⢚","⢛","⢜","⢝","⢞","⢟","⢠","⢡","⢢","⢣","⢤","⢥","⢦","⢧","⢨","⢩","⢪","⢫","⢬","⢭","⢮","⢯","⢰","⢱","⢲","⢳","⢴","⢵","⢶","⢷","⢸","⢹","⢺","⢻","⢼","⢽","⢾","⢿","⣀","⣁","⣂","⣃","⣄","⣅","⣆","⣇","⣈","⣉","⣊","⣋","⣌","⣍","⣎","⣏","⣐","⣑","⣒","⣓","⣔","⣕","⣖","⣗","⣘","⣙","⣚","⣛","⣜","⣝","⣞","⣟","⣠","⣡","⣢","⣣","⣤","⣥","⣦","⣧","⣨","⣩","⣪","⣫","⣬","⣭","⣮","⣯","⣰","⣱","⣲","⣳","⣴","⣵","⣶","⣷","⣸","⣹","⣺","⣻","⣼","⣽","⣾","⣿"};
                long idx = base & 0xFF;

                if (base == 0) {
                    out.append("\033[38;2;" + std::to_string(grayLevel) + ";" + std::to_string(grayLevel) + ";" + std::to_string(grayLevel) + "m"); // set foreground
                    out.append("\033[48;2;" + std::to_string(grayLevel) + ";" + std::to_string(grayLevel) + ";" + std::to_string(grayLevel) + "m"); // set background
                    out.append("#\033[0m");
                } else {
                    out.append("\033[38;2;" + std::to_string(255-grayLevel) + ";" + std::to_string(255-grayLevel) + ";" + std::to_string(255-grayLevel) + "m");
                    out.append("\033[48;2;" + std::to_string(grayLevel) + ";" + std::to_string(grayLevel) + ";" + std::to_string(grayLevel) + "m");
                    out.append(braille[idx]);
                    out.append("\033[0m");

                }   
            }
            out.append("\n");
        }
        for (size_t i = 0; i < width/2; i++) out.append("-");
        out.append("\033[0m");
        std::cout << "\033[?2026h"; // Enable synchronized update
        std::cout << "\033[1;1H";
        std::cout.write(out.data(), out.size());
        std::cout << "\033[0m";
        std::cout << "\033[?2026l"; // Disable + flush + render atomically
        std::cout.flush();   
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
        return (std::get<1>(t)<<32) + (hash(std::get<0>(t)) & 0xFFFF);
    }
};

// Hash for tuple<long, long, bool, long>
struct Hash1 {
    std::size_t operator()(const std::tuple<long, long, bool, long>& t) const {
        return (std::get<1>(t)<<32) + (hash(std::get<0>(t)) & 0xFFFF);
    }
};

int main(int argc, char** argv){

    char live[9] = {0,0,1,1,0,0,0,0,0};
    char mult[9] = {0,0,0,1,0,0,0,0,0};
    if (argc > 3) {
        memcpy(&live, argv[3], 9);
        memcpy(&mult, argv[4], 9);
        for (int i = 0 ; i < 9; i++) {
            live[i] -= 48;
        }
        for (int i = 0 ; i < 9; i++) {
            mult[i] -= 48;
        }
    }

    srand(time(NULL));

    int w;
    int h;
    getTerminalSize(w, h);
    Grid grid(w * 2, h * 4 - 8);
    Grid temp = grid;


    boost::unordered_flat_set<std::tuple<long, long>, Hash> updates;
    boost::unordered_flat_set<std::tuple<long, long, bool, long>, Hash1> changes;
    for (size_t j = 0; j < grid.height; j++) {
        for (size_t i = 0; i < grid.width; i++) { 
            if (argv[1][0] == '2' && rand() % 20 == 0) changes.insert({i, j, 1, 0});
        }
    }

    for (size_t j = 0; j < grid.height; j++) {
        for (size_t i = 0; i < grid.width; i++) { 
            updates.insert({i, j});
        }
    }


    using namespace std::chrono;
    using namespace std::chrono_literals;
    double framerate = 1.0 / std::stod(argv[2]); // Use stod for double
    auto next = std::chrono::steady_clock::now() + std::chrono::duration<double>(framerate);
    bool skip = false;

    std::thread printThread;

    for (;;) {
        grid.step++;

        getTerminalSize(w, h);
        grid.resize(w * 2, h * 4 - 8);

        for (auto [i,j,val,lastupd] : changes) {
            if (i >= w * 2 || j >= h * 4 - 8) {
                continue;
            }
            grid.grid[i][j] = {val, grid.step};
        }

        changes.clear();

        boost::unordered_flat_set<std::tuple<long,long>, Hash> newUpdates;
        newUpdates.swap(updates);

        updates.clear();
        
        if (newUpdates.size() == 0 || skip) {
            for (int i = 0; i < sqrt(w * h) * 3; i++) {
                long a = std::max(0, rand() % (w * 2));
                long b = std::max(0, rand() % (h * 4 - 8));
                changes.insert({a, b, 1, 0});
                updates.insert({a, b});
            }
            if (newUpdates.size() < 64) skip = true;
            else skip = false;
            continue;
        }

        std::this_thread::sleep_until(next);
        next += std::chrono::duration<double>(framerate);

        std::string out;
        out.reserve(w * h * 8);

        static std::future<void> printFuture;

        if (!printFuture.valid() || printFuture.wait_for(0ms) == std::future_status::ready) {

            printFuture = std::async(std::launch::async, [=, frame = std::move(out)]() mutable {
                try {
                    grid.printBraille(live, mult, grid.step, frame);
                } catch (...) {

                }
            });
        }

        for (auto [i, j] : newUpdates) {
            if (i >= w * 2 || j >= h * 4 - 8) {
                continue;
            }
            int neighbors = 0;

            if (grid.grid[(i+1+grid.width) % grid.width][(j-1+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+1+grid.width) % grid.width][(j+0+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+1+grid.width) % grid.width][(j+1+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+0+grid.width) % grid.width][(j-1+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i+0+grid.width) % grid.width][(j+1+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i-1+grid.width) % grid.width][(j-1+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i-1+grid.width) % grid.width][(j+0+grid.height) % grid.height].state == 1) neighbors++;
            if (grid.grid[(i-1+grid.width) % grid.width][(j+1+grid.height) % grid.height].state == 1) neighbors++;

            auto addneighbors = [&](){
                updates.insert({(i+1+grid.width) % grid.width, (j-1+grid.height) % grid.height});
                updates.insert({(i+1+grid.width) % grid.width, (j+0+grid.height) % grid.height});
                updates.insert({(i+1+grid.width) % grid.width, (j+1+grid.height) % grid.height});
                updates.insert({(i+0+grid.width) % grid.width, (j-1+grid.height) % grid.height});
                updates.insert({(i+0+grid.width) % grid.width, (j+0+grid.height) % grid.height});
                updates.insert({(i+0+grid.width) % grid.width, (j+1+grid.height) % grid.height});
                updates.insert({(i-1+grid.width) % grid.width, (j-1+grid.height) % grid.height});
                updates.insert({(i-1+grid.width) % grid.width, (j+0+grid.height) % grid.height});
                updates.insert({(i-1+grid.width) % grid.width, (j+1+grid.height) % grid.height});
            };


            if (!live[neighbors]) {
                if (grid.grid[i][j].state == 1) {
                    addneighbors();
                    changes.insert({i, j, 0, grid.step});
                }
            } else if (mult[neighbors]) { 
                if (grid.grid[i][j].state == 0) {
                    addneighbors();
                    changes.insert({i, j, 1, grid.step});
                }
            } else {
                if (argv[1][0] == '2') {
                    double a = 100; // deviation
                    double b = 4000; // switch point
                    double prob = 1/(1+exp(-(1/a) * (grid.step - grid.grid[i][j].lastupd - b)));
                    bool pass = prob > (double)rand()/INT_MAX;
                    if (pass) {
                        changes.insert({i, j, !grid.grid[i][j].state, grid.step});
                        updates.insert({i, j});
                    }
                }
            }
        }

        if (argv[1][0] == '1') {
            for (size_t j = 0; j < grid.height; j++) {
                for (size_t i = 0; i < grid.width; i++) {         
                    double a = 1; // deviation (approx gap between midpoint and half way to 1 or 0. eg a=10, b=100 100 -> 50%, 110 -> 75%, 120 -> 87.5% etc)
                    double b = 1200; // switch point
                    double prob = 1/(1+exp(-(1/a) * (grid.step - grid.grid[i][j].lastupd - b)));
                    bool pass = prob > (double)rand()/INT_MAX;
                    if (pass) {
                        changes.insert({i, j, !grid.grid[i][j].state, grid.step});
                        updates.insert({i, j});
                    }
                }
            }
        }
    }
}
