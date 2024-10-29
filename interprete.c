#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#ifdef NCURSES
#include <ncurses.h>
#endif
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

////// PILE COMMANDES //////

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
    resultat = mesure(direction);
    empiler_int(pile, resultat);
}

void execution_conditionnelle(pile_cmd *pile, int *ret, int *profondeur);
void executer_commandes(char commande, pile_cmd *pile_commandes, int *ret, int *profondeur);

pile_cmd *separer_groupe_commandes(pile_cmd *pile) {
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

void execution_conditionnelle(pile_cmd *pile, int *ret, int *profondeur) {
    int n;
    char c;
    pile_cmd *V, *F;
    cellule_pile_cmd *cel;

    F = separer_groupe_commandes(pile);
    V = separer_groupe_commandes(pile);

    // On récupère la valeur de n
    c = depiler_char(pile);
    if (isdigit(c)) {
        n = c - '0';
    }
    else {
        executer_commandes(c, pile, ret, profondeur);
        n = depiler_int(pile);
    }
    
    // On remplace la pile de commandes par la pile à exécuter
    pile = n == 0 ? F : V;

    // On exécute les commandes stockées dans la pile
    cel = pile->tete;
    while (cel != NULL) {
        printf("%c\n", cel->valeur);
        if (cel->type == CHAR) {
            executer_commandes(cel->valeur, pile, ret, profondeur);
        }
        cel = cel->suivant;
    }
}

void ignore_commande(pile_cmd *pile) {
    char c;
    cellule_pile_cmd *cel;
    int profondeur_sous_pile;

    cel = pile->tete;
    c = cel->valeur;

    if (c != '}') {
        cel = depiler(pile);
        free(cel);
    }
    else {
        profondeur_sous_pile = 1;
        while (profondeur_sous_pile > 0) {
            cel = depiler(pile);
            if (cel->valeur == '}') profondeur_sous_pile++;
            if (cel->valeur == '{') profondeur_sous_pile--;
            free(cel);
        }
    }

}
////////////


void executer_commandes(char commande, pile_cmd *pile_commandes, int *ret, int *profondeur) {
    *ret = REUSSI;

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

        executer_commandes(commande, pile_commandes, &ret, &profondeur);
        printf("ret: %d\n", ret);
        if (ret == VICTOIRE)
            return VICTOIRE; /* on a atteint la cible */
        if (ret == RATE)
            return RATE; /* tombé dans l'eau ou sur un rocher */

        /* Affichage pour faciliter le debug */
        afficherCarte();
        printf("COMMANDE : %c\n", commande);
        printf("Programme: ");
        afficher(seq_instructions);
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
