#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define MAX_STR_LEN 1024
#ifndef __testing
#define MAT_SIZE 10
#endif

typedef struct user_struct {
    char name[MAX_STR_LEN];
    struct friend_node_struct* friends;
    struct brand_node_struct* brands;
    bool visited;
    int level;
} User;

typedef struct friend_node_struct {
    User* user;
    struct friend_node_struct* next;
} FriendNode;

typedef struct brand_node_struct {
    char brand_name[MAX_STR_LEN];
    struct brand_node_struct* next;
} BrandNode;

FriendNode* allUsers; 

int brand_adjacency_matrix[MAT_SIZE][MAT_SIZE];
char brand_names[MAT_SIZE][MAX_STR_LEN];

/**
 * Checks if a user is inside a FriendNode LL.
 **/
bool in_friend_list(FriendNode *head, User *node) {
  for (FriendNode *cur = head; cur != NULL; cur = cur->next) {
    if (strcmp(cur->user->name, node->name) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * Checks if a brand is inside a BrandNode LL.
 **/
bool in_brand_list(BrandNode *head, char *name) {
  for (BrandNode *cur = head; cur != NULL; cur = cur->next) {
    if (strcmp(cur->brand_name, name) == 0) {
      return true;
    }
  }
  return false;
}

/**
 * Inserts a User into a FriendNode LL in sorted position. If the user 
 * already exists, nothing is done. Returns the new head of the LL.
 **/
FriendNode *insert_into_friend_list(FriendNode *head, User *node) {
  if (node == NULL) return head;

  if (in_friend_list(head, node)) {
    printf("User already in list\n");
    return head;
  }
  FriendNode *fn = calloc(1, sizeof(FriendNode));
  fn->user = node;
  fn->next = NULL;

  if (head == NULL)
    return fn;
    
  if (strcmp(head->user->name, node->name) > 0) {
    fn->next = head;
    return fn;
  } 

  FriendNode *cur;
  for (cur = head; cur->next != NULL && strcmp(cur->next->user->name, node->name) < 0;
       cur = cur->next)
    ;
  fn->next = cur->next;
  cur->next = fn;
  return head;
}

/**
 * Inserts a brand into a BrandNode LL in sorted position. If the brand 
 * already exists, nothing is done. Returns the new head of the LL.
 **/
BrandNode *insert_into_brand_list(BrandNode *head, char *node) {
  if (node == NULL) return head;

  if (in_brand_list(head, node)) {
    printf("Brand already in list\n");
    return head;
  }
  BrandNode *fn = calloc(1, sizeof(BrandNode));
  strcpy(fn->brand_name, node);
  fn->next = NULL;

  if (head == NULL)
    return fn;
    
  if (strcmp(head->brand_name, node) > 0) {
    fn->next = head;
    return fn;
  } 

  BrandNode *cur;
  for (cur = head; cur->next != NULL && strcmp(cur->next->brand_name, node) < 0;
       cur = cur->next)
    ;
  fn->next = cur->next;
  cur->next = fn;
  return head;
}

/**
 * Deletes a User from FriendNode LL. If the user doesn't exist, nothing is 
 * done. Returns the new head of the LL.
 **/
FriendNode *delete_from_friend_list(FriendNode *head, User *node) {
  if (node == NULL) return head;

  if (!in_friend_list(head, node)) {
    printf("User not in list\n");
    return head;
  }

  if (strcmp(head->user->name, node->name) == 0) {
    FriendNode *temp = head->next;
    free(head);
    return temp;
  }

  FriendNode *cur;
  for (cur = head; cur->next->user != node; cur = cur->next)
    ;

  FriendNode *temp = cur->next;
  cur->next = temp->next;
  free(temp);
  return head;
}

/**
 * Deletes a brand from BrandNode LL. If the user doesn't exist, nothing is 
 * done. Returns the new head of the LL.
 **/
BrandNode *delete_from_brand_list(BrandNode *head, char *node) {
  if (node == NULL) return head;

  if (!in_brand_list(head, node)) {
    printf("Brand not in list\n");
    return head;
  }

  if (strcmp(head->brand_name, node) == 0) {
    BrandNode *temp = head->next;
    free(head);
    return temp;
  }

  BrandNode *cur;
  for (cur = head; strcmp(cur->next->brand_name, node) != 0; cur = cur->next)
    ;

  BrandNode *temp = cur->next;
  cur->next = temp->next;
  free(temp);
  return head;
}

/**
 * Prints out the user data.
 **/
void print_user_data(User *user) {
  printf("User name: %s\n", user->name);
  printf("Friends:\n");
  for (FriendNode *f = user->friends; f != NULL; f = f->next) {
    printf("   %s\n", f->user->name);
  }
  printf("Brands:\n");
  for (BrandNode *b = user->brands; b != NULL; b = b->next) {
    printf("   %s\n", b->brand_name);
  }
}

/**
 * Get the index into brand_names for the given brand name. If it doesn't
 * exist in the array, return -1
 **/
int get_brand_index(char *name) {
  for (int i = 0; i < MAT_SIZE; i++) {
    if (strcmp(brand_names[i], name) == 0) {
      return i;
    }
  }
  printf("brand '%s' not found\n", name);
  return -1; // Not found
}
/**
 * Print out brand name, index and similar brands.
 **/
void print_brand_data(char *brand_name) {
  int idx = get_brand_index(brand_name);
  if (idx < 0) {
    printf("Brand '%s' not in the list.\n", brand_name);
    return;
  }
  printf("Brand name: %s\n", brand_name);
  printf("Brand idx: %d\n", idx);
  printf("Similar brands:\n");
  for (int i = 0; i < MAT_SIZE; i++) {
    if (brand_adjacency_matrix[idx][i] == 1 && strcmp(brand_names[i], "") != 0) {
      printf("   %s\n", brand_names[i]);
    }
  }
}

/**
 * Read from a given file and populate a the brand list and brand matrix.
 **/
void populate_brand_matrix(char* file_name) {
    // Read the file
    char buff[MAX_STR_LEN];
    FILE* f = fopen(file_name, "r");
    fscanf(f, "%s", buff);
    char* line = buff;
    // Load up the brand_names matrix
    for (int i = 0; i < MAT_SIZE; i++) {
        if (i == MAT_SIZE - 1) {
            strcpy(brand_names[i], line);
            break;
        }
        int index = strchr(line, ',') - line;
        strncpy(brand_names[i], line, index);
        line = strchr(line, ',') + sizeof(char);
    }
    // Load up the brand_adjacency_matrix
    for (int x = 0; x < MAT_SIZE; x++) {
        fscanf(f, "%s", buff);
        for (int y = 0; y < MAT_SIZE; y++) {
            int value = (int) buff[y*2];
            if (value == 48) { value = 0; }
            else {value = 1;}
            brand_adjacency_matrix[x][y] = value;
        }
    }
}

// Users
/**
 * Creates and returns a user. Returns NULL on failure.
 **/
User* create_user(char* name) {
    for (FriendNode *f = allUsers; f; f=f->next) {
        if (strcmp(name, f->user->name)==0) return NULL;
    }
    User *item = (User *)calloc(1, sizeof(User));
    strcpy(item->name, name);
    item->friends = NULL;
    item->brands = NULL;
    item->visited = false;
    allUsers = insert_into_friend_list(allUsers, item);
    return item;
}

/**
 * Deletes a given user. 
 * Returns 0 on success, -1 on failure.
 **/
int delete_user(User* user) {
    if (!user || !in_friend_list(allUsers, user)) return -1;
    FriendNode *curr = user->friends;
    FriendNode *next = NULL;
    while (curr) {
        curr->user->friends = delete_from_friend_list(curr->user->friends, user);
        next = curr->next;
        free(curr);
        curr = next;
    }
    BrandNode *curr_brand = user->brands;
    BrandNode *next_brand = NULL;
    while (curr_brand) {
        next_brand = curr_brand->next;
        free(curr_brand);
        curr_brand = next_brand;
    }
    allUsers = delete_from_friend_list(allUsers, user);
    free(user);
    return 0;
}

/**
 * Create a friendship between user and friend.
 * Returns 0 on success, -1 on failure.
 **/
int add_friend(User* user, User* friend) {
    if (!user || !friend || user==friend) return -1;
    if (in_friend_list(user->friends, friend)) return -1;
    user->friends = insert_into_friend_list(user->friends, friend);
    friend->friends = insert_into_friend_list(friend->friends, user);
    return 0;
}

/**
 * Removes a friendship between user and friend.
 * Returns 0 on success, -1 on faliure.
 **/
int remove_friend(User* user, User* friend) {
    if (!user || !friend || user==friend) return -1;
    if (!in_friend_list(user->friends, friend)) return -1;
    user->friends = delete_from_friend_list(user->friends, friend);
    friend->friends = delete_from_friend_list(friend->friends, user);
    return 0;
}

/**
 * Creates a follow relationship, the user follows the brand.
 * Returns 0 on success, -1 on faliure.
 **/
int follow_brand(User* user, char* brand_name) {
    if (!user) return -1;
    if (in_brand_list(user->brands, brand_name)) return -1;
    for (int i=0; i<MAT_SIZE; i++) {
        if (strcmp(brand_name, brand_names[i])==0) {
            user->brands = insert_into_brand_list(user->brands, brand_name);
            return 0;
        }
    }
    return -1;
}

/**
 * Removes a follow relationship, the user unfollows the brand.
 * Returns 0 on success, -1 on faliure.
 **/
int unfollow_brand(User* user, char* brand_name) {
    if (!user) return -1;
    if (!in_brand_list(user->brands, brand_name)) return -1;
    user->brands = delete_from_brand_list(user->brands, brand_name);
    return 0;
}

/**
 * Return the number of mutual friends between two users.
 **/
int get_mutual_friends(User* a, User* b) {
    if (!a || !b) return 0;
    int n=0;
    for (FriendNode *friend = a->friends; friend; friend=friend->next) {
        if (in_friend_list(b->friends, friend->user)) n++;
    }
    return n;
}

/*
 * A degree of connection is the number of steps it takes to get from
 * one user to another
 * 
 * For example, if X & Y are friends, then we expect to recieve 1 when calling
 * this on (X,Y). Continuing on, if Y & Z are friends, then we expect to
 * recieve 2 when calling this on (X,Z).
 * 
 * Returns a non-negative integer representing the degrees of connection
 * between two users, -1 on failure.
 **/
void deleteList(FriendNode *head) {
    FriendNode *q = NULL;
    while (head) {
        q = head->next;
        free(head);
        head = q;
    }
}
FriendNode* insertQueue(FriendNode *queue, User *current, int level) {
    FriendNode *node = (FriendNode *)calloc(1, sizeof(FriendNode));
    node->user = current;
    node->user->visited = true;
    node->user->level = level;
    node->next = NULL;
    if (!queue) return node;
    FriendNode *f = NULL;
    for (f = queue; f->next; f=f->next);
    f->next = node;
    return queue;
}
int findUser(FriendNode *queue, User *b) {
    if (!queue) return -1;
    if (in_friend_list(queue->user->friends, b)) return queue->user->level;
    for (FriendNode *f = queue->user->friends; f; f=f->next) {
        if (!f->user->visited) queue = insertQueue(queue, f->user, queue->user->level + 1);
    }
    return findUser(queue->next, b);
}
int get_degrees_of_connection(User* a, User* b) {
    if (!a || !b) return -1;
    if (a==b) return 0;
    for (FriendNode *f = allUsers; f; f=f->next) f->user->visited = false;
    FriendNode *queue = insertQueue(NULL, a, 1);
    int level = findUser(queue, b);
    deleteList(queue);
    return level;
}


/**
 * Marks two brands as similar.
 **/
void connect_similar_brands(char* brandNameA, char* brandNameB) {
    int A = get_brand_index(brandNameA);
    int B = get_brand_index(brandNameB);
    if (A != -1 && B != -1) {        brand_adjacency_matrix[A][B] = 1;
        brand_adjacency_matrix[B][A] = 1;
    }
}

/**
 * Marks two brands as not similar.
 **/
void remove_similar_brands(char* brandNameA, char* brandNameB) {
    int A = get_brand_index(brandNameA);
    int B = get_brand_index(brandNameB);
    if (A != -1 && B != -1) {        brand_adjacency_matrix[A][B] = 0;
        brand_adjacency_matrix[B][A] = 0;
    }
}

/**
 * Returns a suggested friend for the given user, returns NULL on failure.
 * See the handout for how we define a suggested friend.
 **/
User* get_suggested_friend(User* user) {
    if (!user) return NULL;
    int max=0;
    User *suggested = NULL;
    for (FriendNode *friend = allUsers; friend; friend=friend->next) {
        if (friend->user != user && !in_friend_list(user->friends, friend->user)) {
            int n=0;
            for (BrandNode *brand = friend->user->brands; brand; brand=brand->next) {
                if (in_brand_list(user->brands, brand->brand_name)) n++;
            }
            if (n>max) {
                max=n;
                suggested = friend->user;
            }
            else if (n==max) {
                if (!suggested || strcmp(friend->user->name, suggested->name)>0) {
                    suggested = friend->user;
                }
            }
        }
    }
    return suggested;
}

/**
 * Friends n suggested friends for the given user.
 * See the handout for how we define a suggested friend.
 * Returns how many friends were successfully followed.
 **/
int add_suggested_friends(User* user, int n) {
    if (!user) return 0;
    int count=0;
    for (int i=0; i<n; i++) {
        User *suggested = get_suggested_friend(user);
        if (suggested) {
            add_friend(user, suggested);
            count++;
        }
    }
    return count;
}

/**
 * Follows n suggested brands for the given user.
 * See the handout for how we define a suggested brand.     
 * Returns how many brands were successfully followed. 	  	
 **/
int in_suggested(char suggested_array[][MAX_STR_LEN], char *brand_name, int size) {
    for (int i=0; i<size; i++) {
        if (strcmp(brand_name, suggested_array[i])==0) return 1;
    }
    return 0;
}
int follow_suggested_brands(User* user, int n) {
    if (!user || n<1) return 0;
    int followed=0, curr=0, max=0;
    char suggested[MAX_STR_LEN], suggested_array[n][MAX_STR_LEN];
    for (int m=0; m<n; m++) {
        strcpy(suggested, "");
        bool changed = false;
        max=0;
        for (int i=0; i<MAT_SIZE; i++) {
            curr=0;
            if (!in_brand_list(user->brands, brand_names[i]) && !in_suggested(suggested_array, brand_names[i], followed)) {
                for (int j=0; j<MAT_SIZE; j++) {
                    if (in_brand_list(user->brands, brand_names[j]) && brand_adjacency_matrix[i][j]) curr++;
                }
                if (curr>max) {
                    max=curr;
                    strcpy(suggested, brand_names[i]);
                    changed = true;
                }
                else if (curr==max) {
                    if (strcmp(brand_names[i], suggested)>0) {
                        strcpy(suggested, brand_names[i]);
                        changed = true;
                    }
                }
            }
        }
        if (changed) {
            strcpy(suggested_array[followed], suggested);
            followed++;
            changed = false;
        }
    }
    for (int i=0; i<followed; i++) {
        user->brands = insert_into_brand_list(user->brands, suggested_array[i]);
    }
    return followed;
}
