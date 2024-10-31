#ifndef PILE_H
#define PILE_H

typedef enum { INT, CHAR } type_cmd;

typedef struct cellule_pile_cmd_s {
    type_cmd type;
    char valeur;
    struct cellule_pile_cmd_s *suivant;
} cellule_pile_cmd;

typedef struct pile_cmd_s {
    cellule_pile_cmd *tete;
} pile_cmd;

pile_cmd *init_pile(void);

/* Ajoute un élément en haut de pile */
void empiler(pile_cmd*, char, type_cmd);
void empiler_char(pile_cmd*, char);
void empiler_int(pile_cmd*, int);

/* Empile groupe sur pile*/
void empiler_groupe(pile_cmd *pile, pile_cmd *groupe);

/* Supprime et retourne l'élément en haut de la pile */
cellule_pile_cmd *depiler(pile_cmd*);
int depiler_int(pile_cmd*);
char depiler_char(pile_cmd*);

int taille_pile(pile_cmd*);

/* 
Renvoie le premier groupe de commandes trouvé dans la pile 
Exemple :
{ ADAA } { 8M { AGAD } { AA } ? } -> ADAA
*/
pile_cmd *depiler_groupe_commandes(pile_cmd *pile);

void executer_groupe_commandes(pile_cmd *groupe, int *ret, int *profondeur);

/* Inverse une pile (a1, a2, ..., an => an, ..., a2, a1)*/
void inverser_pile(pile_cmd*);

void afficher_pile(pile_cmd*);

#endif

