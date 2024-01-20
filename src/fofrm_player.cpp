// fofrm_player.cpp - Main source file
// Copyright (C) 2024 APAMk2

#include "fofrm_player.h"
#include "fofrm_parser.h"

#include <iostream>
#include <fstream>
#include <format>

#include "SDL.h"
#include "SDL_image.h"
#include "SDL_ttf.h"

using namespace std;

void Init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		cout << "Error SDL2 Initialization : " << SDL_GetError();
		Destroy();
	}
	if (IMG_Init(IMG_INIT_PNG) == 0) {
		cout << "Error SDL2_image Initialization" << IMG_GetError();
		Destroy();
	}
	if (TTF_Init() != 0) {
		cout << "Error SDL2_ttf Initialization" << TTF_GetError();
		Destroy();
	}

	window = SDL_CreateWindow(".fofrm SDL2 Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 150, 150, SDL_WINDOW_OPENGL );
	
	if (window == NULL) {
		cout << "Error while window init" << SDL_GetError();
		Destroy();
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	
	if (renderer == NULL) {
		cout << "Error while render creation" << SDL_GetError();
		Destroy();
	}
	
	font = TTF_OpenFont("C:/Windows/Fonts/vgasysr.fon", 12);
	paused = renderText("Paused", textColor, renderer);
	SDL_QueryTexture(paused, NULL, NULL, &pausedW, &pausedH);
}

void Destroy() {
	SDL_DestroyTexture(paused);
	for (size_t i = 0; i < frames.size(); i++) {
		SDL_DestroyTexture(frames[i]);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	TTF_CloseFont(font);
	IMG_Quit();
	TTF_Quit();
	SDL_Quit();
}

Uint32 TimeLeft() {
	Uint32 now;

	now = SDL_GetTicks();
	if (nextTime <= now)
		return 0;
	else
		return nextTime - now;
}

SDL_Texture* renderText(const string& text, SDL_Color color, SDL_Renderer* renderer) {
	SDL_Surface* surf = TTF_RenderText_Solid(font, text.c_str(), color);
	SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surf);
	SDL_FreeSurface(surf);

	return texture;
}

void PlayFofrm(bool changeFrame, bool decrease) {
	if (changeFrame) {
		if (decrease) {
			currFrame--;
		}
		else {
			currFrame++;
		}

		if (currFrame >= frameCount) currFrame = 0;
		else if (currFrame < 0) currFrame = frameCount - 1;
	}
	SDL_QueryTexture(frames[currFrame], NULL, NULL, &currFrameW, &currFrameH);
	int zoomW = currFrameW * zoomMult, zoomH = currFrameH * zoomMult;
	int txtW, txtH;

	SDL_Rect* texRect = new SDL_Rect{ ((winW / 2) - (zoomW / 2)), ((winH / 2 + 20) - (zoomH / 2)) , zoomW, zoomH };

	SDL_Texture* fpsText = renderText("FPS: " + to_string(fps), textColor, renderer);
	SDL_Texture* frameText = renderText("Frame: " + to_string(currFrame), textColor, renderer);
	SDL_Texture* zoomText = renderText("Zoom: " + to_string( int(100 * zoomMult) ), textColor, renderer);
	SDL_QueryTexture(fpsText, NULL, NULL, &txtW, &txtH);
	SDL_Rect* fpsRect = new SDL_Rect{ 3, 2, txtW, txtH };

	SDL_Rect* pauseRect = new SDL_Rect{ 3, fpsRect->h + 2, pausedW, pausedH };

	SDL_QueryTexture(frameText, NULL, NULL, &txtW, &txtH);
	SDL_Rect* frameRect = new SDL_Rect{ fpsRect->w + 10, 2, txtW, txtH};
	
	SDL_QueryTexture(zoomText, NULL, NULL, &txtW, &txtH);
	SDL_Rect* zoomRect = new SDL_Rect{ 3 + ((pausedW + 10) * isPaused), pauseRect->y, txtW, txtH};

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, frames[currFrame], NULL, texRect);
	SDL_RenderCopy(renderer, fpsText, NULL, fpsRect);
	SDL_RenderCopy(renderer, frameText, NULL, frameRect);
	if (isPaused) {
		SDL_RenderCopy(renderer, paused, NULL, pauseRect);
	}
	SDL_RenderCopy(renderer, zoomText, NULL, zoomRect);
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(fpsText);
	SDL_DestroyTexture(frameText);
	SDL_DestroyTexture(zoomText);
}

void InitLoop() {
	SDL_QueryTexture(frames[currFrame], NULL, NULL, &currFrameW, &currFrameH);
	winW = (currFrameW * zoomMult >= 150) ? currFrameW * zoomMult : 150;
	winH = (currFrameH * zoomMult + 40 >= 150) ? currFrameH * zoomMult + 40 : 150;
	SDL_SetWindowSize(window, winW, winH);

	nextTime = SDL_GetTicks();
	if (fps == 0) {
		isPaused = true;
		PlayFofrm(false, false);
	}
	PlayerLoop();
}

void PlayerLoop() {
	while (true) {
		SDL_Event e;
		if (SDL_PollEvent(&e)) {
			if (e.type == SDL_QUIT) {
				Destroy();
				break;
			}
			if (e.type == SDL_KEYDOWN) {
				switch (e.key.keysym.sym) {
				case SDLK_SPACE:
					isPaused = !isPaused;
					break;
				case SDLK_LEFT:
					PlayFofrm(true, true);
					break;
				case SDLK_RIGHT:
					PlayFofrm(true, false);
					break;
				case SDLK_KP_PLUS:
					fps += 1;
					if (fps == 1) {
						isPaused = false;
					}
					break;
				case SDLK_KP_MINUS:
					fps -= 1;
					if (fps < 1) {
						isPaused = true;
					}
					break;
				}
			}

			if (e.type == SDL_MOUSEWHEEL) {
				if (e.wheel.y > 0) {
					zoomMult += 0.1;
				}
				else {
					zoomMult -= 0.1;
				}
				int tw = currFrameW * zoomMult, th = currFrameH * zoomMult + 40;
				if (tw >= 150 || th >= 150) {
					winW = (tw >= 150) ? tw : 150;
					winH = (th >= 150) ? th : 150;
					SDL_SetWindowSize(window, winW, winH);
				}
				PlayFofrm(false, false);
			}
		}
		if (TimeLeft() == 0) {
			PlayFofrm(!isPaused, false);
			int del = (fps > 0) ? fps : 1;
			nextTime += 1000 / del;
		}
	}
}

void SetupSettings(unordered_map<string, string> ini) {
	if (ini.find("count") != ini.end()) {
		frameCount = stoi(ini["count"]);
	}
	else {
		cout << "Warning: \"count\" is not specified! Frame count is 1 as default.\n";
		frameCount = 1;
	}

	if (ini.find("fps") != ini.end()) {
		fps = stoi(ini["fps"]);
	}
	else {
		cout << "Warning: \"fps\" is not specified! FPS is 0 as default.\n";
		fps = 0;
	}
}

void LoadFrames(filesystem::path file, unordered_map<string, string> ini) {
	if (ini.find("frm_0") != ini.end()) {
		for (int i = 0; i < frameCount; i++) {
			string frmFileName = file.remove_filename().string() + ini["frm_" + to_string(i)];
			frames.push_back(IMG_LoadTexture(renderer, frmFileName.c_str()));
			cout << frmFileName << endl;
		}
	}
	else {
		string frmFileName = file.remove_filename().string() + ini["frm"];
		frames.push_back(IMG_LoadTexture(renderer, frmFileName.c_str()));
		cout << frmFileName << endl;
	}
}

int main(int argc, char* argv[]) {
	Init();

	filesystem::path file = argv[1];

	unordered_map<string, string> ini;

	LoadFofrm(file, ini);
	SetupSettings(ini);
	LoadFrames(file, ini);
	InitLoop();
	
	return 0;
}
