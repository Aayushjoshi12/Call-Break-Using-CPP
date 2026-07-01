chooseBid() {
    int strong = 0;

    for (int i = 0; i < handSize; i++) {
        if (hand[i].suit == "spades" || hand[i].value >= 11)
            strong++;
    }
    
    return max(1, (int)(1.2 * strong / 2));
   // return max(1,(int)(1.5*strong / 2));
}