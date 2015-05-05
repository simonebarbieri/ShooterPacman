//
//  dichiarazioni.h
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

void initialization ( void );
void menu ( void );
void control ( void );
void generaPacman ( int pipeAlControllo[] );
void generaFantasma ( int pipeDaiPersonaggi, int pipeAiFantasmi, int numeroFantasmi, coordinata posizioneIniziale, int morto );
void generaMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore );
void funzionePacman ( int pipeAlControllo );
void funzioneFantasma ( int pipeAlControllo, int pipeDalControllo, int numeroFantasmi, coordinata posizioneIniziale, int morto );
void funzioneMissile ( int pipeAlControllo, direzione direzioneMissile, coordinata posizioneIniziale, int idMissile, char creatore );
void stampaPacman ( personaggio pacman );
void stampaFantasma ( personaggio fantasma );
void stampaMuri ( void );
void inserisciNellaLista ( list_pointer *testa, personaggio missile ); 
void cancellaDallaLista ( list_pointer *testa, personaggio elemento ); 
void fineLivello ( void );
int stampaPallini ( void );
int posizioneSuccessivaConsentita ( int x, int y );
int posizioneSuccessivaConsentitaMissili ( int x, int y );
int calcoloBivio ( int x, int y );
int trovaPipe ( int matrice[][2], int numeroFantasma );
direzione sceltaPercorso ( int x, int y, int direzioniDisponibili );
direzione calcoloDirezioneOpposta ( direzione attuale );
