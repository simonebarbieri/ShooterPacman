//
//  pacman.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

#include "header.h"

// Pacman handling
void funzionePacman ( int pipeAlControllo )
{	
	personaggio pacman;
	char c;
	
	// Initialization of the pacman variable
	pacman.identificatore = 'P';
	pacman.posizione.x = COLONNA_INIZIALE_PACMAN;
	pacman.posizione.y = RIGA_INIZIALE_PACMAN;
	pacman.movimento.direzionex = -1;
	pacman.movimento.direzioney = 0;
	pacman.animazione = 0;
	pacman.pid = getpid();
	pacman.quit = 0;
	pacman.mosso = 0;
	
	// Writes pacman in a pipe
	write( pipeAlControllo, &pacman, sizeof( pacman ) );
	
	while ( true )
	{	
		// Saves in the "c" variable the pressed key
		scanf( "%c", &c );
		
		switch ( c )
		{
			// If a direction is pressed, pacman will move in that direction	
			case SU:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x, pacman.posizione.y - 1 ) == 1 ) 
				{
					pacman.posizione.y -= 1;
					pacman.movimento.direzionex = 0;
					pacman.movimento.direzioney = -1;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;
				}
				
				break;
				
			case GIU:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x, pacman.posizione.y + 1 ) == 1 )
				{	
					pacman.posizione.y += 1;
					pacman.movimento.direzionex = 0;
					pacman.movimento.direzioney = 1;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;	
				}
				
				break;
				
			case DESTRA:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x + 2, pacman.posizione.y ) == 1 )
				{	
					pacman.posizione.x += 2;
					pacman.movimento.direzionex = 2;
					pacman.movimento.direzioney = 0;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;	
				}
				
				break;
				
			case SINISTRA:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x - 2, pacman.posizione.y ) == 1 )
				{	
					pacman.posizione.x -= 2;
					pacman.movimento.direzionex = -2;
					pacman.movimento.direzioney = 0;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;	
				}
				
				break;
				
			// If space is pressed, a missile will be shot in every possible direction
			case SPAZIO:
				pacman.animazione = 0;
				pacman.missile = 1;
				break;
				
			// If "Q" is pressed, the game will quit
			case 'Q':
			case 'q':
				pacman.quit = 1;
				break;
				
		}
		
		pacman.mosso++;
		
		// Writes in the pipe the updated pacman's position
		write( pipeAlControllo, &pacman, sizeof( pacman ) );
		
		pacman.missile = 0;
	}
}

// Prints pacman
void stampaPacman ( personaggio pacman )
{		
	if ( pacman.movimento.direzionex != 0 )
	{	
		if ( pacman.animazione == 0 )
		{	
			if ( pacman.movimento.direzionex == 2 )
			{	
				 mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_LARROW );	
			} 
			else 
			{	
				mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_RARROW );	
			}
		} 
		else 
		{	
			mvaddch( pacman.posizione.y + 2, pacman.posizione.x, '-' );	
		}
	} 
	else 
	{	
		if ( pacman.animazione == 0 )
		{	
			if ( pacman.movimento.direzioney == 1 )
			{	
				mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_UARROW );	
			} 
			else 
			{	
				mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_DARROW );	
			}
		} 
		else 
		{	
			mvaddch( pacman.posizione.y + 2, pacman.posizione.x, '|' );	
		}
	}
}

// Generates pacman
void generaPacman ( int pipeAlControllo[] )
{	
	switch ( fork() )
	{		
		case -1:
			perror( "ERRORE 007: la fork non e' stata eseguita correttamente." );
			exit( 1 );		
		case 0:
			funzionePacman( pipeAlControllo[WRITE] );
	}
}
