#include <iostream>

using namespace std;

const int boardHeight=6;
const int boardWidth=7;
const int winningThreshold=4;
const char player1='X',player2='O';

bool isCyclic;
bool againstAI;

char board[boardHeight][boardWidth];
int currentVacancy[boardWidth];

void initialize(){
	isCyclic=false;
	againstAI=false;
	for(int i=0;i<boardWidth;++i){
		currentVacancy[i]=boardHeight-1;
		for(int j=0;j<boardHeight;++j)
			board[j][i]='.';
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
	if(isCyclic&&(left+1)<winningThreshold){
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
	if(isCyclic&&(right+1)<winningThreshold){
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
	if(isCyclic&&(topLeft+1)<winningThreshold){
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
	if(isCyclic&&(bottomRight+1)<winningThreshold){
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
	if(isCyclic&&(topRight+1)<winningThreshold){
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
	if(isCyclic&&(bottomLeft+1)<winningThreshold){
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
			cout<<"Player "<<player<<"'s turn! Type the column number to insert a piece."<<endl;
			columnNumber=validatePlayerInput()-1;
			board[currentVacancy[columnNumber]--][columnNumber]=player;
			display();
			alignmentNumber=alignment(player,currentVacancy[columnNumber]+1,columnNumber);
			//cout<<alignmentNumber<<endl;
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