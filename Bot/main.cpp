#include <Windows.h>
#include <iostream>
#include <string>
#include <vector>

#define ANTI_LAG 2500

using namespace std;

void leftClick()
{
	//Left down.
	INPUT Input = {0};
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTDOWN;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(50);

	//Left up.
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_MOUSE;
	Input.mi.dwFlags = MOUSEEVENTF_LEFTUP;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(50);
}

void pleinEcran()
{
	//CTRL down.
	INPUT Input = {0};
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = VK_LCONTROL;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(50);

	//F down.
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = 0x46; //F.
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(50);

	//CTRL up.
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = VK_LCONTROL;
	Input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(50);

	//F down.
	ZeroMemory(&Input, sizeof(INPUT));
	Input.type = INPUT_KEYBOARD;
	Input.ki.wVk = 0x46; //F.
	Input.ki.dwFlags = KEYEVENTF_KEYUP;
	SendInput(1, &Input, sizeof(INPUT));
	Sleep(50);
}

int main(int argc, char** argv)
{
	string nomCompte = "";

	do
	{
		cout << "Entrez le nom de votre compte : " << endl;
		cin >> nomCompte;
		cout << endl << endl;
	}
	while(nomCompte == "");

	HWND fenetre = FindWindow(NULL, nomCompte.c_str());
	//Diaporama de la Visionneuse de photos
	if(fenetre == NULL)
	{
		cout << "Fenetre introuvable." << endl;
		Sleep(2000);
		return 1;
	}

	ShowWindow(fenetre, SW_MAXIMIZE);
	SetForegroundWindow(fenetre);
	SetFocus(fenetre);

	//Sleep(2000);

	/*Sleep(2000);
	SetCursorPos(960, 540);
	Sleep(1000);
	COLORREF col = GetPixel(GetDC(fenetre), 960, 540);
	Sleep(5000);*/
	/*int attente = 0;
	cout << "testtt" << endl;
	cin >> attente;*/

	RECT dimensionFenetre;
	RECT positionFenetre;
	GetClientRect(fenetre, &dimensionFenetre);
	GetWindowRect(fenetre, &positionFenetre);
	
	//Mise en plein écran.
	if(positionFenetre.left != 0
	&& positionFenetre.top != 0)
		pleinEcran();
	//Fin mise en plein écran.
	
	//Détermination des dimensions de l'écran de jeu.
	int largeurBandeNoire = 0;
	while(GetPixel(GetDC(fenetre), largeurBandeNoire, dimensionFenetre.bottom - 1) == 0 && largeurBandeNoire < dimensionFenetre.right - 1)
		largeurBandeNoire++;
	if(largeurBandeNoire >= dimensionFenetre.right - 1)
		largeurBandeNoire = 0;
	//Fin détermination des dimensions de l'écran de jeu.

	/*
	Avant de faucher, on vérifie qu'on est pas en combat.

	Si on est en combat, on combat jusqu'à temps de gagner.
	Pour vérifier qu'on est en combat, on regarde si le bouton "passer son tour est allumé" en rouge.
	On lance une attaque en choisissant le sort, puis en cliquant sur les personnages qui ont une barre de vie bleue.
	On termine en cliquant sur fermer.

	Si on est pas en combat, on cherche une case à faucher.
	Pour trouver une case à faucher, on déplace la souris jusqu'à temps de trouver un curseur "faux".
	*/

	bool combat = false;

	int largeurJeu = dimensionFenetre.right - 2 * largeurBandeNoire; //Il y a 14 cases en largeur. les bandes noires ne peuvent se trouver qu'à gauche/droite en fullscreen à cause du format des écrans.
	int hauteurJeu = dimensionFenetre.bottom - (dimensionFenetre.bottom / 7.2); //Il y a 19 cases en hauteur. L'interface mesure environ 1/7.2 de la hauteur de la fenetre.
		
	int decalageX = (int)(largeurJeu / 14 / 2);
	int decalageY = (int)(hauteurJeu / 19 / 2);

	float echelle = largeurJeu / 1350.0; //1350 pixels est le nombre de pixels du jeu en plein écran sur une résolution 1920*1080.

	for(int cursorX = 0; cursorX < largeurJeu; cursorX += decalageX) 
	{
		for(int cursorY = 0; cursorY < hauteurJeu; cursorY += decalageY) 
		{
			//COLORREF col = GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu / 2 + 1, hauteurJeu - 1);

			if(GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu / 2 + 1, hauteurJeu - 1)) >= 252) //Si on est en combat.
			{
				cout << "Combat." << endl;

				vector<POINTS> sorts(2);

				sorts[0].x = 7; sorts[0].y = 1;
				sorts[1].x = 6; sorts[1].y = 1;

				//Début du déroulement du combat (le personnage ne se déplace pas durant le combat, il attaque seulement les ennemis à portée).
				SetCursorPos(largeurBandeNoire + largeurJeu / 2, 1050 * echelle);
				Sleep(75);
				leftClick(); //On indique que l'on est prêt.
				

				do
				{
					Sleep(ANTI_LAG); //Attente de la fin de l'animation.

					do
					{
						Sleep(250);
					}
					while(GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu / 2 + 1, hauteurJeu - 1)) < 252); //On attend notre tour.

					for(int nSort = 0; nSort < sorts.size(); nSort++) //On choisi le sort, puis on le lance sur chaque adversaire.
					{
						/*
						NOTES :
						
						Le décalage dépend du type d'adversaire : normal ou invocation.
						Le type d'adversaire détermine la taille de la case qu'il occupe dans la liste.
						La variable "numeroAdversaire" serait donc remplacée par une variable "decalage" qui contient le nombre de pixels (qu'on multiplie au final par l'échelle).

						On commence nos recherches en bas.
						BOUCLE tant qu'on est dans l'espace de jeu.
						{
							Si on trouve le pixel du bas, on clique et on se décale de la taille d'une grande case.
							Si on ne trouve pas le pixel, on regarde 13 pixels au dessus.
							Si on trouve ce nouveau pixel, on clique et on se déplace de la taille d'une petite case.
						}

						OPTIMISATION
						Si on trouve ni l'un ni l'autre, c'est sans doute qu'il s'agit de notre personnage.
						On peut aussi passer une variable booléen à false quand on ne trouve personne, et stopper la boucle quand on ne retrouve personne avec la variable à false.
						*/

						//Sélection de la cible.
						int numeroAdversaire = 0;

						while(largeurJeu - (85 + 69 * numeroAdversaire) * echelle > 0) //Tant qu'on est dans l'espace de jeu.
						{
							/*if(GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu / 2 + 1, hauteurJeu - 1)) < 252)
								break;*/

							if((GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 900 * echelle)) > 200
							&& GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 900 * echelle)) < 212
							&& GetGValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 900 * echelle)) > 190
							&& GetGValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 900 * echelle)) < 204)
							||
							(GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 887 * echelle)) > 200
							&& GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 887 * echelle)) < 212
							&& GetGValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 887 * echelle)) > 190
							&& GetGValue(GetPixel(GetDC(fenetre), largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire) * echelle, 887 * echelle)) < 204))
							{
								//Sélection du sort.
								SetCursorPos(largeurBandeNoire + (830 + 49 * sorts[nSort].x) * echelle, (965 + 49 * sorts[nSort].y) * echelle); //830;965 est la coordonnée du premier sort. 49 est la taille d'une case de sort.
								Sleep(250);
								leftClick();
								//Fin sélection du sort.

								//Déplacement du curseur pour question d'affichage au survol.
								SetCursorPos(1, 1);
								Sleep(250);
								//Fin déplacement du curseur pour question d'affichage au survol.

								SetCursorPos(largeurBandeNoire + largeurJeu - (85 + 69 * numeroAdversaire), 870 * echelle);
								Sleep(250);
								leftClick(); //On clique sur la cible.
							}

							numeroAdversaire++;
						}
						//Fin sélection de la cible.

						//Sélection des invocations.
						
						//Fin sélection des invocations.
					}

					SetCursorPos(largeurBandeNoire + 645 * echelle, 1045 * echelle);
					Sleep(250);
					leftClick(); //On termine notre tour.

					//Déplacement du curseur pour question d'affichage au survol.
					SetCursorPos(1, 1);
					Sleep(250);
					//Fin déplacement du curseur pour question d'affichage au survol.
				}
				while(GetRValue(GetPixel(GetDC(fenetre), largeurBandeNoire + 645 * echelle, 1045 * echelle)) >= 253); //On continue tant que le bouton "passer son tour" est affiché.
				
				SetCursorPos(largeurBandeNoire + largeurJeu / 2, 780 * echelle);
				Sleep(75);
				leftClick(); //On clique sur le bouton "Terminer".

				//Fin du déroulement du combat.
			}

			//Vérification du poids disponible dans le sac.

			SetCursorPos(cursorX + largeurBandeNoire, cursorY);
			Sleep(75);
			if(GetPixel(GetDC(fenetre), cursorX + largeurBandeNoire, cursorY) == 0 //Si une faux apparait (= si on peut faucher).
			&& GetPixel(GetDC(fenetre), cursorX + largeurBandeNoire + 1, cursorY + 1) == 0xFFFFFF) //I don't know why this second condition doesn't work ...
			{
				leftClick();
				SetCursorPos(cursorX + largeurBandeNoire + 15*echelle, cursorY + 40*echelle);
				Sleep(75);
				if(GetRValue(GetPixel(GetDC(fenetre), cursorX + largeurBandeNoire + 15*echelle, cursorY + 40*echelle)) >= 254)
				{
					leftClick();
					cout << "Fauche" << endl;
					Sleep(2500);
				}
				//Sleep(1000);
			}
		}
	}

	Sleep(2000);
	return 0;
}