#include "game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string>

Game::Game(int width, int height) {
    m_width = width;
    m_height = height;

    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    m_window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, m_width * CELL_SIZE, m_height * CELL_SIZE, SDL_WINDOW_SHOWN);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED);

    m_covered_texture = IMG_LoadTexture(m_renderer, "./assets/covered.png");
    m_uncovered_texture = IMG_LoadTexture(m_renderer, "./assets/uncovered.png");
    m_flag_texture = IMG_LoadTexture(m_renderer, "./assets/flag.png");
    m_mine_texture = IMG_LoadTexture(m_renderer, "./assets/mine.png");

    for (int i = 1; i <= 8; i++) {
        m_number_textures[i - 1] = IMG_LoadTexture(m_renderer, ("./assets/" + std::to_string(i) + ".png").c_str());
    }

    for (int i = 0; i < m_width * m_height; i++) {
        m_cells.push_back({
            .mine = false,
            .uncovered = false,
            .flagged = false
        });
    }
}

void Game::mainloop() {
    while (update()) {
        draw();
    }
}

bool Game::update() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
    }

    return true;
}

void Game::draw() {
    SDL_SetRenderDrawColor(m_renderer, 255, 255, 0, 255);
    SDL_RenderClear(m_renderer);

    for (int x = 0; x < m_width; x++) {
        for (int y = 0; y < m_height; y++) {
            SDL_Rect cell_rect = {
                .x = x * CELL_SIZE,
                .y = y * CELL_SIZE,
                .w = CELL_SIZE,
                .h = CELL_SIZE
            };

            SDL_RenderCopy(m_renderer, m_covered_texture, nullptr, &cell_rect);
        }
    }

    SDL_RenderPresent(m_renderer);
}
