//
//  controllo.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

#include "header.h"
#include "labirinto.h"

/***********************************************************************************
 /																					/
 /	Funzione di controllo del gioco.												/
 /																					/
 ***********************************************************************************/
void control ( void ){
	
	personaggio pacman, personaggioLetto;	// Variabili in cui sono salvati tutti i personaggi, e l'ultimo e' il personaggio che viene letto dalla pipe
	infoFantasmi info[MAX_FANTASMI];
	int vittoria, vite = VITE_INIZIALI, punteggio = 0;	// Variabili di gioco: la prima indica la vittoria se uguale a zero, la seconda le vite disponibili, la terza il punteggio
    int counterCreazioneFantasma = rand() % PASSI_CREAZIONE_FANTASMA, counterRespownFantasma[MAX_FANTASMI];	// Variabile che decide casualmente quanti pallini deve mangiare Pac-Man prima di far comparire un fantasma
	int numeroFantasmi, numeroMissili;	// Variabile che contiene il numero di fantasmi in gioco
	int pipeDaiPersonaggi[2], pipeAiFantasmi[MAX_FANTASMI][2];	// 
	int i, j;	// Contatori
	int err;	// Variabile di supporto per controllare se ci sono errori uccidendo processi.
	int energia;	// Variabile che indica quanta energia rimane a Pac-Man prima di essere ucciso dai missili dei fantasmi.
	int highScore;	// Variabile in cui e' contenuto il record.
	int elementoNonTrovato;	// Variabile che serve per sapere se l'elemento letto e' da inserire nella lista.
	int fantasmi[MAX_FANTASMI];	// Indica se il fantasma esiste o no.
	char labirintoBackup[RIGHE][COLONNE];	// Un labirinto in cui e' contenuta la copia del labirinto iniziale, per riiniziare il gioco a fine livello.
	list_pointer listaFantasmi = NULL, listaMissili = NULL;	// Liste che contengono i fantasmi e i missili
	list_pointer ptr1 = NULL, ptr2 = NULL;	// Elemento di supporto per scorrere le liste
	direzione direzioni[MISSILI];	// Un array contenente tutte le direzioni possibili.
	coordinata posizioneInizialeFantasma;	// Indica la posizione da cui deve ripartire un fantasma che e' stato ucciso.
	FILE *score;	// Un puntatore al file in cui e' salvato il punteggio.
	
	pacman.quit = 0;
	
	// Lettura dal file per il punteggio massimo.
	score = fopen( FILE_PUNTEGGIO, "r" );
	if ( score == NULL ) {
		
		score = fopen( FILE_PUNTEGGIO, "w" );
		fprintf( score, "10000" );
		fclose( score );
		
	}
	score = fopen( FILE_PUNTEGGIO, "r" );
	while ( !feof(score) ) {
		
		fscanf( score, "%d", &highScore);
		
	}
	fclose( score );
	
	posizioneInizialeFantasma.x = COLONNA_CREAZIONE_FANTASMA;
	posizioneInizialeFantasma.y = RIGA_CREAZIONE_FANTASMA;
	
	direzioni[MISSILE_SU].direzionex = 0;
	direzioni[MISSILE_SU].direzioney = -1;
	direzioni[MISSILE_GIU].direzionex = 0;
	direzioni[MISSILE_GIU].direzioney = 1;
	direzioni[MISSILE_DESTRA].direzionex = 2;
	direzioni[MISSILE_DESTRA].direzioney = 0;
	direzioni[MISSILE_SINISTRA].direzionex = -2;
	direzioni[MISSILE_SINISTRA].direzioney = 0;
	
	// Copia del labirinto per il ripristino alla fine del livello.
	for ( i = 0; i < RIGHE; i++ ) {
		
		for ( j = 0; j < COLONNE; j++ ){
			
			labirintoBackup[i][j] = labirinto[i][j];	// Creo un backup del labirinto per partite future.
			
		}
		
	}
	
	// Inizializzazione delle pipes.
	if ( pipe( pipeDaiPersonaggi ) == -1 ){	// Inizializzazione della pipe
		
		perror( "ERRORE 005: la pipe non e' stata creata correttamente." );
		exit( 1 );
		
	}
	for ( i = 0; i < MAX_FANTASMI; i++ ){
		
		if ( pipe( pipeAiFantasmi[i] ) == -1 ){
			
			perror( "ERRORE 006: la pipe non e' stata creata correttamente." );
			exit( 1 );
			
		}
		
	}
	
	// Inizializzazione array per il rimbalzo dei fantasmi.
	for ( i = 0; i < MAX_FANTASMI; i++ ) {
		
		info[i].rimbalzo = 0;
		
	}
	
	do {
		
		// Inizializzazione array per il respown dei fantasmi.
		for ( i = 0; i < MAX_FANTASMI; i++ ) {
			
			counterRespownFantasma[i] = -1;
			info[i].powerPill = 0;
			fantasmi[i] = 0;
			
		}
		
		// Reinizializzo le variabili che vanno ripristinate a ogni livello.
		numeroMissili = 0;
		numeroFantasmi = 0;
		energia = MAX_ENERGIA;
		
		attron( COLOR_PAIR( LABIRINTO ) );
		stampaMuri();
		attroff( COLOR_PAIR( LABIRINTO ) );
		
		generaPacman( pipeDaiPersonaggi );	// Generazione del primo Pac-Man
		
		do{
			
			read( pipeDaiPersonaggi[READ], &personaggioLetto, sizeof( personaggioLetto ) );	// Legge dalla pipe un personaggio
			
			// Aggiorno l'array respown.
			for ( i = 0; i < MAX_FANTASMI; i++ ) {
				
				counterRespownFantasma[i]--;
				
			}
			
			// Genero un numero casuale di passi dopo i quali verra' generato un fantasma.
			if ( counterCreazioneFantasma <= 0 && numeroFantasmi < FANTASMI ) {	// Se e' gia' stato creato un fantasma e il numero dei fantasmi in gioco e' minore del numero di fantasmi massimo, allora crea un fantasma
				
				counterCreazioneFantasma = rand() % PASSI_CREAZIONE_FANTASMA;
				
			}
			
			// Azioni da intraprendere a seconda del personaggio letto.
			if ( personaggioLetto.identificatore == 'P' ) {
				
				pacman = personaggioLetto;	// salva nella variabile "pacman" il personaggio letto.
				
				// Azioni nel caso si abbia mangiato un puntino o una power pill.
				if ( labirinto[pacman.posizione.y][pacman.posizione.x] == '.' || labirinto[pacman.posizione.y][pacman.posizione.x] == 'P' ){
					
					// Se ho mangiato una power pill, assegno 1 alla relativa variabile.
					if ( labirinto[pacman.posizione.y][pacman.posizione.x] == 'P' ) {
						
						for ( i = 0; i < MAX_FANTASMI; i++ ) {
							
							if ( fantasmi[i] == 1 ) {
								
								info[i].powerPill = 1;
								
							}
							
						}
						
					}
					
					// Aggiorno il punteggio
					if ( labirinto[pacman.posizione.y][pacman.posizione.x] == '.' ) {
						
						punteggio += PUNTO_STANDARD;
						
					} else {
						
						punteggio += PUNTO_POWERPILL;
						
					}
					
					labirinto[pacman.posizione.y][pacman.posizione.x] = ' ';	// Aggiorniamo il labirinto togliendo il puntino mangiato
					
					counterCreazioneFantasma--;	// Diminuisco di uno il numero di puntini da mangiare per creare il fantasma
					
				}
				
				// Se si e' premuto q durante il gioco, imposto le vite a -1 e si esce dal gioco.
				if ( personaggioLetto.quit == 1 ) {
					
					vite = -1;
					
				}
				
			} else {	// Altrimenti, se era un fantasma, salva il personaggio letto nell'array di fantasmi, alla posizione indicata dall'id del fantasma
				
				if ( personaggioLetto.identificatore == 'F' ){
					
					elementoNonTrovato = TRUE;
					
					// Scorro la lista dei fantasmi per trovare il fantasma letto.
					for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) {
						
						// Se trovo il personaggio nella lista, assegno il personaggio letto al personaggio corrispondente.
						if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio ) {
							
							ptr1->elemento = personaggioLetto;
							
							elementoNonTrovato = FALSE;
							
						}
						
					}
					
					// Se non lo trovo nella lista, lo aggiungo e riscorro la lista.
					if ( elementoNonTrovato == TRUE ) {
						
						inserisciNellaLista( &listaFantasmi, personaggioLetto );
						
						for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) {
							
							if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio ) {
								
								ptr1->elemento = personaggioLetto;
								
							}
							
						}
						
					}
					
					//
					// DA CORREGGERE
					//
					for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) {
						
						if ( personaggioLetto.idPersonaggio != ptr1->elemento.idPersonaggio && ( ( personaggioLetto.posizione.x + personaggioLetto.movimento.direzionex == ptr1->elemento.posizione.x && personaggioLetto.posizione.y + personaggioLetto.movimento.direzioney == ptr1->elemento.posizione.y ) || ( personaggioLetto.posizione.x + personaggioLetto.movimento.direzionex == ptr1->elemento.posizione.x + ptr1->elemento.movimento.direzionex && personaggioLetto.posizione.y + personaggioLetto.movimento.direzioney == ptr1->elemento.posizione.y + ptr1->elemento.movimento.direzioney ) || ( personaggioLetto.posizione.x == ptr1->elemento.posizione.x + ptr1->elemento.movimento.direzionex && personaggioLetto.posizione.y == ptr1->elemento.posizione.y + ptr1->elemento.movimento.direzioney ) ) ) {
							
							info[personaggioLetto.idPersonaggio].rimbalzo = 1;
							
						}
						
					}
					
					write( pipeAiFantasmi[personaggioLetto.idPersonaggio][WRITE], &info[personaggioLetto.idPersonaggio], sizeof( infoFantasmi ) );
					
					info[personaggioLetto.idPersonaggio].powerPill = 0;
					info[personaggioLetto.idPersonaggio].rimbalzo = 0;
					
				} else {
					
					elementoNonTrovato = TRUE;
					
					for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link ) {
						
						if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio ) {
							
							ptr1->elemento = personaggioLetto;
							
							elementoNonTrovato = FALSE;
							
						}
						
						if ( ptr1->elemento.morto == 1 ){
							
							cancellaDallaLista( &listaMissili, ptr1->elemento );
							
						}
						
					}
					
					if ( elementoNonTrovato == TRUE ) {
						
						inserisciNellaLista( &listaMissili, personaggioLetto );
						
						for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link ) {
							
							if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio ) {
								
								ptr1->elemento = personaggioLetto;
								
							}
							
						}
						
					}
					
				}
				
			}
			
			// Ogni 10000 punti, si ottiene una vita extra.
			if ( ( punteggio % 10000 ) == 0 && punteggio != 0 ) {
				
				vite++;
				
			}
			
			// Scorro la lista dei fantasmi per controllare se Pac-Man e' nella stessa posizione di uno di questi.
			for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) {
				
				if ( pacman.posizione.x == ptr1->elemento.posizione.x && pacman.posizione.y == ptr1->elemento.posizione.y && pacman.mosso != 0 ) {
					
					err = kill( pacman.pid, 1 );			// Uccide il processo Pac-Man
					while ( err != 0 ) {
						
						err = kill( pacman.pid, 1 );
						
					}
					
					vite--;									// Diminuisce il numero di vite di Pac-Man
					
					generaPacman( pipeDaiPersonaggi );		// Crea un nuovo Pac-Man
					energia = MAX_ENERGIA;					// Viene ripristinata l'energia
					
				}
				
			}
			
			// Scorro la lista dei missili per controllare se hanno colpito qualcosa.
			for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link ) {
				
				// Controllo Pac-Man
				if ( pacman.posizione.x == ptr1->elemento.posizione.x && pacman.posizione.y == ptr1->elemento.posizione.y ) {
					
					if ( pacman.mosso != 0 ) {
						
						if ( ptr1->elemento.creatore != 'P') {
							
							energia--;
							
						}
						
					}
					
					err = kill( ptr1->elemento.pid, 1 );
					while ( err != 0 ) {
						
						err = kill( ptr1->elemento.pid, 1 );
						
					}
					cancellaDallaLista( &listaMissili, ptr1->elemento );
					
				}
				
				// Controllo i fantasmi
				for ( ptr2 = listaFantasmi; ptr2 != NULL; ptr2 = ptr2->link ) {
					
					if ( ptr1->elemento.posizione.x == ptr2->elemento.posizione.x && ptr1->elemento.posizione.y == ptr2->elemento.posizione.y ) {
						
						if ( ptr1->elemento.creatore != 'F' ) {
							
							err = kill( ptr2->elemento.pid, 1 );
							while ( err != 0 ) {
								
								err = kill( ptr2->elemento.pid, 1);
								
							}
							cancellaDallaLista( &listaFantasmi, ptr2->elemento );
							fantasmi[ptr2->elemento.idPersonaggio] = 0;
							
							if ( ptr2->elemento.morto == FALSE ) {
								
								generaFantasma( pipeDaiPersonaggi[WRITE], trovaPipe( pipeAiFantasmi, ptr2->elemento.idPersonaggio + FANTASMI ), ptr2->elemento.idPersonaggio + FANTASMI, posizioneInizialeFantasma, TRUE );
								fantasmi[ptr2->elemento.idPersonaggio + FANTASMI] = 1;
								
							}
							
							counterRespownFantasma[ptr2->elemento.idPersonaggio] = rand() % PASSI_RESPOWN_FANTASMA;
							
						}
						
						err = kill( ptr1->elemento.pid, 1 );
						while ( err != 0 ) {
							
							err = kill( ptr1->elemento.pid, 1 );
							
						}
						ptr1->elemento.morto = 1;
						
					}
					
				}
				
				// Controllo i missili
				for ( ptr2 = listaMissili; ptr2 != NULL; ptr2 = ptr2->link ) {
					
					if ( ptr1->elemento.posizione.x == ptr2->elemento.posizione.x && ptr1->elemento.posizione.y == ptr2->elemento.posizione.y && ptr1->elemento.idPersonaggio != ptr2->elemento.idPersonaggio ) {
						
						err = kill( ptr1->elemento.pid, 1 );
						while ( err != 0 ) {
							
							err = kill( ptr1->elemento.pid, 1 );
							
						}
						ptr1->elemento.morto = 1;
						
						err = kill( ptr2->elemento.pid, 1 );
						while ( err != 0 ) {
							
							err = kill( ptr2->elemento.pid, 1);
							
						}
						ptr2->elemento.morto = 1;
						
					}
					
				}
				
			}
			
			// Controllo se l'energia e' scesa a 0, nel qual caso Pac-Man muore.
			if ( energia == 0 ) {	// Se l'energia di Pac-Man finisce, Pac-Man muore.
				
				err = kill( pacman.pid, 1 );			// Uccide il processo Pac-Man
				while ( err != 0 ) {
					
					err = kill( pacman.pid, 1 );
					
				}
				
				generaPacman( pipeDaiPersonaggi );		// Crea un nuovo Pac-Man
				energia = MAX_ENERGIA;					// Viene ripristinata l'energia
				
				vite--;									// Diminuisce il numero di vite di Pac-Man
				
			}
			
			// Controllo se Pac-Man ha mangiato abbastanza pallini per generare un fantasma.
			if ( counterCreazioneFantasma == 0 ) {
				
				generaFantasma( pipeDaiPersonaggi[WRITE], trovaPipe( pipeAiFantasmi, numeroFantasmi ), numeroFantasmi, pacman.posizione, FALSE );	// Genera un fantasma
				fantasmi[numeroFantasmi] = 1;
				numeroFantasmi++;						// Aumenta il numero dei fantasmi di uno
				counterCreazioneFantasma--;				// E fa diminuire di uno il contatore ( per evitare che vengano fatte una serie di fork() ).
				
			}
			
			// Controllo se il countdown per il respown dei fantasmi e' terminato.
			for ( i = 0; i < MAX_FANTASMI; i++ ) {
				
				if ( counterRespownFantasma[i] == 0 ) {	// Se il contatore per la creazione del fantasma scende a zero:
					
					generaFantasma( pipeDaiPersonaggi[WRITE], trovaPipe( pipeAiFantasmi, i ), i, posizioneInizialeFantasma, TRUE );	// Genera un fantasma
					fantasmi[i] = 1;
					counterRespownFantasma[i]--;		// E fa diminuire di uno il contatore ( per evitare che vengano fatte una serie di fork() ).
					
				}
				
			}
			
			// Generazione dei missili.
			if ( personaggioLetto.missile == 1 ) {
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x + 2, personaggioLetto.posizione.y ) == 1 ){
					
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_DESTRA], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;
					
				}
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x - 2, personaggioLetto.posizione.y ) == 1 ){
					
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_SINISTRA], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;
					
				}
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x, personaggioLetto.posizione.y + 1 ) == 1 ){
					
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_GIU], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;
					
				}
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x, personaggioLetto.posizione.y - 1 ) == 1 ){
					
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_SU], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;
					
				}
				
			}
			
			vittoria = stampaPallini();	// Viene stampato il labirinto aggiornato e nella variabile vittoria viene salvato il numero dei puntini rimasti
			
			// Stampa dei missili e cancellazione dalla lista.
			for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link ) {
				
				if ( ptr1->elemento.morto == 1 ){
					
					mvprintw( ptr1->elemento.posizione.y + 2, ptr1->elemento.posizione.x, "%c", labirinto[ptr1->elemento.posizione.y][ptr1->elemento.posizione.x] );
					
					cancellaDallaLista( &listaMissili, ptr1->elemento );
					
				} else {
					
					mvprintw( ptr1->elemento.posizione.y + 2, ptr1->elemento.posizione.x, "=" );
					
				}
				
			}
			
			// Stampa dei fantasmi.
			for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) {
				
				stampaFantasma ( ptr1->elemento );
				
			}
			
			attron( COLOR_PAIR( PACMAN ) );
			stampaPacman( pacman );	// Viene stampato Pac-Man
			attroff( COLOR_PAIR( PACMAN ) );
			
			// Stampo il numero di vite disponibili.
			for ( i = 0; i < RIGHE; i++ ){
				
				mvprintw( INFO_VITE, i, " ");
				
			}
			for ( i = 0; i < vite; i++ ){
				
				attron( COLOR_PAIR( PACMAN ) );
				mvprintw( INFO_VITE, COLONNA_VITE + ( i * 2 ), ">");
				attroff( COLOR_PAIR( PACMAN ) );
				
			}
			
			// Stampo l'energia rimasta a Pac-Man
			mvprintw( INFO_ENERGIA, COLONNA_ENERGIA, "[" );
			mvprintw( INFO_ENERGIA, COLONNA_ENERGIA + MAX_ENERGIA + 1, "]" );
			for ( i = 0; i < MAX_ENERGIA; i++ ) {
				
				mvprintw( INFO_ENERGIA, COLONNA_ENERGIA + i + 1, " " );
				
			}
			for ( i = 0; i < energia; i++ ) {
				
				mvprintw( INFO_ENERGIA, COLONNA_ENERGIA + i + 1, "=" );
				
			}
			
			// Stampo il punteggio.
			mvprintw( INFO_PUNTEGGIO, COLONNA_PUNTEGGIO, "1UP" );
			mvprintw( INFO_PUNTEGGIO + 1, COLONNA_PUNTEGGIO + 1, "%d", punteggio );
			mvprintw( INFO_PUNTEGGIO, COLONNA_HIGH_SCORE, "HIGH SCORE" );
			if ( punteggio < highScore ) {
				
				mvprintw( INFO_PUNTEGGIO + 1, COLONNA_HIGH_SCORE + 1, "%d", highScore );
				
			} else {
				
				mvprintw( INFO_PUNTEGGIO + 1, COLONNA_HIGH_SCORE + 1, "%d", punteggio );
				
			}
			
			refresh();	// Aggiorno la schermata.
			
		} while ( vittoria != 0 && vite > 0 );	// Ripete il ciclo finche' vittoria (ovvero il numero di puntini rimasti) e' diverso da 0 oppure finche' Pac-Man non viene toccato da un fantasma.
		
		// Uccido Pac-Man
		err = kill( pacman.pid, 1);
		while ( err != 0 ) {
			
			err = kill( pacman.pid, 1 );
			
		}
		
		// Uccido i fantasmi
		for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) {
			
			err = kill( ptr1->elemento.pid, 1 );
			while ( err != 0 ) {
				
				err = kill( ptr1->elemento.pid, 1 );
				
			}
			cancellaDallaLista( &listaFantasmi, ptr1->elemento );
			
		}
		
		// Uccido i missili
		for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link ) {
			
			err = kill( ptr1->elemento.pid, 1 );
			while ( err != 0 ) {
				
				err = kill( ptr1->elemento.pid, 1 );
				
			}
			cancellaDallaLista( &listaMissili, ptr1->elemento );
			
		}
		
		// Se si e' riusciti a mangiare tutti i puntini, viene fatta l'animazione
		if ( vittoria == 0 ) {
			
			fineLivello();
			
		}
		
		clear();	// Viene pulito lo schermo.
		
		// Viene ripristinato il labirinto.
		for ( i = 0; i < RIGHE; i++ ) {
			
			for ( j = 0; j < COLONNE; j++ ){
				
				labirinto[i][j] = labirintoBackup[i][j];
				
			}
			
		}
		
		// Scrivo nel file il punteggio nuovo, se piu' grande di quello precedente.
		if ( punteggio > highScore ) {
			
			highScore = punteggio;
			
			score = fopen( FILE_PUNTEGGIO, "w" );
			
			if ( score != NULL ) {
				
				fprintf( score, "%d", highScore);
				fclose( score );
				
			}
			
		}
		
	} while ( vite > 0 && pacman.quit == 0 );
	
}

/***********************************************************************************
 /																					/
 /	Funzione che stampa il labirinto.												/
 /																					/
 ***********************************************************************************/
void stampaMuri ( void ){
	
	int i, j;	// Contatori
	
	//Stampo il labirinto
	for ( i = 0; i < RIGHE; i++ ){
		
		for( j = 0; j < COLONNE; j++ ){
			
			switch( labirinto[i][j] ){
					
				case '<':
					mvaddch( i + 2, j, ACS_ULCORNER );
					break;
					
				case '>':
					mvaddch( i + 2, j, ACS_URCORNER );
					break;
					
				case 'L':
					mvaddch( i + 2, j, ACS_LLCORNER );
					break;
					
				case 'J':
					mvaddch( i + 2, j, ACS_LRCORNER );
					break;
					
				case 'F':
					mvaddch( i + 2, j, ACS_LTEE );
					break;
					
				case 'Y':
					mvaddch( i + 2, j, ACS_RTEE );
					break;
					
				case 'T':
					mvaddch( i + 2, j, ACS_TTEE );
					break;
					
				case '-':
					mvaddch( i + 2, j, ACS_HLINE );
					break;
					
				case '|':
					mvaddch( i + 2, j, ACS_VLINE );
					break;
					
				case '_':
					mvaddch( i + 2, j, ACS_S9 );
					break;
					
				case '^':
					mvaddch( i + 2, j, ACS_S1 );
					break;
					
				case '*':
					mvaddch( i + 2, j, ' ');
					
				default:
					break;
					
			}
			
		}
		
	}
	
	refresh();
	
}

/***********************************************************************************
 /																					/
 /	Funzione che stampa i pallini.													/
 /																					/
 ***********************************************************************************/
int stampaPallini ( void ) {
	
	int i, j;			// Contatori
	int puntini = 0;	// Variabile che tiene conto di quanti puntini sono rimasti
	
	attron( COLOR_PAIR( PACMAN ) );
	
	//Stampo i pallini
	for ( i = 0; i < RIGHE; i++ ){
		
		for( j = 0; j < COLONNE; j++ ){
			
			switch( labirinto[i][j] ){
					
				case '.':
					mvaddch( i + 2, j, ACS_BULLET );
					puntini++;	// Se stampa un puntino, incrementa questa variabile
					break;
					
				case 'P':
					mvaddch( i + 2, j, ACS_DIAMOND);
					puntini++;
					break;
					
				case ' ':
					mvaddch( i + 2, j, ' ' );
					break;
					
				default:
					break;
					
			}
			
		}
		
	}
	
	attroff( COLOR_PAIR( PACMAN ) );
	
	return puntini;
	
}

/***********************************************************************************
 /																					/
 /	Animazione di fine livello.														/
 /																					/
 ***********************************************************************************/
void fineLivello ( void ) {
	
	attron( COLOR_PAIR( LABIRINTO ) );
	
	stampaMuri();
	
	attroff( COLOR_PAIR( LABIRINTO ) );
	
	usleep( 500000 );
	
	stampaMuri();
	
	usleep( 500000 );
	
	attron( COLOR_PAIR( LABIRINTO ) );
	
	stampaMuri();
	
	attroff( COLOR_PAIR( LABIRINTO ) );
	
	usleep( 500000 );
	
	stampaMuri();
	
	usleep( 500000 );
	
	attron( COLOR_PAIR( LABIRINTO ) );
	
	stampaMuri();
	
	attroff( COLOR_PAIR( LABIRINTO ) );
	
	usleep( 500000 );
	
}
