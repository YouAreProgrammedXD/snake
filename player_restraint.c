// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf
#include <time.h>

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL; // ascii used for the walls
extern const char PATH; // ascii used for the paths
extern const char BONUS; // ascii used for the bonuses

extern bool DEBUG; // indicates whether the game runs in DEBUG mode

typedef struct coordonnees coord;
struct coordonnees{
  int x;
  int y;
};

char * binome="Guiffant Théophane"; // student names here

// prototypes of the local functions/procedures
void printAction(action);
void printBoolean(bool);
void item_tree_printer(item_tree);

bool egalite(coord,coord);
int distance(coord,coord);
action coord2action(coord,coord,action);
action inverse(action);
int taille_snake(snake_list);

coord recherche(item_tree, item_tree, item_tree, item_tree,coord);
coord recherche_arbre(item_tree);
coord recherche_troncon(item_tree, action);

void recherche_mur(snake_list, item_tree, item_tree,int*,int*);
void recherche_mur_snake(snake_list,int*,int*);
int recherche_mur_troncon(item_tree,action);

bool verif(coord,snake_list,int,int);
bool verif_serpent(coord,snake_list);
bool verif_mur(coord,int,int);

snake_list modif(snake_list, coord);

void tri_insertion_decision(coord*,coord);
void case_possible_decision(coord, coord, snake_list, coord*,int,int);
coord decision(coord, snake_list,int,int);

void tri_insertion_errer(coord*);
void case_possible_errer(coord, snake_list, coord*,int,int);
coord errer(snake_list,int,int,int);






/*
  snake function:
  This function randomly select a valid move for Snake based on its current position on the game map.
*/
action snake(
	     item_tree itn, // what Snake is seeing when looking at north, up to a distance of 3
	     item_tree ite, // what Snake is seeing when looking at east, up to a distance of 3
	     item_tree its, // what Snake is seeing when looking at south, up to a distance of 3
	     item_tree itw, // what Snake is seeing when looking at west, up to a distance of 3
	     snake_list s, // snake
	     action last_action // last action made, -1 in the be beginning 
	     ) {
  srand(time(NULL));
  action a; // action to choose and return
  coord move;
  coord serpent;
  coord invalide={-100,-100};
  int xmax;
  int ymax;
  int nb_iteration=taille_snake(s);
  int i=1;
  serpent.x=s->x;
  serpent.y=s->y;
  coord fruit=recherche(itn,ite,its,itw,serpent);
  recherche_mur(s,its,ite,&xmax,&ymax);
  if (DEBUG){
    printf("\nxmax=%d, ymax=%d\n",xmax,ymax);
    printf("\nfruit.x=%d, fruit.y=%d\n",fruit.x,fruit.y);
  }
  if (egalite(invalide,fruit)){
    move=errer(s,xmax,ymax,nb_iteration*2);
    while (egalite(move,invalide) || nb_iteration/i==0){// tant que aucun chemin de taille nb_iteration n'est trouvé, on réduit nb_iteration.
	move=errer(s,xmax,ymax,nb_iteration/i);
	i=i+1;
    }
  }
  else{
    move=decision(fruit,s,xmax,ymax);
  }
  a=coord2action(serpent,move,last_action);
  return a; // answer to the game engine
}

/*
  printAction procedure:
  This procedure prints the input action name on screen.
*/
void printAction(action a) {
  switch(a) {
  case NORTH:
    printf("NORTH");
    break;
  case EAST:
    printf("EAST");
    break;
  case SOUTH:
    printf("SOUTH");
    break;
  case WEST:
    printf("WEST");
    break;
  }
}


/*
  printBoolean procedure:
  This procedure prints the input boolan value on screen.
*/
void printBoolean(bool b) {
  if(b) {
    printf("true");
  }
  else {
    printf("false");
  }
}
  

// Renvoie si les deux points donnés en entré ont les même coordonnées
bool egalite(coord point1,coord point2){
  if (point1.x==point2.x && point1.y==point2.y){
    return(true);
  }
  else{
    return(false);
  }
}

// Renvoie la distance entre deux point c'est à dire le nombre de mouvement nécessaire au snake pour aller d'un point à l'autre
int distance(coord point1, coord point2){
  return(abs(point1.x-point2.x)+abs(point1.y-point2.y));
}


// Prend Les coordonnées du point où le serpent doit aller (move) et les coordonnées de la tête du serpent et renvoie l'action qui en découle. Si le serpent n'a pas de case où aller (move={-100,-100}) ou si il y a une incohérence, le serpent se suicide et cette fonction renvoie l'inverse de la derniere action
action coord2action(coord serpent, coord move, action last_action){
  if (serpent.x+1==move.x){
    return(EAST);
  }
  if (serpent.x-1==move.x){
    return(WEST);
  }
  if (serpent.y-1==move.y){
    return(NORTH);
  }
  if (serpent.y+1==move.y){
    return(SOUTH);
  }
  else{
    return(inverse(last_action));
  }
}

// Renvoie l'inverse de l'action passé en entré
action inverse(action last_action){
  if (last_action==NORTH){
    return(SOUTH);
  }
  if (last_action==EAST){
    return(WEST);
  }
  if (last_action==SOUTH){
    return(NORTH);
  }
  else{
    return(EAST);
  }
}
 
// Renvoie la taille du serpent passé en entré de manière récursive
int taille_snake(snake_list s){
  if (s->next==NULL){
    return(1);
  }
  else{
    return(1+taille_snake(s->next));
  }
}
   

// Renvoie les coordonnées du fruit si le serpent le voit et {-100,-100} sinon
coord recherche(item_tree tree_north,item_tree tree_east,item_tree tree_south,item_tree tree_west, coord serpent){
  coord fruit;
  coord invalide={-100,-100};
  fruit=recherche_arbre(tree_north);// on regarde l'intégralité de l'arbre au nord
  if (!egalite(invalide,fruit)){// si on trouve le fruit, on ajuste les coordonnées et on les renvoie
    fruit.y=fruit.y-1;
    fruit.x=serpent.x+fruit.x;
    fruit.y=serpent.y+fruit.y;
    return(fruit);
  }
  fruit=recherche_troncon(tree_east,EAST);// sinon on regarde les 3 cases à l'est du serpent
  if (!egalite(invalide,fruit)){// si on trouve le fruit, on ajuste les coordonnées et on les renvoie
    fruit.x=fruit.x+1;
    fruit.x=serpent.x+fruit.x;
    fruit.y=serpent.y+fruit.y;
    return(fruit);
  }
  fruit=recherche_arbre(tree_south);// sinon on regarde l'intégralité de l'arbre au sud
  if (!egalite(invalide,fruit)){// si on trouve le fruit, on ajuste les coordonnées et on les renvoie
    fruit.y=fruit.y+1;
    fruit.x=serpent.x+fruit.x;
    fruit.y=serpent.y+fruit.y;
    return(fruit);
  }
  fruit=recherche_troncon(tree_west,WEST);// sinon on regarde les 3 cases à l'est du serpent
  if (!egalite(invalide,fruit)){// si on trouve le fruit, on ajuste les coordonnées et on les renvoie
    fruit.x=fruit.x-1;
    fruit.x=serpent.x+fruit.x;
    fruit.y=serpent.y+fruit.y;
    return(fruit);
  }
  return(invalide);// Si on atteint ce stade de la fonction, cela signifie que le fruit n'est pas dans la vision du serpent
}

// Renvoie les coordonnées du fruit si celui-ci se trouve dans l'arbre passé en entré et {-100,-100} sinon. Cette fonction marche par récursivité:
// si on trouve le fruit on renvoie {0,0} et lors de la remonté on ajuste les coordonnées. Ce qui est renvoyé ce sont les coordonnées avec la tête du serpent comme point d'origine
coord recherche_arbre(item_tree a){
  coord fruit;
  coord invalide={-100,-100};
  if (a!=NULL){
    if (a->item==BONUS){// si on voit le fruit on envoie une réponse positive
      fruit.x=0;
      fruit.y=0;
      return(fruit);
    }
    fruit=recherche_arbre(a->pnn);// sinon on regarde dans l'arbre au nord
    if (!egalite(fruit,invalide)){// si on trouve le fruit on ajuste les coordonnées et on les renvoie
      fruit.y=fruit.y-1;
      return(fruit);
    }
    fruit=recherche_arbre(a->pen);// sinon on regarde dans l'arbre à l'est 
    if (!egalite(fruit,invalide)){// si on trouve le fruit on ajuste les coordonnées et on les renvoie
       fruit.x=fruit.x+1;
      return(fruit);
    }
    fruit=recherche_arbre(a->psn);// sinon on regarde dans l'arbre au sud
    if (!egalite(fruit,invalide)){// si on trouve le fruit on ajuste les coordonnées et on les renvoie
      fruit.y=fruit.y+1;
      return(fruit);
    }
    fruit=recherche_arbre(a->pwn);// sinon on regarde dans l'arbre à l'ouest
    if (!egalite(fruit,invalide)){// si on trouve le fruit on ajuste les coordonnées et on les renvoie
      fruit.x=fruit.x-1;
      return(fruit);
    }
    return(invalide);// Si on atteint ce stade de la fonction, cela signifie que le fruit n'est pas dans la vision du serpent
  }
  else{// si on a atteint la fin de l'arbre on renvoie {-100,-100}
    return(invalide);
  }
}
      

// Renvoie les coordonnées du fruit si celui-ci se trouve dans l'une des trois cases dans la direction 'direction' du serpent. Cette fonction marche par récursivité:
// si on trouve le fruit on renvoie {0,0} et lors de la remonté on ajuste les coordonnées. Ce qui est renvoyé ce sont les coordonnées avec la tête du serpent comme point d'origine
coord recherche_troncon(item_tree a, action direction){
  coord fruit;
  coord invalide={-100,-100};
  if (a==NULL){
    return(invalide);
  }
  if (a->item==BONUS){// si on trouve le fruit on renvoie une réponse positive
    fruit.x=0;
    fruit.y=0;
    return(fruit);
  }// sinon on regarde la prochaine case dans la direction 'direction'
  if (direction==EAST){// ici la prochaine case à l'est
    fruit=recherche_troncon(a->pen,EAST);// celle-ci se trouve dans le fils pen de a
    if (!egalite(invalide,fruit)){// si on trouve le fruit on ajuste les coordonnées et on les renvoie
      fruit.x=fruit.x+1;
      return(fruit);
    }
  }
  if (direction==WEST){// ici la prochaine case à l'ouest
    fruit=recherche_troncon(a->pwn,WEST);// celle-ci se trouve dans le fils pwn de a
    if (!egalite(invalide,fruit)){// si on trouve le fruit on ajuste les coordonnées et on les renvoie
      fruit.x=fruit.x-1;
      return(fruit);
    }
  }
  return(invalide);// si on atteint ce stade de la fonction cela veut dire que l'on a pas trouvé le fruit
}



// Modifie les valeurs de xmax et de ymax passés sous forme de poineur en entré. xmax et ymax correspondent aux coordonnées du mur à droite et en bas
void recherche_mur(snake_list s, item_tree tree_south, item_tree tree_east, int* xmax, int* ymax){
  *xmax=0;
  *ymax=0;
  recherche_mur_snake(s,xmax,ymax);
  if (*ymax<(recherche_mur_troncon(tree_south,SOUTH)+s->y)){
    *ymax=recherche_mur_troncon(tree_south, SOUTH)+s->y;
  }
  if (*xmax<(recherche_mur_troncon(tree_east,EAST)+s->x)){
    *xmax=recherche_mur_troncon(tree_east, EAST)+s->x;
  }
}

// Modifie les valeurs de xmax et de ymax avec les coordonnées des cases du serpent
void recherche_mur_snake(snake_list s, int* xmax, int* ymax){
  if (s!=NULL){
    if (s->x>=*xmax){
      *xmax=s->x+1;
    }
    if (s->y>=*ymax){
      *ymax=s->y+1;
    }
    recherche_mur_snake(s->next,xmax,ymax);
  }
}

// Cette fonction marche de la même maniere que recherche_troncon a la difference que là on cherche un mur
int recherche_mur_troncon(item_tree a, action direction){
  int retour;
  if (a==NULL){
    return(1);
  }
  if (a->item==WALL){
    return(1);
  }
  if (direction==SOUTH){
    retour=recherche_mur_troncon(a->psn,direction);
    retour=retour+1;
  }
  else{
    retour=recherche_mur_troncon(a->pen,direction);
    retour=retour+1;
  }
  return(retour);
}
    




// Renvoie true si la case n'est occupé ni par le serpent ni par un mur
bool verif(coord point, snake_list s, int xmax, int ymax){
  return(verif_serpent(point,s) && verif_mur(point,xmax,ymax));
}
// Renvoie true si la case n'est pas occupé par le serpent
bool verif_serpent(coord point, snake_list s){
  if (s==NULL){
    return(true);
  }
  else{
    if (point.x==s->x && point.y==s->y){
      return(false);
    }
    else{
      return(verif_serpent(point,s->next));
    }
  }
}

// Renvoie true si la case n'est pas occupé par un mur
bool verif_mur(coord point, int xmax, int ymax){
  if (point.x<=0 || point.y<=0){
    return(false);
  }
  if (xmax!=-100){
    if (point.x>=xmax){
      return(false);
    }
  }
  if (ymax!=-100){
    if (point.y>=ymax){
      return(false);
    }
  }
  return(true);
}




// Renvoie un serpent comme si s passé en entrée s'était déplacé vers point passé en entrée. Cette fonction marche par récursivité.
snake_list modif(snake_list s,coord point){
  snake_list retour=malloc(sizeof(snake_list));// On alloue de la place pour pouvoir ensuite renvoyé le nouveau serpent sans le supprimer
  coord poubelle={0,0};
  if (s==NULL){// Si le serpent est vide on renvoie le serpent vide
    return(NULL);
  }
  else{// sinon point prend la place de la première case du serpent dans retour et retour->next prend la valeur de modif(s->next,{->x,s->y}) 
    retour->c=s->c;
    (retour->x)=point.x;
    (retour->y)=point.y;
    poubelle.x=s->x;
    poubelle.y=s->y;
    (retour->next)=modif((s->next),poubelle);
  }
  return(retour);
}

// Modifie la liste des cases possibles pour que les cases invalides soit en fin de liste et que la liste soit trié de tel sorte que la distane avec le fruit soit croissante
void tri_insertion_decision(coord t[],coord fruit){
  int i;
  int j;
  int k=3;// fin de la liste valide
  coord invalide={-100,-100};
  coord memoire;
  for (i=0;i<4;i++){// on met les invalide en fin de liste
    if (egalite(invalide,t[i-(3-k)])){
      t[i-(3-k)]=t[k];
      t[k]=invalide;
      k=k-1;
    }
  }
  for (i=1;i<4;i++){// on trie la liste valide par insertion
    j=i;
    while(j>0 && distance(fruit,t[j-1])>distance(fruit,t[j])){
      memoire=t[j];
      t[j]=t[j-1];
      t[j-1]=memoire;
      j--;
    }
  }
}

// Renvoie les cases possibles autour de la tête du serpent trié de tel sorte que la distance au fruit est croissante et que les cases invalides soit en fin de liste
void case_possible_decision(coord point,coord fruit,snake_list s, coord* poss, int xmax, int ymax){
  coord invalide={-100,-100};
  coord poubelle1={point.x,point.y+1};
  coord poubelle2={point.x+1,point.y};
  coord poubelle3={point.x,point.y-1};
  coord poubelle4={point.x-1,point.y};
  poss[0]=poubelle1;
  poss[1]=poubelle2;
  poss[2]=poubelle3;
  poss[3]=poubelle4;
  int i;
  for (i=0;i<4;i++){// on regarde grace à la fonction verif si les cases sont valides
    if (!verif(poss[i],s,xmax,ymax)){
      poss[i]=invalide;
    }
  }
  tri_insertion_decision(poss,fruit);// on trie la liste
}
  
  
      

// Si on connait la position du fruit, cette fonction renvoie les coordonnées du point où le serpent doit aller. Cette fonction marche par récursivité:
// si la tête du serpent se trouve sur le fruit on renvoie une réponse positive
// sinon on détermine les cases posibles autour de la tête du serpent
// on construit un nouveau serpent grâce à la première case possible
// on donne ce nouveau serpent à la fonction
// si on a une réponse positive on renvoie les coordonnées de la case de la liste des cases possibles que l'on a donné
// sinon on emprunte un autre chemin grâce à la case suivante de la liste cases posibles
coord decision(coord fruit,snake_list s,int xmax, int ymax){
  coord point={s->x,s->y};// coordonnées de la tête du serpent
  coord invalide={-100,-100};
  coord poubelle={0,0};
  coord poss[4];// liste des cases possibles
  coord valeur_retourne;
  int i=0;
  struct snake_link inter={SNAKE_HEAD,0,0,NULL};
  snake_list nouvs=&inter;
  if (egalite(point,fruit)){// si la tête du serpent a atteint le fruit on renvoie une réponse positive
    return(poubelle);
  }
  case_possible_decision(point,fruit,s,poss,xmax,ymax);// on calcul la liste des cases possibles
  if (egalite(poss[0],invalide)){// si aucunes case n'est valide ie si il n'y a pas de chemin on envoie une répone négative
    return(invalide);
  }
  nouvs=modif(s,poss[0]);// on calcul le nouveau serpent grâce à l'ancien et à poss[0]
  valeur_retourne=decision(fruit,nouvs,xmax,ymax);// on regarde si il y a un chemin jusqu'au fruit en passant par poss[0]
  while (egalite(valeur_retourne,invalide)){// tant qu'il n'y a pas de chemin valide, on essaye avec une autre case de la liste des cases possibles
    if (i==3 || (egalite(poss[i+1],invalide))){
      free(nouvs);// on libère l'espace du nouveau serpent car on en a pas besoin
      return(invalide);
    }
    else{
      i=i+1;
      nouvs=modif(s,poss[i]);
      valeur_retourne=decision(fruit,nouvs,xmax,ymax);
    }
  }
  free(nouvs);// on libère l'espace du nouveau serpent car on en a pas besoin
  if (!egalite(valeur_retourne,invalide)){
    return(poss[i]);
  }
  else{
    return(invalide);
  }
}

// tri la liste des cases possibles aléatoirement en ayant tout de même les cases invalide en fin de liste
void tri_insertion_errer(coord t[]){
  int i;
  int k=3;// fin de la liste des cases possibles
  int debut=0;// debut de la liste non trié
  int hasard;
  coord invalide={-100,-100};
  coord poubelle;
  for (i=0;i<4;i++){// on place les cases invalides en fin de liste
    if (egalite(t[i-(3-k)],invalide)){
      t[i-(3-k)]=t[k];
      t[k]=invalide;
      k=k-1;
    }
  }
  while (debut!=k && !egalite(t[0],invalide)){// tant que la liste n'est pas trié, on prend un élément de la liste valide non trié aléatoirement et on le place à la fin de la liste valide trié
    hasard=debut+rand()%(k-debut+1);
    poubelle=t[debut];
    t[debut]=t[hasard];
    t[hasard]=poubelle;
    debut=debut+1;
  }
}

// Modifie la liste des cases possibles autour de la tête du serpent de tel sorte que les cases invalides soit en fin de liste et que le reste des cases soit trié aléatoirement
void case_possible_errer(coord point,snake_list s, coord* poss, int xmax, int ymax){
  coord invalide={-100,-100};
  coord poubelle1={point.x,point.y+1};
  coord poubelle2={point.x+1,point.y};
  coord poubelle3={point.x,point.y-1};
  coord poubelle4={point.x-1,point.y};
  poss[0]=poubelle1;
  poss[1]=poubelle2;
  poss[2]=poubelle3;
  poss[3]=poubelle4;
  int i;
  for (i=0;i<4;i++){// on vérifie que les cases sont valide
    if (!verif(poss[i],s,xmax,ymax)){
      poss[i]=invalide;
    }
  }
  tri_insertion_errer(poss);// on trie la liste
}





// Si on ne connait pas la position du fruit, cette fonction renvoie les coordonnées du point où le serpent doit aller. Cette fonction marche par récursivité:
// si on a appelé cette fonction nb_iteration fois, on renvoie une réponse positive
// sinon on détermine les cases posibles autour de la tête du serpent
// on construit un nouveau serpent grâce à la première case possible
// on donne ce nouveau serpent à la fonction
// si on a une réponse positive on renvoie les coordonnées de la case de la liste des cases possibles que l'on a donné
// sinon on emprunte un autre chemin grâce à la case suivante de la liste cases posibles
coord errer(snake_list s,int xmax, int ymax, int nb_iteration){
  coord point={s->x,s->y};
  coord invalide={-100,-100};
  coord poubelle={0,0};
  coord poss[4];// liste de cases possibles 
  coord valeur_retourne={0,0};
  int i=0;
  struct snake_link inter={SNAKE_HEAD,0,0,NULL};
  snake_list nouvs=&inter;
  if (nb_iteration==0){// si on a fait assez d'appel de cette fonction on renvoie une réponse positive
    return(poubelle);
  }
  case_possible_errer(point,s,poss,xmax,ymax);// on modifie la liste des cases possibles comme on le veut 
  if (egalite(poss[0],invalide)){// si aucune case n'est valide on renvoie une réponse négative
    return(invalide);
  }
  nouvs=modif(s,poss[0]);// on calcule le nouveau serpent grâce à l'ancien et à poss[0]
  valeur_retourne=errer(nouvs,xmax,ymax,nb_iteration-1);// on regarde si il y a un chemin assez long en passant par poss[0]
  while (egalite(valeur_retourne,invalide)){// tant qu'il n'y a pas de chemin valide on essaye la case suivant de la liste des cases posibles
    if (i==3 || (egalite(poss[i+1],invalide))){
      free(nouvs);
      return(invalide);
    }
    else{
      i=i+1;
      nouvs=modif(s,poss[i]);
      valeur_retourne=errer(nouvs,xmax,ymax,nb_iteration-1);
    }
  }
  free(nouvs);// on libère l'espace pris par le nouveau serpent car on en a plus besoin
  if (egalite(valeur_retourne,invalide)){
    return(invalide);
  }
  else{
    return(poss[i]);
  }
}


