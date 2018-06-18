#pragma once
#include <utility>
#include <vector>


typedef std::pair<int, int> Pozycja;
typedef std::pair<Pozycja, Pozycja> Przesuniecie;
typedef std::vector<Przesuniecie> Ruch;
// Obiekty i klasy
//Klasa plansza - przechowuje informacje o tym na jakim polu jest jaki pionek
//zwraca liste/wektor ruchow
//zawiera metody w stylu narysuj sie w okienku
//lista legalnych ruchów, z których u¿ytkownik wybiera
//Klasa ruch

class Plansza
{
public:
	Ruch dostepne_ruchy;
	int plansza[8][8];
	void drukujPlansze(int tab[8][8]);
	bool Tura; // Czny = 1, Bly = 0 ;
	bool koniec_gry;
	enum Pole { PolePuste = -1, Bialy = 0, Czarny = 1}; //Jaki stan pola
	
	Plansza(); // Inicjalizacja planszy
	void inicjalizuj();
	Ruch znajdzBicie(int x, int y); // Wazna funkcja szuka czy jest mozliwe bicie i jakie
	Ruch znajdz_Dostepne_ruchy(); // Kluczowa funkcja przeszukuje plansze w poszukiwaniu mozliwych ruchow
	std::vector<int[8][8]> Zapisane_plansze;
	void drukujRuchy(Ruch ruchy);
	bool ruch(); // Gdzie sie przenosimy
	
	Pozycja wybierzPionka(); // Funkcja zbiera informacje, ktorym pionkiem chcemy ruszyc
	Pozycja wybierzPoleDocelowe(); // Gdzie chcemy go postawic

	void zmianaTury(bool tura); // Prosta f. zmienia ture
		//void wykonajRuch();
    void rysuj_plansze();
	
private:
	
	//plansza plansza_;
	bool przymusBicia;
	int zaznaczony_x;
	int zaznaczony_y;
	bool kontynuacja_bicia;
	int pole_docelowe_x ;
	int pole_docelowe_y ;

};
