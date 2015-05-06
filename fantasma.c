//
//  fantasma.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

#include "header.h"

// Declaration of the labyrinth variable that is in "labirinto.h" file
extern char labirinto[RIGHE][COLONNE];

// Ghosts handling
void funzioneFantasma ( int pipeAlControllo, int pipeDalControllo, int numeroFantasma, coordinata posizioneIniziale, int morto )
{
	// The ghost
	personaggio fantasma;
	// The reverse direction of the ghost
	direzione opposta;
	// The number of seconds the ghost has to wait before it can move
	int secondiDaAspettare = rand() % GENERAZIONE_FANTASMA;
	int creazioneMissile;
	infoFantasmi info;

	// Initialization of the ghost variable
	fantasma.identificatore = 'F';
	fantasma.posizione.x = posizioneIniziale.x;
	fantasma.posizione.y = posizioneIniziale.y;
	fantasma.idPersonaggio = numeroFantasma;
	fantasma.pid = getpid();
	fantasma.morto = morto;
	fantasma.missile = 0;
	fantasma.direzioniDisponibili = calcoloBivio( fantasma.posizione.x, fantasma.posizione.y );
	fantasma.movimento = sceltaPercorso( fantasma.posizione.x, fantasma.posizione.y, fantasma.direzioniDisponibili );	// Sceglie una direzione a caso tra quelle disponibili.

	// The ghost waits for a random time before it can move
	usleep( secondiDaAspettare + UN_SECONDO );

	// Write the ghost in a pipe
	write( pipeAlControllo, &fantasma, sizeof( fantasma ) );

	while ( true ) 
	{
		// Read in a pipe
		read( pipeDalControllo, &info, sizeof( infoFantasmi ) );
		
		// If the player got a power pill, the ghost will stop for three seconds
		if ( info.powerPill == 1 )
		{	
			sleep( 3 );	
		}

		// If the ghost finds another one in his path, it will reverse its direction
		if ( info.rimbalzo == 1 ) 
		{
			fantasma.movimento = calcoloDirezioneOpposta( fantasma.movimento );
			fantasma.direzioniDisponibili = 0;	// Per questo ciclo non deve muoversi.
			info.rimbalzo = 0;
		}

		// The ghost moves according to the number of possible paths
		switch ( fantasma.direzioniDisponibili )
		{		
			case 0:
				break;
				
			// If only one direction is possible, it means that the ghost is in a dead end, so it reverse its direction
			case 1:
				fantasma.movimento = calcoloDirezioneOpposta( fantasma.movimento );
				fantasma.posizione.x += fantasma.movimento.direzionex;
				fantasma.posizione.y += fantasma.movimento.direzioney;
				break;

			// With two possible directions the ghost could be in a lane or in a curve
			case 2:
				// If it is on a lane, check if there is a wall in front of it. If there is not, the ghost moves forward.
				if ( posizioneSuccessivaConsentita( fantasma.posizione.x + fantasma.movimento.direzionex, fantasma.posizione.y + fantasma.movimento.direzioney ) == 1 )
				{
					fantasma.posizione.x += fantasma.movimento.direzionex;
					fantasma.posizione.y += fantasma.movimento.direzioney;

					break;
				}

			// With three or more possible directions
			default:

				opposta = calcoloDirezioneOpposta( fantasma.movimento );

				// A new path is generate until it is different from the opposite of the current direction,
				// then it moves
				do
				{
					fantasma.movimento = sceltaPercorso( fantasma.posizione.x, fantasma.posizione.y, fantasma.direzioniDisponibili );
				} 
				while ( fantasma.movimento.direzionex == opposta.direzionex && fantasma.movimento.direzioney == opposta.direzioney );

				if ( posizioneSuccessivaConsentita( fantasma.posizione.x + fantasma.movimento.direzionex, fantasma.posizione.y + fantasma.movimento.direzioney ) == 1 )
				{
					fantasma.posizione.x += fantasma.movimento.direzionex;
					fantasma.posizione.y += fantasma.movimento.direzioney;
				}

				break;
		}

		// Random generation of a missile
		creazioneMissile = rand() % GENERAZIONE_SPARO;
		
		if ( creazioneMissile <= PERCENTUALE_CREAZIONE_SPARO )
		{
			fantasma.missile = 1;
		}

		// Writes on a pipe the update position of the ghost
		write( pipeAlControllo, &fantasma, sizeof( fantasma ) );

		fantasma.missile = 0;
		
		// Waits before move the ghost again
		usleep( VELOCITA_FANTASMI );
		
		// Compute the number possible directions from the current position 
		fantasma.direzioniDisponibili = calcoloBivio( fantasma.posizione.x, fantasma.posizione.y );
	}
}

// Prints the ghosts
void stampaFantasma ( personaggio fantasma )
{
	attron( COLOR_PAIR( ( fantasma.idPersonaggio % FANTASMI ) + 1 ) );

	mvaddch( fantasma.posizione.y + 2, fantasma.posizione.x, ACS_PI );

	attroff( COLOR_PAIR( ( fantasma.idPersonaggio % FANTASMI ) + 1 ) );
}

// Generates a ghost
void generaFantasma ( int pipeAlControllo, int pipeDalControllo, int numeroFantasmi, coordinata posizioneIniziale, int morto )
{
	switch ( fork() )
	{
		case -1:
			perror( "ERRORE 008: la fork non e' stata eseguita correttamente." );
			exit( 1 );
		case 0:
			funzioneFantasma( pipeAlControllo, pipeDalControllo, numeroFantasmi, posizioneIniziale, morto );
	}
}

int trovaPipe ( int matrice[][2], int numeroFantasma )
{
	return matrice[numeroFantasma][READ];
}

// Check if the selected position if allowed
int posizioneSuccessivaConsentita ( int x, int y )
{
	switch ( labirinto[y][x] )
	{
		case ' ':
		case 'P':
		case '.':
			return 1;

		default:
			return 0;
	}
}

// Compute the number of possible direction from the selected position
int calcoloBivio ( int x, int y )
{
	int i = 0;

	if ( posizioneSuccessivaConsentita( x + 2, y ) == 1 )
	{
		i++;
	}
	if ( posizioneSuccessivaConsentita( x - 2, y ) == 1 )
	{
		i++;
	}
	if ( posizioneSuccessivaConsentita( x, y + 1 ) == 1 )
	{
		i++;
	}
	if ( posizioneSuccessivaConsentita( x, y - 1 ) == 1 )
	{
		i++;
	}

	return i;
}

// Establishes the new direction of the ghost
direzione sceltaPercorso( int x, int y, int direzioniDisponibili )
{
	int i = 0, r;
	// An array with the possible directions
	direzione percorsoPossibile[direzioniDisponibili];

	// We check all the directions and, if a direction is possible, then we insert it in the array
	if ( posizioneSuccessivaConsentita( x + 2, y ) == 1 )
	{
		percorsoPossibile[i].direzionex = 2;
		percorsoPossibile[i].direzioney = 0;
		i++;
	}

	if ( posizioneSuccessivaConsentita( x - 2, y ) == 1 )
	{
		percorsoPossibile[i].direzionex = -2;
		percorsoPossibile[i].direzioney = 0;
		i++;
	}

	if ( posizioneSuccessivaConsentita( x, y + 1 ) == 1 )
	{
		percorsoPossibile[i].direzionex = 0;
		percorsoPossibile[i].direzioney = 1;
		i++;
	}

	if ( posizioneSuccessivaConsentita( x, y - 1 ) == 1 )
	{
		percorsoPossibile[i].direzionex = 0;
		percorsoPossibile[i].direzioney = -1;
		i++;
	}

	// Chooses a random direction
	r = rand() % i;

	return percorsoPossibile[r];
}

// Compute an opposite direction of a given one
direzione calcoloDirezioneOpposta ( direzione attuale )
{
	direzione opposta;

	if ( attuale.direzionex != 0 )
	{
		opposta.direzioney = 0;

		if ( attuale.direzionex == 2 ) 
		{
			opposta.direzionex = -2;
		} 
		else 
		{
			opposta.direzionex = 2;
		}
	} 
	else 
	{
		opposta.direzionex = 0;

		if ( attuale.direzioney == 1 ) 
		{
			opposta.direzioney = -1;
		} 
		else 
		{
			opposta.direzioney = 1;
		}
	}

	return opposta;
}
