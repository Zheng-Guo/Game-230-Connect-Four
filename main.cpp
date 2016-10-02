#include <iostream>
#include <cstdlib>
#include <ctime>
#include <limits> 

using namespace std;

const int BOARD_HEIGHT=6;
const int BOARD_WIDTH=7;
const int WINNING_THRESHOLD=4;
const int PLAN_AHEAD=3;
const char EMPTY_SLOT='.';
const char PLAYER1='X',PLAYER2='O';
const int FULL_COLUMN=numeric_limits<int>::min();
const int PENALTY_FACTOR=4;

char board[BOARD_HEIGHT][BOARD_WIDTH];
int currentVacancy[BOARD_WIDTH];

void initialize(){
	srand((int)time(0));
	for(int i=0;i<BOARD_WIDTH;++i){
		currentVacancy[i]=BOARD_HEIGHT-1;
		for(int j=0;j<BOARD_HEIGHT;++j){
			board[j][i]=EMPTY_SLOT;
		}
	}

}

int validatePlayerInput(){
	int columnNumber;
	while(cin>>columnNumber){
		if(columnNumber<1||columnNumber>BOARD_WIDTH)
			cout<<"Invalid column number. Please try again."<<endl;
		else if(currentVacancy[columnNumber-1]<0)
			cout<<"Column "<<columnNumber<<" is full. Choose another column."<<endl;
		else
			break;
	}
	return columnNumber;
}

int alignment(char piece,int rowNumber,int columnNumber,bool isCyclic){
	int left=0,right=0,bottom=0,topLeft=0,topRight=0,bottomLeft=0,bottomRight=0,i,j,max=0;

	//Count the number of player pieces aligned horizontally with the new piece
	i=columnNumber-1;
	while(i>=0&&(left+1)<WINNING_THRESHOLD&&piece==board[rowNumber][i]){
		++left;
		--i;
	}
	if(i<0&&isCyclic&&(left+1)<WINNING_THRESHOLD){
		i=BOARD_WIDTH-1;
		while(i>=0&&(left+1)<WINNING_THRESHOLD&&piece==board[rowNumber][i]){
		++left;
		--i;
		}
	}
	i=columnNumber+1;
	while(i<BOARD_WIDTH&&(right+1)<WINNING_THRESHOLD&&piece==board[rowNumber][i]){
		++right;
		++i;
	}
	if(i>=BOARD_WIDTH&&isCyclic&&(right+1)<WINNING_THRESHOLD){
		i=0;
		while(i>=0&&(right+1)<WINNING_THRESHOLD&&piece==board[rowNumber][i]){
		++right;
		++i;
		}
	}
	max=left+right+1;

	//Count the number of player pieces aligned vertically with the new piece
	i=rowNumber+1;
	while(i<BOARD_HEIGHT&&(bottom+1)<WINNING_THRESHOLD&&piece==board[i][columnNumber]){
		++bottom;
		++i;
	}
	max=max>=(bottom+1)?max:bottom+1;

	//Count the number of player pieces aligned diagonally with the new piece
	i=rowNumber-1,j=columnNumber-1;
	while(i>=0&&j>=0&&(topLeft+1)<WINNING_THRESHOLD&&piece==board[i][j]){
		++topLeft;
		--i;
		--j;
	}
	if(j<0&&isCyclic&&(topLeft+1)<WINNING_THRESHOLD){
		j=BOARD_WIDTH-1;
		while(i>=0&&j>=0&&(topLeft+1)<WINNING_THRESHOLD&&piece==board[i][j]){
		++topLeft;
		--i;
		--j;
		}
	}
	i=rowNumber+1,j=columnNumber+1;
	while(i<BOARD_HEIGHT&&j<BOARD_WIDTH&&(bottomRight+1)<WINNING_THRESHOLD&&piece==board[i][j]){
		++bottomRight;
		++i;
		++j;
	}
	if(j>=BOARD_WIDTH&&isCyclic&&(bottomRight+1)<WINNING_THRESHOLD){
		j=0;
		while(i<BOARD_HEIGHT&&j<BOARD_WIDTH&&(bottomRight+1)<WINNING_THRESHOLD&&piece==board[i][j]){
			++bottomRight;
			++i;
			++j;
		}
	}
	max=max>=(topLeft+bottomRight+1)?max:topLeft+bottomRight+1;

	i=rowNumber-1,j=columnNumber+1;
	while(i>=0&&j<BOARD_WIDTH&&(topRight+1)<WINNING_THRESHOLD&&piece==board[i][j]){
		++topRight;
		--i;
		++j;
	}
	if(j>=BOARD_WIDTH&&isCyclic&&(topRight+1)<WINNING_THRESHOLD){
		j=0;
		while(i>=0&&j<BOARD_WIDTH&&(topRight+1)<WINNING_THRESHOLD&&piece==board[i][j]){
		++topLeft;
		--i;
		++j;
		}
	}
	i=rowNumber+1,j=columnNumber-1;
	while(i<BOARD_HEIGHT&&j>=0&&(bottomLeft+1)<WINNING_THRESHOLD&&piece==board[i][j]){
		++bottomLeft;
		++i;
		--j;
	}
	if(j<0&&isCyclic&&(bottomLeft+1)<WINNING_THRESHOLD){
		j=BOARD_WIDTH-1;
		while(i<BOARD_HEIGHT&&j>=0&&(bottomLeft+1)<WINNING_THRESHOLD&&piece==board[i][j]){
			++bottomLeft;
			++i;
			--j;
		}
	}
	max=max>=(topRight+bottomLeft+1)?max:topRight+bottomLeft+1;
	//The longest alignment among four directions is returned
	return max;
}

bool isBoardFull(){
	for(int i=0;i<BOARD_WIDTH;i++)
		if(currentVacancy[i]>=0) return false;
	return true;
}

//Examine all possible moves with a given number of steps.
//Both the length of alignment and the number of steps needed are considered.
//The fewer the steps needed to produce a long alignment, the higher the score(so the square of remaining step number is multiplied to the score to emphasize its significance).
//If human player could win at some point, a penalty is deducted from the score to be returned, making the previous AI move less favourable. The fewer steps needed for the human player to win, the higher the penalty will be.
void assessSubsequentMoves(char currentPlayer,char AI,int step,int scores[],bool isCyclic){
	char nextPlayer;
	
	if(1==step&&currentPlayer==AI){//Consider current step only
		for(int i=0;i<BOARD_WIDTH;i++){
			if(currentVacancy[i]>=0)
				scores[i]=alignment(currentPlayer,currentVacancy[i],i,isCyclic);	
			else
				scores[i]=FULL_COLUMN;
		}
	}else if(currentPlayer==AI){//If AI is to make a move and consider sebsequent steps afterwards
		int subsequentScores[BOARD_WIDTH];
		for(int i=0;i<BOARD_WIDTH;i++){
			if(currentVacancy[i]>=0){
				int score=alignment(currentPlayer,currentVacancy[i],i,isCyclic);
				if(score>=WINNING_THRESHOLD)
					scores[i]=WINNING_THRESHOLD*step*step;
				else{
					board[currentVacancy[i]][i]=currentPlayer;//The board is marked with an AI piece before considering subsequent moves
					--currentVacancy[i];
					if(isBoardFull())
						scores[i]=score*step*step;
					else{
						if(PLAYER1==currentPlayer)//Switch player and examine possible human player moves
							nextPlayer=PLAYER2;
						else
							nextPlayer=PLAYER1;
						assessSubsequentMoves(nextPlayer,AI,step,subsequentScores,isCyclic);
						int maxScore=subsequentScores[0],losePenalty=0;
						for(int j=0;j<BOARD_WIDTH;j++){
							if(0>subsequentScores[j]&&FULL_COLUMN<subsequentScores[j]&&losePenalty>subsequentScores[j])//The highest penalty value will be used
								losePenalty=subsequentScores[j];
							if(maxScore<subsequentScores[j])
								maxScore=subsequentScores[j];
						}
						if(maxScore<=score)//Current score will be used if subsequent moves do not generate higher scores
							scores[i]=score*step*step;
						else
							scores[i]=maxScore;
						if(losePenalty<0)
							scores[i]+=losePenalty;
					}
					++currentVacancy[i];//Unmark the board
					board[currentVacancy[i]][i]=EMPTY_SLOT;
				}
			}else
				scores[i]=FULL_COLUMN;
		}
	}else{
		int subsequentScores[BOARD_WIDTH];
		for(int i=0;i<BOARD_WIDTH;i++){
			if(currentVacancy[i]>=0){
				int score=alignment(currentPlayer,currentVacancy[i],i,isCyclic);
				if(score>=WINNING_THRESHOLD)//Penalty generated if human player could win with this move
					scores[i]=-PENALTY_FACTOR*step*step;
				else {
					board[currentVacancy[i]][i]=currentPlayer;//The board is marked with a human player piece before considering subsequent moves
					--currentVacancy[i];
					if(isBoardFull())
						scores[i]=0;
					else{
						if(PLAYER1==currentPlayer)
							nextPlayer=PLAYER2;
						else
							nextPlayer=PLAYER1;
						assessSubsequentMoves(nextPlayer,AI,step-1,subsequentScores,isCyclic);
						int maxScore=subsequentScores[0];
						for(int j=0;j<BOARD_WIDTH;j++){
							if(maxScore<subsequentScores[j])
								maxScore=subsequentScores[j];
						}
							scores[i]=maxScore;
					}
					++currentVacancy[i];
					board[currentVacancy[i]][i]=EMPTY_SLOT;
				}
			}else
				scores[i]=FULL_COLUMN;
		}
	}
}

//Find the most promising move within a given number of considerations.
//An AI move which produces longer alignment in shorter steps returns higher score.
//An AI move which leads to human player victory produces very low score, thus will be avoided by the AI.
//The column number to be returned is the move that most likely to create a long alignment as well as stop the human player from winning.
//Penalty is made severe to the score, so avoiding human player to win takes priority over AI's own victory.
int AINextMove(char currentPlayer,char AI,int step,bool isCyclic){
	int scores[BOARD_WIDTH],columns[BOARD_WIDTH],maxScore,maxScoreColumn,selectionUpperBound=1;
	assessSubsequentMoves(currentPlayer,AI,step,scores,isCyclic);
	for(int i=0;i<BOARD_WIDTH;i++)
		columns[i]=i;
	for(int i=0;i<BOARD_WIDTH-1;i++){//Sort the returned scores with insert sort.
		int maxIndex=i;
		for(int j=i;j<BOARD_WIDTH;j++)
			if(scores[maxIndex]<scores[j])
				maxIndex=j;
		int temp=scores[i];
		scores[i]=scores[maxIndex];
		scores[maxIndex]=temp;
		temp=columns[i];
		columns[i]=columns[maxIndex];
		columns[maxIndex]=temp;
	}
	maxScore=scores[0];
	while(selectionUpperBound<BOARD_WIDTH){//If max score occures more than once, one is chosen randomly
		if(scores[selectionUpperBound]<maxScore)
			break;
		else 
			++selectionUpperBound;
	}
	if(selectionUpperBound>1)
		return columns[rand()%selectionUpperBound];
	else
		return columns[0];
}

void display(){
	int i,j;
	for(i=1;i<=BOARD_WIDTH;++i)
		cout<<i;
	cout<<endl;
	for(i=0;i<BOARD_HEIGHT;++i){
		for(j=0;j<BOARD_WIDTH;++j)
			cout<<board[i][j];
		cout<<endl;
	}
}

int main(){
	char response,player;
	int columnNumber,alignmentNumber;
	bool isCyclic,againstAI;

	do{
		initialize();
		player=PLAYER1;
		cout<<"Do you want to play on cyclic board(the left side of the board is connected to the right side)?"<<endl;
		cin>>response;
		while(response!='y'&&response!='Y'&&response!='n'&&response!='N'){
			cout<<"Invalid response. Please try again."<<endl;
			cin>>response;
		}
		isCyclic=('y'==response||'Y'==response);
		cout<<"Do you want single player mode(against AI)?"<<endl;
		cin>>response;
		while(response!='y'&&response!='Y'&&response!='n'&&response!='N'){
			cout<<"Invalid response. Please try again."<<endl;
			cin>>response;
		}
		againstAI=('y'==response||'Y'==response);
		display();
		do{
			if(againstAI&&PLAYER2==player){
				cout<<"Player "<<player<<"'s turn!"<<endl;
				columnNumber=AINextMove(player,PLAYER2,PLAN_AHEAD,isCyclic);
				cout<<"The piece has been placed in column "<<columnNumber+1<<endl;
			}else{
				cout<<"Player "<<player<<"'s turn! Type the column number to insert a piece."<<endl;
				columnNumber=validatePlayerInput()-1;
			}
			board[currentVacancy[columnNumber]--][columnNumber]=player;
			display();
			alignmentNumber=alignment(player,currentVacancy[columnNumber]+1,columnNumber,isCyclic);
			if(alignmentNumber>=WINNING_THRESHOLD){
				cout<<"Player "<<player<<" won!"<<endl;
				break;
			} else if(isBoardFull()){
				cout<<"The board is full. The game is a draw."<<endl;
				break;
			} else{
				if(PLAYER1==player)
					player=PLAYER2;
				else 
					player=PLAYER1;
			}
		}while(true);

		cout<<"Do you want to play another game?"<<endl;
		cin>>response;
		while(response!='y'&&response!='Y'&&response!='n'&&response!='N'){
			cout<<"Invalid response. Please try again."<<endl;
			cin>>response;
		}
	}while('y'==response||'Y'==response);
	return 0;
}