//
//  missile.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 19/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

#include "header.h"

// The missile has a copy of the labyrinth updated up to the moment the fork() is executed
extern char labirinto[RIGHE][COLONNE];

// Missiles handling
void funzioneMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore )
{	
	personaggio missile;
	int err;
	
	// Initialization of the missile variable
	missile.identificatore = 'M';
	missile.movimento = direzioneMissile;
	missile.posizione.x = posizioneIniziale.x + missile.movimento.direzionex;
	missile.posizione.y = posizioneIniziale.y + missile.movimento.direzioney;
	missile.idPersonaggio = idMissile;
	missile.pid = getpid();
	missile.morto = 0;
	missile.creatore = creatore;
	
	// Write the missile in a pipe
	write( pipeAlControllo, &missile, sizeof(missile) );
	
	usleep( VELOCITA_MISSILI );
	
	// Compute the new position of the missile until it reach a not allowed position 
	while ( posizioneSuccessivaConsentitaMissili( missile.posizione.x + missile.movimento.direzionex, missile.posizione.y + missile.movimento.direzioney ) )
	{	
		missile.posizione.x += missile.movimento.direzionex;
		missile.posizione.y += missile.movimento.direzioney;
		
		// Writes the updated missile's position
		write( pipeAlControllo, &missile, sizeof(missile) );
		
		usleep( VELOCITA_MISSILI );	
	}
	
	missile.morto = 1;
	
	write( pipeAlControllo, &missile, sizeof(missile) );
	
	err = kill( missile.pid, 1);
	
	while ( err != 0 )
	{	
		err = kill( missile.pid, 1 );	
	}
}

// Check if the missile can move in the selected position
int posizioneSuccessivaConsentitaMissili ( int x, int y )
{
	switch ( labirinto[y][x] )
	{		
		case ' ':
			return 1;
		default:
			return 0;		
	}
}

// Generates a missile
void generaMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore )
{	
	switch ( fork() )
	{		
		case -1:
			perror( "ERRORE 009: la fork non e' stata eseguita correttamente.");
			exit( 1 );		
		case 0:
			funzioneMissile( pipeAlControllo, direzioneMissile, posizioneIniziale, idMissile, creatore );
	}
}
