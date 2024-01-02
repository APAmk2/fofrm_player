// fofrm_player.h - Header with all main definitions
// Written by APAMk2, all rights reserved.

#pragma once

#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <regex>
#include <filesystem>

#include "SDL.h"
#include "include/SDL_image.h"

void Init();
void Destroy();
Uint32 TimeLeft();
void ProcessLine(std::string line);
void LoadFofrm();
void PlayFofrm(bool decrease);
void PlayerLoop();

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Surface* lastFrame;
std::filesystem::path file;
std::vector< SDL_Texture* > frames(128);
int frameCount = 1, fps = 0, currFrame = 0;
int winH, winW;
std::string frameFiles[128];
static Uint32 nextTime;
bool isPaused = false;