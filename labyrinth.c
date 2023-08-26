#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define MAX_DLUGOSC 1024
char labirynt[MAX_DLUGOSC][MAX_DLUGOSC];		//tablica do przechowywania labiryntu
int labiryntnum[MAX_DLUGOSC][MAX_DLUGOSC];		// tablica do przechowywania numerów grafu w labiryncie
int poczi = -1, poczj = -1, koni = -1, konj = -1;	//zmienne ze współrzędnymi punktów początku i końca
int r, c, V, k;						// r -rozmiar labiryntu (wierszy); c - -||-(kolumny); V - liczba wierzchołków grafu; k - liczba krawędzi
double dlugosc;						// dlugość scieżki
int flag;						// flaga o dotarciu do punktu końcowego
typedef struct Kraw{					// Struktura z informacją o krawędzi
        int pocz, kon;
        double waga;
}kraw_t;

void czytaj_labirynt(FILE *in)
{
	int i, j;
	V = 0;
	if (in != NULL)			//sprawdzanie pliku
	{
		fscanf(in, "%d %d", &r, &c);		//wszytujemy rozmiar labiryntu
		r = r*2 - 1 + 2;				// *2-1 - liczba możliwych przejśc lub scian, +2 bo mamy ściany z dwóch stron
		c = c*2 - 1 + 2;
		i = 0;
		for (i = 0; i < r; i++)
				fscanf(in, "%s", labirynt[i]);          //wczytujemy labirynt
		for (i = 0; i < r; i++)
		{
			for (j = 0; j < c; j++){
				if (labirynt[i][j] == 'S')		//poszukujemy punkt startowy i zapisujemy wspólrzędne
				{
					poczi = i;
					poczj = j;
					labirynt[i][j] = '.';
					V++;
				} else if (labirynt[i][j] == 'K')	// punkt końcowy współrzędne
				{
					koni = i;
					konj = j;
					labirynt[i][j] = '.';
					V++;
				} else if (labirynt[i][j] == '.')
					V++;
				else if (labirynt[i][j] != '-' && labirynt[i][j] != '|' && labirynt[i][j] != '#')
				{
					printf("NAPOTKANO NIEPRAWIDŁOWY SYMBOL NA POZYCJI (%d, %d) LABIRYNTU! PROGRAM ZAKOŃCZONO!\n", i+1, j+1);
					exit(1);
				}
				}
		}
		if (poczi == -1 || poczj == -1 || koni == -1 || konj == -1)	//jeżeli wspólrzędne się nie zmieniły, to nie znaleźliśmy punktów
		{
			printf("NIE ZNALEZIONO STARTU LUB KONCA!\n");
			exit(1);
		}
	} else {
		printf("NIE MOGE PRZECZYTAC PLIKU!\n");
		exit(1);
	}
}
void zapisanie_grafu(kraw_t *kraw)
{
	int i, j;
	int l = 0;
	for (i = 0; i < r; i++) 
		for (j = 0; j < c; j++)
			if (labirynt[i][j] == '.')		//jeżeli w labiryncie napotykamy kropkę, w tablicy numerów grafów pod tą samą pozycją zapisujemy liczbę
			{
				labiryntnum[i][j] = l;
				l++;
			}
	k = -1;
	for (i = 0; i < r; i++)			//zapisujemy informacje o krawędziach
		for (j = 0; j < c; j++)
		{
			if (labirynt[i][j] == '|')
			{ 
				k++;
				kraw[k].pocz = labiryntnum[i-1][j];
				kraw[k].kon = labiryntnum[i+1][j];
				k++;
				kraw[k].pocz = labiryntnum[i+1][j];
				kraw[k].kon = labiryntnum[i-1][j];
			}
			if (labirynt[i][j] =='-')
                        { 
				k++;
                                kraw[k].pocz = labiryntnum[i][j-1];
                                kraw[k].kon = labiryntnum[i][j+1];
				k++;
				kraw[k].pocz = labiryntnum[i][j+1];
				kraw[k].kon = labiryntnum[i][j-1];
                        }
		}
}

void tworzenie_macierzy(double v[][V], kraw_t kraw[])
{
	int i, j, l;
	srand(time(NULL));
	for (i = 0; i <= k; i++)
		for (j = 0; j <= k; j++)
		{
			if (kraw[i].pocz == kraw[j].kon && kraw[i].kon == kraw[j].pocz)		//jezeli napotkamy jednakowe krawędzi, jedna z których ma wagę, przypisujemy wagę do drugiej krawędzi
				if (kraw[i].waga != 0){
					kraw[j].waga = kraw[i].waga;
				} else if (kraw[j].waga != 0){
					kraw[i].waga = kraw[j].waga;
				} else {							//jeżeli nie napotkamy, generujemy wagę
					kraw[i].waga =0.1 + (rand()*(9.9-0.1)/RAND_MAX);
				}
		}
	memset(v, 0, V*V*sizeof(double));		//wypełniamy macierz zerami
        for (i=0; i < V; i++)
                for (j = 0; j < V; j++)
                        for (l = 0; l <= k; l++)
                        if ((kraw[l].pocz == i) && (kraw[l].kon == j))			//zapisujemy wagi w macierz pod odpowiednimi indeksami
                        {
			       	v[i][j] = kraw[l].waga;
                        }
}
int s; 							//długosc tabeli z numerami sciezki
void przeszukaj_labirynt(int n, int v,int odw[], double A[][V], int *sciezka)
{
	int fin = labiryntnum[koni][konj];
	int i;
	odw[v] = 1;
	if (v == labiryntnum[poczi][poczj]){		//działania, gdy element jest startem
		memset(sciezka, 0, V*sizeof(int));
		s = 0;
		sciezka[s] = v;
		flag = 0;
		dlugosc = 0;
	}
	if (v == labiryntnum[koni][konj])		//gdy element jest końcem
	{
		flag = 1;
		return;
	}
	for (i = 0; i < n; i++)
	{
		if ((odw[i] == 0 || i == fin) && (A[v][i] != 0))        //sprawdzamy sąsiedztwo i czy już odwiedzilismy
		{
			dlugosc = dlugosc + A[v][i];
			++s;
			sciezka[s] = i;
			przeszukaj_labirynt(n, i, odw, A, sciezka);     
			if (flag == 1) return;				//jeżeli już napotkaliśmy koniec, to będziemy wychodzić z każdej funkcji
			dlugosc = dlugosc - A[v][i];  			//jeżeli koniec nie napotkaliśmy, to odejmujemy wagę
			--s;		
		}
	}
}	
void wypisywanie_macierzy(double v[][V])
{
	int i,j;
	printf("\t");
        for (i = 0; i < V; i++)
        printf("%d\t", i);
        printf("\n");
        for (i = 0; i < V; i++){
                printf("%d\t", i);
                for (j = 0; j < V; j++){
                        printf("%.1f\t", v[i][j]);
                }
                printf("\n");
        }
}
void wypisywanie_min_sciezki(int minsciezka[], int mins)
{
	int i;
	printf("Najkrótsza ścieżka:\n");
                for (i = 0; i <= mins; i++)
                        printf("%d ", minsciezka[i]);
	printf("\n");
}

int main(int argc, char **argv)
{
	int i;
	if (argc != 2)			//sprawdzamy liczbę argumentów wywołania
	{
		printf("NIEPOPRAWNA LICZBA ARGUMENTÓW WYWOŁANIA! PROSZĘ PODAĆ PLIK Z LABIRYNTEM\n");
		return 1;
	}
	FILE *in= fopen(argv[1], "r");
	czytaj_labirynt(in);			//wczytujemy labirynt
	int kr = 4*2 + ((r - 2)*2 + (c - 2)*2)*3 + (r - 2)*(c - 2)*4;	
	kraw_t kraw[kr];				//tworzymy tablicę krawędzi
	zapisanie_grafu(kraw);			//zapisujemy krawędzi
	double v[V][V];			//tablica sąsiedztwa
	tworzenie_macierzy(v, kraw);		//wypełniamy tablicę sąsiedztwa
//	wypisywanie_macierzy (v);								//WYPISYWANIE MACIERZY SĄSIEDZTWA!!!
	int odw[V];			//tablica odwiedzonych wierzchołków
	int sciezka[V];
	int minsciezka[V];
	double mindlug;
        int mins;
	memset(odw, 0, V*sizeof(int));
	int start = labiryntnum[poczi][poczj];
	przeszukaj_labirynt(V, start, odw, v, sciezka);		//zaczynamy poszuk od elementu startowego
	if (flag == 1)				//jeżeli dotarliśmy do końca, zapisujemy scieżkę i jej długość jako najmniejsze
	{
		mindlug = dlugosc;
		mins = s;
		for (i = 0; i<=s; i++)
			minsciezka[i] = sciezka[i];
	}
	for (i = 0; i < V; i++)			//jeżeli jakiś wierzchołek wciąż nie jest odwiedzony, przeszukujemy jeszcze raz
		if (odw[i]==0){
			przeszukaj_labirynt(V, start, odw, v, sciezka);
			if ((dlugosc < mindlug) && (flag == 1)){		//jeżeli dotarliśmy do końca i długość scieżki jest mniejsza od poprzedniej, zapisujemy jako najmniejszą
				mindlug = dlugosc;
				mins = s;
				for (i = 0; i<=s; i++)
		                        minsciezka[i] = sciezka[i];
			}
		}
	if (abs(mindlug) < 0.0000001)			//jeżeli ścieżka jest blisko 0, to znaczy, że ścieżki od startu do końca nie ma
	{
		printf("NIE ODNALEZIONO SCIEŻKI OD STARTU DO KONCA!\n");
	} else {
//		wypisywanie_min_sciezki(minsciezka, mins);						//WYPISYWANIE NAJKRÓTSZEJ ŚCIEŻKI!!!
		printf("Długość najkrótszej ścieżki wynosi: \n");
		  printf("%.1f\n", mindlug);

	}
	return 0;
}
