// fofrm_player.cpp - Main source file
// Copyright (C) 2024 APAMk2

#include "fofrm_player.h"

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

void ProcessLine(string line) {
	smatch match;

	regex re_fps("fps=(\\d+)");
	regex re_count("count=(\\d+)");
	regex re_frm("frm=");
	regex re_frm_1("frm_(\\d+)=");
	if (regex_search(line, match, re_fps))
	{
		fps = stoi(match[1]);
	}
	else if (regex_search(line, match, re_count))
	{
		frameCount = stoi(match[1]);
	}
	else if (regex_search(line, match, re_frm_1))
	{
		frameFiles[stoi(match[1])] = file.remove_filename().string() + regex_replace(line, re_frm_1, "");
		cout << frameFiles[stoi(match[1])] << endl;
	}
	else if (regex_search(line, match, re_frm))
	{
		frameFiles[0] = file.remove_filename().string() + regex_replace(line, re_frm, "");
		cout << frameFiles[0] << endl;
	}
}

void LoadFofrm() {
	string fofrm_data;
	ifstream input(file);
	if (input.is_open()) {
		while (getline(input, fofrm_data)) {
			ProcessLine(fofrm_data);
		}
	}
	input.close();
	fofrm_data.erase();
	frameFiles->shrink_to_fit();

	for (size_t i = 0; i <= frameFiles->size(); i++) {
		frames[i] = IMG_LoadTexture(renderer, frameFiles[i].c_str());
	}
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
	int tempW = currFrameW * zoomMult, tempH = currFrameH * zoomMult;
	int txtW, txtH;

	SDL_Texture* fpsText = renderText("FPS: " + to_string(fps), textColor, renderer);
	SDL_Texture* frameText = renderText("Frame: " + to_string(currFrame), textColor, renderer);
	SDL_QueryTexture(fpsText, NULL, NULL, &txtW, &txtH);

	SDL_Rect* fpsRect = new SDL_Rect{ 3, 2, txtW, txtH };
	SDL_Rect* pauseRect = new SDL_Rect{ 3, txtH + 2, pausedW, pausedH };
	SDL_Rect* frameRect = new SDL_Rect{ 3 + txtW + 10, 2, pausedW, pausedH };
	SDL_Rect* textRect = new SDL_Rect{ ((winW / 2) - (tempW / 2)), 40 , tempW, tempH };

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, frames[currFrame], NULL, textRect);
	SDL_RenderCopy(renderer, fpsText, NULL, fpsRect);
	SDL_RenderCopy(renderer, frameText, NULL, frameRect);
	if (isPaused) {
		SDL_RenderCopy(renderer, paused, NULL, pauseRect);
	}
	SDL_RenderPresent(renderer);
	SDL_DestroyTexture(fpsText);
	SDL_DestroyTexture(frameText);
}

void PlayerLoop() {
	SDL_QueryTexture(frames[currFrame], NULL, NULL, &currFrameW, &currFrameH);
	winW = (currFrameW * zoomMult >= 150) ? currFrameW * zoomMult : 150;
	winH = (currFrameH * zoomMult + 40 >= 150) ? currFrameH * zoomMult + 40 : 150;
	SDL_SetWindowSize(window, winW, winH);

	nextTime = SDL_GetTicks();
	if (fps == 0) {
		isPaused = true;
		PlayFofrm(false, false);
	}
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
					PlayFofrm(false, false);
					break;
				case SDLK_LEFT:
					PlayFofrm(true, true);
					break;
				case SDLK_RIGHT:
					PlayFofrm(true, false);
					break;
				case SDLK_KP_PLUS:
					fps += 1;
					break;
				case SDLK_KP_MINUS:
					fps -= 1;
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
		if (!isPaused) {
			if (TimeLeft() == 0) {
				PlayFofrm(true, false);
				nextTime += 1000 / fps;
			}
		}
	}
}

int main(int argc, char* argv[]) {
	Init();

	file = argv[1];
	LoadFofrm();
	PlayerLoop();
	
	return 0;
}
