#if defined _greenhouse_inc_
    #endinput
#endif
#define _greenhouse_inc_

#define GreenHouse: gh_

stock GreenHouse:Init()
{
    printf("\tGreenHouse:Init(%d)", __line);

    return true;
}