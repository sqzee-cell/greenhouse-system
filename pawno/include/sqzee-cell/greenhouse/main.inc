#if defined _greenhouse_inc_
    #endinput
#endif
#define _greenhouse_inc_

#define GreenHouse: gh_

// Стринг
static greenhouse_str[512];

// Константы
#define MAX_GH_PER_PLAYER 5
#define TIME_TO_GROW_SEC  600 // 10 минут
#define KEY_INTERACTION   KEY_WALK

// ID моделей для визуала
#define MODEL_GREENHOUSE  19473 // Основной объект теплицы
#define MODEL_TOMATO_BOX  1220  // Ящик с урожаем

// Основные переменные
enum E_GREENHOUSE_DATA {
    gh_ID,
    Float:gh_PosX,
    Float:gh_PosY,
    Float:gh_PosZ,
    gh_Progress,
    gh_Stage,
    bool:gh_Upgraded,
    gh_ObjectID,
    Text3D:gh_Text3D,
    gh_BoxObjects[4] // Массив ящиков для визуализации 4-х стадий роста
};
static PlayerGreenhouses[MAX_PLAYERS][MAX_GH_PER_PLAYER][E_GREENHOUSE_DATA];
static Iterator:ActiveGreenhousePlayers<MAX_PLAYERS>;

// Создание/апдейт/сохранение/удаление
static stock CreateGreenhouseVisuals(playerid, slot)
{
    if(!(0 <= playerid < MAX_PLAYERS))
        return false;

    else if(!(0 <= slot < MAX_GH_PER_PLAYER))
        return false;

    new Float:x = PlayerGreenhouses[playerid][slot][gh_PosX],
        Float:y = PlayerGreenhouses[playerid][slot][gh_PosY],
        Float:z = PlayerGreenhouses[playerid][slot][gh_PosZ];

    PlayerGreenhouses[playerid][slot][gh_ObjectID] = CreateDynamicObject(MODEL_GREENHOUSE, x, y, z, 0.0, 0.0, 0.0, .playerid = playerid);
    PlayerGreenhouses[playerid][slot][gh_Text3D] = CreateDynamic3DTextLabel("Теплица загружается...", 0xFFFFFFFF, x, y, z + 2.0, 15.0, .playerid = playerid);

    return UpdateGreenhouseStage(playerid, slot);
}
static stock UpdateGreenhouseStage(playerid, slot)
{
    if(!(0 <= playerid < MAX_PLAYERS))
        return false;

    else if(!(0 <= slot < MAX_GH_PER_PLAYER))
        return false;

    new prog = PlayerGreenhouses[playerid][slot][gh_Progress];
    new stage = (prog * 4) / TIME_TO_GROW_SEC; // Высчитываем стадию от 0 до 4

    if(stage > 4) stage = 4;
    PlayerGreenhouses[playerid][slot][gh_Stage] = stage;

    // Обновляем 3D Текст
    new labelStr[128];
    if(stage == 4) {
        format(labelStr, sizeof(labelStr), "{32CD32}[Урожай созрел!]{FFFFFF}\nНажмите ALT для сбора");
    } else {
        format(labelStr, sizeof(labelStr), "Прогресс: %d / %d сек.\nСтадия: %d/4\n%s", 
            prog, TIME_TO_GROW_SEC, stage, 
            PlayerGreenhouses[playerid][slot][gh_Upgraded] ? "{FFA500}[Улучшено x2]{FFFFFF}" : "");
    }
    UpdateDynamic3DTextLabelText(PlayerGreenhouses[playerid][slot][gh_Text3D], 0xFFFFFFFF, labelStr);

    // Визуализируем рост ящиками возле теплицы
    new Float:bx = PlayerGreenhouses[playerid][slot][gh_PosX] + 2.0;
    new Float:by = PlayerGreenhouses[playerid][slot][gh_PosY];
    new Float:bz = PlayerGreenhouses[playerid][slot][gh_PosZ];

    for(new i; i < 4; i++) {
        if(i < stage) {
            // Если ящика еще нет на этой стадии - создаем
            if(!IsValidDynamicObject(PlayerGreenhouses[playerid][slot][gh_BoxObjects][i])) {
                PlayerGreenhouses[playerid][slot][gh_BoxObjects][i] = CreateDynamicObject(MODEL_TOMATO_BOX, bx, by + (i * 1.5), bz, 0.0, 0.0, 0.0, .playerid = playerid);
            }
        } else {
            // Если стадия откатилась (например, собрали урожай) - удаляем лишние ящики
            if(IsValidDynamicObject(PlayerGreenhouses[playerid][slot][gh_BoxObjects][i])) {
                DestroyDynamicObject(PlayerGreenhouses[playerid][slot][gh_BoxObjects][i]);
                PlayerGreenhouses[playerid][slot][gh_BoxObjects][i] = INVALID_OBJECT_ID;
            }
        }
    }

    return true;
}
static stock SaveGreenHouse(playerid, index, bool:destroy)
{
    if(!(0 <= playerid < MAX_PLAYERS))
        return false;

    else if(!(0 <= index < MAX_GH_PER_PLAYER))
        return false;

    format(greenhouse_str, sizeof(greenhouse_str), "\
        INSERT INTO `greenhouses` \
            (`id`, `owner_id`, `pos_x`, `pos_y`, `pos_z`, `progress`, `upgraded`) \
        VALUES \
            (%d, %d, %.3f, %.3f, %.3f, %d, %b) \
        ON DUPLICATE KEY UPDATE \
            `progress` = VALUES(`progress`),\
            `upgraded` = VALUES(`upgraded`);",

        PlayerGreenhouses[playerid][index][gh_ID],
        PI[playerid][pID],
        PlayerGreenhouses[playerid][index][gh_PosX],
        PlayerGreenhouses[playerid][index][gh_PosY],
        PlayerGreenhouses[playerid][index][gh_PosZ],
        PlayerGreenhouses[playerid][index][gh_Progress],
        PlayerGreenhouses[playerid][index][gh_Upgraded]
    );
    mysql_tquery(mysql, greenhouse_str);

    if(destroy) DestroyGreenHouse(playerid, index);

    return true;
}
static stock DestroyGreenHouse(playerid, index)
{
    if(!(0 <= playerid < MAX_PLAYERS))
        return false;

    else if(!(0 <= index < MAX_GH_PER_PLAYER))
        return false;

    if(IsValidDynamicObject(PlayerGreenhouses[playerid][index][gh_ObjectID]))
    {
        DestroyDynamicObject(PlayerGreenhouses[playerid][index][gh_ObjectID]);
        PlayerGreenhouses[playerid][index][gh_ObjectID] = INVALID_OBJECT_ID;
    }
    if(IsValidDynamic3DTextLabel(PlayerGreenhouses[playerid][index][gh_Text3D]))
    {
        DestroyDynamic3DTextLabel(PlayerGreenhouses[playerid][index][gh_Text3D]);
        PlayerGreenhouses[playerid][index][gh_Text3D] = Text3D:-1;
    }

    for(new i; i < 4; i++)
    {
        if(IsValidDynamicObject(PlayerGreenhouses[playerid][index][gh_BoxObjects][i]))
        {
            DestroyDynamicObject(PlayerGreenhouses[playerid][index][gh_BoxObjects][i]);
            PlayerGreenhouses[playerid][index][gh_BoxObjects][i] = INVALID_OBJECT_ID;
        }
    }

    PlayerGreenhouses[playerid][index][gh_ID] = 0;
    PlayerGreenhouses[playerid][index][gh_Progress] = 0;
    PlayerGreenhouses[playerid][index][gh_Stage] = 0;
    PlayerGreenhouses[playerid][index][gh_Upgraded] = false;

    return true;
}

// Основная логика
stock GreenHouse:Init()
{
    // Создание таблицы, если она не существует
    mysql_tquery(mysql, "\
        CREATE TABLE IF NOT EXISTS `greenhouses` (\
        `id` int(11) NOT NULL AUTO_INCREMENT,\
        `owner_id` int(11) NOT NULL,\
        `pos_x` float NOT NULL,\
        `pos_y` float NOT NULL,\
        `pos_z` float NOT NULL,\
        `progress` int(11) NOT NULL DEFAULT '0',\
        `upgraded` tinyint(1) NOT NULL DEFAULT '0',\
        PRIMARY KEY (`id`),\
        INDEX `idx_owner_id` (`owner_id`)\
        ) ENGINE=InnoDB DEFAULT CHARSET=utf8;");

    // Таймер под проверку роста
    SetTimer(""#GreenHouse:"OnGlobalTick", 1000, true);

    // Обнуление iterator
    Iter_Clear(ActiveGreenhousePlayers);

    printf("\t[%s] GreenHouse:Init", __file);

    return true;
}

// Игрок
stock GreenHouse:LoadPlayerData(playerid)
{
    format(greenhouse_str, sizeof(greenhouse_str), "SELECT * FROM `greenhouses` WHERE `owner_id` = '%d' LIMIT "#MAX_GH_PER_PLAYER";");
    mysql_tquery(mysql, greenhouse_str, ""#GreenHouse:"OnLoadPlayerData", "d", playerid);
}

void GreenHouse:OnLoadPlayerData(playerid)
{
    new rows = cache_num_rows();

    if(rows < 1)
        return false;

    for(new i; i < (rows <= MAX_GH_PER_PLAYER ? rows:MAX_GH_PER_PLAYER); i++)
    {
        cache_get_value_name_int(i, "id", PlayerGreenhouses[playerid][i][gh_ID]);
        cache_get_value_name_float(i, "pos_x", PlayerGreenhouses[playerid][i][gh_PosX]);
        cache_get_value_name_float(i, "pos_y", PlayerGreenhouses[playerid][i][gh_PosY]);
        cache_get_value_name_float(i, "pos_z", PlayerGreenhouses[playerid][i][gh_PosZ]);
        cache_get_value_name_int(i, "progress", PlayerGreenhouses[playerid][i][gh_Progress]);
        cache_get_value_name_bool(i, "upgraded", PlayerGreenhouses[playerid][i][gh_Upgraded]);

        CreateGreenhouseVisuals(playerid, i);
    }

    Iter_Add(ActiveGreenhousePlayers, playerid);

    return true;
}

// Таймер
void GreenHouse:OnGlobalTick()
{
    foreach(ActiveGreenhousePlayers, i)
    {
        if(!IsPlayerConnected(i))
        {
            continue;
        }

        for(new g; g < MAX_GH_PER_PLAYER; g++)
        {
            if(!PlayerGreenhouses[i][g][gh_ID])
                continue;

            else if(PlayerGreenhouses[i][g][gh_Progress] < TIME_TO_GROW_SEC)
            {
                PlayerGreenhouses[i][g][gh_Progress] += (PlayerGreenhouses[i][g][gh_Upgraded] ? 2:1);

                if(PlayerGreenhouses[i][g][gh_Progress] >= TIME_TO_GROW_SEC) {
                    PlayerGreenhouses[i][g][gh_Progress] = TIME_TO_GROW_SEC;
                }

                new expectedStage = (PlayerGreenhouses[i][g][gh_Progress] * 4) / TIME_TO_GROW_SEC;
                if(expectedStage != PlayerGreenhouses[i][g][gh_Stage] || expectedStage == 4) {
                    UpdateGreenhouseStage(i, g);
                }
            }
        }
    }

    return true;
}