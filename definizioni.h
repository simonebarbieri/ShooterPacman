//
//  definizioni.h
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

// Keyboard Keys settings
#define SU							65
#define GIU							66
#define DESTRA						67
#define SINISTRA					68
#define SPAZIO						32

#define RIGHE						31
#define COLONNE						55

// Print informations locations settings
#define INFO_VITE					RIGHE + 3
#define INFO_ENERGIA				INFO_VITE + 2
#define INFO_PUNTEGGIO				0
#define COLONNA_VITE				5
#define COLONNA_ENERGIA				COLONNA_VITE
#define COLONNA_FRUTTA				20
#define COLONNA_PUNTEGGIO			COLONNA_VITE
#define COLONNA_HIGH_SCORE			COLONNE - COLONNA_VITE - 10	// The -10 is for the length of the "HIGH SCORE" label

// Game settings
#define VITE_INIZIALI				3
#define PUNTO_STANDARD				10
#define PUNTO_POWERPILL				50
#define FILE_PUNTEGGIO				".highscore"
#define MAX_ENERGIA					10

#define UN_SECONDO					1000000

#define WRITE						1
#define READ						0

// Colors settings
#define FANTASMA1					1
#define FANTASMA2					2
#define FANTASMA3					3
#define FANTASMA4					4
#define LABIRINTO					5
#define PACMAN						6
#define SCRITTE						7

// Pacman settings
#define RIGA_INIZIALE_PACMAN		23
#define COLONNA_INIZIALE_PACMAN		26

// Ghost settings
#define FANTASMI					4
#define MAX_FANTASMI				FANTASMI * 2
#define PASSI_CREAZIONE_FANTASMA	50
#define PASSI_RESPOWN_FANTASMA		150
#define VELOCITA_FANTASMI			200000
#define GENERAZIONE_FANTASMA		3000000
#define GENERAZIONE_SPARO			101
#define PERCENTUALE_CREAZIONE_SPARO	20
#define RIGA_CREAZIONE_FANTASMA		11
#define COLONNA_CREAZIONE_FANTASMA	26
#define RIGA_SEZIONE_CENTRALE		14 + 2
#define COLONNA_SEZIONE_CENTRALE	24

// Missiles settings
#define MISSILI						4
#define MISSILE_SU					0
#define MISSILE_GIU					1
#define MISSILE_DESTRA				2
#define MISSILE_SINISTRA			3
#define VELOCITA_MISSILI			50000
