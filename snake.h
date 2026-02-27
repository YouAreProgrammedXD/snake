
/*
  Enumeration for the different actions that the player may choose
 */
enum actions {NORTH, EAST, SOUTH, WEST};
typedef enum actions action;

struct snake_link { // models a piece of Snake
   char c; // SNAKE_HEAD, SNAKE_BODY, or SNAKE_TAIL char 
   int x; // x position for this piece of Snake within the map
   int y; // y position for this piece of Snake within the map
   struct snake_link * next; // pointer to the next piece of Snake
   };
typedef struct snake_link * snake_list; // the linked list that represents Snake

struct item_node { // piece of Snake tree-shaped vision 
  char item; // map item
  struct item_node * pnn; // further item towards north 
  struct item_node * pen; // further item towards east
  struct item_node * psn; // further item towards south
  struct item_node * pwn; // further item towards west
};
typedef struct item_node * item_tree; // Snake tree-shaped vision

action snake(item_tree, item_tree, item_tree, item_tree, snake_list, action);

