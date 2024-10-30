#ifndef INTERPRETE_H
#define INTERPRETE_H

#include "pile.h"
#include "listes.h"

int interprete (sequence_t* seq, bool debug);
void executer_commandes(char commande, pile_cmd *pile_commandes, int *ret, int *profondeur);

#endif
