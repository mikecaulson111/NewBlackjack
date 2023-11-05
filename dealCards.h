#ifndef DEALCARD_H__
#define DEALCARD_H__ 

// void checkWin() {
//     // this should not be a void function!!!!!!!!!!
//     printf("HELLO\n");
// }



void deal_cards_init(playing_card *p_cards, int *p_total, playing_card *p_players_cards, playing_card *p_dealers_cards);

void clear_cards(playing_card *p_players_cards, playing_card *p_dealers_cards);

int check_win(playing_card *p_players_cards, playing_card *p_dealers_cards, bool final);

void hit_card(playing_card *p_cards, int *p_total, playing_card *p_playing_cards);

#endif