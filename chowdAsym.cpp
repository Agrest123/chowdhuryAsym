#include "stdafx.h"

using namespace std;

class automat {
	double W;	//Dlugosc drogi w metrach
	double ceil_len; //Długość komórki w metrach
	int numb_ceil; //ilość komórek 
	bool isChecked; //flaga , sprawdzamy czy dany pojazd byl aktualizowany podczas zmiany pasa
public:
	class pojazd
	{
	public:
		class element {
		public:
			pojazd* parent;
			int index;
		
			element(pojazd *p, int i) {
				parent = p;
				index = i;
			}
			void wyswietl()
			{
				cout << parent->V; //edit !
			}
		};

	public:
		int V; //aktualna predkosc pojazdu
		int Vmax;// maksymalna predkosc pojazdu 
		bool checked;// flaga sprawdzamy czy dany pojazd byl aktualizowany podczas zmiany pasa 
		bool is;		// czy wszystkie elementy pojazdu są już na drodze
		int carLen; //dlugosc pojazdu
		element** children;

		pojazd(bool isChecked, int ceil_len, element* &road)
		{
			Vmax = rand() % 3 + 1; // losowanie max predkosc w przedziale od 1 - 2
			carLen = static_cast<int>(ceil(static_cast<double>(rand() % 3 + 2) / ceil_len)); // losowanie dlugosci pojazu w metrach zamienione na komorki
			children = new element*[carLen];
			for (int i = 0; i < carLen; i++) {
				children[i] = new element(this, i);
			}
			this->V = rand() % Vmax + 1; //losowanie aktualnej predkosci
			this->checked = !isChecked;
			this->is = false;

			road = children[0];

		}
		void speedup() {
			if (V < Vmax) {
				V++;
			}
		}
		void speeddown() {
			if (V > 1) {
				V--;
			}
		}
	};
	typedef pojazd::element element;
	automat(int nW = 200, double nCL = 7.5) {
		W = nW;
		ceil_len = nCL;
		numb_ceil = static_cast<int>(ceil(W /ceil_len));
		isChecked = false;
	}

	void automat_start() {
		srand(static_cast<unsigned int>(time(NULL)));
		
		element ***road = new element**[2];
		road[0] = new element*[numb_ceil];
		road[1] = new element*[numb_ceil];
		for (int i = 0; i < 2; i++) { for (int j = 0; j < numb_ceil; j++) { road[i][j] = NULL; } }
		
		int nr = 0;	// numer pojazdu
		int flaga = 0;// odstęp między wypuszczeniem kolejnego pojazdu
		// max liczba pojazdów
		while (nr<50) {
			// jeżeli jest wolne moejsce na początku 2-go pasa
			if (road[1][0] == NULL) {
				if (flaga == 0)
				{
					flaga = 4;
					nr++;
					generator(road);
				}
				else flaga--;
			}
			wyswietlanie(road);
			odswiezanie(road);
			Sleep(300);
		}
		cout << endl;
		system("PAUSE");
		delete[]road;
		return;
	}
	void generator(element ***&road)
	{
		try {
			new pojazd(isChecked, ceil_len, road[1][0]);
		}
		catch (bad_alloc) {
			road[1][0] = NULL;
		}
	}

void wyswietlanie(element ***&d) {
	system("cls");
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < numb_ceil; j++) {
			if (d[i][j] == NULL)
			{
				cout << '-';
			}
			else {
				d[i][j]->wyswietl();
			}
		}
		cout << endl;
	}

}
// Zlicza gapy - odleglosc luki przed pojazdem
int gap(element ***d, int i, int j) {
	element *t = d[i][j];
	int g = 0;
	while (j + g + 1 < numb_ceil && d[i][j + 1 + g] == NULL) { g++; }
	return g;
}
// Zlicza gapy na drugim pasie - odleglosc luki do tylu
bool gap_back(element ***d, int i, int j) {
	element *t = d[i][j];
	int g = 0;
	while (j - g - 1 >= 0 && d[i][j - g - 1] == NULL) { g++; }
	if (j == g) return true;// brak pojazdow na pasie
	if (d[i][j - g - 1]->parent->Vmax < g) return true;
	return false;
}

void odswiezanie(element ***&d)
{
	element *t;
	pojazd *p;
	int x, y;
	/*zmiana pasa*/
	
	for (int i = 0, l = 1; i < 2; i++, l--) {
		for (int j = 0; j < numb_ceil; j++) {
			t = d[i][j];
			x = y = 0;
			if (t != NULL && t->parent->checked != isChecked)
			{
				p = t->parent;
				p->checked = isChecked; //zmiana co cykl flagi
				x = gap(d, i, j + t->index); // x - dlugosc luki
				bool empty = true;
				if (x < p->V + 1 || i==0) {
					for (int k = 0; k < t->index + 1; k++) {
						if (d[l][j + k] != NULL) {
							empty = false;
						}
					}
					if (empty)
					{
						d[l][j] = t; //przeniesienie ogona
						if (gap_back(d, l, j)) {
							d[i][j] = NULL;
							for (int k = 1; k < t->index + 1; k++) {
								d[l][j + k] = d[i][j + k];
								d[i][j + k] = NULL;
							}
						}
						else {
							d[l][j] = NULL;
						}
					}
				}
			}
		}
	}
	isChecked = !isChecked;
	
	element ***tmp = new element**[2];
	tmp[0] = new element*[numb_ceil];
	tmp[1] = new element*[numb_ceil];
	for (int i = 0; i < 2; i++) { for (int j = 0; j < numb_ceil; j++) { tmp[i][j] = NULL; } }

	//implementacja asymetryczny chowdhury

	for (int i = 0; i < 2; i++)
	{
		for (int j = 0; j < numb_ceil; j++)
		{
			t = d[i][j];
			if (t != NULL)
			{
				p = t->parent;
				if (p->checked != isChecked) {
					p->checked = isChecked;
					y = gap(d, i, j + t->index);// y - dlugosc luki
					if (y > p->V) { p->speedup(); }
					else if (y > 0) { p->V = y; }
					else { p->V = 1; }
					if (rand() % 100 < 25) { p->speeddown(); }//prawdopodobienstwo hamowania
				}
				x = j + p->V; //przyszla pozycja
				if (x < numb_ceil)
				{
					tmp[i][x] = t;
					//wstawiamy pominiete wczesniej 'elementy' pojazdu
					if (p->is == false)
					{
						for (int k = 1; t->index + k < p->carLen; k++) {
							if (x - k >= 0) {
								tmp[i][x - k] = p->children[t->index + k];
								if (tmp[1][x - k] == p->children[p->carLen - 1]) {
									p->is = true;
								}
							}
						}
						
					}
				}
				else { delete t; }
				d[i][j] = NULL;
			}
		}
	}
	delete[]d;
	d = tmp;
	tmp = NULL;
	isChecked = !isChecked;
}

};

int main()
{
	automat* automat1 = new automat;
	automat1->automat_start();
	delete automat1;
}
