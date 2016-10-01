#include <iostream>
#include <cstdlib>
#include <ctime>

using namespace std;

const int boardHeight=6;
const int boardWidth=7;
const int winningThreshold=4;
const int planAhead=3;
const char player1='X',player2='O';

bool isCyclic;
bool againstAI;

char board[boardHeight][boardWidth];
char boardForAI[boardHeight][boardWidth];
int currentVacancy[boardWidth];
int currentVacancyForAI[boardWidth];

void initialize(){
	srand((int)time(0));
	isCyclic=true;
	againstAI=true;
	for(int i=0;i<boardWidth;++i){
		currentVacancy[i]=boardHeight-1;
		currentVacancyForAI[i]=boardHeight-1;
		for(int j=0;j<boardHeight;++j){
			board[j][i]='.';
			boardForAI[j][i]='.';
		}
	}

}

int validatePlayerInput(){
	int columnNumber;
	while(cin>>columnNumber){
		if(columnNumber<1||columnNumber>boardWidth)
			cout<<"Invalid column number. Please try again."<<endl;
		else if(currentVacancy[columnNumber-1]<0)
			cout<<"Column "<<columnNumber<<" is full. Choose another column."<<endl;
		else
			break;
	}
	return columnNumber;
}

int alignment(char piece,int rowNumber,int columnNumber){
	int left=0,right=0,top=0,bottom=0,topLeft=0,topRight=0,bottomLeft=0,bottomRight=0,i,j,max=0;

	i=columnNumber-1;
	while(i>=0&&(left+1)<winningThreshold&&piece==board[rowNumber][i]){
		++left;
		--i;
	}
	if(i<0&&isCyclic&&(left+1)<winningThreshold){
		i=boardWidth-1;
		while(i>=0&&(left+1)<winningThreshold&&piece==board[rowNumber][i]){
		++left;
		--i;
		}
	}
	i=columnNumber+1;
	while(i<boardWidth&&(right+1)<winningThreshold&&piece==board[rowNumber][i]){
		++right;
		++i;
	}
	if(i>=boardWidth&&isCyclic&&(right+1)<winningThreshold){
		i=0;
		while(i>=0&&(right+1)<winningThreshold&&piece==board[rowNumber][i]){
		++right;
		++i;
		}
	}
	max=left+right+1;

	i=rowNumber-1;
	while(i>=0&&(top+1)<winningThreshold&&piece==board[i][columnNumber]){
		++top;
		--i;
	}
	i=rowNumber+1;
	while(i<boardHeight&&(bottom+1)<winningThreshold&&piece==board[i][columnNumber]){
		++bottom;
		++i;
	}
	max=max>=(top+bottom+1)?max:top+bottom+1;

	i=rowNumber-1,j=columnNumber-1;
	while(i>=0&&j>=0&&(topLeft+1)<winningThreshold&&piece==board[i][j]){
		++topLeft;
		--i;
		--j;
	}
	if(j<0&&isCyclic&&(topLeft+1)<winningThreshold){
		j=boardWidth-1;
		while(i>=0&&j>=0&&(topLeft+1)<winningThreshold&&piece==board[i][j]){
		++topLeft;
		--i;
		--j;
		}
	}
	i=rowNumber+1,j=columnNumber+1;
	while(i<boardHeight&&j<boardWidth&&(bottomRight+1)<winningThreshold&&piece==board[i][j]){
		++bottomRight;
		++i;
		++j;
	}
	if(j>=boardWidth&&isCyclic&&(bottomRight+1)<winningThreshold){
		j=0;
		while(i<boardHeight&&j<boardWidth&&(bottomRight+1)<winningThreshold&&piece==board[i][j]){
			++bottomRight;
			++i;
			++j;
		}
	}
	max=max>=(topLeft+bottomRight+1)?max:topLeft+bottomRight+1;

	i=rowNumber-1,j=columnNumber+1;
	while(i>=0&&j<boardWidth&&(topRight+1)<winningThreshold&&piece==board[i][j]){
		++topRight;
		--i;
		++j;
	}
	if(j>=boardWidth&&isCyclic&&(topRight+1)<winningThreshold){
		j=0;
		while(i>=0&&j<boardWidth&&(topRight+1)<winningThreshold&&piece==board[i][j]){
		++topLeft;
		--i;
		++j;
		}
	}
	i=rowNumber+1,j=columnNumber-1;
	while(i<boardHeight&&j>=0&&(bottomLeft+1)<winningThreshold&&piece==board[i][j]){
		++bottomLeft;
		++i;
		--j;
	}
	if(j<0&&isCyclic&&(bottomLeft+1)<winningThreshold){
		j=boardWidth-1;
		while(i<boardHeight&&j>=0&&(bottomLeft+1)<winningThreshold&&piece==board[i][j]){
			++bottomLeft;
			++i;
			--j;
		}
	}
	max=max>=(topRight+bottomLeft+1)?max:topRight+bottomLeft+1;

	return max;
}

bool isBoardFull(){
	for(int i=0;i<boardWidth;i++)
		if(currentVacancy[i]>=0) return false;
	return true;
}

void assessSubsequentMoves(char currentPlayer,char AI,int step,int scores[]){
	if(1==step){
		for(int i=0;i<boardWidth;i++){
			if(currentVacancy[i]>=0)
				scores[i]=alignment(currentPlayer,currentVacancy[i],i);	
			else
				scores[i]=-2;
		}
	/*	while(selectionUpperBound<boardWidth&&(scores[selectionUpperBound]>=winningThreshold||scores[selectionUpperBound]==maxScore))
			++selectionUpperBound;
		if(selectionUpperBound>1)
			return columns[rand()%selectionUpperBound];
		else
			return columns[0];*/
	}else if(currentPlayer==AI){
		int subsequentScores[boardWidth];
		for(int i=0;i<boardWidth;i++){
			if(currentVacancy[i]>=0){
				int score=alignment(currentPlayer,currentVacancy[i],i);
				if(score>=winningThreshold)
					scores[i]=score;
				else{
					board[currentVacancy[i]][i]=currentPlayer;
					--currentVacancy[i];
					if(isBoardFull())
						scores[i]=score;
					else{
						if('X'==currentPlayer)
							currentPlayer='O';
						else
							currentPlayer='X';
						assessSubsequentMoves(currentPlayer,AI,step-1,subsequentScores);
						int maxScore=subsequentScores[0];
						bool lose=false;
						for(int j=0;j<boardWidth;j++){
							if(-1==subsequentScores[j])
								lose=true;
							if(maxScore<subsequentScores[j])
								maxScore=subsequentScores[j];
						}
						if(lose)
							scores[i]=-1;
						else if(0==maxScore)
							scores[i]=score;
						else
							scores[i]=maxScore;
					}
					++currentVacancy[i];
					board[currentVacancy[i]][i]='.';
				}
			}else
				scores[i]=-2;
		}
	}else{
		int subsequentScores[boardWidth];
		for(int i=0;i<boardWidth;i++){
			if(currentVacancy[i]>=0){
				int score=alignment(currentPlayer,currentVacancy[i],i);
				if(score>=winningThreshold)
					scores[i]=-1;
				else {
					board[currentVacancy[i]][i]=currentPlayer;
					--currentVacancy[i];
					if(isBoardFull())
						scores[i]=0;
					else{
						if('X'==currentPlayer)
							currentPlayer='O';
						else
							currentPlayer='X';
						assessSubsequentMoves(currentPlayer,AI,step-1,subsequentScores);
						int maxScore=subsequentScores[0];
						bool lose=false;
						for(int j=0;j<boardWidth;j++){
							if(-1==subsequentScores[j])
								lose=true;
							if(maxScore<subsequentScores[j])
								maxScore=subsequentScores[j];
						}
						if(lose)
							scores[i]=-1;
						else
							scores[i]=maxScore;
					}
					++currentVacancy[i];
					board[currentVacancy[i]][i]='.';
				}
			}else
				scores[i]=-2;
		}
	}
}

int AINextMove(char currentPlayer,char AI,int step){
	int scores[boardWidth],columns[boardWidth],maxScore,maxScoreColumn,selectionUpperBound=1;
	assessSubsequentMoves(currentPlayer,AI,step,scores);
	for(int i=0;i<boardWidth;i++)
		columns[i]=i;
	for(int i=0;i<boardWidth-1;i++){
		int maxIndex=i;
		for(int j=i;j<boardWidth;j++)
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
	while(selectionUpperBound<boardWidth){
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
	for(i=1;i<=boardWidth;++i)
		cout<<i;
	cout<<endl;
	for(i=0;i<boardHeight;++i){
		for(j=0;j<boardWidth;++j)
			cout<<board[i][j];
		cout<<endl;
	}
}

int main(){
	char response,player;
	int columnNumber,alignmentNumber;

	do{
		initialize();
		player='X';
		display();
		do{
			if(againstAI&&'O'==player){
				cout<<"Player "<<player<<"'s turn!"<<endl;
				columnNumber=AINextMove(player,'O',planAhead);
				cout<<"The piece has been placed in column "<<columnNumber+1<<endl;
			}else{
				cout<<"Player "<<player<<"'s turn! Type the column number to insert a piece."<<endl;
				columnNumber=validatePlayerInput()-1;
			}
			board[currentVacancy[columnNumber]--][columnNumber]=player;
			display();
			alignmentNumber=alignment(player,currentVacancy[columnNumber]+1,columnNumber);
			if(alignmentNumber>=winningThreshold){
				cout<<"Player "<<player<<" won!"<<endl;
				break;
			} else if(isBoardFull()){
				cout<<"The board is full. The game is a draw."<<endl;
				break;
			} else{
				if('X'==player)
					player='O';
				else 
					player='X';
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