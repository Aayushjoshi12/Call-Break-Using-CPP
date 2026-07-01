getCardIndex(string leadSuit, int currentBest, bool isAIFirstMove) {
    if (isHuman) {
        Vector2 mouse = GetMousePosition();
        for (int i = 0; i < handSize; i++) {
            if (CheckCollisionPointRec(mouse, rects[i]) &&
                IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) {
                return i;
            }
        }
        return -1;
    } else {
        if(isAIFirstMove){
            return 0;
            isAIFirstMove=false;

        }
        else{

        // ───────── check if must beat ─────────
        bool mustBeat = false;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].suit == leadSuit && hand[i].value > currentBest) {
                mustBeat = true;
                break;
            }
        }

        // ───────── if must beat, choose smallest winning card ─────────
        if (mustBeat) {
            int best = -1;

            for (int i = 0; i < handSize; i++) {
                if (hand[i].suit == leadSuit && hand[i].value > currentBest) {
                    if (best == -1 || hand[i].value < hand[best].value) {
                        best = i;
                    }
                }
            }

            return best;
        }

        // ───────── follow suit normally ─────────
        int lowestLead = -1;
        int lowestLeadVal = 15;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].suit == leadSuit) {
                if (hand[i].value < lowestLeadVal) {
                    lowestLeadVal = hand[i].value;
                    lowestLead = i;
                }
            }
        }

        if (lowestLead != -1) {
            return lowestLead;
        }

        // ───────── no lead suit → play smallest spade ─────────
        int smallestTrump = -1;
        int smallestTrumpVal = 15;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].suit == "spades") {
                if (hand[i].value < smallestTrumpVal) {
                    smallestTrumpVal = hand[i].value;
                    smallestTrump = i;
                }
            }
        }

        if (smallestTrump != -1) return smallestTrump;

        // ───────── fallback ─────────
        int lowestAny = 0;
        int lowestAnyVal = 15;

        for (int i = 0; i < handSize; i++) {
            if (hand[i].value < lowestAnyVal) {
                lowestAnyVal = hand[i].value;
                lowestAny = i;
            }
        }

        return lowestAny;
     }
    }

}