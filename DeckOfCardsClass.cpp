#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <ctime>    // For time()
#include <cstdlib>  // For srand() and rand()
#include <random>
#include <string>
#include <cmath>
#include <vector>


using namespace std;

class DeckOfCards 
{

/*
DeckOfCards will handle constructing a playing deck, shuffling, reshuffling, burning cards and returning (dealing) cards
that a client application can then present anyway that it likes.  Client can query deckType, # of cards left, or deckSize.
*/
    
    
private:
    string m_deckType;
    int m_deckSize;
    int m_cardsRemaining;
    int m_currentCard;
    vector<string> m_deck;
        vector<int> m_shuffledPtrs;
    static bool m_isntSeeded;
    
    
    // get a random number
    // NOTE BENE!!! A lot of work has been done to demonstrate how many pseudo random number generators can be predicted 
    // given the approximate time of generation and knowledge of a few picks, i.e. cards.  If you use this class for any
    // serious card playing game, you need to research that and replace this standard JS random number generator with 
    // a randomize function that can't be hacked.  That's why random has been pulled out into this separate function so
    // it can be easily replaced.  YOU HAVE BEEN WARNED!!
    
    
    long double randomMethod() {
        // rand is seeded in the constructor
        long double rvalue = rand();
        rvalue = (rvalue / RAND_MAX);
        return (rvalue);
    }
    
    void shuffleCards(int topCard, int bottomCard) {
        // used by shuffle and reshuffle
        // initialize some working variables
        int place = 0;
        int savePlace = 0;

        // Now we're going to randomly shuffle pointers from the bottom of the deck until we reach the top
        // It looks counter intuitive that we're skipping the first card, but some other card has already been
        // shuffled to that place by the time we get there - or the first card has randomly remained the same
        for (int counter = topCard; counter > bottomCard; counter--) {

            place = floor (randomMethod() * (counter + 1));
            savePlace = m_shuffledPtrs[counter];
            m_shuffledPtrs[counter] = m_shuffledPtrs[place];
            m_shuffledPtrs[place] = savePlace;

            }
        
    }
    
public:

     DeckOfCards(string deckType = "standard", int numJokers = 0, int numDecks = 1) {
        /* Construct the deck of cards requested.
           deckType can be "standard" (52 cards 2 to Ace), "jass"(36 cards 6 to Ace), "skat"(32 cards 7 to Ace) 
           or "euchre" (24 cards 9 to Ace).  A pinochle deck can be built by using type "euchre" and setting
           numDecks to 2.  If deckType is invalid, it is set to "standard".
           numJokers; the number of jokers added to the deck must be a number from 0 to 10.  If omitted, not
           interpretable, negative, or greater than 10, it is set to 0.  If non-integer, it's rounded.
           numDecks; the number of decks created must be a number from 1 to 10.  If omitted, not interpretable, less
           than 1, or greater than 10, it is set to 1.  If non-integer, it's rounded.
        */
        
        // If someone wants to make Italian or German decks or even Tarot, they cn modify these
        // let's initialize the suits
        int numSuits = 4;
        string suit [4] = {"D", "C", "H", "S"};
        // let's initialize the card values
        int numValues = 13;
        string value [13] = {"2", "3", "4", "5", "6", "7", "8", "9", "T", "J", "Q", "K", "A"};
        // Parameter ranges
        int jokerRange [2] = {0, 10};
        int decksRange [2] = {1, 10};
        string validDeckTypes [4] = {"standard", "jass", "skat", "euchre"};
        int numDeckTypes = 4;
        // see note above for starting cards and thus indices
        int deckTypeLowCardIndex[4] = {0, 4, 5, 7};
        
        
        // seed random() if not already seed by previous instantiation
        
        if (DeckOfCards::m_isntSeeded) {
            srand(time(0)); 
            DeckOfCards::m_isntSeeded = false;
        }
       
        
        // Make sure decktype is lower case
        for (string::size_type i; i < deckType.length(); i++) {
            deckType[i] = tolower(deckType[i]);
            }

        // Validate the class parameters - numJokers, then numDecks,then deckType
        if (numJokers < jokerRange[0] || numJokers > jokerRange[1]) numJokers = 0;
        
        if (numDecks < decksRange[0] || numDecks > decksRange[1]) numDecks = 1;

        int lowCardIndex = deckTypeLowCardIndex[0];
        m_deckType = "standard";
        
        // start at 1 because validDeckTypes[0] "standard" is the default and fallback
        for (unsigned int counter = 1; counter < numDeckTypes; counter++) {
           if (deckType == validDeckTypes[counter]) {
               lowCardIndex = deckTypeLowCardIndex[counter];
               m_deckType = deckType;
               counter = numDeckTypes;
           }
        }


        // Calculate the deck size and create the deck array while saving some key indices

        int highCardIndex = numValues - 1;
        int singleDeckSize = ((highCardIndex - lowCardIndex + 1) * numSuits) + numJokers;
        m_deckSize = singleDeckSize * numDecks;
        
        // create an array for unshuffled card names and one for shuffled deck pointers
        // I should probably use vector for my string arrays -- but I'm not going to.
        // Complain to management.

        m_deck.resize(singleDeckSize);
        m_shuffledPtrs.resize(m_deckSize);

        // let's initialize our card names in an unshuffled deck
        int deckIndex = 0;
        for (unsigned int suitCounter = 0; suitCounter < numSuits; suitCounter++) {
            for (unsigned int valueCounter = lowCardIndex; valueCounter <= highCardIndex; valueCounter++) {
                m_deck[deckIndex++] = value[valueCounter] + suit[suitCounter];
            }
        }
        // Add the jokers, if any
        for (unsigned int jokerCounter = 0; jokerCounter < numJokers; jokerCounter++) {
          m_deck[deckIndex++] = "J";
        }

        // Since the card names will be the same for additional decks, we only need to duplicate pointers to the same
        // card names in order to create multiple decks.
        for (unsigned int deckCounter = 0; deckCounter < numDecks; deckCounter++) {
            for (unsigned int counter = 0; counter < singleDeckSize; counter++) {
                m_shuffledPtrs[counter] = counter;
            }
        }


        // set some object values we're going to need
        m_currentCard = 0;
        m_cardsRemaining = m_deckSize;
        
        
    // End of constructor
    }
 
 
    // method to deal numCards cards.  returns a vector of strings with dealt card names.
    // If no more cards are left, it returns a single null string.
    // If more cards are requested than are remaining, it deals the rest of the cards.
    std::vector<std::string> deal(unsigned int numCards) {
        std::vector<std::string> dealtCards;
        
        if (!(m_cardsRemaining)) {
            dealtCards[0] = "";
        }
        else {
            if (numCards > m_cardsRemaining) numCards = m_cardsRemaining;
            dealtCards.resize(numCards);
            for (unsigned int counter = 0; counter < numCards; counter++) {
                dealtCards[counter] = m_deck[m_shuffledPtrs[m_currentCard++]];
                m_cardsRemaining--;
            }
        }
        
        return(dealtCards);
 
 
    // End of deal method
    }
 
     // shuffle the deck.  Note that first time it's a deck ordered by value (times the number of decks)
     // After that we're shuffling the deck starting in its last shuffled order
    void shuffle() {
        shuffleCards(m_deckSize - 1, 0);
        m_currentCard = 0;
        m_cardsRemaining = m_deckSize;
    } // end of shuffle
    
    // reshuffle the remaining cards.  I'm not sure this is ever done in any card game, but I don't know, so I 
    // figured I'd include it.  Leaves curret card and cards remaining the same.
    void reshuffle() {
        shuffleCards(m_deckSize - 1, m_currentCard);
    }
 
 
     // discard the next numcards in the deck by incrementing currentCard   
    unsigned int burn(unsigned int numCards) {
        if (numCards > m_cardsRemaining) {
            numCards = m_cardsRemaining;
            }
        m_currentCard =+ numCards;
        m_cardsRemaining =- numCards;
        return(numCards);
        
    } // end of burn
 
 unsigned int getCardsRemaining(void) {
     return(m_cardsRemaining);
 }
  
 unsigned int getDeckSize(void) {
     return(m_deckSize);
 } 
  
  
};

// define static variable
bool DeckOfCards::m_isntSeeded = true;

// end of class DeckOfCards
