//
//  fantasma.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

#include "header.h"

extern char labirinto[RIGHE][COLONNE];	// Viene dichiarata la variabile "labirinto" che si trova in un altro file.

/*********************************************************************************** /
/																					 /
/	Gestione dei fantasmi.															 /
/																					 /
/ ***********************************************************************************/
void funzioneFantasma ( int pipeAlControllo, int pipeDalControllo, int numeroFantasma, coordinata posizioneIniziale, int morto ){

	personaggio fantasma;	// Variabile dove e' salvato ogni fantasma che verra' inviato alla funzione controllo.
	direzione opposta;	// Variabile che contiene la direzione opposta a quella attuale,
	int secondiDaAspettare = rand() % GENERAZIONE_FANTASMA;	// Intervallo di secondi casuale che aspetta il fantasma prima di muoversi.
	int creazioneMissile;	// Numero casuale che decide se viene generato un missile.
	infoFantasmi info;

	fantasma.identificatore = 'F';
	fantasma.posizione.x = posizioneIniziale.x;
	fantasma.posizione.y = posizioneIniziale.y;
	fantasma.idPersonaggio = numeroFantasma;
	fantasma.pid = getpid();
	fantasma.morto = morto;
	fantasma.missile = 0;
	fantasma.direzioniDisponibili = calcoloBivio( fantasma.posizione.x, fantasma.posizione.y );
	fantasma.movimento = sceltaPercorso( fantasma.posizione.x, fantasma.posizione.y, fantasma.direzioniDisponibili );	// Sceglie una direzione a caso tra quelle disponibili.

	usleep( secondiDaAspettare + UN_SECONDO );	// Il fantasma aspetta un tempo casuale prima di essere messo nel campo.

	write( pipeAlControllo, &fantasma, sizeof( fantasma ) );	// Scrive nella pipe la posizione iniziale del fantasma.

	while ( 1 ) {

		read( pipeDalControllo, &info, sizeof( infoFantasmi ) );	// Leggo dalla pipe.
		
		if ( info.powerPill == 1 ) {
			
			sleep( 3 );
			
		}

		if ( info.rimbalzo == 1 ) {

			fantasma.movimento = calcoloDirezioneOpposta( fantasma.movimento );
			fantasma.direzioniDisponibili = 0;	// Per questo ciclo non deve muoversi.
			info.rimbalzo = 0;

		}

		// Il fantasma si muove a seconda del numero di direzioni disponibili.
		switch ( fantasma.direzioniDisponibili ) {
				
			case 0:
				break;

			case 1:	// Nel caso di una direzione disponibile, vuol dire che si trova in un vicolo cieco. Inverte la direzione e si muove di una casella.

				fantasma.movimento = calcoloDirezioneOpposta( fantasma.movimento );
				fantasma.posizione.x += fantasma.movimento.direzionex;
				fantasma.posizione.y += fantasma.movimento.direzioney;
				break;

			case 2:	// Nel caso di due direzioni disponibili, ci sono due situazioni possibili: o il fantasma di trova in un corridoio, oppure in una curva,

				// Se si trova in un corridoio, controlla se c'e' una parete di fronte. Se non c'e si muove di una casella in avanti.
				if ( posizioneSuccessivaConsentita( fantasma.posizione.x + fantasma.movimento.direzionex, fantasma.posizione.y + fantasma.movimento.direzioney ) == 1 ){

					fantasma.posizione.x += fantasma.movimento.direzionex;
					fantasma.posizione.y += fantasma.movimento.direzioney;

					break;

				}

			default: // altrimenti, nel caso di tre o piu' direzioni disponibili,

				opposta = calcoloDirezioneOpposta( fantasma.movimento );

				do{ //genera casualmente un percorso, finche' il percorso non e' quello da cui e' venuto, e poi si muove.

					fantasma.movimento = sceltaPercorso( fantasma.posizione.x, fantasma.posizione.y, fantasma.direzioniDisponibili );

				} while ( fantasma.movimento.direzionex == opposta.direzionex && fantasma.movimento.direzioney == opposta.direzioney );

				if ( posizioneSuccessivaConsentita( fantasma.posizione.x + fantasma.movimento.direzionex, fantasma.posizione.y + fantasma.movimento.direzioney ) == 1 ){

					fantasma.posizione.x += fantasma.movimento.direzionex;
					fantasma.posizione.y += fantasma.movimento.direzioney;

				}

				break;

		}

		// Generazione del missile.
		creazioneMissile = rand() % GENERAZIONE_SPARO;
		if ( creazioneMissile <= PERCENTUALE_CREAZIONE_SPARO ) {

			fantasma.missile = 1;

		}

		write( pipeAlControllo, &fantasma, sizeof( fantasma ) );	// Scrive nella pipe la posizione aggiornata del fantasma

		fantasma.missile = 0;
		usleep( VELOCITA_FANTASMI );	// Aspetta prima di muovere di nuovo il fantasma
		fantasma.direzioniDisponibili = calcoloBivio( fantasma.posizione.x, fantasma.posizione.y );	// Ricalcola quante direzioni possibili ci sono intorno alla casella attuale

	}

}

/***********************************************************************************
 /																					/
 /	Stampa i fantasmi.																/
 /																					/
 ***********************************************************************************/
void stampaFantasma ( personaggio fantasma ){

	attron( COLOR_PAIR( ( fantasma.idPersonaggio % FANTASMI ) + 1 ) );

	mvaddch( fantasma.posizione.y + 2, fantasma.posizione.x, ACS_PI );

	attroff( COLOR_PAIR( ( fantasma.idPersonaggio % FANTASMI ) + 1 ) );

}

/***********************************************************************************
 /																					/
 /	Funzione che gestisce la generazione dei fantasmi.								/
 /																					/
 ***********************************************************************************/
void generaFantasma ( int pipeAlControllo, int pipeDalControllo, int numeroFantasmi, coordinata posizioneIniziale, int morto ){

	switch ( fork() ) {

		case -1:
			perror( "ERRORE 008: la fork non e' stata eseguita correttamente." );
			exit( 1 );

		case 0:
			funzioneFantasma( pipeAlControllo, pipeDalControllo, numeroFantasmi, posizioneIniziale, morto );

	}

}

int trovaPipe ( int matrice[][2], int numeroFantasma ) {

	return matrice[numeroFantasma][READ];

}

/***********************************************************************************************************************************
 /																																	/
 /	Funzione che restituisce 1 se nella casella in cui si deve muovere il personaggio c'e' un puntino o niente, 0 altrimenti.		/
 /																																	/
 ***********************************************************************************************************************************/
int posizioneSuccessivaConsentita ( int x, int y ){

	switch ( labirinto[y][x] ) {

		case ' ':
		case 'P':
		case '.':
			return 1;

		default:
			return 0;

	}

}

/***********************************************************************************************************************************************************
 /																																							/
 /	Controlla il numero di direzioni disponibili del fantasma, controllandole tutte e aumentando una variabile quando trova un percorso disponibile.		/
 /																																							/
 ***********************************************************************************************************************************************************/
int calcoloBivio ( int x, int y ){

	int i = 0;

	if ( posizioneSuccessivaConsentita( x + 2, y ) == 1 ) i++;
	if ( posizioneSuccessivaConsentita( x - 2, y ) == 1 ) i++;
	if ( posizioneSuccessivaConsentita( x, y + 1 ) == 1 ) i++;
	if ( posizioneSuccessivaConsentita( x, y - 1 ) == 1 ) i++;

	return i;

}

/***********************************************************************************
 /																					/
 /	Funzione che decide la direzione dei fantasmi nel caso di un bivio.				/
 /																					/
 ***********************************************************************************/
direzione sceltaPercorso( int x, int y, int direzioniDisponibili ){

	int i = 0, r;
	direzione percorsoPossibile[direzioniDisponibili];	//Creiamo un array di direzioni, con un numero di posizioni pari al numero di direzioni disponibili.

	// Adesso controlliamo tutte le posizioni, e se una posizione e' disponibile la salviamo nell'array di direzioni
	if ( posizioneSuccessivaConsentita( x + 2, y ) == 1 ){

		percorsoPossibile[i].direzionex = 2;
		percorsoPossibile[i].direzioney = 0;
		i++;

	}

	if ( posizioneSuccessivaConsentita( x - 2, y ) == 1 ){

		percorsoPossibile[i].direzionex = -2;
		percorsoPossibile[i].direzioney = 0;
		i++;

	}

	if ( posizioneSuccessivaConsentita( x, y + 1 ) == 1 ){

		percorsoPossibile[i].direzionex = 0;
		percorsoPossibile[i].direzioney = 1;
		i++;

	}

	if ( posizioneSuccessivaConsentita( x, y - 1 ) == 1 ){

		percorsoPossibile[i].direzionex = 0;
		percorsoPossibile[i].direzioney = -1;
		i++;

	}

	r = rand() % i;	// Sceglie una direzione a caso tra quelle trovate

	return percorsoPossibile[r];	// La restituisce

}

/***********************************************************************************
 /																					/
 /	Funzione che calcola la direzione opposta a quella attuale.						/
 /																					/
 ***********************************************************************************/
direzione calcoloDirezioneOpposta ( direzione attuale ){

	direzione opposta;

	if ( attuale.direzionex != 0 ) {

		opposta.direzioney = 0;

		if ( attuale.direzionex == 2 ) {

			opposta.direzionex = -2;

		} else {

			opposta.direzionex = 2;

		}

	} else {

		opposta.direzionex = 0;

		if ( attuale.direzioney == 1 ) {

			opposta.direzioney = -1;

		} else {

			opposta.direzioney = 1;

		}

	}

	return opposta;

}
