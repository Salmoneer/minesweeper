#include "game.hpp"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>
#include <string>

Game::Game(int width, int height, int mines) :
    m_width(width),
    m_height(height),
    m_mines(mines),
    m_mines_generated(false)
{
    const int grid_width = m_width * IMG_SIZE;
    const int grid_height = m_height * IMG_SIZE;

    const int window_width = grid_width * WINDOW_SCALE;
    const int window_height = grid_height * WINDOW_SCALE;


    SDL_Init(SDL_INIT_EVERYTHING);
    IMG_Init(IMG_INIT_PNG);

    m_window = SDL_CreateWindow("Minesweeper", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, window_width, window_height, SDL_WINDOW_SHOWN);
    m_renderer = SDL_CreateRenderer(m_window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);

    m_target = SDL_CreateTexture(m_renderer, SDL_PIXELFORMAT_RGB888, SDL_TEXTUREACCESS_TARGET, grid_width, grid_height);

    m_covered_texture = IMG_LoadTexture(m_renderer, "./assets/covered.png");
    m_uncovered_texture = IMG_LoadTexture(m_renderer, "./assets/uncovered.png");
    m_flag_texture = IMG_LoadTexture(m_renderer, "./assets/flag.png");
    m_mine_texture = IMG_LoadTexture(m_renderer, "./assets/mine.png");

    for (int i = 0; i < 8; i++) {
        m_number_textures[i] = IMG_LoadTexture(m_renderer, ("./assets/" + std::to_string(i + 1) + ".png").c_str());
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
        } else if (event.type == SDL_MOUSEBUTTONDOWN) {
            int index = get_index(event.button.x / (IMG_SIZE * WINDOW_SCALE), event.button.y / (IMG_SIZE * WINDOW_SCALE));

            if (event.button.button == SDL_BUTTON_LEFT) {
                if (!m_cells[index].flagged) {
                    if (!m_mines_generated) {
                        generate_mines(index);
                        count_all_adjacent_mines();
                    }

                    m_cells[index].uncovered = true;
                }
            } else if (event.button.button == SDL_BUTTON_RIGHT) {
                if (!m_cells[index].uncovered) {
                    m_cells[index].flagged = !m_cells[index].flagged;
                }
            }
        }
    }

    return true;
}

void Game::draw() {
    SDL_SetRenderDrawColor(m_renderer, 255, 0, 255, 255);
    SDL_RenderClear(m_renderer);
    SDL_SetRenderTarget(m_renderer, m_target);
    SDL_RenderClear(m_renderer);

    for (int x = 0; x < m_width; x++) {
        for (int y = 0; y < m_height; y++) {
            SDL_Rect cell_rect = {
                .x = x * IMG_SIZE,
                .y = y * IMG_SIZE,
                .w = IMG_SIZE,
                .h = IMG_SIZE
            };

            int index = get_index(x, y);
            Cell current_cell = m_cells[index];

            if (current_cell.uncovered && current_cell.flagged) {
                std::cout << "x: " << x << ", y: " << y << "\n";
                throw std::runtime_error("Uncovered and flagged cell found");
            }

            SDL_Texture *current_cell_texture;
            if (!current_cell.uncovered && !current_cell.flagged) {
                current_cell_texture = m_covered_texture;
            } else if (!current_cell.uncovered && current_cell.flagged) {
                current_cell_texture = m_flag_texture;
            } else if (current_cell.uncovered && !current_cell.mine) {
                current_cell_texture = m_uncovered_texture;
            } else if (current_cell.uncovered && current_cell.mine) {
                current_cell_texture = m_mine_texture;
            } else {
                throw std::runtime_error("Unhandled case in choosing texture to render");
            }

            SDL_RenderCopy(m_renderer, current_cell_texture, nullptr, &cell_rect);
        }
    }

    SDL_SetRenderTarget(m_renderer, nullptr);
    SDL_RenderCopy(m_renderer, m_target, nullptr, nullptr);

    SDL_RenderPresent(m_renderer);

    SDL_Delay(1000 / 60);
}

int Game::get_index(int x, int y) {
    if (x < 0 || x >= m_width) {
        std::cout << "x: " << x << ", y: " << y << "\n";
        throw std::runtime_error("X out of range while getting index");
    }

    if (y < 0 || y >= m_height) {
        std::cout << "x: " << x << ", y: " << y << "\n";
        throw std::runtime_error("Y out of range while getting index");
    }

    int index = x + m_width * y;

    if (index < 0 || index >= m_width * m_height) {
        std::cout << "x: " << x << ", y: " << y << ", index: " << index << "\n";
        throw std::runtime_error("Index out of range while getting index");
    }

    return index;
}

void Game::generate_mines(int exclude_index) {
    if (m_mines_generated) {
        throw std::runtime_error("generate_mines called twice");
    }

    m_mines_generated = true;


    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> random_gen(0, m_width * m_height - 1);

    std::set<int> mine_indices;

    while (mine_indices.size() < m_mines) {
        int random_index = random_gen(gen);

        if (random_index != exclude_index) {
            mine_indices.insert(random_gen(gen));
        }
    }

    for (int index : mine_indices) {
        m_cells[index].mine = true;
    }
}

int Game::count_cell_adjacent_mines(int index) {
    int count = 0;

    for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
            if (dx == 0 && dy == 0) {
                continue;
            }

            int x = index % m_width + dx;
            int y = index / m_width + dy;

            if (x < 0 || x >= m_width || y < 0 || y >= m_height) {
                continue;
            }

            int compare_index = get_index(x, y);

            if (m_cells[compare_index].mine) {
                count++;
            }
        }
    }

    return count;
}

void Game::count_all_adjacent_mines() {
    if (m_adjacent_mines.size() != 0) {
        throw std::runtime_error("Adjacent mines counted twice");
    }

    for (int i = 0; i < m_cells.size(); i++) {
        m_adjacent_mines.push_back(count_cell_adjacent_mines(i));
    }

    if (m_adjacent_mines.size() != m_cells.size()) {
        throw std::runtime_error("Incorrect number of cells had their adjacent mines calculated");
    }
}
