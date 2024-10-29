#include <stdlib.h>
#include <stdio.h>
#include "pile.h"

pile_cmd *init_pile(void) {
    pile_cmd *pile;

    pile = malloc(sizeof(pile_cmd));
    pile->tete = NULL;

    return pile;
}

void empiler(pile_cmd *pile, char val, type_cmd type) {
    cellule_pile_cmd *ancienne_tete, *cel;

    cel = malloc(sizeof(cellule_pile_cmd));

    ancienne_tete = pile->tete;

    cel->type = type;
    cel->valeur = val;
    cel->suivant = ancienne_tete;

    pile->tete = cel;
}

void empiler_char(pile_cmd *pile, char c) { empiler(pile, c, CHAR); }

void empiler_int(pile_cmd *pile, int n) { empiler(pile, n + '0', INT); }

cellule_pile_cmd *depiler(pile_cmd *pile) {
    cellule_pile_cmd *ancienne_tete;

    ancienne_tete = pile->tete;
    pile->tete = ancienne_tete->suivant;

    return ancienne_tete;
}

int depiler_int(pile_cmd *pile) {
    int n;
    cellule_pile_cmd *cel;

    cel = depiler(pile);
    n = cel->valeur - '0';
    free(cel);

    return n;
}

char depiler_char(pile_cmd *pile) {
    char c;
    cellule_pile_cmd *cel;

    cel = depiler(pile);
    c = cel->valeur;
    free(cel);

    return c;
}

void inverser_pile(pile_cmd *pile) {
    cellule_pile_cmd *precedent = NULL;
    cellule_pile_cmd *courant = pile->tete;
    cellule_pile_cmd *suivant = NULL;

    while (courant != NULL) {
        suivant = courant->suivant;
        courant->suivant = precedent;

        precedent = courant;
        courant = suivant;
    }

    pile->tete = precedent;
}

void afficher_pile(pile_cmd *pile) {
    int i;
    cellule_pile_cmd *cel;

    cel = pile->tete;
    if (cel == NULL) printf("<PILE VIDE>\n");
    for (i = 0; cel != NULL; i++) {
        printf("[%d] | %s %c\n", i, cel->type == INT ? "INT" : "CHAR", cel->valeur);
        cel = cel->suivant;
    }
    printf("\n");
}

