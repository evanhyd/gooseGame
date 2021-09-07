#include <cstdio>
#include <cctype>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <Windows.h>

enum
{
ENGINEERING,
MATH,
SCIENCE,
ART,
ENVIRONMENTAL,
HEALTH
};

enum
{
    TINY, //[0, 8)
    SMALL, // [8, 18)
    MEDIUM, // [18, 30)
    LARGE // [30, INF)
};



const int MAXNAMENUMBER = 14;
const int MAXGOOSENUMBER = 200;
const int MATING_DISTANCE = 4;
const int SPEED_X_UPPER = 3;
const int SPEED_X_LOWER = -3;
const int SPEED_Y_UPPER = 3;
const int SPEED_Y_LOWER = -3;
const int SCREENX = 80;
const int SCREENY = 300;
const int INVALIDGOOSEINDEX = -1;

void drawGoose();


typedef struct 
{
    char name[MAXNAMENUMBER];
    int level;//affect health and size 
    float exp;//eat food for more
    int health;
    int size;

    //random
    int breed; //6 faculties
    int vector_x;//direction + magnitude
    int vector_y;
    int x; //vertical position
    int y; // horizontal position

}Goose;
Goose gooseList[MAXGOOSENUMBER];



Goose createRandomGoose(const char* name)
{
    Goose goose;
    strcpy_s(goose.name, name);
    goose.health = 3;

    goose.size = TINY;
    goose.level = 1;
    goose.exp = float(rand() % 10240);
    while (goose.exp >= 0.5f * goose.level * goose.level)
    {
        goose.exp -= 0.5f * goose.level * goose.level;
        ++goose.level;
        goose.health = 2 * goose.level + 1;
        if (goose.level >= 30)
        {
            goose.size = LARGE;
        }
        else if (goose.level >= 18)
        {
            goose.size = MEDIUM;
        }
        else if (goose.level >= 8)
        {
            goose.size = SMALL;
        }
        //else its tiny 
    }


    goose.breed = rand() % 6;
    goose.vector_x = rand() % 4;
    goose.vector_y = rand() % 4;
    goose.x = rand() % SCREENX;
    goose.y = rand() % SCREENY;

    return goose;
}

Goose createNewBornGoose(int gooseIndex1, int gooseIndex2)
{
    Goose dad = gooseList[gooseIndex1];
    Goose mom = gooseList[gooseIndex2];


    Goose goose;
    for (int strIndex = 0; strIndex < MAXNAMENUMBER; ++strIndex)
    {
        bool from_dad = rand() % 2;
        if (from_dad) goose.name[strIndex] = dad.name[strIndex];
        else goose.name[strIndex] = mom.name[strIndex];

        //break if the last char copied was a null terminator
        if (goose.name[strIndex] == '\0') break;
    }

    goose.level = 1;
    goose.exp = 0.0f;
    goose.health = 3;
    goose.size = TINY;

    bool from_dad = rand() % 2;
    if (from_dad) goose.breed = dad.breed; //from parent's breed
    else goose.breed = mom.breed;


    goose.vector_x = 0;
    goose.vector_y = 0;

    //always from mom's coordinate
    goose.x = mom.x;
    goose.y = mom.y;

    return goose;
}

void addGoose(Goose goose)
{
    for (int i = 0; i < MAXGOOSENUMBER; ++i)
    {
        if (gooseList[i].health <= 0)
        {
            gooseList[i] = goose;
            break;
        }
    }
}

int findGoose(const char* name)
{
    for (int i = 0; i < MAXGOOSENUMBER; ++i)
    {
        if (strcmp(name, gooseList[i].name) == 0 && gooseList[i].health > 0)
        {
            return i;
        }
    }
    return INVALIDGOOSEINDEX;
}



void levelUp(int gooseIndex)
{
    while (gooseList[gooseIndex].exp >= 0.5f * gooseList[gooseIndex].level * gooseList[gooseIndex].level)
    {
        gooseList[gooseIndex].exp -= 0.5f * gooseList[gooseIndex].level * gooseList[gooseIndex].level;
        ++gooseList[gooseIndex].level;
        gooseList[gooseIndex].health = 2 * gooseList[gooseIndex].level + 1;
        if (gooseList[gooseIndex].level >= 30)
        {
            gooseList[gooseIndex].size = LARGE;
        }
        else if (gooseList[gooseIndex].level >= 18)
        {
            gooseList[gooseIndex].size = MEDIUM;
        }
        else if (gooseList[gooseIndex].level >= 8)
        {
            gooseList[gooseIndex].size = SMALL;
        }
        //else its tiny 
    }
}





//FEED FUNCTIONS

enum
{
    GRASS,
    BERRIES,
    FLOWERS,
    SEEDS,
    STEM,
    CAFEFOOD,
};
const int MAXFOODNUMBER = 6;
int money = 6969;
float foodValue[MAXFOODNUMBER] = { 3.0f, 15.0f, 9.0f, 6.0f, 1.5f, 30.0f };
const char* foodName[MAXFOODNUMBER] = { "Grass", "Berries", "Flowers", "Seeds", "Stem", "Cafefood" };
int foodPrice[MAXFOODNUMBER] = { 10, 45, 28, 19, 6, 85 };
void feed(int gooseIndex, int foodIndex)
{
    float value = foodValue[foodIndex];
    gooseList[gooseIndex].exp += value;
    levelUp(gooseIndex);
  
}





//PET FUNCTIONS
enum
{
    GENTLE,
    FIRM,
    HARSH,
    EXTREME
};
const int MAXPETNUMBER = 4;
float petExpGain[MAXPETNUMBER] = { 1.0f, 5.0f, 12.0f, 30.0f };
float petSuccessChance[MAXPETNUMBER] = { 0.85f, 0.65f, 0.25f, 0.01f };
int petHealthPenalty[MAXPETNUMBER] = { 1, 3, 7, 12 };
const char* petName[MAXPETNUMBER] = {"gentle", "firm", "harsh", "extreme"};


void pet(int gooseIndex, int power)
{
    float randomNumber = float(rand())/float(RAND_MAX);
    if (randomNumber > petSuccessChance[power])
    {
        gooseList[gooseIndex].health -= petHealthPenalty[power];
    }
    else
    {
        gooseList[gooseIndex].exp += petExpGain[power];
        levelUp(gooseIndex);
    }
}


//MATING FUNCTION
const float MATINGRATE = 0.75;
void mating()
{
    for (int i = 0; i < MAXGOOSENUMBER; ++i)
    {
        if (gooseList[i].health <= 0) continue;

        //check dad goose's age
        if (gooseList[i].level < 18) continue;

        for (int j = 0; j < MAXGOOSENUMBER; ++j)
        {
            if (gooseList[j].health <= 0 || i == j) continue;

            //check mom goose's age
            if (gooseList[j].level < 18) continue;

            int x_diff = gooseList[i].x - gooseList[j].x;
            int y_diff = gooseList[i].y - gooseList[j].y;

            int distance = int(sqrtf(float(x_diff * x_diff + y_diff * y_diff)));
            if (distance <= MATING_DISTANCE)
            {
                float mate_chance = float(rand()) / float(RAND_MAX);
                if (mate_chance < MATINGRATE)
                {

                    Goose childGoose = createNewBornGoose(i, j);
                    addGoose(childGoose);
                }
            }
        }
    }
}


void move()
{
    for (int i = 0; i < MAXGOOSENUMBER; ++i)
    {
        //skip the dead goose
        if (gooseList[i].health <= 0) continue;

        //randomize the goose speed
        gooseList[i].vector_x += rand() % 3 - 1;
        gooseList[i].vector_y += rand() % 3 - 1;

        //clamp the speed
        if (gooseList[i].vector_x > SPEED_X_UPPER) gooseList[i].vector_x = SPEED_X_UPPER;
        else if (gooseList[i].vector_x < SPEED_X_LOWER) gooseList[i].vector_x = SPEED_X_LOWER;

        if (gooseList[i].vector_y > SPEED_Y_UPPER) gooseList[i].vector_y = SPEED_Y_UPPER;
        else if (gooseList[i].vector_y < SPEED_Y_LOWER) gooseList[i].vector_y = SPEED_Y_LOWER;

        //update goose's position
        gooseList[i].x += gooseList[i].vector_x;
        gooseList[i].y += gooseList[i].vector_y;

        //clamp the coordinate
        if (gooseList[i].x >= SCREENX)
        {
            gooseList[i].x = SCREENX - 1;
            gooseList[i].vector_x = -gooseList[i].vector_x;
        }
        else if (gooseList[i].x < 0)
        {
            gooseList[i].x = 0;
            gooseList[i].vector_x = -gooseList[i].vector_x;
        }

        //-2 because the last line is reserved for the newline character
        if (gooseList[i].y >= SCREENY - 1)
        {
            gooseList[i].y = SCREENY - 2;
            gooseList[i].vector_y = -gooseList[i].vector_y;
        }
        else if (gooseList[i].y < 0)
        {
            gooseList[i].y = 0;
            gooseList[i].vector_y = -gooseList[i].vector_y;
        }
    }
}



//ascii art taken from https://www.asciiart.eu/animals/birds-water
//index: breed, size
const char* gooseAscii[6][4] =
{
    {
        ">o)\n(_>" ,

        ">(.)__ \n(___ /",

        "    (@_\n\\\\\\_\\ \n< ____)",

        "       ENG_\n      ('`< \n       )(\n (----'  '.\n (         ;\n  (_______,'"

    },
    {
        ">o)\n(_>",

        ">(.)__ \n(___ /",

        "    (@_\n\\\\\\_\\ \n< ____)",

        "       MTH_\n      ('`< \n       )(\n (----'  '.\n (         ;\n  (_______,'"
    },
    {
        ">o)\n(_ >",

        ">(.)__ \n(___ /",

        "    (@_\n\\\\\\_\\ \n< ____)",

        "       SCI_\n      ('`< \n       )(\n (----'  '.\n (         ;\n  (_______,'"
    },
    {
        ">o)\n(_>",

        ">(.)__ \n(___ /",

        "    (o_\n\\\\\\_\\ \n< ____)",

        "       ART_\n      ('`< \n       )(\n (----'  '.\n (         ;\n  (_______,'"
    },
    {
        ">o)\n(_>",

        ">(.)__ \n(___ /",

        "    (o_\n\\\\\\_\\ \n< ____)",

        "       ENV_\n      ('`< \n       )(\n (----'  '.\n (         ;\n  (_______,'"
    },
    {
        ">o)\n(_>",

        ">(.)__ \n(___ /",

        "    (o_\n\\\\\\_\\ \n< ____)",

        "       HLT_\n      ('`< \n       )(\n (----'  '.\n (         ;\n  (_______,'"
    }
}; 

char screen[SCREENX][SCREENY];

void drawGoose()
{
    //clear the old screen
    for (int x = 0; x < SCREENX; ++x)
    {
        for (int y = 0; y < SCREENY; ++y)
        {
            screen[x][y] = ' ';
        }
    }

    for (int i = 0; i < MAXGOOSENUMBER; ++i)
    {
        //skip the dead gooseo
        if (gooseList[i].health <= 0) continue;

        //printing the name
        for (int str_index = 0, x = gooseList[i].x, y = gooseList[i].y; /*No condition*/; ++str_index, ++y)
        {
            if (gooseList[i].name[str_index] == '\0')
            {
                break;
            }
            if (y < SCREENY - 1)
            {
                screen[x][y] = gooseList[i].name[str_index];
            }
        }


        //printing the goose ascii
        const char* ascii_str = gooseAscii[gooseList[i].breed][gooseList[i].size];
        for (int str_index = 0, x = gooseList[i].x + 1, y = gooseList[i].y; x < SCREENX; ++str_index, ++y)
        {
            if (ascii_str[str_index] == '\0')
            {
                break;
            }
            if (ascii_str[str_index] == '\n')
            {
                ++x;
                y = gooseList[i].y - 1;
                continue;
            }
            if (y < SCREENY - 1)
            {
                screen[x][y] = ascii_str[str_index];
            }
        }
    }

    //add the new line
    for (int x = 0; x < SCREENX; ++x)
    {
        screen[x][SCREENY - 1] = '\n';
    }

    //print the screen
    system("cls");
    printf("%s\n\nGoose Profiles:\n", screen);
    for (int i = 0; i < MAXGOOSENUMBER; ++i)
    {
        //skip the dead goose
        if (gooseList[i].health <= 0) continue;

        printf("Name: %16s    Health: %3i    Level: %2i    EXP: %.2f/%.2f\n", gooseList[i].name, gooseList[i].health, gooseList[i].level, gooseList[i].exp, 0.5f * float(gooseList[i].level) * float(gooseList[i].level));
    }
    printf("\nBank: %i\n", money);
}


void feedMenu(int gooseIndex)
{

    while (true)
    {
        printf("Welcome to the uw cafe\n");
        for (int i = 0; i < MAXFOODNUMBER; i++)
        {
            printf("%i %s %.2f exp %i$\n", i + 1, foodName[i], foodValue[i], foodPrice[i]);
        }
        printf("%i quit\n", MAXFOODNUMBER + 1);

        int options;
        scanf_s("%i", &options);
        options -= 1;

        if (options == MAXFOODNUMBER) break;

        int cost = foodPrice[options];
        if (money < cost)
        {
            printf("hahahahah u broke\a\n");
        }
        else
        {
            money -= cost;
            feed(gooseIndex, options);
            move();
            drawGoose();
        }
    }
}

void petMenu(int gooseIndex)
{
    while (true)
    {
        printf("%s looks like he/she really wants to get petted. Pet him/her?\n", gooseList[gooseIndex].name);

        for (int i = 0; i < MAXPETNUMBER; ++i)
        {
            printf("%i-%s\n", i+1, petName[i]);
        }
        printf("%i-quit\n", MAXPETNUMBER + 1);


        int options;
        scanf_s("%i", &options);
        options -= 1;

        if (options == MAXPETNUMBER) break;

        pet(gooseIndex, options);
        move();
        drawGoose();

        if (gooseList[gooseIndex].health <= 0)
        {
            printf("AHHOOH, you murdered %s by accident\n", gooseList[gooseIndex].name);
            break;
        }
    }
}

void gooseMenu(int gooseIndex)
{
    while (true)
    {
        int options;
        printf("1-feed\n2-pet\n3-quit\n");
        scanf_s("%i", &options);
        if (options == 1)
        {
            feedMenu(gooseIndex);
        }
        else if (options == 2)
        {
            petMenu(gooseIndex);
        }
        else
        {
            break;
        }

        //check if the player killed the goose
        if (gooseList[gooseIndex].health <= 0) break;
    }
   
}


void gameMenu()
{
    drawGoose();
    while (true)
    {
        int options;
        printf("1-select goose\n2-skip to next day\n3-quit\n");
        scanf_s("%i", &options);
        if (options == 1)
        {
            char gooseName[MAXNAMENUMBER];
            printf("Enter the goose name:\n");
            scanf_s("%s", &gooseName[0], MAXNAMENUMBER);

            int gooseIndex = findGoose(&gooseName[0]);
            if (gooseIndex != INVALIDGOOSEINDEX)
            {
                gooseMenu(gooseIndex);
            }
            else
            {
                printf("cannot find goose because goose does not exist or is dead\n");
            }
        }
        else if (options == 2)
        {
            money += rand() % 15;

            for (int i = 0; i < 24; ++i)
            {
                move();
                mating();
                drawGoose();
            }
        }
        else
        {
            break;
        }
    }
    
}


int main (void)
{
    //initialize the radom seed with the current time
    srand(time(NULL));
 

    Goose g = createRandomGoose("NEVER");
    addGoose(g);
    g = createRandomGoose("ZAC");
    addGoose(g);
    g = createRandomGoose("UP");
    addGoose(g);
    g = createRandomGoose("COOPER");
    addGoose(g);
    g = createRandomGoose("MARSHMALLOW");
    addGoose(g);
    g = createRandomGoose("GIVE");
    addGoose(g);
    g = createRandomGoose("POTATO");
    addGoose(g);
    g = createRandomGoose("YOU");
    addGoose(g);
    g = createRandomGoose("WAITE");
    addGoose(g);
    g = createRandomGoose("GONNA");
    addGoose(g);


    gameMenu();
}

