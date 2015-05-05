//
//  controllo.c
//  Missile Pacman
//
//  Created by Simone Barbieri on 18/08/11.
//  Copyright 2011 Simone Barbieri. All rights reserved.
//

#include "header.h"
#include "labirinto.h"

// Control function
void control ( void )
{	
	// The characters (personaggioLetto is the character read from the pipe)
	personaggio pacman, personaggioLetto;
	infoFantasmi info[MAX_FANTASMI];
	// Game variables
	int vittoria, vite = VITE_INIZIALI, punteggio = 0;
	// The number of steps that pacman must walk before a ghost is generated
    int counterCreazioneFantasma = rand() % PASSI_CREAZIONE_FANTASMA, counterRespownFantasma[MAX_FANTASMI];
	// The number of ghosts and missiles in game
	int numeroFantasmi, numeroMissili;
	int pipeDaiPersonaggi[2], pipeAiFantasmi[MAX_FANTASMI][2]; 
	int i, j;
	// Support number for any error
	int err;
	// Pacman energy before it dies with a missile
	int energia;
	int highScore;
	int elementoNonTrovato;
	// Indicates whether the ghost exists or it does not
	int fantasmi[MAX_FANTASMI];
	// A backup of the original labyrith
	char labirintoBackup[RIGHE][COLONNE];
	// Lists of the ghosts and missiles
	list_pointer listaFantasmi = NULL, listaMissili = NULL;
	// Elements to browse the lists
	list_pointer ptr1 = NULL, ptr2 = NULL;
	// An array with the possible directions
	direzione direzioni[MISSILI];
	// The respawn position of the ghost
	coordinata posizioneInizialeFantasma;
	// The score file
	FILE *score;
	
	pacman.quit = 0;
	
	// Reading of the high score from the file
	score = fopen( FILE_PUNTEGGIO, "r" );
	if ( score == NULL )
	{	
		score = fopen( FILE_PUNTEGGIO, "w" );
		fprintf( score, "10000" );
		fclose( score );	
	}
	score = fopen( FILE_PUNTEGGIO, "r" );
	while ( !feof(score) )
	{	
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
	
	// Backup of the labyrinth to restore it at the end of the level
	for ( i = 0; i < RIGHE; i++ )
	{	
		for ( j = 0; j < COLONNE; j++ )
		{	
			labirintoBackup[i][j] = labirinto[i][j];	
		}
	}
	
	// Pipes initualization
	if ( pipe( pipeDaiPersonaggi ) == -1 )
	{	
		perror( "ERRORE 005: la pipe non e' stata creata correttamente." );
		exit( 1 );	
	}
	
	for ( i = 0; i < MAX_FANTASMI; i++ )
	{	
		if ( pipe( pipeAiFantasmi[i] ) == -1 )
		{	
			perror( "ERRORE 006: la pipe non e' stata creata correttamente." );
			exit( 1 );	
		}
	}
	
	// Initialization of the array to make the ghost change direction in the case they cross another one's path
	for ( i = 0; i < MAX_FANTASMI; i++ )
	{	
		info[i].rimbalzo = 0;	
	}
	
	do
	{	
		// Array initialization for the respawn of the ghosts
		for ( i = 0; i < MAX_FANTASMI; i++ )
		{	
			counterRespownFantasma[i] = -1;
			info[i].powerPill = 0;
			fantasmi[i] = 0;	
		}
		
		// Initialization of the variables that must be resetted each level
		numeroMissili = 0;
		numeroFantasmi = 0;
		energia = MAX_ENERGIA;
		
		attron( COLOR_PAIR( LABIRINTO ) );
		stampaMuri();
		attroff( COLOR_PAIR( LABIRINTO ) );
		
		// Generates pacman
		generaPacman( pipeDaiPersonaggi );
		
		do
		{	
			// Read a character from the pipe
			read( pipeDaiPersonaggi[READ], &personaggioLetto, sizeof( personaggioLetto ) );
			
			// Update the respawn array
			for ( i = 0; i < MAX_FANTASMI; i++ )
			{	
				counterRespownFantasma[i]--;	
			}
			
			// Generates a random number of steps after which a ghost will be created
			// only if the number of ghost currently in game is less of the maximum
			// numeber of ghosts
			if ( counterCreazioneFantasma <= 0 && numeroFantasmi < FANTASMI )
			{	
				counterCreazioneFantasma = rand() % PASSI_CREAZIONE_FANTASMA;	
			}
			
			// Check what character read from the pipe
			if ( personaggioLetto.identificatore == 'P' )
			{	
				// If the read character is pacman, save it in the pacman variable 
				pacman = personaggioLetto;
				
				// In case pacman ate a dot or a power pill
				if ( labirinto[pacman.posizione.y][pacman.posizione.x] == '.' || labirinto[pacman.posizione.y][pacman.posizione.x] == 'P' )
				{	
					// If pacman ate a power pill, the ghosts will be notified
					if ( labirinto[pacman.posizione.y][pacman.posizione.x] == 'P' )
					{	
						for ( i = 0; i < MAX_FANTASMI; i++ )
						{	
							if ( fantasmi[i] == 1 )
							{	
								info[i].powerPill = 1;	
							}
						}
					}
					
					// Increase the score
					if ( labirinto[pacman.posizione.y][pacman.posizione.x] == '.' )
					{	
						punteggio += PUNTO_STANDARD;	
					}
					else 
					{	
						punteggio += PUNTO_POWERPILL;	
					}
					
					// Update the labyrinth
					labirinto[pacman.posizione.y][pacman.posizione.x] = ' ';
					
					// Decrease the number of dots to eat before a new ghost is created
					counterCreazioneFantasma--;
				}
				
				// If "Q" key is pressed, the game will quit
				if ( personaggioLetto.quit == 1 )
				{	
					vite = -1;	
				}
			}
			else
			{
				if ( personaggioLetto.identificatore == 'F' )
				{
					// If the character is a ghost, save it in the ghost array
					elementoNonTrovato = TRUE;
					
					// Browse the ghost list to find the one read
					for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link )
					{
						// If the read character is on the list, the element in the list is updated
						if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio )
						{	
							ptr1->elemento = personaggioLetto;
							
							elementoNonTrovato = FALSE;
						}
					}
					
					// If the character is not in the list, it is added
					if ( elementoNonTrovato == TRUE )
					{	
						inserisciNellaLista( &listaFantasmi, personaggioLetto );
						
						for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link )
						{	
							if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio )
							{	
								ptr1->elemento = personaggioLetto;	
							}
						}
					}
					
					// Check if the ghost cross the path of another ghost and if it has to change direction
					for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link )
					{	
						if ( personaggioLetto.idPersonaggio != ptr1->elemento.idPersonaggio &&
							( personaggioLetto.posizione.x + personaggioLetto.movimento.direzionex == ptr1->elemento.posizione.x &&
							  personaggioLetto.posizione.y + personaggioLetto.movimento.direzioney == ptr1->elemento.posizione.y ) ||
							( personaggioLetto.posizione.x + personaggioLetto.movimento.direzionex == ptr1->elemento.posizione.x + ptr1->elemento.movimento.direzionex &&
							  personaggioLetto.posizione.y + personaggioLetto.movimento.direzioney == ptr1->elemento.posizione.y + ptr1->elemento.movimento.direzioney ) || 
							( personaggioLetto.posizione.x == ptr1->elemento.posizione.x + ptr1->elemento.movimento.direzionex &&
							  personaggioLetto.posizione.y == ptr1->elemento.posizione.y + ptr1->elemento.movimento.direzioney ) ) 
						{
							info[personaggioLetto.idPersonaggio].rimbalzo = 1;	
						}
					}
					
					// Write the ghost in a pipe
					write( pipeAiFantasmi[personaggioLetto.idPersonaggio][WRITE], &info[personaggioLetto.idPersonaggio], sizeof( infoFantasmi ) );
					
					info[personaggioLetto.idPersonaggio].powerPill = 0;
					info[personaggioLetto.idPersonaggio].rimbalzo = 0;
				} 
				else 
				{
					// If the character is a missile, we search it into the missile array
					elementoNonTrovato = TRUE;
					
					for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link )
					{	
						if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio )
						{	
							ptr1->elemento = personaggioLetto;
							
							elementoNonTrovato = FALSE;	
						}
						
						if ( ptr1->elemento.morto == 1 )
						{	
							cancellaDallaLista( &listaMissili, ptr1->elemento );	
						}
					}
					
					if ( elementoNonTrovato == TRUE )
					{	
						inserisciNellaLista( &listaMissili, personaggioLetto );
						
						for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link )
						{	
							if ( ptr1->elemento.idPersonaggio == personaggioLetto.idPersonaggio )
							{	
								ptr1->elemento = personaggioLetto;	
							}
						}
					}
				}
			}
			
			// Each 10000 points, the player gets one extra life
			if ( ( punteggio % 10000 ) == 0 && punteggio != 0 )
			{	
				vite++;	
			}
			
			// Check if one of the ghost is in the same position of pacman
			for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link )
			{	
				if ( pacman.posizione.x == ptr1->elemento.posizione.x &&
					 pacman.posizione.y == ptr1->elemento.posizione.y && 
					 pacman.mosso != 0 )
				{	
					// Kill the pacman process
					err = kill( pacman.pid, 1 );
					while ( err != 0 )
					{	
						err = kill( pacman.pid, 1 );	
					}
					
					// Decrease the number of lifes
					vite--;
					
					// Create a new pacman
					generaPacman( pipeDaiPersonaggi );
					// Energy is restored
					energia = MAX_ENERGIA;
				}
			}
			
			// Check if missiles hit anything
			for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link )
			{	
				// Check pacman
				if ( pacman.posizione.x == ptr1->elemento.posizione.x && pacman.posizione.y == ptr1->elemento.posizione.y )
				{	
					if ( pacman.mosso != 0 )
					{	
						if ( ptr1->elemento.creatore != 'P')
						{	
							energia--;	
						}
					}
					
					err = kill( ptr1->elemento.pid, 1 );
					while ( err != 0 )
					{	
						err = kill( ptr1->elemento.pid, 1 );
					}
					
					cancellaDallaLista( &listaMissili, ptr1->elemento );
					
				}
				
				// Check the ghosts
				for ( ptr2 = listaFantasmi; ptr2 != NULL; ptr2 = ptr2->link )
				{	
					if ( ptr1->elemento.posizione.x == ptr2->elemento.posizione.x && ptr1->elemento.posizione.y == ptr2->elemento.posizione.y )
					{	
						if ( ptr1->elemento.creatore != 'F' )
						{	
							err = kill( ptr2->elemento.pid, 1 );
							while ( err != 0 )
							{	
								err = kill( ptr2->elemento.pid, 1);	
							}
							
							cancellaDallaLista( &listaFantasmi, ptr2->elemento );
							fantasmi[ptr2->elemento.idPersonaggio] = 0;
							
							if ( ptr2->elemento.morto == FALSE )
							{	
								generaFantasma( pipeDaiPersonaggi[WRITE], trovaPipe( pipeAiFantasmi, ptr2->elemento.idPersonaggio + FANTASMI ), ptr2->elemento.idPersonaggio + FANTASMI, posizioneInizialeFantasma, TRUE );
								fantasmi[ptr2->elemento.idPersonaggio + FANTASMI] = 1;
							}
							
							counterRespownFantasma[ptr2->elemento.idPersonaggio] = rand() % PASSI_RESPOWN_FANTASMA;
						}
						
						err = kill( ptr1->elemento.pid, 1 );
						while ( err != 0 )
						{	
							err = kill( ptr1->elemento.pid, 1 );	
						}
						
						ptr1->elemento.morto = 1;
					}
				}
				
				// Check other missiles
				for ( ptr2 = listaMissili; ptr2 != NULL; ptr2 = ptr2->link )
				{	
					if ( ptr1->elemento.posizione.x == ptr2->elemento.posizione.x &&
						 ptr1->elemento.posizione.y == ptr2->elemento.posizione.y &&
						 ptr1->elemento.idPersonaggio != ptr2->elemento.idPersonaggio )
					{
						err = kill( ptr1->elemento.pid, 1 );
						while ( err != 0 )
						{	
							err = kill( ptr1->elemento.pid, 1 );	
						}
						ptr1->elemento.morto = 1;
						
						err = kill( ptr2->elemento.pid, 1 );
						while ( err != 0 )
						{	
							err = kill( ptr2->elemento.pid, 1);	
						}
						ptr2->elemento.morto = 1;
					}
				}
			}
			
			// If pacman's energy is 0, it dies
			if ( energia == 0 )
			{
				err = kill( pacman.pid, 1 );
				while ( err != 0 )
				{	
					err = kill( pacman.pid, 1 );	
				}
				
				// Generate a new pacman
				generaPacman( pipeDaiPersonaggi );
				// The energy is restores
				energia = MAX_ENERGIA;
				// Reduce player's life
				vite--;
			}
			
			// Check if pacman ate the dots required to generate a ghost
			if ( counterCreazioneFantasma == 0 )
			{	
				// Create a ghost
				generaFantasma( pipeDaiPersonaggi[WRITE], trovaPipe( pipeAiFantasmi, numeroFantasmi ), numeroFantasmi, pacman.posizione, FALSE );
				fantasmi[numeroFantasmi] = 1;
				
				// Increase by one the number of ghosts
				numeroFantasmi++;
				// Decrease the counter to avoid crazy forks
				counterCreazioneFantasma--;	
			}
			
			// Check ghosts' respawn countdown
			for ( i = 0; i < MAX_FANTASMI; i++ )
			{	
				if ( counterRespownFantasma[i] == 0 ) 
				{
					// If the countdown is 0, generate a ghost	
					generaFantasma( pipeDaiPersonaggi[WRITE], trovaPipe( pipeAiFantasmi, i ), i, posizioneInizialeFantasma, TRUE );
					fantasmi[i] = 1;
					// Decrease the counter to avoid crazy forks
					counterRespownFantasma[i]--;
				}
			}
			
			// Missile generation
			if ( personaggioLetto.missile == 1 )
			{	
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x + 2, personaggioLetto.posizione.y ) == 1 )
				{
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_DESTRA], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;	
				}
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x - 2, personaggioLetto.posizione.y ) == 1 )
				{	
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_SINISTRA], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;	
				}
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x, personaggioLetto.posizione.y + 1 ) == 1 )
				{	
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_GIU], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;	
				}
				
				if ( posizioneSuccessivaConsentitaMissili( personaggioLetto.posizione.x, personaggioLetto.posizione.y - 1 ) == 1 )
				{	
					generaMissile( pipeDaiPersonaggi[WRITE], direzioni[MISSILE_SU], personaggioLetto.posizione, numeroMissili, personaggioLetto.identificatore );
					numeroMissili++;	
				}
			}
			
			// The labyrinth is updated and the number of remaining dots is saved
			vittoria = stampaPallini();
			
			// Print missiles
			for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link )
			{
				if ( ptr1->elemento.morto == 1 )
				{	
					mvprintw( ptr1->elemento.posizione.y + 2, ptr1->elemento.posizione.x, "%c", labirinto[ptr1->elemento.posizione.y][ptr1->elemento.posizione.x] );
					
					cancellaDallaLista( &listaMissili, ptr1->elemento );	
				} 
				else 
				{	
					mvprintw( ptr1->elemento.posizione.y + 2, ptr1->elemento.posizione.x, "=" );	
				}
			}
			
			// Print the ghosts
			for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link ) 
			{	
				stampaFantasma ( ptr1->elemento );	
			}
			
			// Print pacman
			attron( COLOR_PAIR( PACMAN ) );
			stampaPacman( pacman );
			attroff( COLOR_PAIR( PACMAN ) );
			
			// Print the number of remaning lifes
			for ( i = 0; i < RIGHE; i++ )
			{	
				mvprintw( INFO_VITE, i, " ");	
			}
			for ( i = 0; i < vite; i++ )
			{	
				attron( COLOR_PAIR( PACMAN ) );
				mvprintw( INFO_VITE, COLONNA_VITE + ( i * 2 ), ">");
				attroff( COLOR_PAIR( PACMAN ) );	
			}
			
			// Print the remaining energy
			mvprintw( INFO_ENERGIA, COLONNA_ENERGIA, "[" );
			mvprintw( INFO_ENERGIA, COLONNA_ENERGIA + MAX_ENERGIA + 1, "]" );
			for ( i = 0; i < MAX_ENERGIA; i++ )
			{	
				mvprintw( INFO_ENERGIA, COLONNA_ENERGIA + i + 1, " " );	
			}
			for ( i = 0; i < energia; i++ )
			{	
				mvprintw( INFO_ENERGIA, COLONNA_ENERGIA + i + 1, "=" );	
			}
			
			// Print the score
			mvprintw( INFO_PUNTEGGIO, COLONNA_PUNTEGGIO, "1UP" );
			mvprintw( INFO_PUNTEGGIO + 1, COLONNA_PUNTEGGIO + 1, "%d", punteggio );
			mvprintw( INFO_PUNTEGGIO, COLONNA_HIGH_SCORE, "HIGH SCORE" );
			
			if ( punteggio < highScore )
			{	
				mvprintw( INFO_PUNTEGGIO + 1, COLONNA_HIGH_SCORE + 1, "%d", highScore );	
			} 
			else
			{	
				mvprintw( INFO_PUNTEGGIO + 1, COLONNA_HIGH_SCORE + 1, "%d", punteggio );	
			}
			
			// Refresh the screen
			refresh();
		} 
		while ( vittoria != 0 && vite > 0 );
		// The cycle is repeated until the number of dots in the labyrinth is 0 or the player ends his lifes
		
		// Kill pacman process
		err = kill( pacman.pid, 1);
		while ( err != 0 )
		{	
			err = kill( pacman.pid, 1 );	
		}
		
		// Kill the ghosts processes
		for ( ptr1 = listaFantasmi; ptr1 != NULL; ptr1 = ptr1->link )
		{	
			err = kill( ptr1->elemento.pid, 1 );
			while ( err != 0 )
			{	
				err = kill( ptr1->elemento.pid, 1 );	
			}
			cancellaDallaLista( &listaFantasmi, ptr1->elemento );
		}
		
		// Kill missiles processes
		for ( ptr1 = listaMissili; ptr1 != NULL; ptr1 = ptr1->link )
		{	
			err = kill( ptr1->elemento.pid, 1 );
			while ( err != 0 )
			{	
				err = kill( ptr1->elemento.pid, 1 );	
			}
			cancellaDallaLista( &listaMissili, ptr1->elemento );
		}
		
		// If all dots are eaten, the winning animation is shown
		if ( vittoria == 0 )
		{	
			fineLivello();	
		}
		
		// Clear the screen
		clear();
		
		// The labyrinth is restored from the backup
		for ( i = 0; i < RIGHE; i++ )
		{	
			for ( j = 0; j < COLONNE; j++ )
			{	
				labirinto[i][j] = labirintoBackup[i][j];	
			}
		}
		
		// The high score is updated if the new score is higher
		if ( punteggio > highScore )
		{	
			highScore = punteggio;
			
			score = fopen( FILE_PUNTEGGIO, "w" );
			
			if ( score != NULL )
			{	
				fprintf( score, "%d", highScore);
				fclose( score );	
			}
		}
	} 
	while ( vite > 0 && pacman.quit == 0 );
	// This cycle is repeated until the player ends his lifes or quit the game
}

// Print the labyrinth's walls
void stampaMuri ( void )
{	
	int i, j;
	
	for ( i = 0; i < RIGHE; i++ )
	{	
		for( j = 0; j < COLONNE; j++ )
		{	
			switch( labirinto[i][j] )
			{		
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

// Print the dots on the labyrinth
int stampaPallini ( void )
{	
	int i, j;
	// Dots counter
	int puntini = 0;
	
	attron( COLOR_PAIR( PACMAN ) );
	
	for ( i = 0; i < RIGHE; i++ )
	{	
		for( j = 0; j < COLONNE; j++ )
		{	
			switch( labirinto[i][j] )
			{		
				case '.':
					mvaddch( i + 2, j, ACS_BULLET );
					// Increase the dots counter
					puntini++;
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

// Winner animation
void fineLivello ( void )
{	
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
