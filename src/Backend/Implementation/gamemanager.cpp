
bool RoundManager::updateRound(Player *players[4])
{
    if (finished)
        return false;

    Player *current = players[currentPlayer];

    // isaifirstmove is only true for the trick leader if they are AI
    // update it per-move so mid-trick bots get correct info
    bool isLeading = moves.empty();
    bool isAILeading = isLeading && (currentPlayer != 0);

    int card_index = current->getCardIndex(leadsuit, currentBest, isAILeading);
    if (card_index == -1)
        return false;

    if (!isValidMove(current, card_index))
        return false;

    Card played = current->playCard(card_index);

    // ── update best card tracking ──
    if (moves.empty())
    {
        leadsuit = played.suit;
        currentBestSuit = leadsuit;
        currentBest = played.value;
    }
    else if (played.suit == "spades" && currentBestSuit != "spades")
    {
        currentBestSuit = "spades";
        currentBest = played.value;
    }
    else if (played.suit == currentBestSuit && played.value > currentBest)
    {
        currentBest = played.value;
    }

    moves.push_back({current->player_id, played});
    currentPlayer = (currentPlayer + 1) % 4;

    // ── trick complete ──
    if (moves.size() == 4)
    {
        finished = true;
        winner = getWinner();
        if (winner != -1)
        {
            for (int i = 0; i < 4; i++)
            {
                if (players[i]->player_id == winner)
                {
                    players[i]->tricksWon++;
                    break;
                }
            }
        }
        else
        {
            // fallback: should never happen, but log if it does
            std::cout << "[ERROR] getWinner() returned -1! currentBestSuit="
                      << currentBestSuit << " currentBest=" << currentBest << std::endl;
        }
    }

    return true;
}