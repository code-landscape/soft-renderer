
#include "Buffer.hpp"
#include "Camera.hpp"
#include "Hittable.hpp"
#include "Renderer.hpp"
#include "glm/ext/scalar_uint_sized.hpp"
#include <SDL3/SDL_gpu.h>
#include <SDL3/SDL_rect.h>
#include <SDL3/SDL_render.h>
#include <SDL3/SDL_video.h>
#include <cstddef>
#include <cstdint>
#define SDL_MAIN_USE_CALLBACKS 1
#include "backends/imgui_impl_sdl3.h"
#include "backends/imgui_impl_sdlrenderer3.h"
#include "imgui.h"
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <stdio.h>

// CONSTANCE

SDL_Window *g_Window;
SDL_Renderer *g_Renderer;
ImVec4 clear_color;
ImGuiIO *g_Io;

constexpr int g_windowWidth{1280};
constexpr int g_windowHeight{720};

HittableList world;

RGBBuffer imageBuffer = RGBBuffer(g_windowWidth * g_windowHeight * 3);

Camera cam(Vec3{0, -5, -30}, 1.6, 0.9, g_windowWidth, g_windowHeight, 0.8,
           Vec3{0, -1, 0}, Vec3{0, 0, 1}, 100);

CPURenderer renderer(g_windowWidth, g_windowHeight, world, cam, imageBuffer, 7);

// FUNCTIONS

void initRayChasing() {

  world.add(std::make_shared<Sphere>(
      Vec3{
          0,
          -5.0001,
          0,
      },
      5, std::make_shared<Lambert>()));

  world.add(std::make_shared<Sphere>(
      Vec3{
          0,
          -3.0001,
          -15,
      },
      3, std::make_shared<Dielectric>()));

  world.add(std::make_shared<Sphere>(
      Vec3{
          -9,
          -4.0001,
          0,
      },
      4, std::make_shared<Metal>()));

  world.add(std::make_shared<Plane>(Vec3{0, 0, 0}, Vec3{0, -1, 0},
                                    std::make_shared<Lambert>()));
}

void draw() {
  renderer.render();

  for (size_t y{0}; y != g_windowHeight; ++y) {
    for (size_t x{0}; x != g_windowWidth; ++x) {
      SDL_FPoint p{static_cast<float>(x), static_cast<float>(y)};
      size_t index = (y * g_windowWidth + x) * 3;
      uint8_t r = imageBuffer[index + 0];
      uint8_t g = imageBuffer[index + 1];
      uint8_t b = imageBuffer[index + 2];
      SDL_SetRenderDrawColor(g_Renderer, r, g, b, 255);
      SDL_RenderPoints(g_Renderer, &p, 1);
    }
  }
}

/* This function runs once at startup. */
SDL_AppResult SDL_AppInit(void **appstate, int argc, char *argv[]) {

  initRayChasing();

  // Setup SDL
  if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)) {
    printf("Error: SDL_Init(): %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }

  // Create window with SDL_Renderer graphics context
  float main_scale = SDL_GetDisplayContentScale(SDL_GetPrimaryDisplay());
  SDL_WindowFlags window_flags =
      SDL_WINDOW_BORDERLESS | SDL_WINDOW_HIDDEN | SDL_WINDOW_HIGH_PIXEL_DENSITY;
  g_Window = SDL_CreateWindow("Dear ImGui SDL3+SDL_Renderer example",
                              g_windowWidth, g_windowHeight, window_flags);
  if (g_Window == nullptr) {
    printf("Error: SDL_CreateWindow(): %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  g_Renderer = SDL_CreateRenderer(g_Window, nullptr);
  SDL_SetRenderVSync(g_Renderer, 1);
  if (g_Renderer == nullptr) {
    SDL_Log("Error: SDL_CreateRenderer(): %s\n", SDL_GetError());
    return SDL_APP_FAILURE;
  }
  SDL_SetWindowPosition(g_Window, SDL_WINDOWPOS_CENTERED,
                        SDL_WINDOWPOS_CENTERED);
  SDL_ShowWindow(g_Window);

  // Setup Dear ImGui context
  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  g_Io = &ImGui::GetIO();
  g_Io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableKeyboard; // Enable Keyboard Controls
  g_Io->ConfigFlags |=
      ImGuiConfigFlags_NavEnableGamepad; // Enable Gamepad Controls

  // Setup Dear ImGui style
  ImGui::StyleColorsDark();
  // ImGui::StyleColorsLight();

  // Setup scaling
  ImGuiStyle &style = ImGui::GetStyle();
  style.ScaleAllSizes(
      main_scale); // Bake a fixed style scale. (until we have a solution for
                   // dynamic style scaling, changing this requires resetting
                   // Style + calling this again)
  style.FontScaleDpi =
      main_scale; // Set initial font scale. (in docking branch: using
                  // io.ConfigDpiScaleFonts=true automatically overrides this
                  // for every window depending on the current monitor)

  // Setup windowrounding
  style.WindowRounding = 7.0f;

  // Setup Platform/Renderer backends
  ImGui_ImplSDL3_InitForSDLRenderer(g_Window, g_Renderer);
  ImGui_ImplSDLRenderer3_Init(g_Renderer);

  ImFont *font = g_Io->Fonts->AddFontFromFileTTF(
      "/usr/share/fonts/TTF/JetBrainsMonoNerdFont-Bold.ttf");
  IM_ASSERT(font != nullptr);

  clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
  return SDL_APP_CONTINUE;
}

/* This function runs when a new event (mouse input, keypresses, etc) occurs. */
SDL_AppResult SDL_AppEvent(void *appstate, SDL_Event *event) {

  if (ImGui_ImplSDL3_ProcessEvent(event))
    return SDL_APP_CONTINUE;

  if (event->type == SDL_EVENT_QUIT)
    return SDL_APP_SUCCESS;
  if (event->type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
      event->window.windowID == SDL_GetWindowID(g_Window))
    return SDL_APP_SUCCESS;

  if (SDL_GetWindowFlags(g_Window) & SDL_WINDOW_MINIMIZED) {
    SDL_Delay(10);
    return SDL_APP_CONTINUE;
  }

  return SDL_APP_CONTINUE;
}
/* This function runs once per frame, and is the heart of the program. */
SDL_AppResult SDL_AppIterate(void *appstate) {
  // Start the Dear ImGui frame
  ImGui_ImplSDLRenderer3_NewFrame();
  ImGui_ImplSDL3_NewFrame();
  ImGui::NewFrame();

  {
    ImGui::Begin("te5t");
    ImGui::Text("ImText");
    ImGui::ColorEdit3("clear_color", &clear_color.x);
    ImGui::End();
  }

  // Rendering
  ImGui::Render();
  SDL_SetRenderScale(g_Renderer, g_Io->DisplayFramebufferScale.x,
                     g_Io->DisplayFramebufferScale.y);
  SDL_SetRenderDrawColorFloat(g_Renderer, clear_color.x, clear_color.y,
                              clear_color.z, clear_color.w);
  SDL_RenderClear(g_Renderer);

  // draw here
  draw();

  ImGui_ImplSDLRenderer3_RenderDrawData(ImGui::GetDrawData(), g_Renderer);
  SDL_RenderPresent(g_Renderer);
  return SDL_APP_CONTINUE;
}
/* This function runs once at shutdown. */
void SDL_AppQuit(void *appstate, SDL_AppResult result) {
  ImGui_ImplSDLRenderer3_Shutdown();
  ImGui_ImplSDL3_Shutdown();
  ImGui::DestroyContext();

  SDL_DestroyRenderer(g_Renderer);
  SDL_DestroyWindow(g_Window);
  SDL_Quit();
}
