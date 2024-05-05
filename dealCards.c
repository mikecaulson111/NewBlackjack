#include <stdio.h>
#include <stdbool.h>
#include "cards.h"
#include "dealCards.h"


void give_card(playing_card *p_playing_cards, playing_card *p_persons_cards, int *p_total, bool showing) {
    int place = 0;

    printf("TOTAL: %i\n", *p_total);

    // find where to place the next card
    for ( int i = 0; i < 17; i++ )
    {
        if (p_persons_cards[i].suit == -1)
        {
            place = i;
            break;
        }
    }

    p_persons_cards[place].suit = p_playing_cards[*p_total].suit;
    p_persons_cards[place].number = p_playing_cards[*p_total].number;
    p_persons_cards[place].place = *p_total;
    p_persons_cards[place].showing = showing;

    *p_total -= 1;
}


void hit_card(playing_card *p_cards, int *p_total, playing_card *p_playing_cards) {
   // something here for hitting card similar to above , actually probably call the above function 

   give_card(p_cards, p_playing_cards, p_total, true);
}

int check_initial_win(playing_card *p_players_cards, playing_card *p_dealers_cards) {
    // this is only if both players have 2 cards:
    int player_total = 0;
    int dealer_total = 0;

    for (int i = 0; i < 17; i++)
    {
        if ( p_players_cards[i].suit != -1 )
        {
            if ( p_players_cards[i].number >= 9 )
            {
                player_total += 10;
            }
            else if ( p_players_cards[i].number == 0 )
            {
                player_total += 11;
            }
            else
            {
                player_total += p_players_cards[i].number + 1;
            }
        }
        if ( p_dealers_cards[i].suit != -1 )
        {
            if ( p_dealers_cards[i].number >= 9 )
            {
                dealer_total += 10;
            }
            else if ( p_dealers_cards[i].number == 0 )
            {
                dealer_total += 11;
            }
            else
            {
                dealer_total += p_dealers_cards[i].number + 1;
            }
        }
    }
    if (dealer_total > player_total) {
        return 1; // MJC TODO same as the comment directly below
    }
    return 0; // MJC TODO I NEed to figure out what I actually need to return here lol
}

int check_win(playing_card *p_players_cards, playing_card *p_dealers_cards, bool final) {
    // final is to check at the end of the game, otherwise just check if player has 21 or dealer has 21

    // win meanings
    // 0: blackjack player winner
    // 1: player wins no blackjack
    // 2: dealer wins
    // 3: push
    // -1: NO WINNER

    int player_total = 0;
    int dealer_total = 0;
    int player_count = 0;
    int dealer_count = 0;

    int winner = -1;

    for ( int i = 0; i < 17; i++ )
    {
        if ( p_players_cards[i].suit != -1 )
        {
            int num = p_players_cards[i].number + 1;
            if ( num > 10 )
            {
                num = 10;
            }
            else if ( num == 1 )
            {
                num = 11; // mjc add support for making this both a 1 or 11!!
            }
            player_total += num;
            player_count++;
        }
        if ( p_dealers_cards[i].suit != -1 )
        {
            int num = p_dealers_cards[i].number + 1;
            if ( num > 10 )
            {
                num = 10;
            }
            else if ( num == 1 )
            {
                num = 11; // mjc add support for making this both a 1 or 11!!
            }
            dealer_total += num;
            dealer_count++;
        }
    }

   if ( player_count == 2 && player_total == 21 )
   {
        if (dealer_total != 21) // BLACKJACK
        {
            winner = 0;
        }
        else // push
        {
            winner = 3;
        }
   }
   else if ( dealer_count == 2 && dealer_total == 21 ) // dealer wins right away
   {
        winner = 2;
   }
   else if ( player_total > 21 ) // player busts dealer wins
   {
        winner = 2;
   }
   else
   {
        if ( final ) {
            if ( dealer_total < 17 )
            {
                if ( player_total > dealer_total ) // player wins
                {
                    winner = 1;
                }
                else if ( player_total < dealer_total ) // dealer wins
                {
                    winner = 2;
                }
                else // push
                {
                    winner = 3;
                }
            }
        }
   }

    return winner;
}


void deal_cards_init(playing_card *p_cards, int *p_total, playing_card *p_players_cards, playing_card *p_dealers_cards) {
    // do dealing
    // need to create an array for both player and dealer of size 17 in main 
    // to account for if they get all aces, and it will hold the cards in front of them

    // check if total > 15 or 15% and then deal top card and decrease total by 1
    // for( int i = 0; i < *p_total + 1; i++ )
    // {
    //     printf("[%i], %i, %i\n",i, p_cards[i].suit, p_cards[i].number);
    // }

    give_card(p_cards, p_players_cards, p_total, true);
    give_card(p_cards, p_dealers_cards, p_total, false);
    give_card(p_cards, p_players_cards, p_total, true);
    give_card(p_cards, p_dealers_cards, p_total, true);
}


void clear_cards(playing_card *p_players_cards, playing_card *p_dealers_cards) {
    // clear both arrays
    for( int i = 0; i < 17; i++ )
    {
        p_players_cards[i].suit = -1;
        p_players_cards[i].number = -1;
        p_players_cards[i].showing = false;
        p_players_cards[i].place = -1;
        p_dealers_cards[i].suit = -1;
        p_dealers_cards[i].number = -1;
        p_dealers_cards[i].showing = false;
        p_dealers_cards[i].place = -1;
        printf("%i, %i, %i\n", i, p_players_cards[i].suit, p_players_cards[i].number);
    }
}