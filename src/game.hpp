#include <SDL2/SDL.h>
#include <array>
#include <vector>

#define CELL_SIZE 16

struct Cell {
    bool mine;
    bool uncovered;
    bool flagged;
};

class Game {
private:
    SDL_Window *m_window;
    SDL_Renderer *m_renderer;

    SDL_Texture *m_covered_texture;
    SDL_Texture *m_uncovered_texture;
    SDL_Texture *m_flag_texture;
    SDL_Texture *m_mine_texture;
    std::array<SDL_Texture *, 8> m_number_textures;


    int m_width;
    int m_height;

    std::vector<Cell> m_cells;

public:
    Game() = delete;
    Game(int width, int height);

    void mainloop();

    bool update();
    void draw();
};
