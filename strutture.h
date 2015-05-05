//
//  strutture.h
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

// Coordinate structure
typedef struct 
{
	int y; // Row
	int x; // Column
} coordinata;

// Direction structure
typedef struct 
{
	int direzioney; // Vertical direction
	int direzionex; // Horizontal direction
} direzione;

// Character structure: pacman, ghosts or missiles
typedef struct 
{
	char identificatore;
	char creatore;			// Missiles only, identify the creator, to know who to kill
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

// Ghost informations structure
typedef struct 
{
	int rimbalzo;
	int powerPill;	
} infoFantasmi;

// Missile list structure
typedef struct nodo *list_pointer;
typedef struct nodo
{
	personaggio elemento;
	list_pointer link;
} elemento;
