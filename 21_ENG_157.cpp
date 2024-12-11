#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <random>
#include <fstream>
#include <map>
#include <iomanip>

using namespace std;

// Define enums for card values and suites
enum cardSuites{ spades, clubs, hearts, diamonds };
enum cardValues{ two = 2, three, four, five, six, seven, eight, nine, ten, jack, queen, king, ace };
int HighCard = 100, OnePair = 200, TwoPair = 300, ThreeOfAKind = 400, Straight = 500, Flush = 600, FullHouse = 700, FourOfAKind = 800, StraightFlush = 900;

class Card
{
public:
    cardValues value;
    cardSuites suite;
};

class Deck
{
public:
    vector<Card> cards; // Vector to store the cards in the deck

    Deck() { //constructor
        for (int i = two; i <= ace; i++) {
            for (int j = spades; j <= diamonds; j++) {
                Card card;
                card.value = static_cast<cardValues>(i);
                card.suite = static_cast<cardSuites>(j);
                cards.push_back(card);
            }
        }
        shuffleCards();
    }

    void shuffleCards() // Method to shuffle the cards in the deck
    {
        random_device randomDevice;
        mt19937 randomGenerator(randomDevice());
        shuffle(cards.begin(), cards.end(), randomGenerator);
    }
};

class Player
{
public:
    Deck hand;
    vector<Card> cards;
    int totscore;
    float wProbability;
    bool checkPlaying;

    Player() : totscore(0), wProbability(0), checkPlaying(true){};

    int scoreOfCurrentHand()
    {
        int score = 0;

        map<cardValues, int> cardValueCounts;
        map<cardSuites, int> cardSuiteCounts;

        for (const auto &card : cards) {  // Loop through the cards in the hand and count the occurrences of card values and suites
            cardValueCounts[card.value]++;
            cardSuiteCounts[card.suite]++;
        }

        int maxCount = 0;
        for (const auto &count : cardValueCounts) {
            maxCount = max(maxCount, count.second);
        }

        bool isFlush = false;
        for (const auto &count : cardSuiteCounts) {
            if (count.second == 5) {
                isFlush = true;
                break;
            }   
        }

        vector<cardValues> values; // Create a vector to store card values and sort it
        for (const auto &card : cards) {
            values.push_back(card.value);
        }
        std::sort(values.begin(), values.end());

        bool isStraight = false;
        for (int i = 0; i < values.size() - 4; i++) {
            if (values[i + 4] - values[i] == 4) {
                isStraight = true;
                break;
            }
        }

        cardValues highestCardValue = values.back(); //determine the highest card value
        // score based on ranking
        if (isStraight && isFlush) {
            score = StraightFlush + highestCardValue;
        } 
        else if (maxCount == 4) {
            score = FourOfAKind + highestCardValue;
        } 
        else if (maxCount == 3 && cardValueCounts.size() == 2) {
            score = FullHouse + highestCardValue;
        } 
        else if (isFlush) {
            score = Flush + highestCardValue;
        } 
        else if (isStraight) {
            score = Straight + highestCardValue;
        } 
        else if (maxCount == 3) {
            score = ThreeOfAKind + highestCardValue;
        } 
        else if (cardValueCounts.size() == 3) {
            score = TwoPair + highestCardValue;
        } 
        else if (cardValueCounts.size() == 4) {
            score = OnePair + highestCardValue;
        } 
        else {
            score = HighCard + highestCardValue;
        }

        return score;
    }

};

void distribution(vector<Player> &players, Deck CardDeck)
{
    CardDeck.cards.erase(CardDeck.cards.begin()); // remove first two cards
    CardDeck.cards.erase(CardDeck.cards.begin());

    for (int i = 0; i < players.size(); i++){
        players[i].cards.clear();
        for (int j = 0; j < 5; j++){
            if (!players[i].checkPlaying)
                continue;

            players[i].cards.push_back(CardDeck.cards[0]);
            CardDeck.cards.erase(CardDeck.cards.begin());
        }
    }
}

void Scorecard(vector<Player> &players)
{
    int maxScore = 0;
    int winner = 0;
    for (int i = 1; i < players.size(); i++){
        if (!players[i].checkPlaying)
            continue;

        int score = players[i].scoreOfCurrentHand();
        if (score > maxScore){
            maxScore = score;
            winner = i;
        }
    }

    for (int i = 1; i < players.size(); i++){
        if (!players[i].checkPlaying)
            continue;

        if (i == winner)
            continue;
        players[i].totscore += -1;
    }

    players[winner].totscore += 10;
}

void winningProbability(vector<Player> &players)
{
    // winning probability is calculated based on the current score of a player
    int totCurrentScore = 0;
    for (int i = 1; i < players.size(); i++){
        if (players[i].checkPlaying)
            totCurrentScore += players[i].scoreOfCurrentHand();
    }
    for (int i = 1; i < players.size(); i++)
        if (players[i].checkPlaying)
            players[i].wProbability = (float(players[i].scoreOfCurrentHand()) / float(totCurrentScore)) * 100.0;
        else
            players[i].wProbability = 0.0;
}

string cardsinfo(Player player)
{
    string data = "";
    for (int i = 0; i < player.cards.size(); i++){
        // value
        switch (player.cards[i].value){
            case two: data += "2 "; break;
            case three: data += "3 "; break;
            case four: data += "4 "; break;
            case five: data += "5 "; break;
            case six: data += "6 "; break;
            case seven: data += "7 "; break;
            case eight: data += "8 "; break;
            case nine: data += "9 "; break;
            case ten: data += "10 "; break;
            case jack: data += "J "; break;
            case queen: data += "Q "; break;
            case king: data += "K "; break;
            case ace: data += "A "; break;
            default:
                break;
        }

        // suite
        switch (player.cards[i].suite){
            case spades: data += "(Spades)"; break;
            case clubs: data += "(Clubs)"; break;
            case hearts: data += "(Hearts)"; break;
            case diamonds: data += "(Diamonds)"; break;
            default:
                break;
        }

        data += "   ";
    }

    return data;
}

void log(string data)
{
    fstream file;
    file.open("log.txt", ios::app);
    file << data << endl;
    file.close();
}

int main()
{
    int noOfPlayers = 0;
    int noOfRounds = 0;

    cout << "Enter the number of players in the game (maximum number is 10): ";
    cin >> noOfPlayers;
    noOfPlayers++; // +1 dealer
    cout << "Enter the number of rounds in the game: ";
    cin >> noOfRounds;

    Deck CardDeck;
    vector<Player> players;
    players.resize(noOfPlayers);

    for (int i = 1; i <= noOfRounds; i++) //simulation of rounds
    { 
        cout << "\n\tRound " << i << endl << endl;
        CardDeck.shuffleCards(); // simulating card shuffling by the dealer
        distribution(players, CardDeck); // simulating card distrubution by the dealer

        cout << "Dealer's cards" << endl; // Display dealer's cards
        cout << cardsinfo(players[0]) << endl << endl;
        log("\tRound " + to_string(i) + "\n");
        log("Cards are shuffled");

        for (int i = 1; i < players.size(); i++){ // from 1 to discard dealer
            if (!players[i].checkPlaying)
                continue;

            cout << "Do you want to withdraw player " << i << " from this round? Yes or No (y/n) : ";
            char choice;
            cin >> choice;

            if (choice == 'n' || choice == 'N'){}
            else if (choice == 'y' || choice == 'Y'){
                cout << "Player " << i << " withdrew from this round" << endl;
                players[i].checkPlaying = false;
            }
            else{
                cout << "Invalid Input!" << endl;
            }
        }

        Scorecard(players);
        winningProbability(players);

        log("Cards are distributed to the players.\n");
        for (int i = 1; i < players.size(); i++){
            string data = "";
            data += "\tPlayer " + to_string(i) + "\n";
            data += "\tCards               :" + cardsinfo(players[i]) + "\n";
            data += "\tWinning probability :" + to_string(players[i].wProbability) + "%\n";
            data += "\tScore               :" + to_string(players[i].totscore) + "\n";
            log(data);
        }

        cout << "\n****** Scores ******" << endl << endl;

        for (int i = 1; i < players.size(); i++){ // from 1 to discard dealer
            if (i == 0)
                continue;

            cout << "\tPlayer " << i << endl;
            cout << "\tScore: " << players[i].totscore << endl;
            cout << fixed << setprecision(2) << "\tWinning probability: " << players[i].wProbability << " %" << endl;

            if (!players[i].checkPlaying){
                cout << "\tPlayer " << i << " is not playing in this round" << endl << endl;
                players[i].checkPlaying = true;
            }
            else { cout << endl; }
        }

        int winnerMarks = 0;
        int winner = 0;
        for (int i = 1; i < players.size(); i++){
            if (players[i].totscore > winnerMarks){
                winnerMarks = players[i].totscore;
                winner = i;
            }
        }
        cout << "\nWinner is player " << winner << endl << endl;
        log("Winner is player " + to_string(winner) + "\n");

    }
    cout << "------------- End of the game. -------------"<<endl;
    log("------------- End of the game. -------------");

    cin.clear();
    cin.get();
    return 0;
}
