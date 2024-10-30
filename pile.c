#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "pile.h"
#include "interprete.h"

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

void empiler_groupe(pile_cmd *pile, pile_cmd *groupe) {
    cellule_pile_cmd *cel, *cel_p;

    cel = groupe->tete;
    while (cel != NULL) {
        empiler(pile, cel->valeur, cel->type);
        cel_p = cel;
        cel = cel->suivant;
        free(cel_p);
    }
}

cellule_pile_cmd *depiler(pile_cmd *pile) {
    cellule_pile_cmd *ancienne_tete;

    if (pile->tete == NULL) return NULL; // Vérifie si la pile est vide

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

pile_cmd *depiler_groupe_commandes(pile_cmd *pile) {
    int profondeur;
    char c;
    pile_cmd *groupe_cmd;

    groupe_cmd = init_pile();
    profondeur = 0;
    c = depiler_char(pile); // c == '{'
    // On empile tous les caractères jusqu'a '{' dans la pile F
    while (true) {
        c = depiler_char(pile);
        if (c == '}') profondeur++;
        if (c == '{') {
            if (profondeur == 0) break;
            profondeur--;
        }
        //if (c == '?') execution_conditionnelle(F, ret, profondeur);
        empiler_char(groupe_cmd, c);
    }

    return groupe_cmd;
}

void executer_groupe_commandes(pile_cmd *groupe, int *ret, int *profondeur) {
    cellule_pile_cmd *cel;

    cel = groupe->tete;
    while (cel != NULL) {
        printf("%c\n", cel->valeur);
        if (cel->type == CHAR) {
            executer_commandes(cel->valeur, groupe, ret, profondeur);
        }
        cel = cel->suivant;
    }
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

