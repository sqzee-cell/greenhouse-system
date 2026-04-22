// GitHub
#tryinclude	github/git.inc

#if !defined GIT_COMMIT_HASH
	#warning "can`t find GIT_COMMIT_HASH"
	#define GIT_COMMIT_HASH "manual"
#endif

#if !defined GIT_BRANCH
	#warning "can`t find GIT_BRANCH"
	#define GIT_BRANCH "master"
#endif

#if !defined GIT_AUTHOR_NAME
	#warning "can`t find GIT_AUTHOR_NAME"
	#define GIT_AUTHOR_NAME "manual"
#endif

#if !defined GIT_RELEASE_TAG
	#warning "can`t find GIT_RELEASE_TAG"
	#define GIT_RELEASE_TAG "manual"
#endif

// Default inc`s
#include <a_samp>
#include <a_mysql>
#include <crashdetect>
#include <foreach>
#include <sscanf2>
#include <Pawn.CMD>
#include <streamer>

// Макросы
#define void%0(%1) forward%0(%1); public%0(%1)

// MySQL
new MySQL:mysql;

// Игрок
enum player_info
{
	pID, // ID в базе
};
new PI[MAX_PLAYERS][player_info];

// Модули
#include sqzee-cell/greenhouse/main.inc

public OnGameModeInit()
{
	// mysql = mysql_connect("", "", "", "", );

	if(mysql_errno(mysql)) {
		printf("\t[MySQL] Ошибка при подключении к базе данных. Code: %04d", mysql_errno(mysql));
		return SendRconCommand("exit");
	} else {
		CallLocalFunction("OnMySQLConnection", "");
	}

	#if defined _greenhouse_inc_
        GreenHouse:Init();
    #endif

    print("\tOnGameModeInit");

    return true;
}

void OnMySQLConnection()
{
	printf("[MySQL] Успешное подключение к базе данных");

	return true;
}