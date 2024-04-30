#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <conio.h>
#include <cmath>

using namespace std;

static string file_name = "2048.data";

string get_centered_string(const string &text, int length) {
    int text_length = text.length();
    if (text_length >= length) {
        return text;
    }
    int right_length = (length-text_length)/2;
    int left_length = length-text_length-right_length;
    string right(right_length, ' ');
    string left(left_length, ' ');
    return left+text+right;
}

string get_colored_text(const string &text, int color) {
    string color_code;
    switch (color%6) {
        case 0:
            color_code = "\033[1;41m"; // Kırmızı
            break;
        case 1:
            color_code = "\033[1;42m"; // Yeşil
            break;
        case 2:
            color_code = "\033[1;44m"; // Mavi
            break;
        case 3:
            color_code = "\033[1;43m"; // Sarı
            break;
        case 4:
            color_code = "\033[1;45m"; // Pembe
            break;
        case 5:
            color_code = "\033[1;46m"; // Turkuaz
            break;
        default:
            color_code = "\033[0m";
            break;
    }
    return color_code+text+"\033[0m";
}

class Field {
private:
    int grid[4][4];
public:
    Field() {
        clear();
    }
    void clear() {
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                grid[x][y] = 0;
            }
        }
    }
    void save() {
        ofstream file(file_name);
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                file << (char)grid[x][y];
            }
        }
        file.close();
    }
    bool load() {
        ifstream file(file_name);
        if (!file) {
            return false;
        }
        char data;
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                file.read(&data, 1);
                grid[x][y] = (int)data;
            }
        }
        file.close();
        return true;
    }
    Field copy() {
        Field copy;
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                copy.grid[x][y] = grid[x][y];
            }
        }
        return copy;
    }
    bool operator==(Field other) {
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                if (other.grid[x][y] != grid[x][y]) {
                    return false;
                }
            }
        }
        return true;
    }
    bool operator!=(Field other) {
        return !(other == *this);
    }
    bool is_full() {
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                if (grid[x][y] == 0) {
                    return false;
                }
            }
        }
        return true;
    }
    bool is_finished() {
        for (int x=0; x<4; ++x) {
            for (int y=0; y<4; ++y) {
                if (grid[x][y] == 0) {
                    return false;
                }
                if (y < 3 && grid[x][y] == grid[x][y+1]) {
                    return false;
                }
                if (x < 3 && grid[x][y] == grid[x+1][y]) {
                    return false;
                }
            }
        }
        return true;
    }
    void spawn_new() {
        int x, y;
        do {
            x = rand()%4;
            y = rand()%4;
        } while (grid[x][y] != 0);
        if (rand()%100 < 10) {
            grid[x][y] = 2;
        } else {
            grid[x][y] = 1;
        }
    }
    void move_horizontal(int dx) {
        for (int y=0; y<4; ++y) {
            for (int i=0; i<3; ++i) {
                for (int xx=0; xx<3; ++xx) {
                    int x = xx;
                    if (dx == 1) x = 3-xx;
                    if (grid[x][y] == 0) {
                        grid[x][y] = grid[x-dx][y];
                        grid[x-dx][y] = 0;
                    }
                }
            }
            for (int xx=0; xx<3; ++xx) {
                int x = xx;
                if (dx == 1) x = 3-xx;
                if (grid[x][y] != 0 && grid[x][y] == grid[x-dx][y]) {
                    grid[x][y]++;
                    grid[x-dx][y] = 0;
                }
            }
            for (int xx=0; xx<3; ++xx) {
                int x = xx;
                if (dx == 1) x = 3-xx;
                if (grid[x][y] == 0) {
                    grid[x][y] = grid[x-dx][y];
                    grid[x-dx][y] = 0;
                }
            }
        }
    }
    void move_vertical(int dy) {
        for (int x=0; x<4; ++x) {
            for (int i=0; i<3; ++i) {
                for (int yy=0; yy<3; ++yy) {
                    int y = yy;
                    if (dy == 1) y = 3-yy;
                    if (grid[x][y] == 0) {
                        grid[x][y] = grid[x][y-dy];
                        grid[x][y-dy] = 0;
                    }
                }
            }
            for (int yy=0; yy<3; ++yy) {
                int y = yy;
                if (dy == 1) y = 3-yy;
                if (grid[x][y] != 0 && grid[x][y] == grid[x][y-dy]) {
                    grid[x][y]++;
                    grid[x][y-dy] = 0;
                }
            }
            for (int yy=0; yy<3; ++yy) {
                int y = yy;
                if (dy == 1) y = 3-yy;
                if (grid[x][y] == 0) {
                    grid[x][y] = grid[x][y-dy];
                    grid[x][y-dy] = 0;
                }
            }
        }
    }
    void draw() {
        system("cls");
        for (int y=0; y<4; ++y) {
            cout << "                                     " << endl;
            cout << " ";
            for (int x=0; x<4; ++x) {
                if (grid[x][y] == 0) {
                    cout << "\033[100m        \033[0m ";
                } else {
                    cout << get_colored_text("        ", grid[x][y]) << " ";
                }
            }
            cout << endl;
            cout << " ";
            for (int x=0; x<4; ++x) {
                if (grid[x][y] == 0) {
                    cout << "\033[100m        \033[0m ";
                } else {
                    cout << get_colored_text(get_centered_string(to_string((int)pow(2, grid[x][y])), 8), grid[x][y]) << " ";
                }
            }
            cout << endl;
            cout << " ";
            for (int x=0; x<4; ++x) {
                if (grid[x][y] == 0) {
                    cout << "\033[100m        \033[0m ";
                } else {
                    cout << get_colored_text("        ", grid[x][y]) << " ";
                }
            }
            cout << endl;
        }
        cout << "                                     " << endl;
    }
};

int main(void) {
    srand(time(0));
    Field field;
    bool new_game = false;
    if (field.load() == false) {
        new_game = true;
    } else if (field.is_finished()) {
        new_game = true;
    }
    if (new_game == false) {
        bool selecting = true;
        while (selecting) {
            system("cls");
            cout << (new_game == false ? "\033[1m --> " : "\033[90m     ") << "Continue\033[0m" << endl;
            cout << (new_game == true  ? "\033[1m --> " : "\033[90m     ") << "New Game\033[0m" << endl;
            switch (_getch()) {
                case '\e': {
                    return 0;
                }
                case ' ': {
                    selecting = false;
                    break;
                }
                case 'w':
                case 's': {
                    new_game = !new_game;
                    break;
                }
            }
        }
    }
    if (new_game) {
        field.clear();
        field.spawn_new();
        field.save();
    }
    bool running = true;
    while (running) {
        field.draw();
        Field copy = field.copy();
        switch (_getch()) {
            case '\e': {
                running = false;
                break;
            }
            case 'd': {
                field.move_horizontal(1);
                break;
            }
            case 'a': {
                field.move_horizontal(-1);
                break;
            }
            case 's': {
                field.move_vertical(1);
                break;
            }
            case 'w': {
                field.move_vertical(-1);
                break;
            }
        }
        if (field.is_finished()) {
            running = false;
            field.draw();
            cout << "Game Over" << endl;
            cout << "Press enter to exit" << endl;
            string a;
            cin >> a;
        }
        if (copy != field) {
            if (!field.is_full()) {
                field.spawn_new();
            }
            field.save();
        }
    }
    return 0;
}
