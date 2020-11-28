#pragma once
#include <functional>
class game_object;
typedef unsigned int uint;
typedef std::vector<shared_ptr<game_object>> game_objects;
typedef vector<shared_ptr<event>> events;

typedef function<void(SDL_Renderer* renderer)> RenderFunc;
