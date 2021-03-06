/*********************
 * date: 2017/06/12
 * group: wrd
 * student ID: 20170259,20170319,20170293,20170289
 *
 *
 ********************/

/********************
  message or todo

  O + $ = P
  @ + O = *

  1. read map: clear(on main)
  2. error: clear(on main)
  3. input name: clear(on main)
  4. game clear: clear(on main)
  5. ranking: clear(on main)
  6. save ranking: clear(on main)
  7. h,j,k,l move: clear(on main)
  8. undo: clear(on main)
  9. new: clear(on main)
  10. exit: clear(on main)
  11. replay: clear(on main)
  12. file load: clear(on main)
  13. save: clear(on main)
  14. help: clear(on main)
  15. top: clear(on main)
  16. each map top: clear(on main)
  17. time: clear(on main)
 *********************/

/* FOR WINDOWS */
//#include <termio.h>

/* FOR MAC OS X */
#include <termios.h>

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
/* FOR ERROR FIX */
int ctrl_key(char ch);
int player_xy(void);

//common var

char map[5][30][30];
char p_map[30][30];
int  mapsize[5];
int player_x, player_y;//player's x,y

int num_top[3];

int stage = 0;
int box = 0;
int rank[5][4];

char name[10];
char name_r[5][4][10];
char undo[2][5] = {0,0,0,0,0,0,0,0,0,0};//undo배열 값 초기화.
time_t start,end;
int m_time, savetime=0;
//common func
int getch(void){
	int ch;

	struct termios buf;
	struct termios save;

	tcgetattr(0, &save);
	buf = save;

	buf.c_lflag&=~(ICANON|ECHO);
	buf.c_cc[VMIN] = 1;
	buf.c_cc[VTIME] = 0;

	tcsetattr(0, TCSAFLUSH, &buf);

	ch = getchar();
	tcsetattr(0, TCSAFLUSH, &save);

	return ch;
}

//Seung-mo
int readmap(void){ //read original map from map.txt

	FILE  *mapf;
	char a;
	int b = -1, i = 0, j = 0;

	mapf = fopen("map.txt", "r");

	for(int x = 0; x <5 ; x++)
		for(int y = 0; y< 30; y++)
			for(int z = 0; z < 30; z++)
				map[x][y][z] = ' '; //initialize all map arrays

	while (fscanf(mapf, "%c", &a) != EOF)
	{
		if((a == ' ')||(a == '#')||(a == '$')||(a =='O')||(a =='@')){
			map[b][i][j] = a;
			j++; //move to next horizontal block
		}
		else if(a =='\n'){
			i++; //move to next vertical block
			j = 0; //initialize horizontal block
		}
		else if (a == 'm'||a =='e'){
			mapsize[b] = ++i;
			b++; //move to next stage block
			i =-1; //initialize horizontal block - 1
			j =0; //initialize vertical block
		}
		else{}
	}

	fclose(mapf);
	return 0;
}

int playmap(int a)
{
	for(int i =0; i < 30; i++)
		for(int j = 0; j < 30; j++)
			p_map[i][j] = map[a][i][j]; //depending on the stage value, copy source map to p_map

	return 0;
}

int showgame()
{
	for(int i =0; i < mapsize[stage]-1; i++){
		for(int j = 0; j < 30; j++){
			if(p_map[i][j] == '*') //O + $ = $
				printf("$"); // display * as $
			else if (p_map[i][j] == 'P') //O + @ = P
				printf("@"); // display P as @
			else
				printf("%c", p_map[i][j]); //display
		}
		printf("\n");
	}
	return 0;
}
int command(char x){
	switch (x) //processing input value
	{
		case 'd':
			displayhelp();
			break;
		case 'u':
			if(undo[0][0] != 0)//오류 해결.
	      	undo_key();
			break;
		case 'r':
			playmap(stage);
			for (int i =0 ; i < 10; i++)
			{
				undo[0][i] = 0;
			}
			break;
		case 'n':
			stage =0;
			savetime = 0;
			playmap(stage);
			start=time(NULL);
			for (int i =0 ; i < 10; i++)
			{
				undo[0][i] = 0;
			}
			break;
		case 'e':
			system("clear");
			printf("SEE YOU ");
			for(int i = 0; i <10; i++)
				printf("%c", name[i]);
			printf("....\n");
			exit(1);
			break;
		case 's':
			save();
			break;
		case 'f':
			fileload();
			break;
		case 't':
			break;
		case '1':
			stage++;
			playmap(stage);
			break;

	}
}

int get_key(void) //input values
{
	static char a, a_sav;;
	static int pin;
  	int c;
	if(num_top[0] == 't') //t can get 3
	{
		char w;
		printf("(command) %c%c%c", num_top[0], num_top[1], num_top[2]);//t can get 3 letters
		w = getch();
		if(w == '\b'||w == 127){
			num_top[pin-1] = 0;
			pin--;
		}
		else if (w == '\n')
		{
			if(('0'<num_top[2])&&('9'>=num_top[2]))
				top_output(num_top[2] - '0');
			else
				top_output(127);

			num_top[0] = 0;
			num_top[1] = 0;
			num_top[2] = 0;

		}
		else{
			num_top[pin] = w;
			pin++;
		}
		if(pin == 4)
			pin = 3;
	}
	else{
		printf("(command) %c", a);
		a = getch();
	}
	if((a == 'h')||(a == 'j')||(a == 'k')||(a == 'l')){ // move key
		c = ctrl_key(a);
    	if(c == 1){//player만 움직였을 경우
      		for(int i = 0; i <2; i++){
        		for(int j = 3; j>=0; j--){
          			undo[i][j+1] = undo[i][j];//배열값을 뒤로 밀어준다.
       			 }
			}
			undo[0][0] = a;//키를 입력받은 값이 저장됨.
	  	undo[1][0] = 0;//화물이 안움직였을 경우 2번째 행에 값은 0로 저장
		}
    	if(c == 2){//player와 화물이 움직였을 경우
      		for(int i = 0; i <2; i++){
        		for(int j = 3; j>=0; j--){
          			undo[i][j+1] = undo[i][j];//배열값을 뒤로 밀어준다.
       			}
			}
			undo[0][0] = a;
			undo[1][0] = 1;//화물이 움직이면 2번째 행의 값은 1로 저장
    	}
    	a = 0;//키입력마다 a값 초기화
	}
	else if(a == '\n')
		command(a_sav);

	else if(a == 't'){
		num_top[0] = 't';
		pin = 1;
		a = 0;
	}
	a_sav = a;
}
int printname(void){ // print name

	printf("\tHello ");
	for(int i = 0; i < 10; i++)
		printf("%c", name[i]);
	printf("\n");
}
int re_inputname(void){ // run when the name is entered incorrectly

	int nm=0;

	for (int j = 0; j <10; j++)
	{
		name[j] = 0;
	}
	system("clear");

	printf("다시 사용자 명을 입력하시오(영어로 최대10자): ");
	for(nm =0; nm<10; nm++){
		name[nm] = getchar(); //입력받은 값을 name배열값에 지정.
		if(name[nm] == '\n')
		{
			name[nm] = 0;
			for(int i = 0; i<nm; i++){
				if(!(((name[i] >= 'a')&&(name[i] <= 'z'))||((name[i] >= 'A')&&(name[i] <= 'Z')))){
					re_inputname();
				}
				else
					break;
			}
			break;
		}
	}
	return 0;
}

int top_output(int a) //top output
{
	system("clear");
	switch (a)
	{
		case 127:
		case 1:
			printf("\nstage 1 top\n");
			for(int i =0; i< 3; i++){
				if(name_r[0][i][0] == '!'||name_r[0][i][0] == 0)
					break;
				printf("#%d\t", i+1);
				for(int j = 0; j < 10; j++){
					printf("%c", name_r[0][i][j]);
				}
				printf("%d.0sec\n", rank[0][i]);
			}
			if(a == 1)
				break;
		case 2:
			printf("\nstage 2 top\n");
			for(int i =0; i< 3; i++){
				if((name_r[1][i][0] == '!')||(name_r[1][i][0] == 0))
					break;
				printf("#%d\t", i+1);
				for(int j = 0; j < 10; j++){
					printf("%c", name_r[1][i][j]);
				}
				printf("%d.0sec\n", rank[1][i]);
			}
			if(a == 2)
				break;
		case 3:
			printf("\nstage 3 top\n");
			for(int i =0; i< 3; i++){
				if((name_r[2][i][0] == '!')||(name_r[2][i][0] == 0))
					break;
				printf("#%d\t", i+1);
				for(int j = 0; j < 10; j++){
					printf("%c", name_r[2][i][j]);
				}
				printf("%d.0sec\n", rank[2][i]);
			}
			if(a == 3)
				break;
		case 4:
			printf("\nstage 4 top\n");
			for(int i =0; i< 3; i++){
				if((name_r[3][i][0] == '!')||(name_r[3][i][0] == 0))
					break;
				printf("#%d\t", i+1);
				for(int j = 0; j < 10; j++){
					printf("%c", name_r[3][i][j]);
				}
				printf("%d.0sec\n", rank[3][i]);
			}
			if(a == 4)
				break;
		case 5:
			printf("\nstage 5 top\n");
			for(int i =0; i< 3; i++){
				if((name_r[4][i][0] == '!')||(name_r[4][i][0] == 0))
					break;
				printf("#%d\t", i+1);
				for(int j = 0; j < 10; j++){
					printf("%c", name_r[4][i][j]);
				}
				printf("%d.0sec\n", rank[4][i]);
			}
			if(a == 5)
				break;
		break;
	}
	getch();
}

int read_ranking(void) //read ranking data from ranking.txt
{
	char c = 0;
	FILE* ranking;
	ranking = fopen("ranking.txt", "r");

	for(int j=0; j<20; j++){
		fscanf(ranking,"%d", &rank[0][j]);
	}
	fgetc(ranking);
//	fgetc(ranking);
	for(int i = 0; i < 5; i++)
		for(int j = 0; j < 4; j++)
			for(int nm =0; nm<10; nm++){
				name_r[i][j][nm] = fgetc(ranking); //입력받은 값을 name배열값에 지정.
				if(name_r[i][j][nm] == '\n')
				{
					name_r[i][j][nm] = 0;
					break;
				}
			}

	fclose(ranking);
}

//Cheol-soon
int inputname(void)
{
	int nm=0;

	printf("사용자 명을 입력하시오(영어로 최대10자): ");
	for(nm =0; nm<10; nm++){
		name[nm] = getchar(); //입력받은 값을 name배열값에 지정.
		if(name[nm] == '\n')
		{
			name[nm] = 0;
			for(int i = 0; i<nm; i++){
				if(!(((name[i] >= 'a')&&(name[i] <= 'z'))||((name[i] >= 'A')&&(name[i] <= 'Z')))){
					re_inputname();
				}
				else
					break;
			}
			break;
		}
	}
		return 0;
}

/* display help(d) */
int displayhelp(void){
	//만약 d키가 눌렸을 때 아래의 내용들이 보여짐.//
	system("clear");
	printf("-u(undo) : 이전에 있던 위치로 돌아가기 (최대 5번 할 수 있음.)\n");
	printf("-r(replay) : 현재 맵을 처음부터 다시 시작(게임시간은 계속 유지.)\n");
	printf("-n(new) : 첫 번째 맵부터 다시 시작(현재까지의 시간 기록 삭제.)\n");
	printf("-e(exit) : 게임 종료.\n");
	printf("-s(save) : 현재상태 파일에 저장\n");
	printf("-f(file load) : 저장한 파일 불러와서 이어서 게임시작.\n");
	printf("-d(display help) : 명령어 보여줌.\n");
	printf("-t(top) : 게임 순위 보기.(t만 입력하면 전체 순위, t다음 숫자가 오면 해당 맵의 순위)\n");
	printf("----------------나가기 'q'-----------------\n");
	if(getch() == 'q');//	'q' = return to map
	else
		displayhelp();
}

int player_xy(void)//find a player's x, y
{
	for(int i=0;i<30;i++)//find '@', 'P'
	{
		for(int j=0;j<30;j++)
		{
			if ((p_map[i][j] == '@')||(p_map[i][j] == 'P')){
				player_y = i, player_x = j;
				break;
			}
		}
	}
	return 0;
}

/* undo(u) */
int undo_key(void)
{
	player_xy();
	int box_x, box_y;
  char reverse;
//Divide the number of cases according to the number of keys pressed.
	switch (undo[0][0]) {
		case 'l':
			box_x = 1;
			box_y = 0;
			reverse = 'h';
			break;
		case 'k':
			box_x = 0;
			box_y = -1;
			reverse = 'j';
			break;
		case 'j':
			box_x = 0;
			box_y = 1;
			reverse = 'k';
			break;
		case 'h':
			box_x = -1;
			box_y = 0;
			reverse = 'l';
			break;
		default:
			break;
	}

    if(undo[1][0] == 1)//화물이 같이 움직였을 경우
    {
      ctrl_key(reverse);//player의 위치를 먼저 원래의 위치로.
			if((p_map[player_y+box_y][player_x+box_x] == '*')&&(p_map[player_y][player_x] == ' ')){
				p_map[player_y+box_y][player_x+box_x] = 'O';
				p_map[player_y][player_x] = '$';//
			}
			else if((p_map[player_y+box_y][player_x+box_x] == '*')&&(p_map[player_y][player_x] == 'O')) {
      	p_map[player_y+box_y][player_x+box_x] = 'O';
      	p_map[player_y][player_x] = '*';
			}
			else if((p_map[player_y+box_y][player_x+box_x] == '$')&&(p_map[player_y][player_x] == ' ')) {
      	p_map[player_y+box_y][player_x+box_x] = ' ';
      	p_map[player_y][player_x] = '$';
			}
			else if((p_map[player_y+box_y][player_x+box_x] == '$')&&(p_map[player_y][player_x] == 'O')) {
      	p_map[player_y+box_y][player_x+box_x] = ' ';
      	p_map[player_y][player_x] = ' ';
			}
    }
    else if (undo[1][0] == 0)//player만 움직였을 경우 반대의 키를 입력한다.
      ctrl_key(reverse);
		else
		{}
  for(int i = 1; i >=0; i--){//undo기능을 사용시 앞으로 배열값을 당겨온다.
		if(i == 0){
			for(int j = 0; j <5; j++){
			undo[i][j] = undo[i][j+1];
			}
			undo[0][4] = 0;//마지막 배열값은 다시 0으로 초기화
		}
		else{
      for(int j = 0; j < 5; j++){
        undo[i][j] = undo[i][j+1];
			}
			undo[1][4] = 0;//마지막 배열값은 다시 0으로 초기화
		}
	}
}
//Jae-woo
int error(void){ //맵의 오류를 확인하는 함수
	int box=0, place=0;
	for(int b=0; b<5; b++){
		for(int i=0; i<30; i++)
			for(int j=0; j<30; j++)
			{
				if(map[b][i][j]=='$')
					box++;
				else if(map[b][i][j]=='O')
					place++;
				else{}
			} //맵 전부를 확인해서 $가 있으면 box변수 증가, O가 있으면 place변수 증가
		if(box!=place){
			printf("%d맵에 오류가 있습니다.\n", b+1);
			exit(1);
		}//box와 place가 같지 않으면 오류메시지를 출력하고 게임을 종료
	}
}
int gameclear(void)
	//game을 클리어 했을 때
{
	if(stage> 4){
	system("clear");
	printf("축하합니다. 모든 게임을 완료했습니다.\n");
	exit(1);
	}//stage가 4보다 크면 모든 맵을 완료했다는 뜻이므로 축하메시지를 출력하고 게임을 종료
}
int fileload(void)
{ //f키를 눌렀을 때, fileload기능
	FILE *sokoban;
	sokoban=fopen("sokoban.txt","r");
	for(int i=0;i<30;i++)
		for(int j=0;j<30;j++)
			fscanf(sokoban,"%c",&p_map[i][j]);	//저장된 맵을 로드
	for(int k = 0; k <10; k++)
		fscanf(sokoban,"%c", &name[k]); //유저 네임을 로드
	for(int h = 0; h < 10; h++)
		fscanf(sokoban,"%d", &undo[0][h]); //저장된 undo 로드
	fscanf(sokoban,"%f",&savetime); //시간 로드
	fscanf(sokoban,"%d",&stage);//stage 로드
	fclose(sokoban);
	time(&start); //시간 측정 다시 시작
}
int save(void)
{ //s키를 눌렀을 때 save기능
	time(&end); //시간 측정 종료
	savetime+=end-start;
	FILE *sokoban;
	sokoban=fopen("sokoban.txt","w");
	for(int i=0; i<30;i++)
		for(int j=0; j<30; j++)
		{
			fprintf(sokoban,"%c",p_map[i][j]);
		} //현재 맵 저장
	for(int k = 0; k <10; k++)
		fprintf(sokoban,"%c", name[k]); //유저 네임 저장
	fprintf(sokoban,"\n");
	for(int h = 0; h < 10; h++)
		fprintf(sokoban,"%d\n", undo[0][h]); //현재 undo저장
	fprintf(sokoban,"%f\n",savetime); //지나간 시간 저장
	fprintf(sokoban,"%d\n",stage); //현재 단계 저장
	fclose(sokoban);
	time(&start);//시간 측정 다시 시작
}

int timeprint(void){
  m_time=savetime+(end-start);
  rank[stage][3] = m_time;
  for (int i = 0; i <10; i++)
  {
	if(name[i] == 0 ||name[i] == '\n')
		break; //map을 클리어 한 유저의 이름 전달
  	name_r[stage][3][i] = name[i];
  }
}

int mapclear(void)
{ //map을 클리어 했을 때
	if(box==0) //box가 0이라는 뜻은 맵이 클리어 되었다는 것이다
	{
		time(&end); //시간 측정을 멈춘다
		timeprint();
		ranking();
		stage++; //stage증가
		time(&start); //시간 측정 다시 시작
		savetime = 0.0;
		playmap(stage); // 다음 단계 시작
		for (int i =0 ; i < 10; i++)
		{
			undo[0][i] = 0;
		} //undo 초기화
	}
}

//Jae-hyun*********************************
int ranking(void)
{
	int i, j, tmp, n, tmp_n;
	for (i = 0; i < 3; i++)//bubble sorting
	{
		for (j = 0; j < 3  - i; j++)
		{
			if (rank[stage][j] > rank[stage][j + 1])
			{
				tmp = rank[stage][j];
				rank[stage][j] = rank[stage][j + 1];
				rank[stage][j+1] = tmp;
				for (int g = 0; g <= 9; g++)
				{
					tmp_n = name_r[stage][j][g];
					name_r[stage][j][g] = name_r[stage][j+1][g];
					name_r[stage][j+1][g] = tmp_n;
				}
			}
		}
	}

	char c = 0;
	FILE* ranking;

if (box == 0)

	{
		ranking = fopen("ranking.txt","w");//ranking file writing
		int ranknum = 1;
		for(int j=0; j<20; j++){
			fprintf(ranking,"%d",rank[0][j]);
			fprintf(ranking,"\n");
		}
		for(int i = 0; i < 20; i++)//name file writing
		{
			for (int j = 0; j < 10; j++) {
				fprintf(ranking, "%c", name_r[0][i][j]);
				if(name_r[0][i][j] == 0)
					break;
			}
			fprintf(ranking,"\n");
		}
		fclose(ranking);
	}
}
int ctrl_key(char ch)//control part
{
    int a, b, c, d;
    for(int i=0;i<30;i++)
    {
      for(int j=0;j<30;j++)
      {
		  if ((p_map[i][j] == '@') || (p_map[i][j] == 'P')) {
			  b = i, a = j;
		  }
      }
    }


      switch (ch) {//key accepting
        case 'l':
          c = 1;
          d = 0;
          break;
        case 'k':
          c = 0;
          d = -1;
          break;
        case 'j':
          c = 0;
          d = 1;
          break;
        case 'h':
          c = -1;
          d = 0;
          break;
      }

      if(p_map[b][a] == '@')//user on space*******************
      {
        if((p_map[b+d][a+c] == '#'))//wall  1
        {
          return 0;
        }
        else if((p_map[b+d][a+c] == 'O'))//location  2
        {
          p_map[b+d][a+c] = 'P';
          p_map[b][a] = ' ';
          return 1;
        }

        else if((p_map[b+d][a+c] == '$'))//package  3
        {
          if(p_map[b+(d*2)][a+(c*2)] == '#')//package facing wall
          {
            return 0;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == ' ')//package facing space
          {
            p_map[b+(d*2)][a+(c*2)] = '$';
            p_map[b+d][a+c] = '@';
            p_map[b][a] = ' ';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == 'O')//package facing location
          {
            p_map[b+(d*2)][a+(c*2)] = '*';
            p_map[b+d][a+c] = '@';
            p_map[b][a] = ' ';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == '*')//package facing other package on location
          {
            return 0;
          }
          else if(p_map[b+d][a+c] == '$')//packaage facing other package
          {
            return 0;
          }
        }

        else if((p_map[b+d][a+c] == ' '))//space  4
        {
          p_map[b+d][a+c] = '@';
          p_map[b][a] = ' ';
          return 1;
        }

        else if((p_map[b+d][a+c] == '*'))//package on the location  5
        {
          if(p_map[b+(d*2)][a+(c*2)] == '#')//package on location facing wall
          {
            return 0;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == ' ')//package on location facing space
          {
            p_map[b+(d*2)][a+(c*2)] = '$';
            p_map[b+d][a+c] = 'P';
            p_map[b][a] = ' ';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == 'O')//package on location facing location
          {
            p_map[b+(d*2)][a+(c*2)] = '*';
            p_map[b+d][a+c] = 'P';
            p_map[b][a] = ' ';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == '*')//package on location facing other package on location
          {
            return 0;
          }
          else if(p_map[b+d][a+c] == '$')//packaage on location facing other package
          {
            return 0;
          }
        }
      }
      else if(p_map[b][a] == 'P')//user on the location******************
      {
        if((p_map[b+d][a+c] == '#'))//wall                1
        {
          return 0;
        }
        else if((p_map[b+d][a+c] == '$'))//package        2
        {
          if(p_map[b+(d*2)][a+(c*2)] == '#')//package facing wall
          {
            return 0;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == ' ')//package facing space
          {
            p_map[b+(d*2)][a+(c*2)] = '$';
            p_map[b+d][a+c] = '@';
            p_map[b][a] = 'O';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == 'O')//package facing location
          {
            p_map[b+(d*2)][a+(c*2)] = '*';
            p_map[b+d][a+c] = 'P';
            p_map[b][a] = 'O';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == '*')//package facing other package on location
          {
            return 0;
          }
          else if(p_map[b+d][a+c] == '$')//packaage facing other package
          {
            return 0;
          }
        }

        else if((p_map[b+d][a+c] == '*'))//package on the location3
        {
          if(p_map[b+(d*2)][a+(c*2)] == '#')//package on location facing wall
          {
            return 0;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == ' ')//package on location facing space
          {
            p_map[b+(d*2)][a+(c*2)] = '$';
            p_map[b+d][a+c] = 'P';
            p_map[b][a] = 'O';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == 'O')//package on location facing location
          {
            p_map[b+(d*2)][a+(c*2)] = '*';
            p_map[b+d][a+c] = 'P';
            p_map[b][a] = 'O';
            return 2;
          }
          else if(p_map[b+(d*2)][a+(c*2)] == '*')//package on location facing other package on location
          {
            return 0;
          }
          else if(p_map[b+d][a+c] == '$')//packaage on location facing other package
          {
            return 0;
          }
        }

        else if((p_map[b+d][a+c] == ' '))//space              4
        {
          p_map[b+d][a+c] = '@';
          p_map[b][a] = 'O';
          return 1;
        }
        else if((p_map[b+d][a+c] == 'O'))//location          5
        {
          p_map[b+d][a+c] = 'P';
          p_map[b][a] = 'O';
          return 1;
        }
      }
      else if((p_map[b][a] != '@')||(p_map[b][a] != 'P'))//error checking
      printf("move error");
      else
      printf("error");
}

int box_check(void)//counting boxes and moves to mext stage
{
	box = 0;
    for(int i=0;i<30;i++)
    {
      for(int j=0;j<30;j++)
      {
		  if (p_map[i][j] == '$')
			  box++;
      }
    }
}

//main
int main(void)
{
	system("clear");
    readmap();
    error();
    playmap(stage);
	read_ranking();
	inputname();
    system("clear");
	time(&start);
    while(1){
        printname();
        showgame();
        get_key();
		box_check();
		mapclear();
        gameclear();
		system("clear");
        }
}
