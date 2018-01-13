#include <ege.h>
#include<map>
#include<string>
#include<typeinfo>
#include<fstream>
#include<algorithm>
#include<Windows.h>
#include<vector>

using namespace std;

#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4


int scrollSpeed = 1;	//小车移动速度
int carSpeed = 4;		//小车敏捷度

bool STATE = true;		//游戏状态
bool bomb = false;		//炸弹爆炸
int AddScore=0;			//道具得分

int WinHeight = ::GetSystemMetrics(SM_CYSCREEN);	//窗口高度(全屏)
int WinWidth = ::GetSystemMetrics(SM_CXSCREEN);		//窗口宽度(全屏)
fstream file;
map<string, int> AllScore;;

ege::key_msg key = { 1 };


//自定义函数，用来返回一个0 - m之间的浮点数
int myrand(int m)
{
	return (ege::randomf() * m);
}

//定义一个Object基类
class Object
{
protected:
	int _x, _y;
	int _dx, _dy;
	int _w, _h;
	ege::PIMAGE _img;

public:
	//初始化，创建IMAGE等
	Object(int w,int h,char * file)
	{
		_x = myrand(WinWidth);
		_y = myrand(WinHeight)-WinHeight;
		_w = w;
		_h = h;
		_img = ege::newimage(w,h);
		ege::PIMAGE img_t = ege::newimage();
		ege::getimage(img_t, file);
		ege::putimage(_img, 0, 0, w, h, img_t, 0, 0, ege::getwidth(img_t), ege::getheight(img_t));
		ege::delimage(img_t);
	}

	void setx(int x){ _x = x; }
	void sety(int y){ _y = y; }

	int getx() { return _x; }
	int gety() { return _y; }

	int getw() { return _w; }
	int geth() { return _h; }

	//更新位置等相关属性
	virtual void update()
	{
		_dy = scrollSpeed;
		// 当前位置 + 速度
		if ( _y < ege::getheight())
			_y += _dy;
		else {
			_y = myrand(WinHeight)-WinHeight;
			_x = myrand(WinWidth);
		}
	}

	//根据属性值绘画
	virtual void render()
	{
		ege::putimage_transparent(NULL, _img, _x, _y, 0xFFFFFF);
	}

	//释放这个对象时调用
	~Object()
	{
		ege::delimage(_img);
	}
};

/*游戏结束*/
class GameOver:public Object
{
public:
	GameOver(int w, int h, char* file) :Object(w, h, file)
	{
		_x = (WinWidth-w)/2;
		_y = WinHeight / 4;
	}
	void render()
	{
		ege::putimage_transparent(NULL, _img, (int)_x, (int)_y, 0xFFFFFF);
	}

};
/*背景类*/
class Road :public Object
{
public:
	Road(int w, int h, char* file) :Object(w, h, file)
	{
		_x = 0;
		_y = -WinHeight;
	}
	void update()
	{
		_dy = scrollSpeed;
		_y += _dy;
		if (_y > 0)
			_y = -WinHeight;
	}
};
/*僵尸类*/
class Zombie :public Object
{
public:
	Zombie(int w, int h, char* file) :Object(w, h, file)
	{
	}
};
/*炸弹类*/
class Bomb :public Object
{
public:
	Bomb(int w, int h, char* file) :Object(w, h, file) {}
	virtual void update()
	{
		_dy = scrollSpeed;
		// 当前位置 + 速度
		if (_y < ege::getheight())
			_y += _dy;
		else {
			_y = myrand(WinHeight) - 2 * WinHeight;
			_x = myrand(WinWidth);
		}
	}
};
/*星星类*/
class Star :public Object
{
public:
	Star(int w, int h, char* file) :Object(w, h, file)
	{
	}
};
/*油箱类*/
class Oil:public Object
{
public:
	Oil(int w, int h, char* file) :Object(w, h, file){}
};
/*猴子类*/
class Monkey :public Object
{
public:
	Monkey(int w, int h, char* file) :Object(w, h, file) {}
};
/*赛车类*/
class Car :public Object
{
private:
	int direct;
	int LIFE=5;

	void onKey()
	{
		if(ege::kbmsg()) {                //kbmsg()非阻塞函数
			key = ege::getkey();           //getkey()阻塞函数
			if (key.msg == ege::key_msg_up)     direct = 0;
			else if (key.key == ege::key_left)  direct = LEFT;   //向左
			else if (key.key == ege::key_up)    direct = UP;   //向上
			else if (key.key == ege::key_right) direct = RIGHT;   //向右
			else if (key.key == ege::key_down)  direct = DOWN;   //向下
			else if (key.key == ege::key_esc)   STATE = false;      //ESC退出
		}
	}

public:
	Car(int w, int h, char* file) :Object(w, h, file)
	{
		_x = WinHeight;
		_y = WinWidth / 2;
	}

	void update()
	{
		this->onKey();
		_dx = _dy = carSpeed;
		if (direct == UP)
			_y = (_y > 0) ? _y - _dy : _y;
		else if (direct == DOWN)
			_y =(_y < WinHeight) ? _y + _dy : _y;
		else if (direct == LEFT)
			_x = (_x> 0) ? _x - _dx : _x;
		else if(direct==RIGHT)
			_x = ( _x < WinWidth) ? _x + _dx : _x;
	}

	int getLIFE() { return LIFE; }

	/*碰撞检测*/
	void collision(Object* obj)
	{
		int x1 = _x, y1 = _y, w1 = _w, h1 = _h;
		int x2 = obj->getx(), y2 = obj->gety(), w2 =obj->getw(), h2 = obj->geth();
		if (((x1 > x2&&x1 < x2 + w2) && (y1 > y2&&y1 < y2 + h2)) ||((x1 + w1 > x2&&x1 + w1 < x2 + w2) && (y1 > y2&&y1 < y2 + h2) )|| ((x1 > x2&&x1 < x2 + w2) && (y1 + h1 > y2&&y1 + h1 < y2 + h2)) || ((x1 + w1 > x2&&x1 + w1 < x2 + w2) && (y1 + h1 > y2&&y1 + h1 < y2 + h2))) {
			if (typeid(*obj) == typeid(Zombie)) {
				this->LIFE -= 6;
				obj->setx(myrand(WinWidth));
				obj->sety(myrand(WinHeight)-WinHeight);
				return;
			}
			else if (typeid(*obj) == typeid(Monkey)) {
				this->LIFE -= 3;
				obj->setx(myrand(WinWidth));
				obj->sety(myrand(WinHeight)-WinHeight);
			}
			else if (typeid(*obj) == typeid(Oil)) {

				if (carSpeed < 10)
					carSpeed++;
				else
					this->LIFE++;
				obj->setx(myrand(WinWidth));
				obj->sety(myrand(WinHeight)-WinHeight);
				AddScore += 5;
			}
			else if (typeid(*obj) == typeid(Star)) {
				this->LIFE++;
				obj->setx(myrand(WinWidth));
				obj->sety(myrand(WinHeight)-WinHeight);
				AddScore += 5;
			}
			else if (typeid(*obj) == typeid(Bomb)) {
				bomb = true;
				obj->setx(myrand(WinWidth));
				obj->sety(myrand(WinHeight)-2*WinHeight);
				AddScore += 10;
			}
		}
		return;
	}
};

void mainloop()
{
	const int MAXZOM = 6;		//僵尸的数量
	const int MAXWAR = 4;		//猴子的数量
	const int MAXBOMB = 1;		//炸弹的数量
	const int BUFF = 3;			//加成类道具的数量
	Road* bg = new Road(WinWidth, 2*WinHeight, "img//road.bmp");
	Car* car = new Car(70, 120, "img//car.bmp");
	GameOver *game = new GameOver(WinWidth / 2, WinHeight / 2, "img//title.bmp");
	vector<Object*> objs;		//可碰撞物体，运用多态
	
	for (int i = 0; i < MAXZOM; i++) {
		Object* zombie = new Zombie(75, 75, "img//zombie.bmp");
		objs.push_back(zombie);
	}

	for (int i = 0; i < MAXBOMB; i++) {
		Object* b = new Bomb(60, 60, "img//bomb.bmp");
		objs.push_back(b);
	}

	for (int i = 0; i < MAXWAR; i++) {
		Object* monkey = new Monkey(60, 80, "img//monkey.bmp");
		objs.push_back(monkey);
	}

	for (int i = 0; i < BUFF; i++) {
		Object* oil = new Oil(70, 70, "img//oil.bmp");
		Object* star = new Star(60, 60, "img//star.bmp");
		objs.push_back(star);
		objs.push_back(oil);
	}

	STATE = true;
	int interval = 0;

	
	/*背景音乐*/
	ege::MUSIC mus;
	mus.OpenFile("mus//Rage Your Dream.mp3");

	for (; ege::is_run(); ege::delay_fps(60))
	{

		/*每隔一段时间加速度，最大速度为20*/
		if (scrollSpeed < 30) {
			interval++;
			if (interval == 400) {
				scrollSpeed++;
				interval = 0;
			}
		}

		bg->update();
		ege::cleardevice();

		for (auto obj:objs) {
			obj->update();
			car->collision(obj);
		}
		if (bomb) {
			for (auto obj : objs) {
				obj->sety(obj->gety() - WinHeight);
				obj->setx(myrand(WinWidth));
			}
			bomb = false;
		}
		car->update(); //更新位置

		if (car->getLIFE() <= 0)
			STATE = false;

		if (mus.GetPlayStatus() == ege::MUSIC_MODE_STOP) { // 发现停止了就重新播放
			mus.Play(0);
		}
		bg->render();
		car->render(); //绘画
		for (auto obj:objs) {
			obj->render();
		}

		/*状态栏*/
		char str1[100],str2[100],str3[100],str4[100];
		string life;
		for (int i = 0; i < car->getLIFE(); i++) {
			life += '*';
		}
		ege::setfont(20, 0, "Consolas");
		ege::setcolor(ege::RED);
		ege::setbkmode(TRANSPARENT);		//文字背景透明
		int score = ege::fclock() * scrollSpeed;
		sprintf(str1, "life value: %s", life.c_str());
		sprintf(str2,"Score: %d", score+AddScore);
		sprintf(str3, "FPS: %.2f",ege::getfps());
		sprintf(str4, "SPEED: %d", scrollSpeed);
		ege::outtextxy(0, 0, str1);
		ege::outtextxy(0, 20, str2);
		ege::outtextxy(0, 40, str3);
		ege::outtextxy(0, 60, str4);

		/*游戏结束后*/
		if (STATE == false) {
			game->render();
			scrollSpeed = 0;
			carSpeed = 0;
			char name[100];
			ege::inputbox_getline("Game is over","Please enter your name", name, sizeof(name) / sizeof(*name));
			string str(name);
			AllScore[str] = score + AddScore;
			int paiming = 1;
			vector<pair<string, int>> vec(AllScore.begin(), AllScore.end());
			AllScore.clear();
			sort(vec.begin(), vec.end(), [](const pair<string, int>&a, const pair<string, int>& b) {return a.second > b.second; });	//键值对按分数降序排名
			char pai[100];
			sprintf(pai, "%4s\t%20s\t%8s", "Ranking", "ID", "Score");
			ege::outtextxy(100+WinWidth/2, 20*(paiming-1),pai);
			for (auto it = vec.begin(); it != vec.end(); it++) {
				ege::setcolor(ege::WHITE);
				if (it->first.empty())
					continue;
				char scores[100];
				sprintf(scores, "%4d\t%20s\t%8d", paiming++, it->first.c_str(), it->second);
				string temp(scores);
				/*此次成绩检测*/
				if(it->first==str){
					temp += "   <-----This is your grade ";
					if (it == vec.begin()) {
						char cou[100]= "Congratulations, breaking the record! ! !";
						ege::setfont(50, 0, "黑体");
						ege::setcolor(ege::RED);
						ege::outtextxy(WinWidth / 3-100, WinHeight / 4, cou);
					}
					ege::setcolor(ege::BLACK);
				}
				ege::setfont(20, 0, "Consolas");
				ege::outtextxy(100+WinWidth/2, 20*(paiming-1), temp.c_str());
			}
			/*清空文件流，准备写入文件*/
			file.clear();
			for (char& s : str) {		//游戏昵称和分数加密
				s += 100;
			}
			score = ~score;
			file << str << "   " << score << endl;
			break;
		}
	}
	ege::getch();
	
	/*删除对象*/
	delete car;
	delete game;
	delete bg;
	for(auto obj:objs)
		delete obj;
}

int main(void)
{

	file.open("log.txt", ios::in | ios::out);

	string strs;
	int nums = 1000;
		/*读取游戏记录文件*/
	while (!file.eof()&&nums--) {
		int score;
		string name;
		file >> name >> score;
		for (char &s : name) {	//游戏昵称和分数解密
			s -= 100 ;
		}
		score = ~score ;
		AllScore[name] = score;
	}
	//无边框
	ege::setinitmode(ege::INIT_NOBORDER | ege::INIT_NOFORCEEXIT);
	//全屏
	ege::initgraph(WinWidth,WinHeight);

	// 随机数初始化，如果需要使用随机数的话
	ege::randomize();
	// 程序主循环
	mainloop();
	// 关闭绘图设备

	file.close();
	ege::closegraph();
	return 0;
}
