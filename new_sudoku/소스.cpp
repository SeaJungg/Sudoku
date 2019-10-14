#include <iostream>
using namespace std;

struct cell{
	int xLocation = 0;
	int yLocation = 0;
	int boxLocation = 0;
	int sol = 0;
	int candidate[9] = { 1,1,1,1,1,1,1,1,1 };
	int check = 0;
};

struct Stackinfo {
	int hint;
	int Location[2];
};

void fill_sudoku_pannel(cell sudoku[9][9], int row, int col, int sol); //주어진 솔루션으로 스도쿠 틀을 만듭니다.
void fill_stack(int &top, cell sudoku[9][9], Stackinfo stack[81], int row, int col); // 주어진 솔루션이나 찾은 솔루션을 힌트 스택에 추가합니다.
void printCurrentPannel(cell sudoku[9][9]); //현재까지 채워진 스도쿠 판을 출력합니다.

void crossOut(cell sudoku[9][9], Stackinfo stack[81], int &top); // 힌트 스택을 기반으로 Candidate를 삭제하는 함수를 호출하는 함수입니다.
void cross_RowCol(cell sudoku[9][9], Stackinfo stack[81], int top); //열과 행에서 에서 Candidate를 삭제합니다.
void cross_Box(cell sudoku[9][9], Stackinfo stack[81], int top); //박스에서 Candidate를 삭제합니다.
void count_after_cross(cell sudoku[9][9], Stackinfo stack[81], int i, int j, int &top, bool &isThere); //각 셀에서 남아 있는 Candidate의 개수를 확인하고 Naked Single이라면 힌트 스택에 추가합니다.
void NakedSingle(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere); //Naked Single을 찾는 상기 함수들을 호출하는 함수입니다.

void crossRowForHidden(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere, int row, bool &isThereHidden); //열에서 Hidden Single을 찾는 함수입니다.
void crossColForHidden(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere, int col, bool &isThereHidden); //행에서 Hidden Single을 찾는 함수입니다.
void crossBoxForHidden(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere, int box, bool &isThereHidden); //박스에서 Hidden Single을 찾는 함수입니다.
void HiddenSingle(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere); //Hidden Single을 찾는 상기 함수들을 호출하는 함수입니다.


void crossOut(cell sudoku[9][9], Stackinfo stack[81], int &top)
{
	int count = 1;
	bool isThere = false;
	do 
	{
		isThere = false;
		NakedSingle(sudoku, stack, top, isThere);
		HiddenSingle(sudoku, stack, top, isThere);
	} while (isThere == true);
}

int main() {
	cell sudoku[9][9];
	Stackinfo stack[81];
	int top = 0;

	int input[9][9] = 
	{
		{0, 0, 0, 0, 8, 3, 0, 0, 0},
		{7, 0, 0, 0, 6, 0, 0, 9, 8},
		{9, 0, 8, 7, 0, 0, 0, 0, 0},
		{0, 0, 1, 3, 0, 0, 8, 0, 0},
		{5, 9, 0, 1, 7, 8, 0, 3, 4},
		{0, 0, 3, 0, 0, 2, 9, 0, 0},
		{0, 0, 0, 0, 0, 7, 4, 0, 2},
		{8, 2, 0, 0, 9, 0, 0, 0, 7},
		{0, 0, 0, 5, 2, 0, 0, 0, 0}
	};
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			fill_sudoku_pannel(sudoku, i, j, input[i][j]);
			fill_stack(top, sudoku, stack, i, j);
		}
	}
	printCurrentPannel(sudoku);
	cout << endl;
	crossOut(sudoku, stack, top);
	cout << "나는야 스도쿠 대장!" << endl;
	printCurrentPannel(sudoku);
	system("pause");
	return 0;
}
void NakedSingle(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere)
{
	cross_RowCol(sudoku, stack, top);
	cross_Box(sudoku, stack, top);
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++) 
		{
			count_after_cross(sudoku, stack, i, j, top, isThere);
		}
	}
}

void count_after_cross(cell sudoku[9][9], Stackinfo stack[81], int row, int col, int &top, bool &isThere)
{
	// 셀을 하나씩 가져와서 살아 있는 candidate가 있는지, 있다면 몇 개인지 확인합니다.
	int num = 9;
	int found = 10;
	for (int i = 0; i < 9; i++)
	{
		if (sudoku[row][col].candidate[i] == 0)
		{
			num--;
		}
		else
		{
			found = i; // 살아 있는 candidate가 혹시나 네이키드 싱글일 수도 있으니 그 위치를 저장해둡니다.
		}
	}
	if (num == 1 && found < 10)
	{
		sudoku[row][col].sol = found + 1;
		fill_stack(top, sudoku, stack, row, col);
		isThere = true;
	}
}
void cross_RowCol(cell sudoku[9][9], Stackinfo stack[81], int top)
{
	int row, col, hint;
	for (int i = 0; i < top; i++) {
		hint = stack[i].hint; //예를 들어 1행 5열에 있는 8을 가져왔을 때,
		row = stack[i].Location[0]; 
		col = stack[i].Location[1];
		for (int j = 0; j < 9; j++) {
			sudoku[row][col].candidate[j] = 0; // 힌트로 주어진 위치(1행 5열의 셀)는 모든 candidate를 0으로 변경 
			sudoku[row][j].candidate[hint - 1] = 0; // 1행의 모든 candidate에서 8을 삭제
			sudoku[j][col].candidate[hint - 1] = 0; // 5열의 모든 candidate에서 8을 삭제
		}
	}
}
void cross_Box(cell sudoku[9][9], Stackinfo stack[81], int top)
{
	int row, col, hint;
	for (int i = 0; i < top; i++) {
		hint = stack[i].hint; //예를 들어 1행 5열에 있는 8을 가져왔을 때,
		row = stack[i].Location[0];
		col = stack[i].Location[1];
		if (sudoku[row][col].boxLocation == 1) { //그 셀이 속한 박스를 찾기 위해 sudoku에 진입
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 3; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 2) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 3; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 3) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 3; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 4) {
			for (int j = 0; j < 3; j++) {
				for (int k = 3; k < 6; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 5) {
			for (int j = 3; j < 6; j++) {
				for (int k = 3; k < 6; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 6) {
			for (int j = 6; j < 9; j++) {
				for (int k = 3; k < 6; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 7) {
			for (int j = 0; j < 3; j++) {
				for (int k = 6; k < 9; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 8) {
			for (int j = 3; j < 6; j++) {
				for (int k = 6; k < 9; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
		else if (sudoku[row][col].boxLocation == 9) {
			for (int j = 6; j < 9; j++) {
				for (int k = 6; k < 9; k++) {
					sudoku[j][k].candidate[hint - 1] = 0;
				}
			}
		}
	}
}
void fill_sudoku_pannel(cell sudoku[9][9], int row, int col, int sol)
{
	sudoku[row][col].xLocation = row;
	sudoku[row][col].xLocation = col;
	sudoku[row][col].sol = sol;
	if (row >= 0 && row < 3 && col >= 0 && col < 3) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				sudoku[row][col].boxLocation = 1;
			}
		}
	}
	else if (row >= 3 && row < 6 && col >= 0 && col < 3) {
		for (int i = 3; i < 6; i++) {
			for (int j = 0; j < 3; j++) {
				sudoku[row][col].boxLocation = 2;
			}
		}
	}
	else if (row >= 6 && row < 9 && col >= 0 && col < 3) {
		for (int i = 6; i < 9; i++) {
			for (int j = 0; j < 3; j++) {
				sudoku[row][col].boxLocation = 3;
			}
		}
	}
	else if (row >= 0 && row < 3 && col >= 3 && col < 6) {
		for (int i = 0; i < 3; i++) {
			for (int j = 3; j < 6; j++) {
				sudoku[row][col].boxLocation = 4;
			}
		}
	}
	else if (row >= 3 && row < 6 && col >= 3 && col < 6) {
		for (int i = 3; i < 6; i++) {
			for (int j = 3; j < 6; j++) {
				sudoku[row][col].boxLocation = 5;
			}
		}
	}
	else if (row >= 6 && row < 9 && col >= 3 && col < 6) {
		for (int i = 6; i < 9; i++) {
			for (int j = 3; j < 6; j++) {
				sudoku[row][col].boxLocation = 6;
			}
		}
	}
	else if (row >= 0 && row < 3 && col >= 6 && col < 9) {
		for (int i = 0; i < 3; i++) {
			for (int j = 6; j < 9; j++) {
				sudoku[row][col].boxLocation = 7;
			}
		}
	}
	else if (row >= 3 && row < 6 && col >= 6 && col < 9) {
		for (int i = 3; i < 6; i++) {
			for (int j = 6; j < 9; j++) {
				sudoku[row][col].boxLocation = 8;
			}
		}
	}
	else if (row >= 6 && row < 9 && col >= 6 && col < 9) {
		for (int i = 6; i < 9; i++) {
			for (int j = 6; j < 9; j++) {
				sudoku[row][col].boxLocation = 9;
			}
		}
	}
}
void fill_stack(int &top, cell sudoku[9][9], Stackinfo stack[81], int row, int col)
{
	if (sudoku[row][col].sol != 0 && sudoku[row][col].check == 0)
	{
		stack[top].hint = sudoku[row][col].sol;
		stack[top].Location[0] = row;
		stack[top].Location[1] = col;
		top++;
		
		sudoku[row][col].check = 1;
	}
}

void printCurrentPannel(cell sudoku[9][9]) {
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			cout << sudoku[i][j].sol << " ";
			if (j == 2 || j == 5)
			{
				cout << " ";
			}
		}
		cout << endl;
		if (i == 2 || i == 5)
		{
			cout << endl;
		}
	}
	cout << endl;
}

void HiddenSingle(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere)
{
	bool isThereHidden = false;
	for (int i = 0; i < 9; i++)
	{
		if (isThereHidden == false)
		{
			crossRowForHidden(sudoku, stack, top, isThere, i, isThereHidden);
		}
	}
	for (int i = 0; i < 9; i++)
	{
		if (isThereHidden == false)
		{
			crossColForHidden(sudoku, stack, top, isThere, i, isThereHidden);
		}
	}
	for (int i = 0; i < 9; i++)
	{
		if (isThereHidden == false)
		{
			crossBoxForHidden(sudoku,stack,top,isThere,i,isThereHidden );

		}
	}

}
void crossRowForHidden(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere, int row, bool &isThereHidden)
{
	int sum[9] = { 0 };
	int num = 0;

	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (sudoku[row][i].candidate[j] == 1)
			{
				sum[j]++;
			}
		}
	}
	for (int i = 0; i < 9; i++)
	{
		if (sum[i] != 0)
		{
			num++;
		}
	}
	for (int i = 0; i < 9; i++)
	{
		for (int j = 0; j < 9; j++)
		{
			if (sum[j] == 1 && sudoku[row][i].candidate[j] == 1 && num > 2)
			{
				sudoku[row][i].sol = j + 1;
				fill_stack(top, sudoku, stack, row, i);
				isThere = true;
				isThereHidden = true;
			}
		}
	}
}

void crossColForHidden(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere, int col, bool &isThereHidden)
{
	int sum[9] = { 0 };
	int num = 0;
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (sudoku[i][col].candidate[j] == 1) {
				sum[j]++;
			}
		}
	}
	for (int i = 0; i < 9; i++) {
		if (sum[i] != 0) {
			num++;
		}
	}
	for (int i = 0; i < 9; i++) {
		for (int j = 0; j < 9; j++) {
			if (sum[j] == 1 && sudoku[i][col].candidate[j] == 1 && num > 2) {
				sudoku[i][col].sol = j + 1;
				fill_stack(top,sudoku,stack,i,col);
				isThere = true;
				isThereHidden = true;
			}
		}
	}
}

void crossBoxForHidden(cell sudoku[9][9], Stackinfo stack[81], int &top, bool &isThere, int box, bool &isThereHidden) 
{
	int sum[9] = { 0 };
	int num = 0;

	if (box == 1) {
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}

	else if (box == 2) {
		for (int i = 3; i < 6; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 3; i < 6; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 3) {
		for (int i = 6; i < 9; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 6; i < 9; i++) {
			for (int j = 0; j < 3; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 4) {
		for (int i = 0; i < 3; i++) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 5) {
		for (int i = 3; i < 6; i++) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 3; i < 6; i++) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 6) {
		for (int i = 6; i < 9; i++) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 6; i < 9; i++) {
			for (int j = 3; j < 6; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 7) {
		for (int i = 0; i < 3; i++) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 0; i < 3; i++) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 8) {
		for (int i = 3; i < 6; i++) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 3; i < 6; i++) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
	else if (box == 9) {
		for (int i = 6; i < 9; i++) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 9; k++) {
					if (sudoku[i][j].candidate[k] == 1) {
						sum[j]++;
					}
				}
			}
		}
		for (int i = 0; i < 9; i++) {
			if (sum[i] != 0) {
				num++;
			}
		}
		for (int i = 6; i < 9; i++) {
			for (int j = 6; j < 9; j++) {
				for (int k = 0; k < 9; k++) {
					if (sum[k] == 1 && sudoku[i][j].candidate[k] == 1 && num > 2) {
						sudoku[i][j].sol = k + 1;
						fill_stack(top, sudoku, stack, i, j);
						isThere = true;
						isThereHidden = true;
					}
				}
			}
		}
	}
}

