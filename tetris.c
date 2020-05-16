#include "tetris.h"

static struct sigaction act, oact;

int main(){
	int exit=0;

	initscr();
	noecho();
	keypad(stdscr, TRUE);	

	srand((unsigned int)time(NULL));
    createRankList();
	while(!exit){
		clear();
		switch(menu()){
			case MENU_PLAY: play(); break;
			case MENU_EXIT: exit=1; break;
            case MENU_RANK: rank(); break;
            case MENU_REAI: recommendedPlay(); break;
			default: break;
		}
	}
    writeRankFile();
	endwin();
	system("clear");
	return 0;
}

void InitTetris(){
	int i,j;

	for(j=0;j<HEIGHT;j++)
		for(i=0;i<WIDTH;i++)
			field[j][i]=0;
    for(int i = 0;i<BLOCK_NUM;i++){
	    nextBlock[i]=rand()%7;
    }
	blockRotate=0;
	blockY=-1;
	blockX=WIDTH/2-2;
	score=0;
    blockcount = 0;
	gameOver=0;
	timed_out=0;

    root.score = 0;
    root.curBlockID = 0;
    root.recBlockX = 0;
    root.recBlockY = 0;
    root.recBlockRotate = 0;
    recflag = 0;

	DrawOutline();
	DrawField();
	DrawBlockWithFeatures(blockY,blockX,nextBlock[0],blockRotate);
	DrawNextBlock(nextBlock);
	PrintScore(score);
}

void DrawOutline(){	
	int i,j;
	/* 블럭이 떨어지는 공간의 태두리를 그린다.*/
	DrawBox(0,0,HEIGHT,WIDTH);

	/* next block을 보여주는 공간의 태두리를 그린다.*/
	move(2,WIDTH+10);
	printw("NEXT BLOCK");
	DrawBox(3,WIDTH+10,4,8);
	DrawBox(9,WIDTH+10,4,8);
	/* score를 보여주는 공간의 태두리를 그린다.*/
	move(16,WIDTH+10);
	printw("SCORE");
	DrawBox(17,WIDTH+10,1,8);
}

int GetCommand(){
	int command;
	command = wgetch(stdscr);
	switch(command){
		case KEY_UP:
			break;
		case KEY_DOWN:
			break;
		case KEY_LEFT:
			break;
		case KEY_RIGHT:
			break;
		case ' ':	/* space key*/
			/*fall block*/
            while(CheckToMove(field,nextBlock[0],blockRotate,++blockY,blockX));
            blockY--;
            timed_out = 1;
            alarm(0);
            BlockDown(0);
			break;
		case 'q':
		case 'Q':
			command = QUIT;
			break;
		default:
			command = NOTHING;
			break;
	}
	return command;
}

int ProcessCommand(int command){
	int ret=1;
	int drawFlag=0;
    int flag=0;
	switch(command){
		case QUIT:
			ret = QUIT;
			break;
		case KEY_UP:
			if((drawFlag = CheckToMove(field,nextBlock[0],(blockRotate+1)%4,blockY,blockX)))
				blockRotate=(blockRotate+1)%4;
			break;
		case KEY_DOWN:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX)))
				blockY++;
			break;
		case KEY_RIGHT:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX+1)))
				blockX++;
			break;
		case KEY_LEFT:
			if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY,blockX-1)))
				blockX--;
			break;
        default:
			break;
	}
	if(drawFlag) DrawChange(field,command,nextBlock[0],blockRotate,blockY,blockX);
	return ret;	
}

void DrawField(){
	int i,j;
	for(j=0;j<HEIGHT;j++){
		move(j+1,1);
		for(i=0;i<WIDTH;i++){
			if(field[j][i]==1){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(".");
		}
	}
}


void PrintScore(int score){
	move(18,WIDTH+11);
	printw("%8d",score);
}

void DrawNextBlock(int *nextBlock){
	int i, j;
	for( i = 0; i < 4; i++ ){
		move(4+i,WIDTH+13);
		for( j = 0; j < 4; j++ ){
			if( block[nextBlock[1]][0][i][j] == 1 ){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else printw(" ");
		}
		move(10+i,WIDTH+13);
		for(j = 0;j<4;j++){
			if(block[nextBlock[2]][0][i][j]){
				attron(A_REVERSE);
				printw(" ");
				attroff(A_REVERSE);
			}
			else
				printw(" ");
		}

	}
}

void DrawBlock(int y, int x, int blockID,int blockRotate,char tile){
	int i,j;
	for(i=0;i<4;i++)
		for(j=0;j<4;j++){
			if(block[blockID][blockRotate][i][j]==1 && i+y>=0){
				move(i+y+1,j+x+1);
				attron(A_REVERSE);
				printw("%c",tile);
				attroff(A_REVERSE);
			}
		}

	move(HEIGHT,WIDTH+10);
}

void DrawBox(int y,int x, int height, int width){
	int i,j;
	move(y,x);
	addch(ACS_ULCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_URCORNER);
	for(j=0;j<height;j++){
		move(y+j+1,x);
		addch(ACS_VLINE);
		move(y+j+1,x+width+1);
		addch(ACS_VLINE);
	}
	move(y+j+1,x);
	addch(ACS_LLCORNER);
	for(i=0;i<width;i++)
		addch(ACS_HLINE);
	addch(ACS_LRCORNER);
}

void play(){
	int command;
	clear();
	act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
    timed_out = 0;
	do{
		if(timed_out==0){
			alarm(1);
			timed_out=1;
		}

		command = GetCommand();
		if(ProcessCommand(command)==QUIT){
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();

			return;
		}
	}while(!gameOver);

	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
	getch();
	newRank(score);
}

char menu(){
	printw("1. play\n");
	printw("2. rank\n");
	printw("3. recommended play\n");
	printw("4. exit\n");
	return wgetch(stdscr);
}

int CheckToMove(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int check = 0;
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j])
				if(i+blockY>=HEIGHT || i+blockY<0 || j+blockX>=WIDTH || j+blockX<0 ||1 == f[i+blockY][j+blockX]){
					check = 1;
					break;
				}
		}
		if(check)
			break;

	}
	if(check)
		return 0;
	else
		return 1;
}
void DrawChange(char f[HEIGHT][WIDTH],int command,int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int pblockRotate = blockRotate;
	int pblockY = blockY;
	int pblockX = blockX;
	int shadowY;
	switch(command){
		case KEY_UP:
			pblockRotate = (pblockRotate+3)%4;
			break;
		case KEY_DOWN:
			pblockY--;
			break;
		case KEY_RIGHT:
			pblockX--;
			break;
		case KEY_LEFT:
			pblockX++;
			break;
		default:
			break;
	}
	shadowY = pblockY;
	while(CheckToMove(field,currentBlock,pblockRotate,++shadowY,pblockX));
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			if(block[currentBlock][pblockRotate][i][j]){
				if(i+pblockY>=0){
					move(i+pblockY+1,j+pblockX+1);
					printw(".");
				}
				if(i+shadowY>=0){
					move(i+shadowY,j+pblockX+1);
					printw(".");
				}
			}
		}
	}
	DrawBlockWithFeatures(blockY,blockX,currentBlock,blockRotate);
    DrawRecommend(&root,blockY, blockX, nextBlock[0],blockRotate);
	move(HEIGHT,WIDTH+10);
}

void BlockDown(int sig){
	// user code
	int drawFlag=0;
	int i;
	if((drawFlag = CheckToMove(field,nextBlock[0],blockRotate,blockY+1,blockX))){
		blockY++;
		DrawChange(field,KEY_DOWN,nextBlock[0],blockRotate,blockY,blockX);
        DrawRecommend(&root,blockY, blockX, nextBlock[0],blockRotate);
	}
	else{
		if(blockY==-1) gameOver=1;
		score+=AddBlockToField(field,nextBlock[0],blockRotate,blockY,blockX);
		score+=DeleteLine(field);
        for(int i = 0;i<BLOCK_NUM-1;i++){
		    nextBlock[i] = nextBlock[i+1];
        }
		nextBlock[BLOCK_NUM-1] = rand()%7;			
		blockRotate=0;
		blockY=-1;
		blockX=WIDTH/2-2;
        blockcount++;
        
        recflag = 0;
        DrawRecommend(&root,blockY, blockX, nextBlock[0],blockRotate);
		
        DrawNextBlock(nextBlock);
		PrintScore(score);
		DrawField();
	}
	timed_out = 0;

}

int AddBlockToField(char f[HEIGHT][WIDTH],int currentBlock,int blockRotate, int blockY, int blockX){
	// user code
	int touched = 0;
	for(int i = 0;i<4;i++){
		for(int j = 0;j<4;j++){
			if(block[currentBlock][blockRotate][i][j]){
				if(i+blockY+1==HEIGHT || f[i+blockY+1][j+blockX])
					touched++;
				f[i+blockY][j+blockX] = 1;
			}
			
		}
	}
	return touched*10;
}

int DeleteLine(char f[HEIGHT][WIDTH]){
	// user code
	int sum = 0;
	for(int i = 0;i<HEIGHT;i++){
		int ch = 0;
		for(int j = 0;j<WIDTH;j++){
			if(f[i][j] == 0){
				ch = 1;
				break;
			}
		}
		if(!ch){
			sum++;
			for(int j = i-1;j>=0;j--)
				for(int k = 0;k<WIDTH;k++)
					f[j+1][k] = f[j][k];
		}
	}
	return sum*sum*100;
}

void DrawShadow(int y, int x, int blockID,int blockRotate){
	// user code
	while(CheckToMove(field,blockID,blockRotate,++y,x));
	DrawBlock(y-1,x,blockID,blockRotate,'/');
}

void DrawBlockWithFeatures(int y, int x, int blockID, int blockRotate){	
	DrawBlock(y,x,blockID,blockRotate,' ');
	DrawShadow(y,x,blockID,blockRotate);
}

void createRankList(){
	// user code
    FILE *fp;
    fp = fopen("rank.txt","r");
    if(fp == NULL)
        return;
    fscanf(fp,"%d",&rank_num);
    Head = NULL;
    List *ptemp = NULL;
    List *temp = Head;
    for(int i = 0;i<rank_num;i++){
        List *new_list = (List*)malloc(sizeof(List));
        new_list->link = NULL;
        fscanf(fp,"%s",(new_list->name));
        fscanf(fp,"%d",&(new_list->score));
        if(temp == NULL){
            temp = new_list;
            Head = new_list;
        }
        else{
            temp->link = new_list;
            temp = new_list;
        }
    }
    fclose(fp);
}
void rank_menu(){
    printw("1. list ranks from X to Y\n");
    printw("2. list ranks by a specific name\n");
    printw("3. delete a specific rank\n");
}

void print_rank(){
    int x = 0,y = 987654321,cnt = 0;
    echo();
    printw("X : ");
    scanw("%d",&x);
    printw("Y : ");
    scanw("%d",&y);
    noecho();
    printw("%d %d\n",x,y);
    printw("        name        |   score\n");
    printw("-----------------------------\n");
    if(x>y)
        printw("search failure: no such rank in the list\n");
    else{
        if(Head == NULL)
            return;
        else{
            List* temp = Head;
            while(temp != NULL){
                cnt++;
                if(cnt>=x && cnt<=y)
                    printw("%-20s|   %d\n",temp->name,temp->score);
                else if(cnt>y)
                    break;
                temp = temp->link;
            }
        }
    }
}

void rank(){
    clear();
    rank_menu(); 
    switch(wgetch(stdscr)){
        case '1' : print_rank(); break;
        case '2' : FindRankName(); break;
        case '3' : DeleteRank(); break;
        default : break;
    }
    getch();
}

void writeRankFile(){
    FILE *fp;
    fp = fopen("rank.txt","w");
    if(fp == NULL)
        return;
    fprintf(fp,"%d\n",rank_num);
    if(rank_num==0)
        return;
    List *temp = Head;
    List *pre = Head;
    for(int i = 0;i<rank_num;i++){
        fprintf(fp,"%s %d\n",temp->name,temp->score);
        pre = temp;
        temp = temp->link;
        free(pre);
    }
    fclose(fp);
}

void newRank(int score){
    List *new_list = (List*)malloc(sizeof(List));
    new_list->link = NULL;
    new_list->score = score;
    clear();
    echo();
    printw("your name: ");
    scanw("%s",new_list->name);
    noecho();
    List *pre = NULL;
    List *cur = Head;
    if(Head == NULL)
        Head = new_list;
    while(cur!=NULL){
        if(cur->score < score)
            break;
        else if(cur->score == score)
            if(strcmp(cur->name,new_list->name)<0)
                break;
        pre = cur;
        cur = pre->link;
    }
    if(pre!=NULL){
        new_list->link = cur;
        pre->link = new_list;
    }
    else{
        new_list->link = cur;
        Head = new_list;
    }
    rank_num++;
}

void FindRankName(){
    char str[NAMELEN];
    int ch = 0;
    List *cur = Head;
    echo();
    printw("input the name: ");
    scanw("%s",str);
    noecho();
    printw("        name        |   score\n");
    printw("-----------------------------\n");
    while(cur!=NULL){
        if(strcmp(cur->name,str)==0){
            printw("%-20s|   %d\n",cur->name,cur->score);
            ch++;
        }
        cur = cur->link;
    }
    if(ch == 0)
        printw("search failure: no name in the list\n");
}

void DeleteRank(){
    int num = -1;
    int ch = 0;
    List *pre = NULL;
    List *cur = Head;
    echo();
    printw("input the rank: ");
    scanw("%d",&num);
    noecho();
    for(int i = 0;i<rank_num;i++){
        ch++;
        if(ch == num)
            break;
        pre = cur;
        cur = pre->link;
    }
    if(ch == num){
        if(pre == NULL)
            Head = cur->link;
        else
            pre->link = cur->link;
        free(cur);
        rank_num--;
        printw("result: the rank deleted\n");
    }
    else
        printw("search failure: the rank not in the list\n");
}
    

void DrawRecommend(Node* root, int y, int x, int blockID,int blockRotate){
    if(recflag == 0){
        recflag = 1;
        root->score = 0;
        root->curBlockID = blockID;
        root->recBlockX = x;
        root->recBlockY = y;
        root->recBlockRotate = 0;
        
        //root->score = recommend(root,0);
        
        root->score = modified_recommend(root,0);

        if(root->recBlockY == y){                       //modified_recommend함수를 위해 추가
            int maxtouched = -1;
            for(int k = 0;k<4;k++){
                if(root->curBlockID == 0 ||root->curBlockID >4)
                    if(k>1) continue;
                if(root->curBlockID == 4)
                    if(k>0) continue;
                for(int i = -2;i<=WIDTH+1;i++){
                    if(CheckToMove(field,root->curBlockID,k,0,i)){
                        int minY = 0;
                        while(CheckToMove(field,root->curBlockID,k,++minY,i));
                        minY--;
                        int touched = 0;
                        for(int ii = 0;ii<4;ii++){
                            for(int jj = 0;jj<4;jj++){
                                if(block[root->curBlockID][k][ii][jj]){
                                    if(ii+minY+1==HEIGHT)
                                        touched+=2;
                                    else if(field[ii+minY+1][jj+i])
                                        touched++;
                                    if(field[ii+minY][jj+i-1])
                                        touched++;
                                    if(field[ii+minY][jj+i+1])
                                        touched++;

                                }
                            }
                        }
                        if(touched>maxtouched){
                            root->recBlockY = minY;
                            root->recBlockX = i;
                            root->recBlockRotate = k;
                        }
                    }
                }

            }
        }
    }
    DrawBlock(root->recBlockY,root->recBlockX,root->curBlockID,root->recBlockRotate,'R');
}

int recommend(Node *root,int level){
    int maxscore=-987654321; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
    int sum = 0;
    if(level<BLOCK_NUM){
        for(int k = 0;k<4;k++){
            for(int i = -2;i<=WIDTH+1;i++){
                if(CheckToMove(field,root->curBlockID,k,0,i)){
                    int minY = 0;
                    while(CheckToMove(field,root->curBlockID,k,++minY,i));
                    minY--;
                    int touched = 0;
                    for(int ii = 0;ii<4;ii++){
                        for(int jj = 0;jj<4;jj++){
                            if(block[root->curBlockID][k][ii][jj]){
                                if(ii+minY+1==HEIGHT)
                                    touched++;
                                else if(field[ii+minY+1][jj+i])
                                    touched++;
                            }
                        }
                        for(int jj = 0;jj<4;jj++)
                            if(block[root->curBlockID][k][ii][jj]) 
                                field[minY+ii][i+jj] = 1;
                    }
                     
                    Node *newnode = (Node*)malloc(sizeof(Node));
                    newnode->score = 0;
                    newnode->curBlockID = nextBlock[level+1];
                    newnode->recBlockX = 0;
                    newnode->recBlockY = minY;                 //성능향상위해 변경
                    int curscore = recommend(newnode,level+1);
                    curscore += touched*10;
                    if(maxscore<curscore){
                        maxscore = curscore;
                        root->recBlockX = i;
                        root->recBlockY = minY;
                        root->recBlockRotate = k;
                    }
                    free(newnode);
                    for(int a = 0;a<4;a++)
                        for(int b = 0;b<4;b++)
                            if(block[root->curBlockID][k][a][b])
                                field[minY+a][i+b] = 0;
                }    
            }
        }
    }
    else{
        //calculate deleteLine
        for(int i = 0;i<HEIGHT;i++){
            int ch = 0;
            for(int j = 0;j<WIDTH;j++){
                if(field[i][j] == 0){
                    ch = 1;
                    break;
                }
            }
            if(!ch){
                sum++;
            }
        }
        sum = sum*sum*100;
    }
    maxscore += sum;
    return maxscore;
}
int modified_recommend(Node *root,int level){
    int maxscore=-987654321; // 미리 보이는 블럭의 추천 배치까지 고려했을 때 얻을 수 있는 최대 점수
    int sum = 0;
    if(level<BLOCK_NUM){
        for(int k = 0;k<4;k++){
            if(root->curBlockID == 0 ||root->curBlockID >4)
                if(k>1) continue;
            if(root->curBlockID == 4)
                if(k>0) continue;
            for(int i = -2;i<=WIDTH+1;i++){
                if(CheckToMove(field,root->curBlockID,k,0,i)){
                    int minY = 0;
                    while(CheckToMove(field,root->curBlockID,k,++minY,i));
                    minY--;
                    if(minY<root->recBlockY-4 && danger == 0)
                        continue;
                    if((level>1&&minY<root->recBlockY-3) && danger == 0)
                        continue;
                    int touched = 0;
                    for(int ii = 0;ii<4;ii++){
                        for(int jj = 0;jj<4;jj++){
                            if(block[root->curBlockID][k][ii][jj]){
                                if(ii+minY+1==HEIGHT)
                                    touched+=2;
                                else if(field[ii+minY+1][jj+i])
                                    touched++;
                                if(field[ii+minY][jj+i-1])
                                    touched++;
                                if(field[ii+minY][jj+i+1])
                                    touched++;
                                
                            }
                        }
                        for(int jj = 0;jj<4;jj++)
                            if(block[root->curBlockID][k][ii][jj]) 
                                field[minY+ii][i+jj] = 1;
                    }

                    if(touched>2 || danger == 1){
                        Node *newnode = (Node*)malloc(sizeof(Node));
                        newnode->curBlockID = nextBlock[level+1];
                        newnode->recBlockY = minY;                 //성능향상위해 변경
                        int curscore = modified_recommend(newnode,level+1);
                        curscore += touched*10;
                        if(maxscore<curscore){
                            maxscore = curscore;
                            root->recBlockX = i;
                            root->recBlockY = minY;
                            root->recBlockRotate = k;
                        }
                        else if(maxscore==curscore){          //성능향상위해 추가
                            if(root->recBlockY<minY){
                                root->recBlockX = i;
                                root->recBlockY = minY;
                                root->recBlockRotate = k;
                            }
                        }
                        free(newnode);
                    }
                    for(int a = 0;a<4;a++)
                        for(int b = 0;b<4;b++)
                            if(block[root->curBlockID][k][a][b])
                                field[minY+a][i+b] = 0;
                }    
            }
        }
    }
    else{
        //calculate deleteLine
        for(int i = 0;i<HEIGHT;i++){
            int ch = 0;
            for(int j = 0;j<WIDTH;j++){
                if(field[i][j] == 0){
                    ch = 1;
                    break;
                }
            }
            if(!ch){
                sum++;
            }
        }
        sum = sum*sum*100;
    }
    maxscore += sum;
    return maxscore;
}

void recommendedPlay(){	
    int command;
	clear();
    //act.sa_handler = BlockDown;
	sigaction(SIGALRM,&act,&oact);
	InitTetris();
    timed_out = 0;
    double start_time = (double)clock(); 
    double tot_time = 0;
    timeout(0);
	
	do{
        switch(wgetch(stdscr)){
            case 'q' : command = QUIT; break;
            default:
            DrawRecommend(&root,blockY, blockX, nextBlock[0],blockRotate);
            blockX = root.recBlockX;
            blockY = root.recBlockY;
            blockRotate = root.recBlockRotate;
            BlockDown(1);
            timed_out = 1;
            
            tot_time = ((double)clock() - start_time)/CLOCKS_PER_SEC;
            
            move(HEIGHT - 1, WIDTH*2+1);
            printw("Play Time: %d sec\n",(int)tot_time);
            move(HEIGHT, WIDTH*2+1);
            printw("Score_per_sec: %d\n",(int)(score/tot_time));
            move(HEIGHT+1, WIDTH*2+1);
			printw("Total_Block: %d\n",blockcount);
			move(HEIGHT+2, WIDTH*2+1);
            printw("Block_per_sec: %d\n",(int)(blockcount/tot_time));
            
            int ff = 0;
            for(int i = 0;i<WIDTH;i++){
                if(field[HEIGHT/2-2][i] == 1){
                    danger = 1;
                    ff = 1;
                    break;
                }
            }
            if(ff == 0)
                danger = 0;
        }
		if(command==QUIT){
            timeout(-1);
			alarm(0);
			DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
			move(HEIGHT/2,WIDTH/2-4);
			printw("Good-bye!!");
			refresh();
			getch();
			return;
		}

	}while(!gameOver);
	alarm(0);
	getch();
	DrawBox(HEIGHT/2-1,WIDTH/2-5,1,10);
	move(HEIGHT/2,WIDTH/2-4);
	printw("GameOver!!");
	refresh();
    timeout(-1);
	getch();
}
