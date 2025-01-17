#include "Manager.h"
#include <ctime>
#include <cstdlib>
#include <iostream>


//konstruktor 
Manager::Manager(Pacman & pac, Board & boa,Bonus & bon): pacman_m(pac), board_m(boa), bonus_m(bon)
{
    score=0;
    
    for(int i=0;i<board_m.get_number_of_viruses();++i)
    {
        viruses_directions.push_back(ANY);
        rand_number_of_moves.push_back(0);
        steps.push_back(0); 
    }
    generate_first_directions();
}

//-------------------------------------------
//GENEROWANIE STANU POCZĄTKOWEGO 

//generuje początkowe kierunki wirusów 
void Manager::generate_first_directions()
{
    for(int i=0;i<board_m.get_number_of_viruses();++i)
    {
        int number=rand()%4;
        if(number==0) viruses_directions[i]=NORTH;
        else if(number==1) viruses_directions[i]=SOUTH;
        else if(number==2) viruses_directions[i]=WEST;
        else  viruses_directions[i]=EAST;
        int nr=rand()%8+2;
        rand_number_of_moves[i]=nr;
    }
}

//---------------------------------------------
//STEROWANIE GRĄ

//steruje rozgrywką 
void Manager::play(Move_direction direction)
{
    if(is_pacman_alive()==true)
    {
      if(clk_pacman.getElapsedTime().asSeconds()>pacman_m.get_pacman_speed())
      {    
       move_pacman( direction);
       clk_pacman.restart();
      }
      
      if(clk_viruses.getElapsedTime().asSeconds()>board_m.get_viruses_speed())
      {
       move_viruses( );
       clk_viruses.restart();
      }
      
      if(uniform==true) 
      {
          if(clk_uniform.getElapsedTime().asSeconds()>20) uniform=false;
      }
      
    }
    
}


//zarządza ruchem pacmana
void Manager::move_pacman(Move_direction direction)
{
   if(did_pacman_meet_virus()==true)
   {
     if(uniform==false) pacman_meet_virus();
     return;
   } 
       
   if(direction==ANY) return;
 
   if(board_m.can_pacman_move(direction)==false) return;
  
   if(board_m.can_pacman_move(direction)==true) pacman_m.move(direction);
    
   if(did_pacman_meet_virus()==true)
   {
       if(uniform==false) pacman_meet_virus();
        return;
   }  
     sf::Vector2f position=pacman_m.get_position();
     int row=position.y;
     int col=position.x;
     
  if(board_m.is_vaccine_on_field(row,col)==true)
    {
       if(antivaxx==true) score++;
       score++;
       vaccines++;
       board_m.vaccinate(row,col);
    }
    else if(board_m.is_bonus_on_field(row,col)==true)
    {
        is_bonus=true;
        type_of_bonus=bonus_m.get_type_of_bonus( row,  col);
        bonus_m.bonus_activated(row,col);
       board_m.delete_bonus(row,col);
       do_something(type_of_bonus);   
    }
}


//funkcja wywoływana jeśli aktywował się bonus i wymagane jest wykonanie jakiejś aktywnosci 
void Manager::do_something(Bonus_type type)
{
   if(type==SCHOOL)
   {
       int size=board_m.get_number_of_viruses();
       for(int i=0;i<size;++i)
       {
         viruses_directions.push_back(NORTH);
         rand_number_of_moves.push_back(1);
       }       
   }
   else if(type==PLANE)
   {
       clk_uniform.restart();
       uniform=true;
   }
   else if(type==LOCKDOWN)
   {
       clk_lockdown.restart();
   }
   else if(type==DEATH)
   {
       int size=board_m.get_number_of_viruses();
       int last_size=viruses_directions.size();
       for(int i=last_size;i>size;--i)
       {
         viruses_directions.pop_back();
         rand_number_of_moves.pop_back();
       }
   }
   else if(type==ANTIVAXXERS)
   {
       antivaxx=true;
   }
}

//sprawdza czy pacman i wirus spotkali się na polu 
bool Manager::did_pacman_meet_virus() const
{
  sf::Vector2f position=pacman_m.get_position();
  int row=position.y;
  int col=position.x;
  if(board_m.is_any_virus_on_field(row,col)==true) return true;

  return false;
}

//wywołuje reset planszy jeśli pacman spotkał wirusa (obsługa śmierci pacmana)
void Manager::pacman_meet_virus()
{
    pacman_m.remove_live();
    if(pacman_m.get_lives_number()>0) 
    {
      board_m.reset();
    }
    else 
    {
        pacman_died();
    }
    
}

//zarządza ruchem wirusów 
void Manager::move_viruses( )
{ 
    if(bonus_m.is_lockdown()==true)
    {
        if(clk_lockdown.getElapsedTime().asSeconds()>20) bonus_m.end_lockdown();
        return;
    } 
   for(int i=0;i<board_m.get_number_of_viruses();++i)
   {   
     if(board_m.can_virus_move(i,viruses_directions[i])==true)
     {
         board_m.move_virus(i, viruses_directions[i]);
         steps[i]++;
          if(steps[i]==rand_number_of_moves[i])
          {  
            randomize(i);
          }
     }
     else
     {      
        randomize(i);
     }
    
   }
   if(did_pacman_meet_virus()==true) 
   {
       if(uniform==false) pacman_meet_virus();
   }
}
/* OPIS ALGORYTMU PORUSZANIA SIĘ wirusów
Tworzymy dwa wektory pomocnicze int o długości takiej samej jak długość wektora wirusów
Pierwszy będzie służył do przechowywania wylosowanej wartości od 2 do 8--> ilości kroków jakie wirus ma zrobić w daną stronę. W drugim przechowujemy ilość wykonanaych kroków.

a) konstruktor
Początkowo oba wektory inicjalizujemy zerami. W konstruktorze losujemy pierwszy kierunek i ilość kroków w jego stronę. 

b) właściwa funkcja 
(poniższe instrukcje powtarzamy dla każdego wirusa z wektora)
    Nakazujemy planszy sprawdzić czy wirus może ruszyć się w daną stronę; 
    b1) jeśli tak wykonujemy ruch i inkrementujemy zmienną z wektora steps; teraz sprawdzamy czy wykonaliśmy już zaplanowaną ilość kroków (czy steps==rand_number_of_moves) i jeśli tak losujemy nową ilość kroków i nowy kierunek  
    b2) jeśli nie losujemy nowy kierunek i nową ilość kroków (tak długo aż wylosujemy inny kierunek), steps ustawiamy na 0
     
Na koniec sprawdzamy czy pacman nie umarł
*/

//funkcja pomocnicza losująca parametry ruchu wirusa
void Manager::randomize(int i)
{
  do
  {
      int number=rand()%4;
      if(number==0) viruses_directions[i]=NORTH;
      else if(number==1) viruses_directions[i]=SOUTH;
      else if(number==2) viruses_directions[i]=WEST;
      else  viruses_directions[i]=EAST;
      steps[i]=0;
      int nr=rand()%8+2;
      rand_number_of_moves[i]=nr;
 }while(board_m.can_virus_move(i,viruses_directions[i])==false);
}

//-----------------------------------
//OBSŁUGA ŚMIERCI PACMANA

//wywoływana po (ostatecznej) śmierci pacmana
void Manager::pacman_died()
{
    std::ifstream plik("../wyniki.txt");

  if(!plik)
  {
    std::cerr<<"Błąd: "<<strerror(errno)<<std::endl;
    exit(-1);
  }
  
   while(plik)
    {
      results.push_back("");      
      getline(plik, results[idx]);
    idx++;
    }
   if(results.size()<=10) vec_not_full();
   else vec_full();
  
  plik.close();
}

//wczytuje wyniki jeśli w pliku nie ma 10 wyników 
void Manager::vec_not_full()
{
   std::ofstream plik("../wyniki.txt");

  if(!plik)
  {
    std::cerr<<"Błąd: "<<strerror(errno)<<std::endl;
    exit(-1);
  }
  plik<<pacman_m.get_name()<<" "<<get_score()<<" pkt "<<100*(get_done_vaccine_number())/(board_m.get_total_vaccine_number())<<"% populacji"<<std::endl;

  for(size_t i=0;i<results.size()-1;++i)
  {
    plik<<results[i]<<std::endl;
  }

  plik.close();
}

//wczytuje wyniki jeśli w pliku jest 10 wyników 
void Manager::vec_full()
{
  std::ofstream plik("../wyniki.txt");

  if(!plik)
  {
    std::cerr<<"Błąd: "<<strerror(errno)<<std::endl;
    exit(-1);
  }
  
  plik<<pacman_m.get_name()<<" "<<get_score()<<" pkt "<<100*(get_done_vaccine_number())/(board_m.get_total_vaccine_number())<<"% populacji"<<std::endl;

  for(size_t i=0;i<9;++i)
  {
    plik<<results[i]<<std::endl;
  }

  plik.close();
}


//-----------------------------------
//INNE

//kończy bonus
void Manager::end_bonus()
{
    is_bonus=false;
}

//resetuje zegar clk_lockdown
 void Manager::reset_clk_lockdown()
 {
     clk_lockdown.restart();
 }

 //resetuje zegar clk_uniform
 void Manager::reset_clk_uniform()
 {
     clk_uniform.restart();
 }

//--------------------------------------------
//FUNKCJE ZWRACAJĄCE INFORMACJE 


//zwraca ilość zdobytych punktów
int Manager::get_score() const
{
    return score;
}
  
//zwraca ilość wziętych szcepionek 
int Manager::get_done_vaccine_number() const
{
    return vaccines;
}

//sprawdza czy pacman "nosi" kombinezon
bool Manager::get_uniform_state() const
{
    return uniform;
}

//sprawdza czy jest aktywny bonus 
bool Manager::get_is_bonus_state() const
{
    return is_bonus;
}

//zwraca typ aktywnego bonusu 
Bonus_type Manager::get_active_bonus_type() const
{
    return type_of_bonus;
}

//zwraca prawdę jeśli wszystkie szczepionki zjedzone 
bool Manager::all_vaccine_taken() const
{
    if(board_m.get_total_vaccine_number()==vaccines) return true;
    return false;    
}

//zwraca prawdę jeśli pacman żyje
bool Manager::is_pacman_alive() const
{
  if(pacman_m.get_lives_number()==0) return false;
  return true;
}

