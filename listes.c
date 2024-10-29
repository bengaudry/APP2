#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
#include "listes.h"


/*
 *  Auteur(s) :
 *  Date :
 *  Suivi des Modifications :
 *
 */

bool silent_mode = false;


cellule_t* nouvelleCellule(void)
{
    cellule_t* cel;

    cel = malloc(sizeof(cellule_t));
    cel->suivant = NULL;

    return cel;
}


void detruireCellule(cellule_t* cel)
{
    free(cel);
}


void conversion(char* texte, sequence_t* seq)
{
    int i;
    cellule_t* cel, * cel_p;

    if (texte[0] == '\0') {
        seq->tete = NULL;
        return;
    }

    cel_p = nouvelleCellule();
    cel_p->command = texte[0];
    cel_p->suivant = NULL;
    seq->tete = cel_p;

    for (i = 1; texte[i] != '\0'; i++) {
        if (texte[i] != '\n' && texte[i] != ' ') {
            cel = nouvelleCellule();

            cel->command = texte[i];
            cel->suivant = NULL;

            cel_p->suivant = cel;
            cel_p = cel;
        }
    }
}






void afficher(sequence_t* seq)
{
    int i;
    cellule_t* cel;

    assert(seq); /* Le pointeur doit être valide */

    cel = seq->tete;
    for (i = 0; cel != NULL; i++) {
        printf("%c, ", cel->command);
        cel = cel->suivant;
    }
    printf("\n");
}

