#include "common.h"

#include "draw.h"
#include "stage.h"

extern App   app;
extern Stage stage;

static void logic(void);
static void draw(void);
static void initPlayer(void);
static void fireBullet(void);
static void doPlayer(void);
static void doBullets(void);
static void drawPlayer(void);
static void drawBullets(void);
static void spawnEnemies(void);

static void doFighters();

static void drawFighters();

static Entity      *player;
static SDL_Texture *bulletTexture;
static SDL_Texture *enemyTexture;
static int          enemySpawnTimer;

void initStage(void)
{
    app.delegate.logic = logic;
    app.delegate.draw = draw;

    memset(&stage, 0, sizeof(Stage));
    stage.fighterTail = &stage.fighterHead;
    stage.bulletTail = &stage.bulletHead;

    initPlayer();

    bulletTexture = loadTexture("resources/playerBullet.png");

    enemyTexture = loadTexture("resources/enemy.png");

    enemySpawnTimer = 0;
}

static void initPlayer()
{
    player = malloc(sizeof(Entity));
    memset(player, 0, sizeof(Entity));
    stage.fighterTail->next = player;
    stage.fighterTail = player;

    player->x = 100;
    player->y = 100;
    player->texture = loadTexture("resources/player.png");
    SDL_QueryTexture(player->texture, NULL, NULL, &player->w, &player->h);
}

static void logic(void)
{
    doPlayer();

    doFighters();

    doBullets();

    spawnEnemies();
}

static void doPlayer(void)
{
    player->dx = player->dy = 0;

    if (player->reload > 0)
    {
        player->reload--;
    }

    if (app.keyboard[SDL_SCANCODE_UP])
    {
        player->dy = -PLAYER_SPEED;
    }

    if (app.keyboard[SDL_SCANCODE_DOWN])
    {
        player->dy = PLAYER_SPEED;
    }

    if (app.keyboard[SDL_SCANCODE_LEFT])
    {
        player->dx = -PLAYER_SPEED;
    }

    if (app.keyboard[SDL_SCANCODE_RIGHT])
    {
        player->dx = PLAYER_SPEED;
    }

    if (app.keyboard[SDL_SCANCODE_LCTRL] && player->reload == 0)
    {
        fireBullet();
    }

    player->x += player->dx;
    player->y += player->dy;
}

static void fireBullet(void)
{
    Entity *bullet;

    bullet = malloc(sizeof(Entity));
    memset(bullet, 0, sizeof(Entity));
    stage.bulletTail->next = bullet;
    stage.bulletTail = bullet;

    bullet->x = player->x;
    bullet->y = player->y;
    bullet->dx = PLAYER_BULLET_SPEED;
    bullet->health = 1;
    bullet->texture = bulletTexture;
    SDL_QueryTexture(bullet->texture, NULL, NULL, &bullet->w, &bullet->h);

    bullet->y += (player->h / 2) - (bullet->h / 2);

    player->reload = 8;
}

static void doFighters() {
    Entity *e, *prev;

    prev = &stage.fighterHead;

    for (e = stage.fighterHead.next ; e != NULL ; e = e->next)
    {
        e->x += e->dx;
        e->y += e->dy;

        if ( e != player && e->x < -e->w)
        {
            if ( e == stage.fighterTail)
            {
                stage.bulletTail = prev;
            }
            prev->next = e->next;
            free(e);
            e = prev;
        }
        prev = e;
    }
}

static void doBullets(void)
{
    Entity *b, *prev;

    prev = &stage.bulletHead;

    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        b->x += b->dx;
        b->y += b->dy;

        if (b->x > SCREEN_WIDTH)
        {
            if (b == stage.bulletTail)
            {
                stage.bulletTail = prev;
            }

            prev->next = b->next;
            free(b);
            b = prev;
        }

        prev = b;
    }
}

static void draw(void)
{
    //drawPlayer();
    drawFighters();

    drawBullets();
}

/**
 * Removed call temporarily (?)
 */
static void drawPlayer(void)
{
    blit(player->texture, player->x, player->y);
}

static void drawFighters()
{
    Entity *e;
    for ( e = stage.fighterHead.next ; e != NULL ; e = e->next)
    {
        blit(e->texture, e->x, e->y);
    }
}

static void drawBullets(void)
{
    Entity *b;

    for (b = stage.bulletHead.next; b != NULL; b = b->next)
    {
        blit(b->texture, b->x, b->y);
    }
}

static void spawnEnemies(void)
{
    Entity *enemy;

    if (--enemySpawnTimer <= 0)
    {
        enemy = malloc(sizeof(Entity));
        memset(enemy, 0, sizeof(Entity));
        stage.fighterTail->next = enemy;
        stage.fighterTail = enemy;

        enemy->x = SCREEN_WIDTH;
        enemy->y = rand() % SCREEN_HEIGHT;
        enemy->texture = enemyTexture;
        SDL_QueryTexture(enemy->texture, NULL, NULL, &enemy->w, &enemy->h);

        enemy->dx = -(2 + (rand() % 4));

        enemySpawnTimer = 30 + (rand() % 60);
    }
}