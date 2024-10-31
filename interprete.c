#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
#include "interprete.h"
#include "curiosity.h"
#include "listes.h"
#include "pile.h"

/*
 *  Auteur(s) :
 *  Date :
 *  Suivi des Modifications :
 *
 */

void stop(void) {
    char enter = '\0';
    printf("Appuyer sur entrée pour continuer...\n");
    while (enter != '\r' && enter != '\n') {
        enter = getchar();
    }
}

////// ACTIONS PILE COMMANDES //////
int addition(pile_cmd *pile) {
    int a, b;

    a = depiler_int(pile);
    b = depiler_int(pile);

    empiler_int(pile, a + b); // ATTENTION : vérifier que a+b <= 9
    return a + b;
}

int soustraction(pile_cmd *pile) {
    int a, b;

    a = depiler_int(pile);
    b = depiler_int(pile);

    empiler_int(pile, a - b); // ATTENTION : vérifier que a-b >= 0
    return a - b;
}

int multiplication(pile_cmd *pile) {
    int a, b;

    a = depiler_int(pile);
    b = depiler_int(pile);

    empiler_int(pile, a * b); // ATTENTION : vérifier que 9 >= a*b >= 0
    return a * b;
}

void poser_marque(pile_cmd *pile) {
    int f;

    f = depiler_int(pile);
    pose(f);
}

void effectuer_mesure(pile_cmd *pile) {
    int direction, resultat;

    direction = depiler_int(pile);
    printf("DIRECTION : %c\n", direction + '0');
    resultat = mesure(direction);
    empiler_int(pile, resultat);
}


/* n V F ? exécute la pile V si n != 0, sinon la pile F */
void execution_conditionnelle(pile_cmd *pile, int *ret, int *profondeur) {
    int n;
    char c;
    pile_cmd *V, *F;

    F = depiler_groupe_commandes(pile);
    V = depiler_groupe_commandes(pile);

    // On récupère la valeur de n
    c = depiler_char(pile);
    if (isdigit(c)) {
        n = c - '0';
    }
    else {
        executer_commande(c, pile, ret, profondeur);
        n = depiler_int(pile);
    }

    // On remplace la pile de commandes par la pile à exécuter
    pile = n == 0 ? F : V;

    // On exécute les commandes stockées dans la pile
    executer_groupe_commandes(pile, ret, profondeur);
}

void mysterieuze(pile_cmd *pile) {
    return;
}

void exec(pile_cmd *pile, int *ret, int *profondeur) {
    char commande;
    pile_cmd *groupe;

    if (pile->tete->valeur == '}') {
        groupe = depiler_groupe_commandes(pile);
        if (groupe == NULL) return;
        executer_groupe_commandes(groupe, ret, profondeur);
    } else {
        commande = depiler_char(pile);
        executer_commande(commande, pile, ret, profondeur);
    }
}

void echange(pile_cmd *pile) {
    pile_cmd *A, *B;

    A = depiler_groupe_commandes(pile);
    B = depiler_groupe_commandes(pile);

    empiler_groupe(pile, A);
    empiler_groupe(pile, B);
}

void clone(pile_cmd *pile) {
    pile_cmd *groupe;
    cellule_pile_cmd *cel;

    if (pile->tete->valeur == '}') {
        // Dépile le groupe de commandes et l'empile deux fois
        groupe = depiler_groupe_commandes(pile);

        // Empile le groupe deux fois sur la pile principale
        empiler_groupe(pile, groupe);
        empiler_groupe(pile, groupe);

        // Libère la mémoire du groupe après avoir transféré les commandes
        free(groupe);
    } else {
        // Si la valeur n'est pas '}', cloner le sommet de la pile normalement
        cel = depiler(pile);
        if (cel != NULL) {
            empiler(pile, cel->valeur, cel->type);
            empiler(pile, cel->valeur, cel->type);
            free(cel);
        }
    }
}

void boucle(pile_cmd *pile_commandes, int *ret, int *profondeur) {
    int n;
    pile_cmd *cmd;
    
    n = depiler_int(pile_commandes);
    cmd = depiler_groupe_commandes(pile_commandes);
    while (n > 0) {
        executer_groupe_commandes(cmd, ret, profondeur);
        n--;
    }
}

/* Ignore la commande ou le groupe de commandes au sommet de la pile */
void ignore_commande(pile_cmd *pile) {
    pile_cmd *groupe;

    printf("tete : %c\n", pile->tete->valeur);
    if (pile->tete->valeur == '}') {
        groupe = depiler_groupe_commandes(pile);
        afficher_pile(groupe);
    } else depiler_char(pile);
}
////////////

/* Transforme un char en opération sur la carte */
void executer_commande(char commande, pile_cmd *pile_commandes, int *ret, int *profondeur) {
    *ret = REUSSI;

    printf("exec %c\n", commande);

    switch (commande) {
    case 'A':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else *ret = avance();
        break;

    case 'D':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else droite();
        break;

    case 'G':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else gauche();
        break;

    case 'P':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else poser_marque(pile_commandes);
        break;

    case 'M':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else effectuer_mesure(pile_commandes);
        break;

    case 'Z':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else mysterieuze(pile_commandes);
        break;

    case 'X':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else echange(pile_commandes);
        break;

    case 'C':
        clone(pile_commandes);
        break;

    case 'B':
        boucle(pile_commandes, ret, profondeur);
        break;

    case '!':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else exec(pile_commandes, ret, profondeur);
        break;

    case '+':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else addition(pile_commandes);
        break;

    case '-':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else soustraction(pile_commandes);
        break;

    case '*':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else multiplication(pile_commandes);
        break;

    case '?':
        if (*profondeur > 0) empiler_char(pile_commandes, commande);
        else execution_conditionnelle(pile_commandes, ret, profondeur);
        break;

    case '{':
        (*profondeur)++;
        empiler_char(pile_commandes, '{');
        break;

    case '}':
        (*profondeur)--;
        empiler_char(pile_commandes, '}');
        break;

    case 'I':
        ignore_commande(pile_commandes);
        break;

    default:
        if (isdigit(commande)) {
            empiler_int(pile_commandes, commande - '0');
        }
        else {
            eprintf("Caractère inconnu: '%c'\n", commande);
        }
    }
}


int interprete(sequence_t *seq_instructions, bool debug) {
    int ret; // utilisée pour les valeurs de retour
    int profondeur;
    char commande;

    cellule_t *cel_commande, *cel_prev;
    pile_cmd *pile_commandes;

    ////// INITIALISATION //////
    profondeur = 0;
    cel_commande = seq_instructions->tete;
    commande = cel_commande->command;
    pile_commandes = init_pile();

    ////// AFFICHAGE //////
    debug = true; /* À enlever par la suite et utiliser "-d" sur la ligne de
                     commandes */

    printf("Programme: ");
    afficher(seq_instructions);
    printf("\n");
    if (debug)
        stop();

    while (cel_commande != NULL) {
        commande = cel_commande->command;

        executer_commande(commande, pile_commandes, &ret, &profondeur);
        if (ret == VICTOIRE)
            return VICTOIRE; /* on a atteint la cible */
        if (ret == RATE)
            return RATE; /* tombé dans l'eau ou sur un rocher */

        /* Affichage pour faciliter le debug */
        afficherCarte();
        printf("Programme: ");
        afficher(seq_instructions);
        printf("Pile :\n");
        afficher_pile(pile_commandes);
        printf("\n");
        if (debug)
            stop();

        cel_prev = cel_commande;
        cel_commande = cel_commande->suivant;
        seq_instructions->tete = cel_commande;
        free(cel_prev);
    }

    /* Si on sort de la boucle sans arriver sur la cible,
     * c'est raté :-( */

    return CIBLERATEE;
}
