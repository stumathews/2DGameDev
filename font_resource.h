#pragma once
#include "asset.h"
#include "Logger.h"

class font_resource final : public asset
{
public:
	font_resource(int uid, string name, string path, string type, int scene);

	TTF_Font* get_font() const;

	void load() override;
	void unload() override;
private:
	TTF_Font* font = nullptr;
};

