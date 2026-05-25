#include <bits/stdc++.h>
using namespace std;

using Hand=vector<uint16_t>;
using Count=vector<int>;
using Result=pair<vector<string>,vector<uint8_t>>;
using Deck=vector<string>;

#define PRINT_STRENGTHS false

mt19937_64 rng(chrono::steady_clock::now().time_since_epoch().count()); // initialize rng
uint16_t STRAIGHT_MASK=0b11111;
string suits[]={"c","d","h","s"};
string ranks[]={"","A","2","3","4","5","6","7","8","9","T","J","Q","K","A"}; // wheel

















/* HELPER FUNCTIONS
====================================================================
====================================================================*/

struct CountInfo{
    vector<int> quads,trips,pairs,high;
};

vector<string> card_generator(Hand& hand){
    vector<string> cards;

    for(int suitNum=0;suitNum<4;suitNum++)
        for(int rank=14;rank>=2;rank--)
            if(hand[suitNum]&(1<<rank))
                cards.push_back(ranks[rank]+suits[suitNum]);

    return cards;
}

Hand hand_generator(vector<string> cards){
    Hand hand(4,0);

    unordered_map<char,int> suitMap={{'c',0},{'d',1},{'h',2},{'s',3}};
    unordered_map<char,int> rankMap={
        {'A',14},{'2',2},{'3',3},{'4',4},{'5',5},
        {'6',6},{'7',7},{'8',8},{'9',9},
        {'T',10},{'J',11},{'Q',12},{'K',13}
    };

    for(auto card:cards){
        int rank=rankMap[card[0]];
        int suit=suitMap[card[1]];

        hand[suit]|=(1<<rank);

        if(rank==14)
            hand[suit]|=(1<<1);
    }

    return hand;
}

Count get_rank_count(Hand& hand){
    Count count(15,0);

    for(auto suit:hand)
        for(int rank=2;rank<=14;rank++)
            if(suit&(1<<rank))
                count[rank]++;

    return count;
}






















/* HAND STRENGTH EVALUATOR
====================================================================
====================================================================*/

Result check_straight_flush(Hand& hand){
    for(int suitNum=0;suitNum<4;suitNum++){
        uint16_t suit=hand[suitNum];

        for(int start=10;start>=1;start--){
            uint16_t mask=STRAIGHT_MASK<<start;

            if((suit&mask)==mask){
                vector<string> cards;
                vector<uint8_t> strength={(uint8_t)(start==10?10:9)};

                for(int rank=start+4;rank>=start;rank--){
                    cards.push_back(ranks[rank]+suits[suitNum]);
                    strength.push_back(rank);
                }

                return {cards,strength};
            }
        }
    }

    return {{},{}};
}

Result check_four_kind(Hand& hand, Count& count){
    int quad=0,kicker=0;

    for(int rank=14;rank>=1;rank--){
        if(count[rank]==4) quad=rank;
        else if(count[rank] && !kicker) kicker=rank;
    }

    if(!quad) return {{},{}};

    vector<string> cards;
    vector<uint8_t> strength={8,(uint8_t)quad,(uint8_t)quad,(uint8_t)quad,(uint8_t)quad,(uint8_t)kicker};

    for(int suitNum=0;suitNum<4;suitNum++)
        if(hand[suitNum]&(1<<quad))
            cards.push_back(ranks[quad]+suits[suitNum]);

    for(int suitNum=0;suitNum<4;suitNum++)
        if(hand[suitNum]&(1<<kicker)){
            cards.push_back(ranks[kicker]+suits[suitNum]);
            break;
        }

    return {cards,strength};
}
 
Result check_full_house(Hand& hand, Count& count){
    int trips=0,pair=0;

    for(int rank=14;rank>=1;rank--){
        if(count[rank]>=3 && !trips) trips=rank;
        else if(count[rank]>=2 && !pair) pair=rank;
    }

    if(!trips || !pair) return {{},{}};

    vector<string> cards;
    vector<uint8_t> strength={7,(uint8_t)trips,(uint8_t)trips,(uint8_t)trips,(uint8_t)pair,(uint8_t)pair};

    for(int suitNum=0;suitNum<4 && cards.size()<3;suitNum++)
        if(hand[suitNum]&(1<<trips))
            cards.push_back(ranks[trips]+suits[suitNum]);

    for(int suitNum=0;suitNum<4 && cards.size()<5;suitNum++)
        if(hand[suitNum]&(1<<pair))
            cards.push_back(ranks[pair]+suits[suitNum]);

    return {cards,strength};
}

Result check_flush(Hand& hand){
    for(int suitNum=0;suitNum<4;suitNum++){
        if(__builtin_popcount(hand[suitNum] & ~(1<<1)) >= 5){
            vector<string> cards;
            vector<uint8_t> strength={6};

            for(int rank=14;rank>=1 && cards.size()<5;rank--)
                if(hand[suitNum]&(1<<rank)){
                    cards.push_back(ranks[rank]+suits[suitNum]);
                    strength.push_back(rank);
                }

            return {cards,strength};
        }
    }

    return {{},{}};
}

Result check_straight(Hand& hand){
    uint16_t allRanks=0;

    for(auto suit:hand) allRanks|=suit;

    for(int start=10;start>=1;start--)
        if((allRanks&(STRAIGHT_MASK<<start))==(STRAIGHT_MASK<<start)){
            vector<string> cards;
            vector<uint8_t> strength={5};

            for(int rank=start+4;rank>=start;rank--){
                strength.push_back(rank);

                for(int suitNum=0;suitNum<4;suitNum++)
                    if(hand[suitNum]&(1<<rank)){
                        cards.push_back(ranks[rank]+suits[suitNum]);
                        break;
                    }
            }

            return {cards,strength};
        }

    return {{},{}};
}

Result check_trips(Hand& hand, Count& count){
    int trips=0;
    vector<int> kickers;

    for(int rank=14;rank>=1;rank--){
        if(count[rank]==3) trips=rank;
        else if(count[rank]) kickers.push_back(rank);
    }

    if(!trips) return {{},{}};

    vector<string> cards;
    vector<uint8_t> strength={4,(uint8_t)trips,(uint8_t)trips,(uint8_t)trips,(uint8_t)kickers[0],(uint8_t)kickers[1]};

    for(int suitNum=0;suitNum<4;suitNum++)
        if(hand[suitNum]&(1<<trips))
            cards.push_back(ranks[trips]+suits[suitNum]);

    for(auto kicker:kickers)
        for(int suitNum=0;suitNum<4;suitNum++)
            if(hand[suitNum]&(1<<kicker)){
                cards.push_back(ranks[kicker]+suits[suitNum]);
                break;
            }

    return {cards,strength};
}

Result check_two_pair(Hand& hand, Count& count){
    vector<int> pairs;
    int kicker=0;

    for(int rank=14;rank>=1;rank--){
        if(count[rank]>=2 && pairs.size()<2) pairs.push_back(rank);
        else if(count[rank] && !kicker) kicker=rank;
    }

    if(pairs.size()<2) return {{},{}};

    vector<string> cards;
    vector<uint8_t> strength={3,(uint8_t)pairs[0],(uint8_t)pairs[0],(uint8_t)pairs[1],(uint8_t)pairs[1],(uint8_t)kicker};

    for(int i=0;i<2;i++)
        for(int suitNum=0;suitNum<4 && cards.size()<2*(i+1);suitNum++)
            if(hand[suitNum]&(1<<pairs[i]))
                cards.push_back(ranks[pairs[i]]+suits[suitNum]);

    for(int suitNum=0;suitNum<4;suitNum++)
        if(hand[suitNum]&(1<<kicker)){
            cards.push_back(ranks[kicker]+suits[suitNum]);
            break;
        }

    return {cards,strength};
}

Result check_pair(Hand& hand, Count& count){
    int pair=0;
    vector<int> kickers;

    for(int rank=14;rank>=1;rank--){
        if(count[rank]==2) pair=rank;
        else if(count[rank]) kickers.push_back(rank);
    }

    if(!pair) return {{},{}};

    vector<string> cards;
    vector<uint8_t> strength={2,(uint8_t)pair,(uint8_t)pair,(uint8_t)kickers[0],(uint8_t)kickers[1],(uint8_t)kickers[2]};

    for(int suitNum=0;suitNum<4;suitNum++)
        if(hand[suitNum]&(1<<pair))
            cards.push_back(ranks[pair]+suits[suitNum]);

    for(auto kicker:kickers)
        for(int suitNum=0;suitNum<4;suitNum++)
            if(hand[suitNum]&(1<<kicker)){
                cards.push_back(ranks[kicker]+suits[suitNum]);
                break;
            }

    return {cards,strength};
}

Result check_high(Hand& hand, Count& count){
    vector<string> cards;
    vector<uint8_t> strength={1};

    for(int rank=14;rank>=1 && cards.size()<5;rank--)
        for(int suitNum=0;suitNum<4;suitNum++)
            if(hand[suitNum]&(1<<rank)){
                cards.push_back(ranks[rank]+suits[suitNum]);
                strength.push_back(rank);
                break;
            }

    return {cards,strength};
}

CountInfo cluster_count(Count& count){
    CountInfo c;

    for(int rank=14;rank>=1;rank--){
        if(count[rank]==4) c.quads.push_back(rank);
        else if(count[rank]==3) c.trips.push_back(rank);
        else if(count[rank]==2) c.pairs.push_back(rank);
        else if(count[rank]==1) c.high.push_back(rank);
    }

    return c;
}

Result count_hand_strength(Hand& hand,Count& count){
    CountInfo c=cluster_count(count);

    if(c.quads.size())
        return check_four_kind(hand,count);

    if(c.trips.size() && (c.pairs.size() || c.trips.size()>1))
        return check_full_house(hand,count);

    if(c.trips.size())
        return check_trips(hand,count);

    if(c.pairs.size()>=2)
        return check_two_pair(hand,count);

    if(c.pairs.size())
        return check_pair(hand,count);

    return check_high(hand,count);
}

Result noncount_hand_strength(Hand& hand){
    Result flush=check_flush(hand);
    Result straight=check_straight(hand);

    if(!flush.second.empty() && !straight.second.empty()){
        Result sf=check_straight_flush(hand);
        if(!sf.second.empty()) return sf; // RF is a subset of SF
    }

    if(!flush.second.empty())
        return flush;

    if(!straight.second.empty())
        return straight;

    return {{},{}};
}

Result eval_hand_strength(Hand& hand){
    Count count=get_rank_count(hand);

    Result countResult=count_hand_strength(hand,count);
    Result nonCountResult=noncount_hand_strength(hand);

    return (countResult.second>nonCountResult.second)?
            countResult:nonCountResult;
}











/* EQUITY CALCULATOR USING EXHAUSTIVE SEARCH AND MONTE CARLO SIMULATION
==================================================================================
==================================================================================*/

vector<double> multiway_equity(vector<Hand>& players,Hand& board,bool montecarlo=true,int sims=100000){
    int n=players.size();
    vector<double> equity(n,0);

    Hand used(4,0);

    for(auto& p:players)
        for(int suit=0;suit<4;suit++)
            used[suit]|=p[suit];

    for(int suit=0;suit<4;suit++)
        used[suit]|=board[suit];

    Hand deck(4,0b111111111111110);

    for(int suit=0;suit<4;suit++)
        deck[suit]&=~used[suit];

    int need=5;

    for(int suit=0;suit<4;suit++)
        for(int rank=2;rank<=14;rank++)
            if(board[suit]&(1<<rank))
                need--;

    auto process=[&](Hand currBoard){

        vector<Result> strength;

        for(auto p:players){
            Hand curr=p;

            for(int suit=0;suit<4;suit++)
                curr[suit]|=currBoard[suit];

            strength.push_back(
                eval_hand_strength(curr)
            );
        }

        vector<uint8_t> best={0};
        vector<int> winners;

        for(int p=0;p<n;p++){
            if(PRINT_STRENGTHS){
                cout<<"Player "<<(p+1)<<" (";
                for(int i: strength[p].second) 
                    cout<<i<<", ";
                cout<<")"<<endl;
            }
            

            if(strength[p].second>best){
                best=strength[p].second;
                winners={p};
            } else if(strength[p].second==best)
                winners.push_back(p);
        }

        double share=1.0/winners.size();

        for(auto w:winners)
            equity[w]+=share;
    };

    int total=0;

    if(montecarlo){

        uniform_int_distribution<int> dist(0,51);

        for(int sim=0;sim<sims;sim++){

            Hand currBoard=board;
            Hand finalDeck=deck;

            int cnt=0;

            while(cnt<need){

                int x=dist(rng);

                int suit=x/13;
                int rank=x%13+1;

                if(!(finalDeck[suit]&(1<<rank)))
                    continue;

                currBoard[suit]|=(1<<rank);
                finalDeck[suit]&=~(1<<rank);

                if(rank==1){
                    currBoard[suit]|=(1<<14);
                    finalDeck[suit]&=~(1<<14);
                }

                cnt++;
            }

            process(currBoard);
        }

        total = sims;
    }

    else if(need==0){
        process(board);
        total=1;
    }

    else if(need==1){

        for(int x=0;x<52;x++){

            int suit=x/13;
            int rank=x%13+1;

            if(!(deck[suit]&(1<<rank)))
                continue;

            Hand curr=board;

            curr[suit]|=(1<<rank);

            if(rank==14)
                curr[suit]|=(1<<1);

            process(curr);
            total++;
        }
    }

    else if(need==2){

        for(int x=0;x<52;x++){

            int suit1=x/13;
            int rank1=x%13+1;

            if(!(deck[suit1]&(1<<rank1)))
                continue;

            for(int y=x+1;y<52;y++){

                int suit2=y/13;
                int rank2=y%13+1;

                if(!(deck[suit2]&(1<<rank2)))
                    continue;

                Hand curr=board;

                curr[suit1]|=(1<<rank1);
                curr[suit2]|=(1<<rank2);

                if(rank1==14)
                    curr[suit1]|=(1<<1);

                if(rank2==14)
                    curr[suit2]|=(1<<1);

                process(curr);
                total++;
            }
        }
    }

    for(auto& e:equity)
        e=100.0*e/total;

    return equity;
}













 
/*==================================================================================
==================================================================================*/

int main(){
    ios_base::sync_with_stdio(0);
    cin.tie(0);
    cout.tie(0);

    int t;
    cin>>t;

    while(t--){

        int n,m;
        cin>>n>>m;

        vector<Hand> players;

        for(int i=0;i<n;i++){
            vector<string> cards(2);

            cin>>cards[0]>>cards[1];

            players.push_back(hand_generator(cards));
        }

        vector<string> boardCards(m);

        for(int i=0;i<m;i++)
            cin>>boardCards[i];

        Hand board = hand_generator(boardCards);

        vector<double> equity = multiway_equity(players, board, m<3);

        cout<<fixed<<setprecision(3);

        for(double x:equity)
            cout<<x<<" ";

        cout<<endl;
    }

    return 0;
}
