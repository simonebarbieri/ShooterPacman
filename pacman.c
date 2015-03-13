//
//  pacman.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

#include "header.h"

/***********************************************************************************
 /																					/
 /	Gestione di Pac-Man.															/
 /																					/
 ***********************************************************************************/
void funzionePacman ( int pipeAlControllo ){
	
	personaggio pacman;	// La variabile dove e' salvato Pac-Man
	char c;
	
	// Vengono inizializzati tutti i campi della struttura pacman.
	
	pacman.identificatore = 'P';
	pacman.posizione.x = COLONNA_INIZIALE_PACMAN;
	pacman.posizione.y = RIGA_INIZIALE_PACMAN;
	pacman.movimento.direzionex = -1;
	pacman.movimento.direzioney = 0;
	pacman.animazione = 0;
	pacman.pid = getpid();
	pacman.quit = 0;
	pacman.mosso = 0;
	
	write( pipeAlControllo, &pacman, sizeof( pacman ) );	// Scrive sulla pipe la posizione iniziale di Pac-Man
	
	while ( 1 ) {
		
		scanf( "%c", &c );	// Salva nella variabile "c" il tasto premuto.
		
		// Se viene selezionato un tasto direzionale, Pac-Man si muove nella direzione selezionata.
		switch ( c ) {
			
			case SU:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x, pacman.posizione.y - 1 ) == 1 ) {
					
					pacman.posizione.y -= 1;
					pacman.movimento.direzionex = 0;
					pacman.movimento.direzioney = -1;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;
					
				}
				
				break;
				
			case GIU:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x, pacman.posizione.y + 1 ) == 1 ) {
					
					pacman.posizione.y += 1;
					pacman.movimento.direzionex = 0;
					pacman.movimento.direzioney = 1;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;
					
				}
				
				break;
				
			case DESTRA:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x + 2, pacman.posizione.y ) == 1 ) {
					
					pacman.posizione.x += 2;
					pacman.movimento.direzionex = 2;
					pacman.movimento.direzioney = 0;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;
					
				}
				
				break;
				
			case SINISTRA:
				if ( posizioneSuccessivaConsentita( pacman.posizione.x - 2, pacman.posizione.y ) == 1 ) {
					
					pacman.posizione.x -= 2;
					pacman.movimento.direzionex = -2;
					pacman.movimento.direzioney = 0;
					pacman.animazione = ( pacman.animazione + 1 ) % 2;
					
				}
				
				break;
				
			// Se viene premuta la barra spaziatrice, viene sparato un missile in tutte le direzioni possibili.
				
			case SPAZIO:
				pacman.animazione = 0;
				pacman.missile = 1;
				break;
				
			case 'Q':
			case 'q':
				pacman.quit = 1;
				break;
				
		}
		
		pacman.mosso++;
		
		write( pipeAlControllo, &pacman, sizeof( pacman ) );	// Scrive nella pipe la posizione aggiornata di Pac-Man
		
		pacman.missile = 0;
				
	}
	
}

/***********************************************************************************
 /																					/
 /	Stampa Pac-Man.																	/
 /																					/
 ***********************************************************************************/
void stampaPacman ( personaggio pacman ){
		
	if ( pacman.movimento.direzionex != 0 ) {
		
		if ( pacman.animazione == 0 ){
			
			if ( pacman.movimento.direzionex == 2 ) {
				
				 mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_LARROW );
				
			} else {
				
				mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_RARROW );
				
			}
			
		} else {
			
			mvaddch( pacman.posizione.y + 2, pacman.posizione.x, '-' );
			
		}
		
	} else {
		
		if ( pacman.animazione == 0 ){
			
			if ( pacman.movimento.direzioney == 1 ) {
				
				mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_UARROW );
				
			} else {
				
				mvaddch( pacman.posizione.y + 2, pacman.posizione.x, ACS_DARROW );
				
			}
			
		} else {
			
			mvaddch( pacman.posizione.y + 2, pacman.posizione.x, '|' );
			
		}
		
	}
	
}

/***********************************************************************************
 /																					/
 /	Funzione che genera Pac-Man.													/
 /																					/
 ***********************************************************************************/
void generaPacman ( int pipeAlControllo[] ){
	
	switch ( fork() ) {
			
		case -1:
			perror( "ERRORE 007: la fork non e' stata eseguita correttamente." );
			exit( 1 );
			
		case 0:
			funzionePacman( pipeAlControllo[WRITE] );
			
	}
	
}
