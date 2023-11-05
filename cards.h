#ifndef CARDS_H__
#define CARDS_H__

#define TOTAL_NUM_CARDS 51

typedef struct t_playing_card {
    int suit;
    int number;
    bool showing;
    int place;
} playing_card;

#endif