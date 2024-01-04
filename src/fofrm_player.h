﻿// fofrm_player.h - Header with all main definitions
// Copyright (C) 2024 APAMk2

#pragma once

#include <iostream>
#include <fstream>
#include <string> 
#include <vector>
#include <regex>
#include <filesystem>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

void Init();
void Destroy();
Uint32 TimeLeft();
void ProcessLine(std::string line);
void LoadFofrm();
SDL_Texture* renderText(const std::string &text, SDL_Color color, SDL_Renderer* renderer);
void PlayFofrm(bool changeFrame, bool decrease);
void PlayerLoop();

SDL_Window* window;
SDL_Renderer* renderer;
SDL_Color textColor = { 175,0,42 };
TTF_Font* font;
SDL_Texture* paused;

std::filesystem::path file;
std::vector< SDL_Texture* > frames(128);

int frameCount = 1, fps = 0, currFrame = 0;
int winH = 150, winW = 150;
int pausedH, pausedW;
int currFrameH, currFrameW;
float zoomMult = 1.0;

std::string frameFiles[128];
static Uint32 nextTime;

bool isPaused = false;