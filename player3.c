// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf

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

char * binome="Random"; // student names here

// prototypes of the local functions/procedures
void printAction(action);
void printBoolean(bool);





// cette structure va nous permettre d'exprimer les coordonnées d'un point sur la grille
typedef struct coord coord;
struct coord{
  int x;
  int y;
};

// cette structure va nous permettre d'exprimer les 4 coordonnées des cases autour de le tête du serpent et de déterminer le chemin a prendre
typedef struct possibilites possibilites;
struct possibilites{
  coord a;
  coord b;
  coord c;
  coord d;
};


// cette fonction renvoie les coordonnées du fruit grâce a la grille et ses dimensions
coord fruit(char ** map,int mapxsize,int mapysize){
  coord coordfruit;
  int c=1;
  int l=1;
  int x=-1;
  while (x==-1){
    l=1;
    while (l<mapysize-1 && x==-1){
      if (map[l][c]==BONUS){
	x=c;
      }
      else{
	l=l+1;
      }
    }
    if (x==-1){
      c=c+1;
    }
  }
  coordfruit.x=x;
  coordfruit.y=l;
  return(coordfruit);
}


// cette fonction donne le nombre d'action nécessaire pour aller d'une case à une autre
int dist(coord point,coord objectif){
  return(abs(point.x-objectif.x)+abs(point.y-objectif.y));
}

// cette fonction teste si deux points sont égaux
bool egalite(coord coord1,coord coord2){
  if (coord1.x==coord2.x && coord1.y==coord2.y){
    return(true);
  }
  else{
    return(false);
  }
}

//  renvoie true si la case indiquée par point n'est pas libre (il y a le serpent autre que la queue)
bool verif_snake(coord point,snake_list s){
  if (s->next==NULL){
    return(false);
  }
  else if (point.x==s->x && point.y==s->y){
    return(true);
  }
  else{
    return(verif_snake(point,s->next));
  }
}

// renvoie true si la case indiquée par point n'est pas libre (il y a le serpent ou un mur) ps: on considère que la queue est une case libre
bool verif(char**map, coord point,snake_list s){
  if (map[point.y][point.x]==WALL || verif_snake(point,s)){
    return(true);
  }
  else{
    return(false);
  }
}

// cette fonction renvoie les cases possibles autour d'un point et les trie:
// on teste tour a tour les 4 cases autour du point
// si une case n'est pas libre on la remplace par le point invalide={-1,-1} dans la structure renvoyée, ce qui est compris par le reste du programme
// après avoir testée une nouvelle case on regarde sa distance au fruit (si ladite case est valide)
// on insère cette nouvelle case dans la structure possibilites que l'on veut renvoyer de telle sorte que les cases les plus proche du fruit sont les premières dans la structure
// si deux cases sont a distance équivalente au fruit on les départage de manière aléatoire (un ordre de préférence a été mis mais c'est sans importance)
// les cases non libre sont remplacée par invalide dans la structure renvoyée
possibilites case_possible(char** map,coord point,coord coordfruit,snake_list s){
  possibilites poss;
  coord invalide={-1,-1};
  coord case1={(point.x)+1,point.y};
  coord case2={(point.x)-1,point.y};
  coord case3={point.x,(point.y)+1};
  coord case4={point.x,(point.y)-1};
  if (verif(map,case1,s)){ // on teste si la première case est libre
    poss.a=invalide;
  }
  else{
    poss.a=case1;
  }
  if (verif(map,case2,s)){// on teste si la deuxième case est libre
    poss.b=invalide;
  }
  else if (dist(case2,coordfruit)<dist(poss.a,coordfruit) || egalite(poss.a,invalide)){// et on l'insère a la place voulue
    poss.b=poss.a;
    poss.a=case2;
  }
  else{
    poss.b=case2;
  }
  if (verif(map,case3,s)){// on teste si la troisième case est libre
    poss.c=invalide;
  }
  else if (dist(case3,coordfruit)<dist(poss.a,coordfruit) || egalite(poss.a,invalide)){// et on l'insère à la case voulue
    poss.c=poss.b;
    poss.b=poss.a;
    poss.a=case3;
  }
  else{
    if (dist(case3,coordfruit)<dist(poss.b,coordfruit) || egalite(poss.b,invalide)){
      poss.c=poss.b;
      poss.b=case3;
    }
    else{
      poss.c=case3;
    }
  }
  if (verif(map,case4,s)){// on teste si la quatrième case est libre
    poss.d=invalide;
  }
  else if (dist(case4,coordfruit)<dist(poss.a,coordfruit) || egalite(poss.a,invalide)){// et on l'insère à la case voulue
    poss.d=poss.c;
    poss.c=poss.b;
    poss.b=poss.a;
    poss.a=case4;
  }
  else{
    if (dist(case4,coordfruit)<dist(poss.b,coordfruit) || egalite(poss.b,invalide)){
      poss.d=poss.c;
      poss.c=poss.b;
      poss.b=case4;
    }
    else{
      if (dist(case4,coordfruit)<dist(poss.c,coordfruit) || egalite(poss.c,invalide)){
	poss.d=poss.c;
	poss.c=case4;
      }
      else{
	poss.d=case4;
      }
    }
  }
  return(poss);
}



// renvoie un nouveau serpent par récursivité: le serpent après s'être déplacée vers le point
// chaque partie du serpent occupe la place de la suivante en cascade
// modif renvoie un serpent dont la tête est occupée par "point" et le reste du corps est le "next" du serpent initial se déplaçant vers la tête initial
// et ce sans modifié le serpent s
snake_list modif(snake_list s,coord point){
  snake_list retour=malloc(sizeof(snake_list)); // on a besoin de réserver la place sinon la fonction renvoie un serpent de taille 2 avec des coordonnée incohérente (hors des dimensions de la grille)
  coord poubelle;
  if (s->next!=NULL){// si le serpent ne se résume pas à une case, on renvoie un serpent dont la tête est "point" et on décale les autres cases du serpent en cascade
    retour->c=s->c;
    (retour->x)=point.x;
    (retour->y)=point.y;
    poubelle.x=s->x;
    poubelle.y=s->y;
    (retour->next)=modif((s->next),poubelle);
  }
  else{// sinon on renvoie on renvoie un serpent-case dont la tête (et l'unique case) est occupé par "point"
    retour->c=s->c;
    (retour->x)=point.x;
    (retour->y)=point.y;
    (retour->next)=NULL;
  }
  return(retour);
}

		 

// decision renvoie la case où le serpent doit aller selon le principe de l'algorithme de recherche A* (par récursivité)
// decision teste les chemins possibles entre la tête du serpent et objectif2 en imposant au serpent de passé par objectif1
// cases_possibles renvoie les cases possibles autour de la tête du serpent en privilégiant les cases qui rapproche le serpent de l'objectif1
// modif créer un nouveau serpent grâce à la première case possible et on rappelle decision
// et ainsi de suite
// comme ça au final soit le serpent est bloqué soit il trouve un chemin
// si il trouve un chemin, decision renvoie la case ou le serpent doit aller à action
// sinon il remonte et dès que il y a un moment où un autre chemin est possible il le teste
// ce qui rend cette fonction pas (trop) gourmande c'est le tri effectué par cases_possibles, ainsi on teste en priorité les chemins qui nous rapproche de l'objectif1

// lorsque l'on a atteint l'objectif1 on demande à decision d'aller à invalide en passant par l'objectif2 ce que la fonction comprend par aller à objectif2
// l'intérêt de rajouter la condition d'aller à l'objectif1 en passant par l'objectif2 est de s'assurer que le serpent ne se coince pas en allant manger un fruit
coord decision(char** map,coord objectif1,coord objectif2,snake_list s){
  coord point={s->x,s->y};
  coord invalide={-1,-1};
  coord poubelle={0,0};
  possibilites poss;
  coord valeur_retourne;
  int i=0;
  snake_list nouvs;
  if (egalite(point,objectif1)){// si on a atteint l'objectif1
    if (egalite(objectif2,invalide)){// si c'est le dernier objectif
      return(poubelle);
    }
    else{// sinon il nous reste à aller à l'objectif2
      return(decision(map,objectif2,invalide,s));
    }
  }
  poss=case_possible(map,point,objectif1,s);
  if (egalite(poss.a,invalide)){// si aucune case n'est possible (=le serpent est dans une impasse) on renvoie qu'il n'y a pas de chemin
    return(invalide);
  }
  nouvs=modif(s,poss.a);
  valeur_retourne=decision(map,objectif1,objectif2,nouvs);// on regarde si il y a un chemin en passant par la première case possible
  while (egalite(valeur_retourne,invalide)){// tant que ce n'est pas le cas on teste les autres cases possibles
    if (i==3 || (egalite(poss.b,invalide) && i==0) ||  (egalite(poss.c,invalide) && i==1) || (egalite(poss.d,invalide) && i==2)){// si il n'y a plus de case possible on libère l'espace occupé par le nouveau serpent (on en a plus besoin) et on renvoie qu'il n'y a pas de chemin
      free(nouvs);
      return(invalide);
    }
    else{// si il y a une autre case possible on la teste
      i=i+1;
      if (i==1){
  	nouvs=modif(s,poss.b);
  	valeur_retourne=decision(map,objectif1,objectif2,nouvs);
      }
      else if (i==2){
	nouvs=modif(s,poss.c);
	valeur_retourne=decision(map,objectif1,objectif2,nouvs);
      }
      else if (i==3){
	nouvs=modif(s,poss.d);
	valeur_retourne=decision(map,objectif1,objectif2,nouvs);
      }
    }
  }
  free(nouvs);// on a le réultat, on a donc plus besoin du nouveau serpent
  if (i==0){
    return(poss.a);
  }
  else if (i==1){
    return(poss.b);
  }
  else if (i==2){
    return(poss.c);
  }
  else{
    return(poss.d);
  }
}

// renvoie l'actoin inverse de l'action en entrée
action inverse(action move){
  if (move==WEST){
    return(EAST);
  }
  else if (move==EAST){
    return(WEST);
  }
  else if (move==NORTH){
    return(SOUTH);
  }
  else{
    return(NORTH);
  }
}

// transforme les coordonnées renvoyées par decision en action grâce au serpent
// si decision n'a pas trouvé de chemin le serpent se suicide grâce à la fonction inverse (pas besoin de calcul inutile)
action coord2action(snake_list s,coord point,action last_action){
  int x=s->x;
  int y=s->y;
  if (point.x==-1 && point.y==-1){
    return(inverse(last_action));
  }
  if (point.x==x+1){
    return(EAST);
  }
  else if (point.x==x-1){
    return(WEST);
  }
  else if (point.y==y+1){
    return(SOUTH);
  }
  else {
    return(NORTH);
  }
}

action snake(
	     char * * map, // array of char modeling the game map
	     int mapxsize, // x size of the map
	     int mapysize, // y size of the map
	     snake_list s, // snake
	     action last_action // last action made, -1 in the beginning
	     ) {
  action a; // action to choose and return
  coord coordfruit;
  coord nouvpoint;
  coord objectif2;
  coord invalide={-1,-1};
  coordfruit=fruit(map,mapxsize,mapysize);
  objectif2.x=mapxsize-1-coordfruit.x;
  objectif2.y=mapysize-1-coordfruit.y;
  // l'objectif se trouve de l'autre côté de la grille pour s'assurer que le serpent ne se coince pas en mangeant le fruit
  nouvpoint=decision(map,coordfruit,objectif2,s);
  if (egalite(nouvpoint,invalide)){// si il n'y a pas de chemin possible jusqu'à objectif2 en passant par objectif1, on veut juste aller au fruit, on demande donc le chemin jusqu'à objectif1
    nouvpoint=decision(map,coordfruit,invalide,s);
  }
  a=coord2action(s,nouvpoint,last_action);// on tranforme la coordonnée donnée par decision en action
  if(DEBUG) {
    printf("Candidate action is: ");
    printAction(a);
    printf("\n");
  }
  return a; 
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

