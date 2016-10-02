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

bool isCyclic;
bool againstAI;

char board[BOARD_HEIGHT][BOARD_WIDTH];
char boardForAI[BOARD_HEIGHT][BOARD_WIDTH];
int currentVacancy[BOARD_WIDTH];
int currentVacancyForAI[BOARD_WIDTH];

void initialize(){
	srand((int)time(0));
	char response;
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
	for(int i=0;i<BOARD_WIDTH;++i){
		currentVacancy[i]=BOARD_HEIGHT-1;
		currentVacancyForAI[i]=BOARD_HEIGHT-1;
		for(int j=0;j<BOARD_HEIGHT;++j){
			board[j][i]=EMPTY_SLOT;
			boardForAI[j][i]=EMPTY_SLOT;
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

int alignment(char piece,int rowNumber,int columnNumber){
	int left=0,right=0,bottom=0,topLeft=0,topRight=0,bottomLeft=0,bottomRight=0,i,j,max=0;

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

	i=rowNumber+1;
	while(i<BOARD_HEIGHT&&(bottom+1)<WINNING_THRESHOLD&&piece==board[i][columnNumber]){
		++bottom;
		++i;
	}
	max=max>=(bottom+1)?max:bottom+1;

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

	return max;
}

bool isBoardFull(){
	for(int i=0;i<BOARD_WIDTH;i++)
		if(currentVacancy[i]>=0) return false;
	return true;
}

void assessSubsequentMoves(char currentPlayer,char AI,int step,int scores[]){
	char nextPlayer;
	if(1==step&&currentPlayer==AI){
		for(int i=0;i<BOARD_WIDTH;i++){
			if(currentVacancy[i]>=0)
				scores[i]=alignment(currentPlayer,currentVacancy[i],i);	
			else
				scores[i]=FULL_COLUMN;
		}
	}else if(currentPlayer==AI){
		int subsequentScores[BOARD_WIDTH];
		for(int i=0;i<BOARD_WIDTH;i++){
			if(currentVacancy[i]>=0){
				int score=alignment(currentPlayer,currentVacancy[i],i);
				if(score>=WINNING_THRESHOLD)
					scores[i]=WINNING_THRESHOLD*step*step;
				else{
					board[currentVacancy[i]][i]=currentPlayer;
					--currentVacancy[i];
					if(isBoardFull())
						scores[i]=score*step*step;
					else{
						if(PLAYER1==currentPlayer)
							nextPlayer=PLAYER2;
						else
							nextPlayer=PLAYER1;
						assessSubsequentMoves(nextPlayer,AI,step,subsequentScores);
						int maxScore=subsequentScores[0],losePenalty=0;
						for(int j=0;j<BOARD_WIDTH;j++){
							if(0>subsequentScores[j]&&FULL_COLUMN<subsequentScores[j]&&losePenalty>subsequentScores[j])
								losePenalty=subsequentScores[j];
							if(maxScore<subsequentScores[j])
								maxScore=subsequentScores[j];
						}
						if(maxScore<=score)
							scores[i]=score*step*step;
						else
							scores[i]=maxScore;
						if(losePenalty<0)
							scores[i]+=losePenalty;
					}
					++currentVacancy[i];
					board[currentVacancy[i]][i]=EMPTY_SLOT;
				}
			}else
				scores[i]=FULL_COLUMN;
		}
	}else{
		int subsequentScores[BOARD_WIDTH];
		for(int i=0;i<BOARD_WIDTH;i++){
			if(currentVacancy[i]>=0){
				int score=alignment(currentPlayer,currentVacancy[i],i);
				if(score>=WINNING_THRESHOLD)
					scores[i]=-PENALTY_FACTOR*step*step;
				else {
					board[currentVacancy[i]][i]=currentPlayer;
					--currentVacancy[i];
					if(isBoardFull())
						scores[i]=0;
					else{
						if(PLAYER1==currentPlayer)
							nextPlayer=PLAYER2;
						else
							nextPlayer=PLAYER1;
						assessSubsequentMoves(nextPlayer,AI,step-1,subsequentScores);
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

int AINextMove(char currentPlayer,char AI,int step){
	int scores[BOARD_WIDTH],columns[BOARD_WIDTH],maxScore,maxScoreColumn,selectionUpperBound=1;
	assessSubsequentMoves(currentPlayer,AI,step,scores);
	for(int i=0;i<BOARD_WIDTH;i++)
		columns[i]=i;
	for(int i=0;i<BOARD_WIDTH-1;i++){
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
	while(selectionUpperBound<BOARD_WIDTH){
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

	do{
		initialize();
		player=PLAYER1;
		display();
		do{
			if(againstAI&&PLAYER2==player){
				cout<<"Player "<<player<<"'s turn!"<<endl;
				columnNumber=AINextMove(player,PLAYER2,PLAN_AHEAD);
				cout<<"The piece has been placed in column "<<columnNumber+1<<endl;
			}else{
				cout<<"Player "<<player<<"'s turn! Type the column number to insert a piece."<<endl;
				columnNumber=validatePlayerInput()-1;
			}
			board[currentVacancy[columnNumber]--][columnNumber]=player;
			display();
			alignmentNumber=alignment(player,currentVacancy[columnNumber]+1,columnNumber);
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