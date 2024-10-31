#ifndef INTERPRETE_H
#define INTERPRETE_H

#include "pile.h"
#include "listes.h"

int interprete (sequence_t* seq, bool debug);
void execution_conditionnelle(pile_cmd *pile, int *ret, int *profondeur);
void executer_commande(char commande, pile_cmd *pile_commandes, int *ret, int *profondeur);

#endif
