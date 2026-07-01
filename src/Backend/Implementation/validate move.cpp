
isValidMove(Player *p, int cardIndex)
{
    if (leadsuit.empty())
        return true;

    if (p->hand[cardIndex].suit == leadsuit)
    {
        // must beat if possible
        bool canBeat = false;
        for (int i = 0; i < p->handSize; i++)
        {
            if (p->hand[i].suit == leadsuit && p->hand[i].value > currentBest)
            {
                canBeat = true;
                break;
            }
        }
        if (canBeat)
        {
            return p->hand[cardIndex].value > currentBest;
        }
        else
        {
            return true; // can't beat, any lead suit card is fine
        }
    }
    else
    {
        // playing off-suit — only allowed if player has no lead suit
        bool hasLeadSuit = false;
        for (int i = 0; i < p->handSize; i++)
        {
            if (p->hand[i].suit == leadsuit)
            {
                hasLeadSuit = true;
                break;
            }
        }

        if (hasLeadSuit)
        {
            return false; // must follow suit
        }

        // no lead suit — must play spade if they have one
        bool hasSpades = false;
        for (int i = 0; i < p->handSize; i++)
        {
            if (p->hand[i].suit == "spades")
            {
                hasSpades = true;
                break;
            }
        }

        if (hasSpades)
        {
            return p->hand[cardIndex].suit == "spades";
        }

        return true; // no lead suit, no spades — play anything
    }
}

