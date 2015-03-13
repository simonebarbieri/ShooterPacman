//
//  main.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

#include "header.h"

int main ( void ){

	initialization();		// Inizializza lo schermo

	menu();					// Mostra il menu

	endwin();

	return 0;

}

/***********************************************************************************
 /																					/
 /	Funzione che stampa il menu.													/
 /																					/
 ***********************************************************************************/
void menu ( void ){

	char c = 'a';
	int i, j;

	while ( c != 'q' && c != 'Q' ) {

		attron( COLOR_PAIR( SCRITTE ) );

		// Viene colorato lo schermo di blu.
		for ( i = 0; i < RIGHE; i++ ) {

			for ( j = 0; j < COLONNE + 7; j++ ) {

				printw( " " );

			}

			printw( "\n" );

		}

		// Disegno il logo di Pac-Man
		mvprintw( 0, 1, " ____                                                        " );
		mvprintw( 1, 1, "/\\  _`\\                         /'\\_/`\\                      " );
		mvprintw( 2, 1, "\\ \\ \\L\\ \\ __      ___          /\\      \\     __      ___     " );
		mvprintw( 3, 1, " \\ \\ ,__/'__`\\   /'___\\ _______\\ \\ \\__\\ \\  /'__`\\  /' _ `\\   " );
		mvprintw( 4, 1, "  \\ \\ \\/\\ \\L\\.\\_/\\ \\__//\\______\\\\ \\ \\_/\\ \\/\\ \\L\\.\\_/\\ \\/\\ \\  " );
		mvprintw( 5, 1, "   \\ \\_\\ \\__/.\\_\\ \\____\\/______/ \\ \\_\\\\ \\_\\ \\__/.\\_\\ \\_\\ \\_\\ " );
		mvprintw( 6, 1, "    \\/_/\\/__/\\/_/\\/____/          \\/_/ \\/_/\\/__/\\/_/\\/_/\\/_/ \n" );
		
		// Stampo il menu.
		mvprintw( 10, 25, "BENVENUTO!" );
		mvprintw( 14, 16, "Scegli cosa fare:" );
		mvprintw( 16, 16, "[P] Gioca" );
		mvprintw( 17, 16, "[Q] Esci" );

		attroff( COLOR_PAIR( SCRITTE ) );

		refresh();

		scanf( "%c", &c );	// Salvo la selezione della voce del menu nella variabile "c".

		switch ( c ) {
			case 'p':
			case 'P':
				clear();
				control();
				break;

			default:
				break;

		}

	}

}

/***********************************************************************************
 /																					/
 /	Funzione che inizializza lo schermo e imposta diverse caratteristiche.			/
 /																					/
 ***********************************************************************************/
void initialization ( void ){

	initscr();		// Crea una finestra di curses.
	noecho();		// Non fa visualizzare a schermo il tasto premuto
	curs_set( 0 );	// Non fa visualizzare il cursore
	start_color();	// Colora lo schermo

	// Inizializzazione dei colori
	init_pair( LABIRINTO, COLOR_BLUE, COLOR_BLACK );
	init_pair( PACMAN, COLOR_YELLOW, COLOR_BLACK );
	init_pair( FANTASMA1, COLOR_RED, COLOR_BLACK );
	init_pair( FANTASMA2, COLOR_CYAN, COLOR_BLACK );
	init_pair( FANTASMA3, COLOR_MAGENTA, COLOR_BLACK );
	init_pair( FANTASMA4, COLOR_GREEN, COLOR_BLACK );
	init_pair( SCRITTE, COLOR_YELLOW, COLOR_BLUE );

	srand( (int) time( NULL ) );

}

/***********************************************************************************
 /																					/
 /	Inserisce un elemento in una lista concatenata.									/
 /																					/
 ***********************************************************************************/
void inserisciNellaLista ( list_pointer *testa, personaggio elemento ){

	list_pointer ii, temp;

	if ( *testa == NULL ) {

		temp = (list_pointer) malloc( sizeof( struct nodo ) );
		temp->elemento = elemento;
		temp->link = NULL;
		*testa = temp;

	} else {

		for ( ii = *testa; ii->link != NULL; ii = ii->link);

		ii->link = (list_pointer) malloc( sizeof( struct nodo ) );
		ii->link->elemento = elemento;
		ii->link->link = NULL;

	}

}

/***********************************************************************************
 /																					/
 /	Cancella un elemento dalla lista.												/
 /																					/
 ***********************************************************************************/
void cancellaDallaLista ( list_pointer *testa, personaggio elemento ) {

	list_pointer ii, temp;

	if ( (*testa)->elemento.idPersonaggio == elemento.idPersonaggio ) {

		temp = *testa;
		*testa = (*testa)->link;
		free( temp );

	} else {

		for ( ii = *testa; ii != NULL && ii->elemento.idPersonaggio != elemento.idPersonaggio; ii = ii->link ){

			temp = ii;

		}

		if ( ii == NULL ) {

			perror( "ERRORE 004: l'elemento non e' nella lista." );
			exit( 1 );

		} else {

			temp->link = ii->link;
			free( ii );

		}

	}

}
