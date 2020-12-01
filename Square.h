#pragma once
#include <SDL.h>
#include "game_object.h"
#include "rect_details.h"

class square : public game_object   // NOLINT(cppcoreguidelines-special-member-functions)
{
private: 
    int width;
protected:
	bool walls[4]{};
	std::shared_ptr<rect_details> rect_details_;
    std::shared_ptr<rect_details> get_rect_details() const;;
	int fill = true;
	SDL_Rect player_bounds_;
	SDL_Rect my_bounds_;
public: 
	square(int x, int y, int rw, bool fill = false, bool supports_move_logic = true, bool is_visible = true);

    int get_x() const;
    int get_y() const;
    int get_w() const;
    int get_h() const;

    bool is_walled(int wall);
    void remove_wall(int wall);
	std::string get_identifier() override;

	std::vector<std::shared_ptr<event>> handle_event(std::shared_ptr<event> event) override;
    void draw(SDL_Renderer* renderer) override;;
    virtual ~square();
    void update() override;
};


