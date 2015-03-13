//
//  missile.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 19/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

#include "header.h"

extern char labirinto[RIGHE][COLONNE];	// Il missile vede il labirinto aggiornato fino al momento in cui si fa la fork(). Se si aggiorna il labirinto successivamente, il missile non vede la modifica.

/***********************************************************************************
 /																					/
 /	Funzione che gestisce i missili.												/
 /																					/
 ***********************************************************************************/
void funzioneMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore ){
	
	personaggio missile;
	int err;
	
	missile.identificatore = 'M';
	missile.movimento = direzioneMissile;
	missile.posizione.x = posizioneIniziale.x + missile.movimento.direzionex;
	missile.posizione.y = posizioneIniziale.y + missile.movimento.direzioney;
	missile.idPersonaggio = idMissile;
	missile.pid = getpid();
	missile.morto = 0;
	missile.creatore = creatore;
	
	write( pipeAlControllo, &missile, sizeof(missile) );
	
	usleep( VELOCITA_MISSILI );
	
	while ( posizioneSuccessivaConsentitaMissili( missile.posizione.x + missile.movimento.direzionex, missile.posizione.y + missile.movimento.direzioney ) ) {
		
		missile.posizione.x += missile.movimento.direzionex;
		missile.posizione.y += missile.movimento.direzioney;
		
		write( pipeAlControllo, &missile, sizeof(missile) );
		
		usleep( VELOCITA_MISSILI );
		
	}
	
	missile.morto = 1;
	
	write( pipeAlControllo, &missile, sizeof(missile) );
	
	err = kill( missile.pid, 1);
	while ( err != 0 ) {
		
		err = kill( missile.pid, 1 );
		
	}
	
}

/***********************************************************************************
 /																					/
 /	Controlla se il missile puo' muoversi nella casella selezionata.				/
 /																					/
 ***********************************************************************************/
int posizioneSuccessivaConsentitaMissili ( int x, int y ){
	
	switch ( labirinto[y][x] ) {
			
		case ' ':
			return 1;
			
		default:
			return 0;
			
	}
	
}

/***********************************************************************************
 /																					/
 /	Funzione che genera i missili.													/
 /																					/
 ***********************************************************************************/
void generaMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore ){
	
	switch ( fork() ) {
			
		case -1:
			perror( "ERRORE 009: la fork non e' stata eseguita correttamente.");
			exit( 1 );
			
		case 0:
			funzioneMissile( pipeAlControllo, direzioneMissile, posizioneIniziale, idMissile, creatore );
	}
	
}
