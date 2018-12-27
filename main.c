/*
 * MagAdventure
 * Text-based adventure game by @kostasvs, a christmas gift for @anastmag
 * 28/12/2018
 */  

#include <stdio.h>
#include <string.h>

#include <ctype.h>
#include "adshell.h"
#define SAME(A,B) (!strcmp(A,B))
#define TRUE 1
#define FALSE !(TRUE)
#define NULL (char *)0

#define OpDoor 1
#define OpDoorLock 2
#define OpDrawer 3
#define OpDildo 4
#define OpPistol 5

int parse (char *line, struct look_up_func tab[]) {
      
    //char *line;
    //struct look_up_func tab[];	/* array of structs */
    int acted = FALSE;		/* we have not done anything yet */
    static int wordstarts[LINELEN / 2];
  
    /*
     * As each word is seperated from the next by
     * whitespace there can only be at most LINELEN / 2
     */ 
    static char words[LINELEN / 2][WORDLEN];
      
    int slot;			/* slot in table */
    int lindex;			/* maximum used slot in table */
    int cn;			/* character counter */
    int x;			/* dogsbody variable ! */
    char *lineLow;			/* input string */
    char *str;			/* input string */
    char c;			/* next character */
      
    int fpslot;
    struct look_up_func *fp;
    char between_words;
  
    
    lineLow = line;
    for(int i = 0; lineLow[i]; i++){
        lineLow[i] = tolower(lineLow[i]);
    }
    
    /*
     * Build up tables of words and word starts
     */ 
    str = lineLow;
  
    between_words = TRUE;
      
    for (lindex = 0, slot = 0, cn = 0; lindex < LINELEN; lindex++) {
      
        if (((c = *str++) == '\0') || isspace (c)) {
	  
            if (cn > 0) {
	      
        		/* terminated a word */ 
        		strncpy (words[slot], &lineLow[wordstarts[slot]], cn);
                words[slot][cn] = '\0';
                cn = 0;
                slot++;
            }
	  
            between_words = TRUE;
	  
            if (c == '\0') {
                break;		/* no more input line */
            }
        }
        else {
	  
            if (between_words) {
	      
        		/* started a word */ 
        		wordstarts[slot] = lindex;
                between_words = FALSE;
            }
	  
            if (cn < WORDLEN) cn++;		/* count a character */
        }
    }
  
    /* now try to find a match */ 
    int place = 0;
    for (x = 0; x < slot;) {
      
        fpslot = 0;
      
        while ((fp = &tab[fpslot++]) != NULL) {
	  
            if (SAME (words[x], fp->token)) {
	      
                acted = TRUE;
                x += (*fp->action) (lineLow, place); //wordstarts[x]);
                if (x < 0) x = 0;
                break;
            }
	  
            if (SAME (fp->token, "")) break;
        }
        place++;
      
    	/* if no match was found above ignore this word */ 
    	if (SAME (fp->token, "")) x++;
    }
  
    return acted;
}

char status[2048];
int operation = 0;
int operand = 0;
int operand2 = 0;
int room = 0;
int hasPistol = -1;
int hasPistolBullet = 1;
int isDildoDead = 0;
int hasDildo = 0;
int hasPenetratedLock = 0;
int mustReset = 0;

// ACTIONS AND OBJECTS
int help(char *line, int index) {
    printf("--- Available commands: ---\n");
    printf("    examine <object>\n"); // operation 1
    printf("    goto <object>\n"); // operation 2
    printf("    take <object>\n"); // operation 3
    printf("    use <object1> [on <object2>]\n"); // operation 4
    printf("    inventory\n"); // operation 5
    printf("    status\n"); // operation 6
    printf("    exit\n");
    /* tell parser to examine next word */
    operation = -1; // suppress syntax warning
    return 1;
}
int fexit(char *line, int index) {
    /* allows you to get out of prog */
    printf("Really quit? (y = yes, n = no): ");
    gets(line);
    if (line[0] == 'y' || line[0] == 'Y') exit(0);
    operation = -1; // suppress syntax warning
}
int examine(char *line, int index) {
    if (index == 0)  operation = 1;
    return 1;
}
int go_to(char *line, int index) {
    if (index == 0)  operation = 2;
    return 1;
}
int take(char *line, int index) {
    if (index == 0)  operation = 3;
    return 1;
}
int use(char *line, int index) {
    if (index == 0)  operation = 4;
    return 1;
}
int useopen(char *line, int index) {
    printf ("\"open\" is not a valid command. Try \"use\" instead. ");
    return 1;
}
int inventory(char *line, int index) {
    if (index == 0)  operation = 5;
    return 1;
}
int showstatus(char *line, int index) {
    if (index == 0)  operation = 6;
    return 1;
}
int door(char *line, int index) {
    if (room != 0 && room != 1) return 1;
    if (index == 1)  operand = OpDoor;
    else if (index > 1)  operand2 = OpDoor;
    return 1;
}
int doorlock(char *line, int index) {
    if (room != 1) return 1;
    if (index == 1)  operand = OpDoorLock;
    else if (index > 1)  operand2 = OpDoorLock;
    return 1;
}
int drawer(char *line, int index) {
    if (room != 1) return 1;
    if (index == 1)  operand = OpDrawer;
    else if (index > 1)  operand2 = OpDrawer;
    return 1;
}
int dildo(char *line, int index) {
    if (room != 1) return 1;
    if (index == 1)  operand = OpDildo;
    else if (index > 1)  operand2 = OpDildo;
    return 1;
}
int pistol(char *line, int index) {
    if (room != 1 || hasPistol == -1) return 1;
    if (index == 1)  operand = OpPistol;
    else if (index > 1)  operand2 = OpPistol;
    return 1;
}
struct look_up_func tab[] =
{
    "?", help,
    "help", help,
    "exit", fexit,
    "examine", examine,
    "goto", go_to,
    "go", go_to,
    "take", take,
    "use", use,
    "open", useopen,
    "inventory", inventory,
    "status", showstatus,
    "door", door,
    "lock", doorlock,
    "drawer", drawer,
    "dildo", dildo,
    "pistol", pistol,
    "gun", pistol,
    "", (int (*)())0
};

void reset() {
    
    room = 0;
    strcpy (status, "You are in an empty room. A DOOR is ahead of you.\n");
    printf (status);
    operation = 0;
    operand = 0;
    operand2 = 0;
    room = 0;
    hasPistol = -1;
    hasPistolBullet = 1;
    isDildoDead = 0;
    hasDildo = 0;
    hasPenetratedLock = 0;
    mustReset = 0;
}

int main() {
     
    char line[LINELEN];
    
    // game start
    reset();
    
    while(1) {
        
        if (mustReset > 0) {
            printf("...\n...\n...\n...\n...\n");
            reset();
        }
        // reset variables
        operation = 0;
        operand = 0;
        operand2 = 0;
        
        printf("What next? ");
        gets(line);
        if(!parse(line, tab)) {
            printf("Cannot do that! Type '?' for help\n");
        }
        else {
            if (operation == 0) {
                printf("Cannot do that! Type '?' for help\n");
            }
            // EXAMINE
            if (operation == 1) {
                if (operand == 0) printf("Examine what?\n");
                if (operand == OpDoor) printf("It's a door. Doors are cool.\n");
                if (operand == OpDoorLock) printf("It has a big keyhole. Good luck finding a key this big, if you know what I mean.\n");
                if (operand == OpDrawer) {
                    if (hasPistol == -1) printf("Drawers usually have stuff'n'shit in them.\n");
                    else if (hasPistol == 0) printf("It has a PISTOL in it.\n");
                    else printf("It's empty now.\n");
                }
                if (operand == OpDildo) {
                    if (isDildoDead == 0) printf("It's a dildo with wings, flying around the room.\n");
                    else printf("The dildo is dead, so it no longer flies. It can still function as a normal dildo, though.\n");
                }
                if (operand == OpPistol) {
                    if (hasPistolBullet == 1) printf("It's loaded with a single bullet.\n");
                    else printf("The pistol is empty.\n");
                }
            }
            // GOTO
            if (operation == 2) {
                if (operand == 0) printf("Go where?\n");
                if (operand == OpDoor) operation = 4;
            }
            // TAKE
            if (operation == 3) {
                if (operand == 0) printf("Take what?\n");
                if (operand == OpDoor) printf("The door does not seem willing to come with you.\n");
                if (operand == OpDoorLock) printf("The lock is attached to the door. Both physically and emotionally.\n");
                if (operand == OpDrawer) printf("You should probably \"use\" the drawer to open it.\n");
                if (operand == OpDildo) {
                    if (hasDildo == 1) printf("You already have it.\n");
                    else if (hasDildo == 2) printf("The dildo has broken.\n");
                    else if (isDildoDead == 0) printf("It flies too high, you can't reach it.\n");
                    else {
                        printf("You take the dildo.\n");
                        strcpy (status, "You are in a larger room with a DOOR. There is a LOCK on the DOOR.\n");
                        strcat (status, "There is a DRAWER next to you.\n");
                        hasDildo = 1;
                        hasPistolBullet = 0;
                    }
                }
                if (operand == OpPistol) {
                    if (hasPistol == -1) printf("What pistol?\n");
                    if (hasPistol == 1) printf("You already have it.\n");
                    if (hasPistol == 0) {
                        printf("You take the pistol.\n");
                        hasPistol = 1;
                    }
                }
            }
            // USE
            if (operation == 4) {
                if (operand == 0) printf("Use what?\n");
                else if (operand == operand2) printf("Can't use an object with itself.\n");
                else if (operand == OpDoor) {
                    if (operand2 > 0) printf ("You can't use this on something else.\n");
                    else if (room == 0) {
                        room++;
                        printf ("\nYou go through the door.\n");
                        strcpy (status, "You are in a larger room with a DOOR. There is a LOCK on the DOOR.\n");
                        strcat (status, "There is a DRAWER next to you. Also, a DILDO is flying around the room.\n");
                        printf (status);
                    }
                    else if (room == 1) {
                        if (hasPenetratedLock == 1) {
                            printf("\nYou go through the door.\n...\n");
                            printf("You reach Hell, and the Devil appears in front of you.\n");
                            if (isDildoDead > 0) {
                                printf("He exclaims: \"HOW DARE YOU KILL MY FLYING DILDO! YOU WILL PAY FOR THIS!\"\n");
                                printf("And so he traps your soul and you spend an eternity in hell.\n");
                                printf("It's a cool story though, the other damned souls always laugh when they hear it, and you become popular among them.\n");
                                printf("That's something, I guess.\n");
                            }
                            else {
                                printf("He says to you: \"OH YOU FOUND MY FLYING DILDO! THANK YOU, I'VE BEEN LOOKING FOR IT EVERYWHERE!\"\n");
                                printf("Being unable to understand why the Devil has a flying dildo, or why he is here anyway, ");
                                printf("you decide you've had enough of this shit. So, you tell him to go fuck himself, and he does, using his dildo.\n");
                                printf("You walk away towards the sunset, completely brainfucked by this experience.\n");
                                printf("This is supposed to be the game's happy ending. Don't ask why. I wrote it at 01:00 AM.\n");
                            }
                            printf("RESTART GAME? (y = yes, anything else = exit):\n");
                            gets(line);
                            if (line[0] == 'y' || line[0] == 'Y') mustReset = 1;
                            else exit(0);
                        }
                        else printf ("The door is locked. It has a LOCK, remember?\n");
                    }
                }
                if (operand == OpDoorLock) {
                    if (operand2 > 0) printf ("You can't use this on something else.\n");
                    else printf("You must use a key on the lock... Or something else.\n");
                }
                if (operand == OpDrawer) {
                    if (operand2 > 0) printf ("You can't use this on something else.\n");
                    else if (hasPistol >= 1) printf("It's empty now.\n");
                    else {
                        if (hasPistol == -1) {
                            printf("You open the drawer. ");
                            hasPistol = 0;
                        }
                        printf("It has a PISTOL in it.\n");
                    }
                }
                if (operand == OpDildo) {
                    if (hasDildo <= 0) printf ("You must catch the dildo if you want to use it.\n");
                    else {
                        if (operand2 > 0) {
                            if (operand2 == OpDoor) {
                                if (hasPenetratedLock == 0) {
                                    printf ("\nSlapping the dildo on the door didn't work.\n");
                                    printf ("You can try using the dildo on the LOCK, though.\n");
                                }
                                else printf ("How about you just open the door like a normal person?\n");
                            }
                            else if (operand2 == OpDoorLock) {
                                if (hasPenetratedLock == 0) {
                                    printf ("\nYou insert the dildo into the lock. Hard.\n");
                                    printf ("It looks like the door is unlocked now. Impressive.\n");
                                    hasPenetratedLock = 1;
                                }
                                else printf ("\nYou insert the dildo on the lock, but nothing happens, since the door is already unlocked.\nI guess penetrating locks arouses you?\n");
                            }
                            else printf ("You can't use the dildo on that.\n");
                        }
                        else {
                            printf("You put the dildo in your butt. It's okay if you enjoyed it, but it didn't help your quest, so you pull it back out.\n");
                        }
                    }
                }
                if (operand == OpPistol) {
                    if (hasPistol < 1) printf("You must TAKE the pistol before using it.\n");
                    else if (hasPistolBullet <= 0) printf("The pistol is empty, can't use it.\n");
                    else {
                        if (operand2 > 0) {
                            if (operand2 == OpDoor) {
                                printf ("\nShooting the door is unlikely to help.\n");
                                printf ("You can try shooting the LOCK, though.\n");
                            }
                            else if (operand2 == OpDoorLock) {
                                printf ("You shoot the LOCK, and it breaks, unlocking the door.\n");
                                hasPenetratedLock = 1;
                                hasPistolBullet = 0;
                            }
                            else if (operand2 == OpDildo) {
                                printf ("You shoot down the DILDO. It drops to the floor, and lays there dead.\n");
                                isDildoDead = 1;
                                hasPistolBullet = 0;
                            }
                            else printf ("There is no point in shooting that.\n");
                        }
                        else {
                            printf("\nSince you didn't specify what to shoot, you shoot yourself.\n...\n");
                            printf("You wake up in Hell, because you committed suicide.\n");
                            printf("The Devil laughs, and decides to give you one more chance.\n");
                            printf("RESTART GAME? (y = yes, anything else = exit):\n");
                            gets(line);
                            if (line[0] == 'y' || line[0] == 'Y') mustReset = 1;
                            else exit(0);
                        }
                    }
                }
            }
            // INVENTORY
            if (operation == 5) {
                if (hasDildo > 0 && hasPistol > 0) printf("You have a dildo and a pistol.\n");
                else if (hasDildo > 0) printf("You have a dildo.\n");
                else if (hasPistol > 0) printf("You have a pistol.\n");
                else printf("You are not holding anything.\n");
            }
            // STATUS
            if (operation == 6) {
                printf(status);
            }
        }
    }
}
