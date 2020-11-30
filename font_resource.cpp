#include "font_resource.h"

font_resource::font_resource(int uid, const string name, string path, string type, int scene): asset(
	uid, name, path, type, scene)
{
}

void font_resource::load()
{
	font =  TTF_OpenFont( name.c_str(), 28 );
}

void font_resource::unload()
{
	TTF_CloseFont(font);
    font = nullptr;
}
