//
//  dichiarazioni.h
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 odioillatino. All rights reserved.
//

void initialization ( void );																											// Inizializzazione dello schermo.
void menu ( void );																														// Visualizza il menu iniziale.
void control ( void );																													// Funzione di controllo.
void generaPacman ( int pipeAlControllo[] );																							// Funzione che genera Pac-Man.
void generaFantasma ( int pipeDaiPersonaggi, int pipeAiFantasmi, int numeroFantasmi, coordinata posizioneIniziale, int morto );			// Funzione che genera i fantasmi.
void generaMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore );		// Funzione che genera i missili.
void funzionePacman ( int pipeAlControllo );																							// Gestione di Pac-Man.
void funzioneFantasma ( int pipeAlControllo, int pipeDalControllo, int numeroFantasmi, coordinata posizioneIniziale, int morto );		// Gestione dei fantasmi.
void funzioneMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore );	// Gestione dei missili.
void stampaPacman ( personaggio pacman );																								// Stampa Pac-Man.
void stampaFantasma ( personaggio fantasma );																							// Stampa i fantasmi.
void stampaMuri ( void );																												// Stampa i muri del labirinto.
void inserisciNellaLista ( list_pointer *testa, personaggio missile );																	// 
void cancellaDallaLista ( list_pointer *testa, personaggio elemento );																	// 
void fineLivello ( void );																												// Animazione di fine livello.
int stampaPallini ( void );																												// Stampa i pallini.
int posizioneSuccessivaConsentita ( int x, int y );																						// Controlla se il personaggio puo' muoversi nella casella selezionata.
int posizioneSuccessivaConsentitaMissili ( int x, int y );																				// Controlla se il missile puo' muoversi nella casella selezionata.
int calcoloBivio ( int x, int y );																										// Controlla se il fantasma e' in un bivio.
int trovaPipe ( int matrice[][2], int numeroFantasma );
direzione sceltaPercorso ( int x, int y, int direzioniDisponibili );																	// Sceglie il percorso quando si trova in un bivio.
direzione calcoloDirezioneOpposta ( direzione attuale );																				// Calcola la direzione opposta all'attuale.
