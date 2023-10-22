/**
* Author: Tahmid Asif
* Assignment: Pong Clone
* Date due: 2023-10-21, 11:59pm
* I pledge that I have completed this assignment without
* collaborating with anyone else, in conformance with the
* NYU School of Engineering Policies and Procedures on
* Academic Misconduct.
**/

#define GL_SILENCE_DEPRECATION
#define STB_IMAGE_IMPLEMENTATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include "stb_image.h"

#include <iostream>
#include <time.h>
using namespace std;

SDL_Window* display_window;
ShaderProgram g_shader_program;

const char PLAYER_SPRITE1_FILEPATH[] = "/Users/tahmidasif/Desktop/Pong/Project2Pong/Proj2Pong/Proj2Pong/paddle.png";
const char PLAYER_SPRITE2_FILEPATH[] = "/Users/tahmidasif/Desktop/Pong/Project2Pong/Proj2Pong/Proj2Pong/ball.png";
const char V_SHADER_PATH[] = "shaders/vertex_textured.glsl",
           F_SHADER_PATH[] = "shaders/fragment_textured.glsl";

bool g_game_is_running = true;
int winner = -1;
int paddle1_score, paddle2_score = 0;

void print_score() {
    cout << "\n````````````````````````````````\n";
    cout << "\t\t P1: " << paddle1_score << "\t";
    cout << "P2: " << paddle2_score << "\n";
    cout << "````````````````````````````````\n\n";
}

// ------------------- INITIALIZATION OF ONSCREEN ITEMS -------------------
// Initialize Matricies for paddles + ball
glm::mat4 view_matrix, m_projection_matrix;
glm::mat4 m_model_matrix_paddle1, m_model_matrix_paddle2, m_model_matrix_ball;

// Ball Initalization
glm::vec3 ball_pos = glm::vec3(0, 0, 0);
glm::vec3 ball_move = glm::vec3(0, 0, 0);
glm::vec3 ball_scale = glm::vec3(0.35f, 0.35f, 1.0f);
float ball_height = 1.0f * ball_scale.y;
float ball_width = 1.0f * ball_scale.x;

float ball_speed = 4.5f;
float ball_rotate = 1.0f;
bool ball_path_reversed = false;

// Paddle 1 Initalization
glm::vec3 paddle1_pos = glm::vec3(-4.75, 0, 0);
glm::vec3 paddle1_move = glm::vec3(0, 0, 0);
float paddle1_speed = 3.85f;

// Paddle 2 Initalization
glm::vec3 paddle2_pos = glm::vec3(4.75, 0, 0);
glm::vec3 paddle2_move = glm::vec3(0, 0, 0);
float paddle2_speed = 3.85f;

// Both Paddles
glm::vec3 paddle_scale = glm::vec3(0.35f, 2.55f, 1.0f);
float paddle_height = 1.0f * paddle_scale.y;
float paddle_width = 1.0f * paddle_scale.x;

// ------------------ /INITIALIZATION OF ONSCREEN ITEMS -------------------

// ------------------------------- TEXTURES -------------------------------
GLuint paddleTextureID;
GLuint ballTextureID;

GLuint load_texture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
    }
    
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    
    stbi_image_free(image);
    
    return textureID;
}
// ------------------------------ /TEXTURES -------------------------------

void initialise() {
    SDL_Init(SDL_INIT_VIDEO);
    display_window = SDL_CreateWindow("Pong Game!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(display_window);
    SDL_GL_MakeCurrent(display_window, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    g_shader_program.load(V_SHADER_PATH, F_SHADER_PATH);
    
    view_matrix = glm::mat4(1.0f);
    m_model_matrix_ball = glm::mat4(1.0f);
    m_projection_matrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    g_shader_program.set_projection_matrix(m_projection_matrix);
    g_shader_program.set_view_matrix(view_matrix);
    
    glUseProgram(g_shader_program.get_program_id()); //is it get_program_id or m_program_id
    
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    paddleTextureID = load_texture(PLAYER_SPRITE1_FILEPATH);
    ballTextureID = load_texture(PLAYER_SPRITE2_FILEPATH);
}

bool ceiling(glm::vec3 position, float height, float top) {
    return !((position.y + (height / 2.0f)) < top);
}

bool floor(glm::vec3 position, float height, float bottom) {
    return !((position.y - (height / 2.0f)) > bottom);
}

bool onePlayerMode = false;

void process_input() {
    paddle1_move = glm::vec3(0); // so that if nothing is pressed, we don't go anywhere
    paddle2_move = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                g_game_is_running = false;
                break;
                
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_LEFT:
                        break;
                    case SDLK_RIGHT:
                        break;
                    case SDLK_SPACE:
                        break;
                    case SDLK_t:
                        onePlayerMode = !onePlayerMode;
                        break;
                }
                break; // SDL_KEYDOWN
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    // ball
    if (keys[SDL_SCANCODE_SPACE]) {
        srand(time(0));                     // random seed
        int slope = (rand() % 3) - 1;       // spice up the initial throw so it's not the same every time
        ball_move.x = 1.0f * slope;
        ball_move.y = 1.0f * slope;
    }
    if (glm::length(ball_move) > 1.0f) {
        ball_move = glm::normalize(ball_move);
    }
    
    if (!onePlayerMode) {
        // paddle_1
        if ((keys[SDL_SCANCODE_W]) && !ceiling(paddle1_pos, paddle_height, 3.7f)) {
            paddle1_move.y = 1.0f;
        }
        else if ((keys[SDL_SCANCODE_S]) && !floor(paddle1_pos, paddle_height, -3.7f)) {
            paddle1_move.y = -1.0f;
        }
        if (glm::length(paddle1_move) > 1.0f) {
            paddle1_move = glm::normalize(paddle1_move);
        }
    }
    // paddle_2
    if ((keys[SDL_SCANCODE_UP]) && !ceiling(paddle2_pos, paddle_height, 3.7f)) {
        paddle2_move.y = 1.0f;
    }
    else if ((keys[SDL_SCANCODE_DOWN]) && !floor(paddle2_pos, paddle_height, -3.7f)) {
        paddle2_move.y = -1.0f;
    }
    if (glm::length(paddle2_move) > 1.0f) {
        paddle2_move = glm::normalize(paddle2_move);
    }
}

bool colliding(glm::vec3 ball_pos, glm::vec3 p_position) {
    float x1 = ball_pos.x;
    float y1 = ball_pos.y;
    float w1 = ball_width - 0.2f;
    float h1 = ball_height - 0.2f;

    float x2 = p_position.x;
    float y2 = p_position.y;
    float w2 = paddle_width;
    float h2 = paddle_height;

    float x_diff = fabs(x2 - x1);
    float y_diff = fabs(y2 - y1);

    float x_dist = x_diff - ((w1 + w2) / 2);
    float y_dist = y_diff - ((h1 + h2) / 2);

    return ((x_dist < 0) && (y_dist < 0));
}

void update_ball(float deltaTime) {
    // ---------------------------- ball ----------------------------
    m_model_matrix_ball = glm::mat4(1.0f);
    
    if ((ceiling(ball_pos, ball_height, 3.7f) || floor(ball_pos, ball_height, -3.7f))) {
//        cout << "Touching a wall!\n";
        ball_move.y *= -1.0f;
    }
    else if ((colliding(ball_pos, paddle1_pos)) || (colliding(ball_pos, paddle2_pos))) {
//        cout << "Touching a paddle! \n";
//        cout << "Ball: (" << ball_pos.x << ", " << ball_pos.y << ") \n";
//        cout << "Pad1: (" << paddle1_pos.x << ", " << paddle1_pos.y << ") \n";
//        cout << "Pad2: (" << paddle2_pos.x << ", " << paddle2_pos.y << ") \n\n";
        ball_move.x *= -1.0f;
    }
    
    ball_pos += ball_move * ball_speed * deltaTime;
    m_model_matrix_ball = glm::translate(m_model_matrix_ball, ball_pos);
    m_model_matrix_ball = glm::scale(m_model_matrix_ball, ball_scale);
    // --------------------------- /ball ----------------------------
}

void update_paddle1(float deltaTime) {
    // ---------------------------- pad1 ----------------------------
    paddle1_pos += paddle1_move * paddle1_speed * deltaTime;
    m_model_matrix_paddle1 = glm::mat4(1.0f);
    m_model_matrix_paddle1 = glm::translate(m_model_matrix_paddle1, paddle1_pos);
    m_model_matrix_paddle1 = glm::scale(m_model_matrix_paddle1, paddle_scale);
    // --------------------------- /pad1 ----------------------------
}

void update_paddle2(float deltaTime) {
    // ---------------------------- pad2 ----------------------------
    paddle2_pos += paddle2_move * paddle2_speed * deltaTime;
    m_model_matrix_paddle2 = glm::mat4(1.0f);
    m_model_matrix_paddle2 = glm::translate(m_model_matrix_paddle2, paddle2_pos);
    m_model_matrix_paddle2 = glm::scale(m_model_matrix_paddle2, paddle_scale);
    // --------------------------- /pad2 ----------------------------
}

bool ball_on_walls(glm::vec3 ball_pos) {
    float right = 4.8f;
    float left = -4.8f;
    
    if (ball_pos.x > right) {
        paddle1_score += 1;
        if (paddle1_score == 3){
            winner = 1;
            g_game_is_running = false;
        }
        
        cout << "\n````````````````````````````````\n";
        cout << "\t\tCurrent Scores: \n";
        print_score();
        
        return true;
    }
    else if (ball_pos.x < left) {
        paddle2_score += 1;
        if (paddle2_score == 3) {
            winner = 2;
            g_game_is_running = false;
        }
        cout << "\n````````````````````````````````\n";
        cout << "\t\tCurrent Scores: \n";
        print_score();
        
        return true;
    }
    return false;
}

float m_previous_ticks = 0.0f;

void update() {
    if (ball_on_walls(ball_pos)) {
        if ((paddle1_score == 3) || (paddle2_score == 3))
            g_game_is_running = false;
        else
            ball_pos = glm::vec3(0, 0, 0);
    }
    
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - m_previous_ticks;
    m_previous_ticks = ticks;
    
    update_ball(deltaTime);
    update_paddle1(deltaTime);
    update_paddle2(deltaTime);
}

void render_object(glm::mat4 m_model_matrix, GLuint textureID) { //modelMatrix or  m_model_matrix_ball?
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float texture_coordinates[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    g_shader_program.set_model_matrix(m_model_matrix);
    
    glVertexAttribPointer(g_shader_program.get_position_attribute(), 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(g_shader_program.get_position_attribute());
    glVertexAttribPointer(g_shader_program.get_tex_coordinate_attribute(), 2, GL_FLOAT, false, 0, texture_coordinates);
    glEnableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(g_shader_program.get_position_attribute());
    glDisableVertexAttribArray(g_shader_program.get_tex_coordinate_attribute());
}

void render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    render_object(m_model_matrix_ball, ballTextureID);
    render_object(m_model_matrix_paddle1, paddleTextureID);
    render_object(m_model_matrix_paddle2, paddleTextureID);
    
    SDL_GL_SwapWindow(display_window);
}

void shutdown() {
    cout << "\n================================\n";
    cout << "\t\t Player " << winner << " wins!\n";
    cout << "\t\t Final Scores:\n";
    print_score();
    cout << " Thanks for playing, game over!\n";
    cout << "================================\n\n";
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    initialise();
    
    while (g_game_is_running) {
        process_input();
        update();
        render();
    }
    
    shutdown();
    return 0;
}

