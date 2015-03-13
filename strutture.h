//
//  strutture.h
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

// Struttura per le coordinate
typedef struct {

	int y; // Riga
	int x; // Colonna

} coordinata;

// Struttura per la direzione
typedef struct {

	int direzioney; // Movimento verticale
	int direzionex; // Movimento orizzontale

} direzione;

// Struttura per i personaggi: Pac-Man, i fantasmi o i missili
typedef struct {

	char identificatore;
	char creatore;			// Solo per i missili, identifica se e' stato creato da un fantasma o da Pac-Man, per sapere chi deve uccidere.
	coordinata posizione;
	direzione movimento;
	pid_t pid;
	int animazione;
	int idPersonaggio;
	int missile;
	int morto;
	int quit;
	int mosso;
	int direzioniDisponibili;

} personaggio;

typedef struct {
	
	int rimbalzo;
	int powerPill;
	
} infoFantasmi;

// Implementazione della lista per i missili
typedef struct nodo *list_pointer;
typedef struct nodo {

	personaggio elemento;
	list_pointer link;

} elemento;
