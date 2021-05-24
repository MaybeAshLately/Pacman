#ifndef EVENTS_H
#define EVENTS_H
#include "SFML/Graphics.hpp"
#include "SFML/Window.hpp"
#include <iostream>
#include "Pacman.h"
#include "Coronavirus.h"
#include "Board.h"
#include "Manager.h"
#include "SFMLapp.h"
#include "Bonus.h"


class Events
{
   Pacman & pacman_e;
   Board & board_e;
   Bonus & bonus_e;
   Manager & manager_e;
   SFMLapp & sfml_e;
   
  public:
  explicit Events(Pacman & pacman_e, Board & board_e,Bonus & bonus_e,Manager & manager_e,SFMLapp & sfml_e);
  void key_was_pressed(sf::Event event);
  
};

#endif