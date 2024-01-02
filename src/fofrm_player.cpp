// fofrm_player.cpp - Main source file
// Written by APAMk2, all rights reserved.

#include "fofrm_player.h"

using namespace std;

void Init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		cout << "Error SDL2 Initialization : " << SDL_GetError();
		Destroy();
	}
	if (IMG_Init(IMG_INIT_PNG) == 0) {
		cout << "Error SDL2_image Initialization" << SDL_GetError();
		Destroy();
	}

	window = SDL_CreateWindow(".fofrm SDL2 Player", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_OPENGL);
	if (window == NULL) {
		cout << "Error while window init";
		Destroy();
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
	if (renderer == NULL) {
		cout << "Error renderer creation";
		Destroy();
	}
}

void Destroy() {
	for (size_t i = 0; i < frames.size(); i++) {
		SDL_DestroyTexture(frames[i]);
	}
	SDL_DestroyRenderer(renderer);
	SDL_DestroyWindow(window);
	IMG_Quit();
	SDL_Quit();
}

Uint32 TimeLeft()
{
	Uint32 now;

	now = SDL_GetTicks();
	if (nextTime <= now)
		return 0;
	else
		return nextTime - now;
}

void ProcessLine(string line)
{
	smatch match;

	regex re_fps("fps=(\\d+)");
	regex re_count("count=(\\d+)");
	regex re_frm("frm_(\\d+)=(\\w+)");
	if (regex_search(line, match, re_fps))
	{
		fps = stoi(match[1]);
	}
	else if (regex_search(line, match, re_count))
	{
		frameCount = stoi(match[1]);
	}
	else if (regex_search(line, match, re_frm))
	{
		frameFiles[stoi(match[1])] = file.parent_path().string() + "\\" + string(match[2]) + ".png";
		cout << frameFiles[stoi(match[1])] << endl;
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

	lastFrame = IMG_Load(frameFiles[frameCount - 1].c_str());
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

	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, frames[currFrame], NULL, NULL);
	SDL_RenderPresent(renderer);
}

void PlayerLoop() {
	if (lastFrame->w < 100 || lastFrame->h < 100) {
		SDL_SetWindowSize(window, lastFrame->w * 2, lastFrame->h * 2);
		winW = lastFrame->w * 2;
		winH = lastFrame->h * 2;
	}
	else {
		SDL_SetWindowSize(window, lastFrame->w, lastFrame->h);
		winW = lastFrame->w;
		winH = lastFrame->h;
	}
	nextTime = SDL_GetTicks() + 1000 / fps;
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
				}
			}
			if (e.type == SDL_MOUSEWHEEL) {
				winW = winW + winW * (e.wheel.y / 10.0);
				winH = winH + winH * (e.wheel.y / 10.0);
				SDL_SetWindowSize(window, winW, winH);
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
