#include <SDL2/SDL.h>
#include <array>
#include <vector>

#define IMG_SIZE 16
#define WINDOW_SCALE 4

enum class Direction {
    Left,
    Up,
    Right,
    Down,
};

enum class GameState {
    Playing,
    Lose,
    Win,
};

const std::array<std::pair<int, int>, 4> direction_deltas = {{
    {-1,  0},
    { 0, -1},
    { 1,  0},
    { 0,  1},
}};

struct Cell {
    bool mine;
    bool uncovered;
    bool flagged;
};

class Game {
private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;

    SDL_Texture *m_target;

    SDL_Texture *m_covered_texture;
    SDL_Texture *m_uncovered_texture;
    SDL_Texture *m_flag_texture;
    SDL_Texture *m_mine_texture;
    std::array<SDL_Texture *, 8> m_number_textures;

    SDL_Texture *m_win_texture;
    SDL_Texture *m_lose_texture;


    int m_width;
    int m_height;
    int m_mines;

    std::vector<Cell> m_cells;

    bool m_mines_generated;
    std::vector<int> m_adjacent_mines;

    GameState m_state;


    int get_index(int x, int y);

    void generate_mines(int exclude_index);

    int count_cell_adjacent_mines(int index);
    void count_all_adjacent_mines();

    SDL_Texture *get_cell_texture(int index);

    std::vector<int> get_neighbors(int index);
    void uncover(int index);

public:
    Game() = delete;
    Game(int width, int height, int mines);

    void mainloop();

    bool update();
    void draw();
};
