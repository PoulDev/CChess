#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/*
	----- COSA MANCA AL COMPLETAMENTO? -----
	 - I pedoni dovrebbero potersi spostare di 2 pedine alla mossa iniziale, per ora non possono.
	 - Se sei sotto scacco puoi fare quel che vuoi, invece dovrebbe renderti possibile fare solamente mosse che ti tolgono dallo scacco.
	 - Lo stallo non e' implementato.
	 - L'En passant non e' implementato.
	 - Possibili bug(???)
*/

/*
 * 0 = Null
 * 1 = Pedina
 * 2 = Torre
 * 3 = Alfiere
 * 4 = Cavallo
 * 5 = Regina
 * 6 = Re
*/

#define DEBUG 0

// ---------------- TESTS BOARDS ---------------- 

void clear_board(short int board[8][8]) {
	int linea, quadrante;

	for (linea = 0; linea < 8; linea++) {
		for (quadrante = 0; quadrante < 8; quadrante++) {
			board[linea][quadrante] = 0;
		}
	}
}

void test_board_cavallo(short int scacchiera[8][8]) {
	clear_board(scacchiera);
	scacchiera[4][4] = 4;
	scacchiera[3][2] = 1;
	scacchiera[3][6] = -1;
}

void test_board_alfiere(short int scacchiera[8][8]) {
	clear_board(scacchiera);
	scacchiera[3][4] = 3;
	//scacchiera[3][2] = 1;
	//scacchiera[3][6] = -1;
}

void test_board_regina(short int scacchiera[8][8]) {
	clear_board(scacchiera);
	scacchiera[3][4] = 5;
	scacchiera[3][2] = 1;
	scacchiera[3][6] = -1;
}

void test_board_scacco(short int scacchiera[8][8]) {
	clear_board(scacchiera);
	scacchiera[0][5] = -6;
    //scacchiera[4][4] = -2;

	scacchiera[0][0] = 2;
    scacchiera[1][0] = 2;
	scacchiera[7][7] = 6;
}

void test_board_torre(short int scacchiera[8][8]) {
	clear_board(scacchiera);
	scacchiera[4][4] = 2;
	scacchiera[4][2] = -1;
	scacchiera[4][6] = -1;
	scacchiera[2][4] = -1;
	scacchiera[6][4] = -1;
}

void copy_board(short int from[8][8], short int to[8][8]) {
    int x, y;

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {
            to[y][x] = from[y][x];
        }
    }
}

int pedina_presente(short int scacchiera[8][8], int x, int y) {
	return scacchiera[y][x] != 0;
}

int pedina_avversaria_presente(short int scacchiera[8][8], int selected[2], int x, int y) {
	int pedina, altra_pedina;

	if (!pedina_presente(scacchiera, x, y)) {
		return 0;
	}

	pedina = scacchiera[selected[1]][selected[0]];
	altra_pedina = scacchiera[y][x];

	if (pedina > 0 && altra_pedina < 0) {return 1;}
	else if (pedina < 0 && altra_pedina > 0) {return 1;}
	else {return 0;}
}

int is_out_board(int x, int y) {
	return x > 7 || x < 0 || y > 7 || y < 0;
}

// -------- MOVIMENTO PEDINE ---------- // 

void movimenti_torre(short int scacchiera[8][8], short int highlights[8][8], int selected[2]) {
	int x, y, pedina_avversaria;

	// Linea Verticale in basso
	for (y = selected[1]+1; y < 8; y++) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, selected[0], y);
		if (!pedina_avversaria && pedina_presente(scacchiera, selected[0], y)) {break;}
		highlights[y][selected[0]] = 1;
		if (pedina_avversaria) {break;}
	}

	// Linea Verticale in alto 
	for (y = selected[1]-1; y >= 0; y--) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, selected[0], y);
		if (!pedina_avversaria && pedina_presente(scacchiera, selected[0], y)) {break;}
		highlights[y][selected[0]] = 1;
		if (pedina_avversaria) {break;}
	}

	// Linea Orizzontale a destra 
	for (x = selected[0]+1; x < 8; x++) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, x, selected[1]);
		if (!pedina_avversaria && pedina_presente(scacchiera, x, selected[1])) {break;}
		highlights[selected[1]][x] = 1;
		if (pedina_avversaria) {break;}
	}

	// Linea Orizzontale a sinistra
	for (x = selected[0]-1; x >= 0; x--) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, x, selected[1]);
		if (!pedina_avversaria && pedina_presente(scacchiera, x, selected[1])) {break;}
		highlights[selected[1]][x] = 1;
		if (pedina_avversaria) {break;}
	}

}

void movimenti_cavallo(short int scacchiera[8][8], short int highlights[8][8], int selected[2]) {
	int x, y;

	y = selected[1] - 1;
	x = selected[0] + 2;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}

	y = selected[1] - 1;
	x = selected[0] - 2;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}

	y = selected[1] - 2;
	x = selected[0] + 1;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}

	y = selected[1] - 2;
	x = selected[0] - 1;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}


	// verso il basso

	y = selected[1] + 2;
	x = selected[0] + 1;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}

	y = selected[1] + 2;
	x = selected[0] - 1;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}

	y = selected[1] + 1;
	x = selected[0] + 2;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}

	y = selected[1] + 1;
	x = selected[0] - 2;
	if ((!pedina_presente(scacchiera, x, y) || pedina_avversaria_presente(scacchiera, selected, x, y)) && !is_out_board(x, y)) {
		highlights[y][x] = 1;
	}
}

void movimenti_alfiere(short int scacchiera[8][8], short int highlights[8][8], int selected[2]) {
	int x, y, pedina_avversaria;

	x = selected[0]+1;
	for (y = selected[1]+1; y < 8; y++) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, x, y);
		if (!pedina_avversaria && pedina_presente(scacchiera, x, y)) {break;}
		highlights[y][x] = 1;
		if (pedina_avversaria) {break;}
		x += 1;
		if (x > 7) {break;}
	}

	x = selected[0]+1;
	for (y = selected[1]-1; y >= 0; y--) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, x, y);
		if (!pedina_avversaria && pedina_presente(scacchiera, x, y)) {break;}
		highlights[y][x] = 1;
		if (pedina_avversaria) {break;}
		x += 1;
		if (x > 7) {break;}
	}

	x = selected[0]-1;
	for (y = selected[1]-1; y >= 0; y--) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, x, y);
		if (!pedina_avversaria && pedina_presente(scacchiera, x, y)) {break;}
		highlights[y][x] = 1;
		if (pedina_avversaria) {break;}
		x -= 1;
		if (x < 0) {break;}
	}

	x = selected[0]-1;
	for (y = selected[1]+1; y < 8; y++) {
		pedina_avversaria = pedina_avversaria_presente(scacchiera, selected, x, y);
		if (!pedina_avversaria && pedina_presente(scacchiera, x, y)) {break;}
		highlights[y][x] = 1;
		if (pedina_avversaria) {break;}
		x -= 1;
		if (x < 0) {break;}
	}

}

void movimenti_regina(short int scacchiera[8][8], short int highlights[8][8], int selected[2]) {
	movimenti_torre(scacchiera, highlights, selected);
	movimenti_alfiere(scacchiera, highlights, selected);
}

void movimenti_re(short int scacchiera[8][8], short int highlights[8][8], int selected[2]) {
    int x, y;

    x = selected[0] + 1;
    y = selected[1];
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0] - 1;
    y = selected[1];
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0];
    y = selected[1] + 1;
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0] + 1;
    y = selected[1] + 1;
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0] - 1;
    y = selected[1] + 1;
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0];
    y = selected[1] - 1;
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0] + 1;
    y = selected[1] - 1;
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}

    x = selected[0] - 1;
    y = selected[1] - 1;
    if (!is_out_board(x, y)) {highlights[y][x] = 1;}
}

void init_board(short int scacchiera[8][8]) {
	int linea, quadrante;

	clear_board(scacchiera);

	for (quadrante = 0; quadrante < 8; quadrante++) {
		scacchiera[1][quadrante] = 1;
		scacchiera[6][quadrante] = -1;
	}

	scacchiera[0][0] = 2;
	scacchiera[0][7] = 2;
	scacchiera[0][1] = 4;
	scacchiera[0][6] = 4;
	scacchiera[0][2] = 3;
	scacchiera[0][5] = 3;
	scacchiera[0][3] = 5;
	scacchiera[0][4] = 6;

	scacchiera[7][0] = -2;
	scacchiera[7][7] = -2;
	scacchiera[7][1] = -4;
	scacchiera[7][6] = -4;
	scacchiera[7][2] = -3;
	scacchiera[7][5] = -3;
	scacchiera[7][3] = -6;
	scacchiera[7][4] = -5;
}

char number_to_pedina(int pedina) {
	switch (abs(pedina)) {
		case 0:
			return ' ';
		case 1:
			return '@';
		case 2:
			return 'T';
		case 3:
			return '^';
		case 4:
			return '?';
		case 5:
			return '&';
		case 6:
			return '#';
	}
	return '!';
}

void print_white_spaces(int linea, int selected[2], int is_white) {
	int quadrante, is_selected;

	for (quadrante = 0; quadrante < 8; quadrante++) {
		is_selected = (linea == selected[1] && quadrante == selected[0]);

		if (is_selected) {
			printf("\033[2;30;41m");
			if (is_white == 1) {is_white = 0;} else {is_white = 1;}
		} else {
			if (is_white == 1) {
				printf("\033[7;30;47m");
				is_white = 0;
			} else {
				printf("\033[2;30;47m");
				is_white = 1;
			}
		}
		printf("      \033[0m");

	} 
}

void space(int length) {
	int i;
	for (i = 0; i < length; i++) {
		printf(" ");
	}
}

int print_info(char* infos[3*8], int info_index) {
	if (info_index < 3*8) {
		space(5);
		printf(infos[info_index]);
	}
	return info_index + 1;
}

void print_board(short int scacchiera[8][8], int selected[2], short int highlights[8][8], char* additional_info[3*8]) {
	int linea, quadrante, is_white, is_selected, info_index;

	if (!DEBUG) {printf("\e[1;1H\e[2J");}
	else {printf("\n");}

	info_index = 0;
	


	for (linea = 0; linea < 8; linea++) {
		//printf("%d | ", 8-linea);
		if (linea % 2 == 0) {
			is_white = 1;
		} else {
			is_white = 0;
		}

		print_white_spaces(linea, selected, is_white);
		info_index = print_info(additional_info, info_index);
		printf("\n");

		for (quadrante = 0; quadrante < 8; quadrante++) {
			is_selected = ((linea == selected[1] && quadrante == selected[0]) || highlights[linea][quadrante]);

			if (is_selected) {
				printf("\033[2;30;41m \033[0m");
			}

			if (is_white == 1) {
				if (scacchiera[linea][quadrante] > 0) {
					printf("\033[7;30;43m");
				} else {
					printf("\033[7;30;44m");
				}
				is_white = 0;
			} else {
				if (scacchiera[linea][quadrante] > 0) {
					printf("\033[7;37;43m");
				} else {
					printf("\033[7;37;44m");
				}
				is_white = 1;
			}
			if (!is_selected) {
				printf(" ");
			}

			printf(" %c  ", number_to_pedina(scacchiera[linea][quadrante]));
			
			if (is_selected) { printf("\033[0m\033[2;30;41m"); }
			printf(" \033[0m");
		}
		info_index = print_info(additional_info, info_index);
		printf("\n");
		print_white_spaces(linea, selected, is_white);
		info_index = print_info(additional_info, info_index);
		printf("\n");
	}
}

void calculate_possible_moves(short int scacchiera[8][8], short int highlights[8][8], int selected[2]) {
	const int PEDINA = scacchiera[selected[1]][selected[0]];
	int x, y, pedina_avversaria;

	switch (PEDINA) {
		case 1: { // Pedone in alto
			// Muoversi in basso a destra
			if (pedina_avversaria_presente(scacchiera, selected, selected[0]+1, selected[1]+1)) {
				highlights[selected[1]+1][selected[0]+1] = 1;
			}
			// Muoversi in basso a sinistra
			if (pedina_avversaria_presente(scacchiera, selected, selected[0]-1, selected[1]+1)) {
				highlights[selected[1]+1][selected[0]-1] = 1;
			}
			// Muoversi in basso
			if (!pedina_presente(scacchiera, selected[0], selected[1]+1)) {
				highlights[selected[1]+1][selected[0]] = 1;
			}
			break;
		} case -1: { // Pedone in basso
			// Muoversi in alto a destra
			if (pedina_avversaria_presente(scacchiera, selected, selected[0]+1, selected[1]-1)) {
				highlights[selected[1]-1][selected[0]+1] = 1;
			}
			// Muoversi in alto a sinistra
			if (pedina_avversaria_presente(scacchiera, selected, selected[0]-1, selected[1]-1)) {
				highlights[selected[1]-1][selected[0]-1] = 1;
			}
			// Muoversi in alto
			if (!pedina_presente(scacchiera, selected[0], selected[1]-1)) {
				highlights[selected[1]-1][selected[0]] = 1;
			}
			break;
		} case 2: { // Torre in alto
			movimenti_torre(scacchiera, highlights, selected);
			break;
		} case -2: {
			movimenti_torre(scacchiera, highlights, selected);
			break;
		} case 3: {
			movimenti_alfiere(scacchiera, highlights, selected);
			break;
		} case -3: {
			movimenti_alfiere(scacchiera, highlights, selected);
			break;
		} case 4: {
			movimenti_cavallo(scacchiera, highlights, selected);
			break;
		} case -4: {
			movimenti_cavallo(scacchiera, highlights, selected);
			break;
		} case 5: {
			movimenti_regina(scacchiera, highlights, selected);
			break;
		} case -5: {
			movimenti_regina(scacchiera, highlights, selected);
			break;
		} case 6: {
            movimenti_re(scacchiera, highlights, selected);
            break;
        } case -6: {
            movimenti_re(scacchiera, highlights, selected);
            break;
        }
	}
}

// ------------ CONTROLLI SCACCO / SCACCO MATTO ----------------

/*
 * Return -1: Scacco per re Blu
 * Return +1: Scacco per re Rosso
 * Return 0: Nessuno Scacco
*/
int controllo_scacco(short int scacchiera[8][8], int squadra) {
	short int temp_highlights[8][8];
	int linea, casella, pedina, selected[2], king_red_pos[2], king_blue_pos[2];

	for (linea = 0; linea < 8; linea ++) {
		for (casella = 0; casella < 8; casella++) {
			if (scacchiera[linea][casella] == 6) {
				king_red_pos[0] = casella;
				king_red_pos[1] = linea;
			} else if (scacchiera[linea][casella] == -6) {
				king_blue_pos[0] = casella;
				king_blue_pos[1] = linea;
			}
		}
	}

	for (linea = 0; linea < 8; linea ++) {
		for (casella = 0; casella < 8; casella++) {
            //printf("%d ", scacchiera[linea][casella]);
			if (pedina_presente(scacchiera, casella, linea)) {
                clear_board(temp_highlights);
				selected[0] = casella;
				selected[1] = linea;
                pedina = scacchiera[linea][casella];

				calculate_possible_moves(scacchiera, temp_highlights, selected);
                //printf("Il re in %d;%d sotto scacco da %d;%d: %d | pedina: %d\n", king_blue_pos[0], king_blue_pos[1], selected[0], selected[1], temp_highlights[king_blue_pos[1]][king_blue_pos[0]], pedina);
				if (pedina > 0 && squadra <= 0) {
                    if (temp_highlights[king_blue_pos[1]][king_blue_pos[0]] == 1) {
                        return -1; // Re blu sotto scacco 
                    }
                } else if (pedina < 0 && squadra >= 0) {
                    if (temp_highlights[king_red_pos[1]][king_red_pos[0]] == 1) {
                        return 1; // Re rosso sotto scacco 
                    }
                }
			}
		}

	}

	return 0;
}

/*
 * Return 0: Nessuno scacco matto 
 * Return 1: Scacco matto sulla la squadra data
*/
int controllo_scacco_matto(short int scacchiera[8][8], int squadra) {
    short int scacchiera_temp[8][8], highlights[8][8];
    int x, y, px, py, pedina, selected[2], scacco_matto;

    for (y = 0; y < 8; y++) {
        for (x = 0; x < 8; x++) {    // Per ogni pedina del re sotto scacco 
            pedina = scacchiera[y][x];
            if (pedina > 0 && squadra < 0) {continue;} // Se la pedina e' della squadra opposta skippa
            if (pedina < 0 && squadra > 0) {continue;}

            clear_board(highlights);
            selected[0] = x;
            selected[1] = y;

            calculate_possible_moves(scacchiera, highlights, selected);
            for (py = 0; py < 8; py++) {
                for (px = 0; px < 8; px++) {

                    // Per ogni mossa possibile verifica se effettuandola si esce dallo scacco
                    if (highlights[py][px] != 1) {continue;}
                    //printf("Pedina %d (%d;%d) spostata in (%d;%d)...", scacchiera[y][x], x, y, px, py);
                    
                    copy_board(scacchiera, scacchiera_temp);
                    scacchiera_temp[py][px] = scacchiera_temp[y][x];
                    scacchiera_temp[y][x] = 0;

                    scacco_matto = controllo_scacco(scacchiera_temp, squadra);
                    //printf("%d ", scacco_matto);

                    if (scacco_matto != squadra) {
                        //printf("Il re puo uscire dallo scacco spostandosi in %d;%d\n", px, py);
                        return 0; // E' possibile uscire dallo scacco
                    }
                }
            }
        }
    }

    return 1;
}

void compila_informazioni(char* additional_info[3*8], int punti_rosso, int punti_blu) {
	additional_info[5] = "* -------------------------------------- *";
	additional_info[6] = "   Blu ha mangiato 0 pedine";
	additional_info[7] = "   Rosso ha mangiato 0 pedine";
	additional_info[8] = "    ( Non ho la minima idea di come "; 
	additional_info[9] = "       concatenare int e stringhe in C )";
	additional_info[10] = "* -------------------------------------- *";
}

int main() {
	short int scacchiera[8][8], highlights[8][8];
	int pointer[2], selected[2];

	int linea, quadrante, highlights_count, exit_loop, turn, punti_rosso, punti_blu, scacco;
	char input;
	char* additional_info[3*8];

	selected[0] = -1;

	for (linea = 0; linea < 3*8; linea++) {
		additional_info[linea] = "";
	}

	clear_board(highlights);

	init_board(scacchiera);
	//test_board_cavallo(scacchiera);
	//test_board_torre(scacchiera);
	//test_board_alfiere(scacchiera);
	//test_board_regina(scacchiera);
	//test_board_scacco(scacchiera);

	pointer[0] = 0;
	pointer[1] = 0;

	turn = 0;

	while (1) {
		compila_informazioni(additional_info, punti_rosso, punti_blu);
		print_board(scacchiera, pointer, highlights, additional_info);
		
        scacco = controllo_scacco(scacchiera, 0);
		if (scacco == 1 || scacco == -1) {
			printf("Scacco!\n", scacco);
			if (controllo_scacco_matto(scacchiera, scacco)) {
                printf("SCACCO MATTO!\n");
                return 0;
            }
		}
		printf("(x)Select (c)Confirm Move\n");

		scanf("%c", &input);

		switch (input) {
			case 'w':
				pointer[1] -= 1;
				break;
			case 's':
				pointer[1] += 1;
				break;
			case 'a':
				pointer[0] -= 1;
				break;
			case 'd':
				pointer[0] += 1;
				break;

			case 'x':
				if (selected[0] != -1) {
					highlights[selected[1]][selected[0]] = 0;
				}
				clear_board(highlights);

				if (scacchiera[pointer[1]][pointer[0]] == 0) {break;}
				if (turn && scacchiera[pointer[1]][pointer[0]] > 0) {break;}
				if (!turn && scacchiera[pointer[1]][pointer[0]] < 0) {break;}

				selected[0] = pointer[0];
				selected[1] = pointer[1];

				highlights[pointer[1]][pointer[0]] = 1;
				calculate_possible_moves(scacchiera, highlights, selected);
				break;
			case 'c': {
				if (highlights[pointer[1]][pointer[0]] == 0) {break;}
				if (pointer[0] == selected[0] && pointer[1] == selected[1]) {break;}

				if (pedina_avversaria_presente(scacchiera, selected, pointer[1], pointer[0])) {
					if (scacchiera[selected[1]][selected[0]] > 0) {
						punti_rosso += 1;
					} else {
						punti_blu += 1;
					}
				}

				scacchiera[pointer[1]][pointer[0]] = scacchiera[selected[1]][selected[0]];
				scacchiera[selected[1]][selected[0]] = 0;
				clear_board(highlights);
				turn = !turn;
				break;
			}
		}

        if (pointer[0] < 0) {
            pointer[0] = 7;
        } else if (pointer[0] > 7) {
            pointer[0] = 0;
        }
        
        if (pointer[1] < 0) {
            pointer[1] = 7;
        } else if (pointer[1] > 7) {
            pointer[1] = 0;
        }
	}

	return 0;
}
