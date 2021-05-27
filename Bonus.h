#ifndef BONUS_H
#define BONUS_H
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include "Pacman.h"
#include "Coronavirus.h"
#include "Board.h"
#include <iostream>
#include <vector>

enum Bonus_type{NONE,SCHOOL,PLANE,BORDER,BRITAIN,RESPIRATOR,LOCKDOWN,SUMMER,LITE_LOCKDOWN,DEATH,ANTIVAXXERS };

class Bonus
{
    Pacman & pacman_b;
    Board & board_b;
    std::vector<Bonus_type> bonuses_types;
    std::vector<int> randomize_vec;
  Bonus_type rand_type_of_bonus();

    void bonus_one();
    void bonus_two();
    void bonus_three();
    void bonus_four();
    void bonus_five();
    void bonus_six();
    void bonus_seven();
    void bonus_eight();
    void bonus_nine();
    void bonus_ten();

    bool lockdown=false;

    public:
    explicit Bonus(Pacman & pacman_b, Board & board_b);
    
    Bonus_type get_type_of_bonus(int row, int col);
    void bonus_activated(int row, int col);

    bool is_lockdown();
    bool end_lockdown();
};


#endif