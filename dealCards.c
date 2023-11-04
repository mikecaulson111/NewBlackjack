#include <stdio.h>
#include "cards.h"
#include "dealCards.h"


void deal_cards_init(playing_card *cards, int total) {
    // do dealing
    // need to create an array for both player and dealer of size 17 in main 
    // to account for if they get all aces, and it will hold the cards in front of them

    // check if total > 15 or 15% and then deal top card and decrease total by 1
    for( int i = 0; i < total; i++ )
    {
        printf("[%i], %i, %i\n",i, cards[i].suit, cards[i].number);
    }
}