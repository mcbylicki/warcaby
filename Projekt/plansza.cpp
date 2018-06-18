#include <iostream>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include "allegro5/allegro_image.h"
#include <vector>
#include "plansza.h"

using namespace std;

ALLEGRO_DISPLAY *display = NULL;
ALLEGRO_EVENT_QUEUE *event_queue = NULL;
ALLEGRO_TIMER *timer = NULL;

// Konfiguracja

const float FPS = 60;       // Frames Per Second

const int map_w = 736;   // szerokosc planszy
const int map_h = 736;    // wysokosc planszy

const int ramka = 40;  // szerokosc ramki

const int pionek_w = 82;   // szerokosc pionka
const int pionek_h = 82;    // wysokosc pionka

const int liczba_obrazkow = 3;

const string sciezki_do_obrazkow[liczba_obrazkow] =
{
    "obrazki/board.png", // deska
    "obrazki/white.png", // biala
    "obrazki/black.png" // czarna
};

enum typ_obrazka
{
    mapa, bialy, czarny
    // potrzebne do deklaracji tablicy poniżej
};

struct kafelek
{
    enum typ_obrazka typ;
};


ALLEGRO_BITMAP* bitmapa[liczba_obrazkow];


bool przygotuj_bitmapy() {

    for (int i = 0; i < liczba_obrazkow; i++) {
        bitmapa[i] = al_load_bitmap(sciezki_do_obrazkow[i].c_str());

        if (!bitmapa[i]) {
            cerr << "Blad podczas wczytywania bitmapy ";
            cerr << sciezki_do_obrazkow[i] << "." << endl;
            return false;
        }
    }

    return true;
}

void Plansza::rysuj_plansze()
{
    al_clear_to_color(al_map_rgb(0,0,0));

    al_draw_bitmap_region(bitmapa[mapa], 0, 0, map_w, map_h, 0, 0, 0);

	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			if (plansza[i][j] == Bialy) {
                al_draw_bitmap_region(bitmapa[bialy], 0, 0, pionek_w, pionek_h, ramka + i * pionek_w, ramka + j * pionek_h, 0);

            } else if (plansza[i][j] == Czarny) {
                al_draw_bitmap_region(bitmapa[czarny], 0, 0, pionek_w, pionek_h, ramka + i * pionek_w, ramka + j * pionek_h, 0);
            }
		}
	};
    al_flip_display();
}


//Inicjalizujemy plansze
Plansza::Plansza()
{
	int zaznaczony_x = -1; // wspolrzedne zaznaczonego pionka
	int zaznaczony_y = -1;

	int pole_docelowe_x = -1;
	int pole_docelowe_y = -1;

	bool Tura = 0;
	bool przymusBicia = true;

	bool koniec_gry = false;
}

void Plansza::inicjalizuj()
{
	for (int i = 0; i < 8; ++i) {
		for (int j = 0; j < 8; ++j) {
			plansza[i][j] = PolePuste;
		}
	};

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 3; ++j) {
			plansza[i * 2 + (j % 2)][j] = Bialy;
		}
	};

	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 3; j++) {
			plansza[1 + i * 2 - (j % 2)][7 - j] = Czarny;
		}
	};
    koniec_gry = false;
    Tura = false;
}


Ruch Plansza::znajdzBicie(int x, int y) // Funkcja szuka bicia dla danych współrzędnych pionka x y i zapisuje je w wektorze dostępnych bić
{
	const int dx[] = { -1, 1, 1, -1 };
	const int dy[] = { -1, -1, 1, 1 };
	for (int i = 0; i < 4; ++i)
	{
		// To są pola na ukos od naszego pionka, na których szukamy figórek przeciwnika
		const int _x = x + dx[i];
		const int _y = y + dy[i];
		if (_x < 0 || _x >= 8 || _y < 0 || _y >= 8) // Jeśli pole na skos są poza planszą wychodzimy z tej iteracji pętli
			continue;
		if (plansza[_x][_y] == PolePuste || plansza[_x][_y] == Tura || plansza[_x][_y] == Tura + 2 ) // Jeśli pole na skos jest puste lub nasz pionek też wychodzimy
			continue;
		const int _2x = x + 2 * dx[i]; // W przecinym przypadku niż powyżej, zapisujemy współrzędne i sprawdzamy co się dzieje na kolejnym polu na skos (co jest za pionkiem przeciwnika)
		const int _2y = y + 2 * dy[i];
		if (_2x < 0 || _2x >= 8 || _2y < 0 || _2y >= 8) // Jeśli za przeciwnikiem jest koniec planszy to wychodzimy z tej iteracji
			continue;
		if (_2x < 0 || _2x >= 8 || _2y < 0 || _2y >= 8) // Jeśli za przeciwnikiem jest koniec planszy to wychodzimy z tej iteracji
			continue;

		if (plansza[_2x][_2y] == PolePuste) // Za przeciwnikiem jest puste pole - mamy obowiązek bicia
		{
			dostepne_ruchy.push_back(Przesuniecie(Pozycja(x, y), Pozycja(_2x, _2y)));  // Wrzucamy do wektora ruchu pozycje poczatkowa i koncowa pionka bijacego
		}

	}
	return dostepne_ruchy;
}

Ruch Plansza::znajdz_Dostepne_ruchy()
{	// Najpierw sprawdzam, czy jest bicie na orbicie
	dostepne_ruchy.clear();
	for (int x = 0; x < 8; ++x)
	{
		for (int y = 0; y < 8; ++y) // po kolei dla każdego pionka
		{
			if (plansza[x][y] == Tura)
			{
				dostepne_ruchy = znajdzBicie(x, y);
			}
		}
	}

	//Jesli nie	ma bicia
	if (dostepne_ruchy.empty())
	{
		przymusBicia = false;
		for (int i = 0; i < 8; ++i)
		{
			for (int j = 0; j < 8; ++j)
			{
				if (plansza[i][j] == Tura) // znowu wybieramy naszego pionka
				{
					int _y = j + 1;
					if (Tura) _y = j - 1; //Jesli czarny
					if (_y >= 0 && _y < 8) // Sprawdzamy pola w obrebie planszy
						for (int _x = i - 1; _x <= i + 1; _x += 2)
						{
							if (_x < 0)
								continue;
							if (_x > 8)
								continue;
							if (plansza[_x][_y] == PolePuste)
							{
								dostepne_ruchy.push_back(Przesuniecie(Pozycja(i, j), Pozycja(_x, _y)));
							}
						}
				}
			}
		}

	}
	return dostepne_ruchy;
}

std::pair<float, float> kliknij() {
	bool running = true;
	float x = 0, y = 0;

	int width = al_get_display_width(display);
	while (running) {
		ALLEGRO_EVENT event;

		if (!al_is_event_queue_empty(event_queue)) {
			al_wait_for_event(event_queue, &event);
			if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE)
				running = false;

			if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
				x = event.mouse.x;
				y = event.mouse.y;
			}

			if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
                running = false;
				x = event.mouse.x;
				y = event.mouse.y;
				// al_set_mouse_xy(display, 0, 0);
			}
		}
	}
    return pair<float, float>(x, y);
}

pair<int, int> zamien_na_pole(pair<float, float> wspolrzedne) {
    float x = (wspolrzedne.first - ramka) / pionek_w;
    float y = (wspolrzedne.second - ramka) / pionek_h;
    return pair<int, int>(x, y);
}

pair<int, int> kliknij_pole() {
    pair<float, float> wspolrzedne = kliknij();
    return zamien_na_pole(wspolrzedne);
}

Pozycja Plansza::wybierzPionka()
{
    pair<int, int> pole = kliknij_pole();

	int x, y;
    x = pole.first;
    y = pole.second;
    while (true) {
        if (x < 8 && x >= 0 && y >= 0 && y < 8 && (plansza[x][y] == Tura || plansza[x][y] == Tura +2))
        {
            zaznaczony_x = x;
            zaznaczony_y = y;
            cout << "Zaznaczono pionka o wspolrzędnych: (x = " << zaznaczony_x << ", y = " << zaznaczony_y << ")\n";
            return std::make_pair(zaznaczony_x, zaznaczony_y);
        }
        cout << " Zaznaczono niewłaściwego pionka \n";
        zaznaczony_x = -1;
        zaznaczony_y = -1;
        pole = kliknij_pole();
        x = pole.first;
        y = pole.second;
    }
	return std::make_pair(zaznaczony_x, zaznaczony_y);
}

Pozycja Plansza::wybierzPoleDocelowe()
{
    pair<int, int> pole = kliknij_pole();
	int x, y;
    x = pole.first;
    y = pole.second;
    while (true) {
        if (x < 8 && x >= 0 && y >= 0 && y < 8 && (plansza[x][y] == PolePuste))
        {
            pole_docelowe_x = x;
            pole_docelowe_y = y;
            cout << "Wybrano pole o wspolrzędnych: (x = " << pole_docelowe_x << ", y = " << pole_docelowe_y << ")\n";
            return std::make_pair(pole_docelowe_x, pole_docelowe_y);
        }
        cout << " Zaznaczono niewłaściwe pole docelowe \n";
        zaznaczony_x = -1;
        zaznaczony_y = -1;
        pole = kliknij_pole();
        x = pole.first;
        y = pole.second;
    }
	return std::make_pair(pole_docelowe_x, pole_docelowe_y);
}

void Plansza::zmianaTury(bool tura)
{
	Tura = !tura; // biały = 0 ; czarny 1;
}


/*
void Plansza::wykonajRuch(bool Tura) {
	std::vector<Ruch> Dostepne_ruchy;
	znajdz_Dostepne_ruchy(Tura, Dostepne_ruchy);
		if (Dostepne_ruchy.empty())
		{
			*this = Plansza();
			return false;
		}
		int Punkty = 0;
		std::vector<Ruch>::iterator best = Dostepne_ruchy.end();
		for (std::vector<Ruch>::iterator ruch = Dostepne_ruchy.begin(); ruch != Dostepne_ruchy.end(); ++ruch)
		{
			Plansza plansza = *this;

		}
}
*/

void Plansza::drukujPlansze(int plansza[8][8]) {
	for (int i = 0; i < 8; i++)
	{
		for (int j = 0; j < 8; j++)
			std::cout << plansza[i][j] << "	";
		std::cout << std::endl;
		cout << endl;
	};
}

void Plansza::drukujRuchy(Ruch dostepne_ruchy) {
	cout << "Gracz " << Tura << endl;
	for (int i = 0; i < dostepne_ruchy.size(); i++)
	{
		cout << "pole startowe x1, y1 " << i << ": " << dostepne_ruchy[i].first.first << ", " << dostepne_ruchy[i].first.second << endl;
		cout << "pole docelowe x2, y2 " << i << ": " << dostepne_ruchy[i].second.first << ", " << dostepne_ruchy[i].second.second << endl;
	}
}

bool Plansza::ruch()
{
	dostepne_ruchy = znajdz_Dostepne_ruchy();
	drukujRuchy(dostepne_ruchy);
	//Najpierw sprawdzamy jakiego piona i gdzie chce gracz przsunac.
	Pozycja wybranyPionek = wybierzPionka();
	Pozycja poleDocelowe = wybierzPoleDocelowe();
	Przesuniecie wskazanePrzesuniecie = Przesuniecie(wybranyPionek, poleDocelowe);
	if (wybranyPionek == make_pair(-1, -1))
		koniec_gry = true; //tymczasowo, wychodzi z petli while jesli blad

	if (dostepne_ruchy.empty())
	{
		//*this = Plansza();
		cout << "Brak dostepnych ruchow.\n";
		koniec_gry = true;
		return true;
	}
	// Inicjalizujemy plansze, jesli nie ma dostepnych ruchów

	bool po_ruchu = false;

	// Sprawdzamy, czy nasze przesunięcie jest na liście dostępnych ruchów 
	for (Ruch::iterator i = dostepne_ruchy.begin(); i != dostepne_ruchy.end(); i++) 
	{
		if ((wskazanePrzesuniecie == *i) && (!kontynuacja_bicia ))// || (kontynuacja_bicia && abs(zaznaczony_x - pole_docelowe_x) == 2))
		{
			//ustawiamy plansze po ruchu
			po_ruchu = true;
			plansza[wybranyPionek.first][wybranyPionek.second] = PolePuste;
			plansza[poleDocelowe.first][poleDocelowe.second] = Tura;
			if (abs(zaznaczony_x - pole_docelowe_x) == 2) // W przypadku gdy nasz ruch to jest bicie
			{
				plansza[(zaznaczony_x + pole_docelowe_x) / 2][(zaznaczony_y + pole_docelowe_y) / 2] = PolePuste; //Bity pionek jest 0
				zaznaczony_x = pole_docelowe_x;
				zaznaczony_y = pole_docelowe_y;
				kontynuacja_bicia = true;
			}
			else kontynuacja_bicia = false;
		}
	}

	if (kontynuacja_bicia || po_ruchu)
	{
		kontynuacja_bicia = false;
	}
    return po_ruchu;

	//zaznaczony_x = -1;
	//zaznaczony_y = -1;
}


int init()
{
    srand( time(NULL) );

    if(!al_init()) {
        cerr << "Błąd podczas inicjalizacji allegro." << endl;
        return -1;
    }

    if (!al_init_primitives_addon()) {
        cerr << "Błąd podczas inicjalizacji dodatku 'primitives'." << endl;
        return -1;
    }

    if(!al_init_image_addon()) {
        cerr << "Błąd podczas inicjalizacji dodatku 'image'." << endl;
        return -1;
    }

    if(!al_install_keyboard()) {
        cerr << "Błąd podczas inicjalizacji klawiatury." << endl;
        return -1;
    }

    if(!al_install_mouse()) {
        cerr << "Błąd podczas inicjalizacji myszki." << endl;
        return -1;
    }

    timer = al_create_timer(1.0 / FPS);
    if(!timer) {
        cerr << "Błąd podczas inicjalizacji zegara." << endl;
        return -1;
    }

    display = al_create_display(map_w, map_h);
    if(!display) {
        cerr << "Błąd podczas inicjalizacji ekranu." << endl;
        al_destroy_timer(timer);
        return -1;
    }

    event_queue = al_create_event_queue();
    if(!event_queue) {
        cerr << "Błąd podczas inicjalizacji kolejki zdarzeń." << endl;
        al_destroy_display(display);
        al_destroy_timer(timer);
        return -1;
    }

    al_register_event_source(event_queue, al_get_display_event_source(display));
    al_register_event_source(event_queue, al_get_timer_event_source(timer));
    al_register_event_source(event_queue, al_get_keyboard_event_source());
    al_register_event_source(event_queue, al_get_mouse_event_source());

    al_clear_to_color(al_map_rgb(0,0,0));

    al_flip_display();
    al_start_timer(timer);

    return 0;
}


int main() {
    if (init() != 0) {
        cerr << "Inicjalizacja nie powiodła się." << endl;
        return -1;
    }

    if (!przygotuj_bitmapy()) {
        exit(1);
    }
    Plansza gra;
    gra.inicjalizuj();
    gra.rysuj_plansze();
   	while (!gra.koniec_gry)
   	{
   		cout << "Plansza :  " << endl;

   		// gra.drukujPlansze(gra.plansza);

   		while (!gra.ruch()) {

        }
   		gra.zmianaTury(gra.Tura);
        gra.rysuj_plansze();
   	}
   	gra.drukujPlansze(gra.plansza);
   	gra.drukujRuchy(gra.dostepne_ruchy);
   	char quit; // Tymczasowa Zmienna słuzy do wyjścia z gry
   	cout << "Wcisnij dowolne klawisz aby wyjsc" << endl;
   	cin >> quit;
   	return 0;
}
